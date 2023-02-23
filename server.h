#ifndef __SERVER_H__
#define __SERVER_H__

#include "utils/result.h"
#include "utils/beeps.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <FS.h>

#include <utility>

class StartsWithUri : public Uri
{
public:
  StartsWithUri(const String& uri)
    : Uri(uri)
  {
  }

  Uri* clone() const override
  {
    return new StartsWithUri(*this);
  };

  bool canHandle(const String& requestUri, std::vector<String>&) override
  {
    return requestUri.startsWith(_uri);
  }
};

template <typename Cfg>
class WedServer
{
  ESP8266WebServer m_server;
  FS&              m_fs;

public:
  WedServer()
    : m_fs(SPIFFS)
  {
  }

  void loop()
  {
    m_server.handleClient();
  }

  template <typename SetupFn>
  void start(SetupFn setup)
  {
    startFS();
    startWiFi();
    startServer(setup);
  }

  template <typename Handle>
  void on(const String& uri, const char* contentType, Handle handle)
  {
    m_server.on(uri, [this, contentType, handle]()
    {
      StringResult result = handle();
      if (result.isError())
      {
        return plainResponse(500, result.toHumanReadableString());
      }
      return m_server.send(200, contentType, std::move(result.data));
    });
  }

  template <typename Handle>
  void onCmd(const String& startsWith, Handle handle)
  {
    m_server.on(StartsWithUri(startsWith), [this, startsWith, handle]()
    {
      StringResult cmd = removeUriRoot(startsWith);
      if (cmd.isError())
      {
        return plainResponse(std::move(cmd));
      }

      StringResult data = m_server.method() == HTTP_POST ? getPlainData() : getArg("data");
      if (data.isError())
      {
        return plainResponse(std::move(data));
      }

      StringResult result = handle(std::move(cmd.data), std::move(data.data));
      if (result.isError())
      {
        return plainResponse(500, result.toHumanReadableString());
      }

      return plainResponse(200, std::move(result.data));
    });
  }

  void serveStatic(const String uri, const String path)
  {
    m_server.serveStatic(uri.c_str(), m_fs, path.c_str());
  }

  void plainResponse(unsigned code, String&& data)
  {
    m_server.send(code, "text/plain", data);
  }

  void plainResponse(StringResult&& resp)
  {
    plainResponse(resp.err, std::move(resp.data));
  }

  StringResult badRequest(String&& reason) const
  {
    return StringResult::error(400, std::move(reason));
  }

  StringResult removeUriRoot(const String& root) const
  {
    const String& uri = m_server.uri();
    if (!uri.startsWith(root))
    {
      return badRequest(String(F("URI '")) + uri + F("' is not started with the '") + root + '\'');
    }
    return StringResult::ok(uri.substring(root.length()));
  }

  StringResult getArg(const String& arg) const
  {
    return StringResult::ok(m_server.arg(arg));
  }

  StringResult ensureHeader(const String& header) const
  {
    if (!m_server.hasHeader(header))
    {
      return badRequest(String(F("The '")) + header + F("' header is missing"));
    }
    return StringResult::ok(m_server.header(header));
  }

  StringResult getPlainData() const
  {
    StringResult plain = getArg(String(F("plain")));
    if (plain.isError())
    {
      return plain;
    }

    StringResult contentLength = ensureHeader(String(F("Content-Length")));
    if (contentLength.isError())
    {
      return contentLength;
    }

    auto contentLengthValue = contentLength.data.toInt();
    if (contentLengthValue < 0 || contentLengthValue != static_cast<decltype(contentLengthValue)>(plain.data.length()))
    {
      return badRequest(String(F("Corrupted request: Content-Length=")) + contentLengthValue + F("; plain=") + plain.data.length());
    }

    return plain;
  }

  static String getRootPage()
  {
    String closePre = F("</pre>");
    String info = "";
    info.reserve(2048);
    info += F("<!DOCTYPE html>");
    info += F("<html>");
    info += F("<style>");
    info += F("  pre {");
    info += F("    font-family:Menlo,Consolas,Monaco,Liberation Mono,Lucida Console,monospace;");
    info += F("    font-size: 1.0em;");
    info += F("    line-height: 0.7;");
    info += F("  }");
    info += F("  a {");
    info += F("    font-weight: bolder;");
    info += F("    font-size: 2em;");
    info += F("    background: #afa;");
    info += F("    height: 3vh;");
    info += F("    padding-left: 5vh;");
    info += F("    padding-right: 5vh;");
    info += F("  }");
    info += F("</style>");
    info += F("<body>");
    info += F("  <pre><a href=\"ui/ir.html\">UI</a></pre>");
    info += F("\n  <pre>-----------------------------------------------------------</pre>");
    info += String(F("\n  <pre> On:                   ")) + (millis() / 1000) + " seconds"            + closePre;
    info += String(F("\n  <pre> Build date:           ")) + __DATE__ + " " + __TIME__                 + closePre;
    info += String(F("\n  <pre> Full Version:         ")) + ESP.getFullVersion()                      + closePre;
    info += String(F("\n  <pre> Cpu Freq MHz:         ")) + ESP.getCpuFreqMHz()                       + closePre;
    info += String(F("\n  <pre> Flash Chip Speed:     ")) + ESP.getFlashChipSpeed()                   + closePre;
    info += String(F("\n  <pre> Flash Chip Size:      ")) + ESP.getFlashChipSize()                    + closePre;
    info += String(F("\n  <pre> Flash Chip Real Size: ")) + ESP.getFlashChipRealSize()                + closePre;
    info += String(F("\n  <pre> Sketch Size:          ")) + ESP.getSketchSize()                       + closePre;
    info += String(F("\n  <pre> Free Sketch Space:    ")) + ESP.getFreeSketchSpace()                  + closePre;
    info += String(F("\n  <pre> Free Heap:            ")) + ESP.getFreeHeap()                         + closePre;
    info += String(F("\n  <pre> Max Free Block Size:  ")) + ESP.getMaxFreeBlockSize()                 + closePre;
    info += String(F("\n  <pre> Heap Fragmentation:   ")) + ESP.getHeapFragmentation()                + closePre;
    info += String(F("\n  <pre> PIN_SEND:             GPIO ")) + Utils::Pins::pin_cast(Cfg::PIN_SEND) + closePre;
    info += F("\n  <pre>-----------------------------------------------------------</pre>");
    info += F("</body>");
    info += F("</html>");
    return info;
  }

private:
  void startWiFi()
  {
    static_assert((sizeof(Cfg::NET_NAME) > 1 || sizeof(Cfg::AP_SSID) > 1) && "NET_NAME or AP_SSID should be specified");

    decltype(WiFi.status()) status;
    if (sizeof(Cfg::NET_NAME) > 1)
    {
      WiFi.begin(Cfg::NET_NAME, Cfg::NET_PASS);
      for (unsigned i = 0; i < Cfg::ATTEMPTS_TO_START_WIFI; ++i)
      {
        Utils::Pins::setPinState(Cfg::PIN_INDICATOR, false);
        delay(1000);
        Utils::Beeps::playSoftware(Cfg::PIN_BEEP, Utils::Beeps::melodyPew());
        Utils::Pins::setPinState(Cfg::PIN_INDICATOR, true);
        delay(50);

        status = WiFi.status();
        printInfo(String(F("wifi: ")) + status);
        if (status == WL_CONNECTED)
        {
          printInfo(String(F("wifi: connected: ")) + Cfg::NET_NAME + ' ' + WiFi.localIP().toString() + ' ' + WiFi.macAddress());
          break;
        }
      }

      if (status != WL_CONNECTED)
      {
        WiFi.disconnect(false);
        printInfo(String(F("wifi: not connected: ")) + Cfg::NET_NAME);
      }
    }

    if (status != WL_CONNECTED && sizeof(Cfg::AP_SSID) > 1)
    {
      if (WiFi.softAP(Cfg::AP_SSID, Cfg::AP_PASS) != true)
      {
        Utils::Beeps::playSoftware(Cfg::PIN_BEEP, Utils::Beeps::melodyReject());
        printInfo(String(F("softAP failed")));
      }

      printInfo(String(F("wifi softAP: ")) + Cfg::AP_SSID + ' ' + WiFi.softAPIP().toString());
    }
  }

  void startFS()
  {
    const auto fs = m_fs.begin();
    printInfo(String(F("fs: ")) + (fs ? F("started") : F("not started")));
    Dir dir = m_fs.openDir("/");
    while (dir.next())
    {
      printInfo(dir.fileName());
    }
  }

  template <typename SetupFn>
  void startServer(SetupFn setup)
  {
    setup(*this);
    m_server.enableCORS(true);
    m_server.collectHeaders("Content-Length");
    m_server.begin(Cfg::PORT);
    printInfo(String(F("server: started ")));
  }

  void printInfo(const String& info) const
  {
    Serial.println(info);
    Serial.flush();
  }
};

#endif
