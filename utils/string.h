#ifndef __UTILS_STRING_H__
#define __UTILS_STRING_H__

#include "result.h"

#include <WString.h>
#include <inttypes.h>

#include <string_view>
#include <cctype>

namespace Utils
{
  String concat(String str, const std::string_view& view)
  {
    str.reserve(str.length() + view.size());
    for (auto ch : view)
    {
      str += ch;
    }
    return std::move(str);
  }

  String makeString(const std::string_view& view)
  {
    String result;
    result.reserve(view.size());
    for (auto ch : view)
    {
      result += ch;
    }
    return std::move(result);
  }

  template <typename F>
  StringResult splitTrimmed(const char* str, size_t size, char separator, F handle)
  {
    const int length = static_cast<int>(size);
    int idxFirst = 0;
    do
    {
      int idxEnd = idxFirst;
      for (auto it = str + idxFirst, end = str + size; it != end; ++it, ++idxEnd)
      {
        if (*it == separator)
        {
          break;
        }
      }
      int idxLast = idxEnd - 1;
      for (; idxFirst != idxEnd && std::isspace(str[idxFirst]); ++idxFirst);
      for (; idxLast > idxFirst && std::isspace(str[idxLast]); --idxLast);
      const int len = idxLast - idxFirst + 1;
      if (len > 0)
      {
        auto res = handle(std::string_view(str + idxFirst, idxLast - idxFirst + 1));
        if (res.isError())
        {
          return res;
        }
      }
      idxFirst = idxEnd + 1;
    } while (idxFirst < length);

    return StringResult::ok();
  }

  template<size_t S> struct SizeType {};
  static constexpr const char* hexFmtImpl(SizeType<1>)       { return "%02X"; }
  static constexpr const char* hexFmtImpl(SizeType<2>)       { return "%04X"; }
  static constexpr const char* hexFmtImpl(SizeType<4>)       { return "%08X"; }
  template<typename T> static constexpr const char* hexFmt() { return hexFmtImpl(SizeType<sizeof(T)>()); }
  template<typename T> static constexpr size_t hexSize()     { return sizeof(T) * 2;}

  bool hexCharToU(char ch, uint8_t& result)
  {
    if ('0' <= ch && ch <= '9')
    {
      result = ch - '0';
      return true;
    }

    if ('A' <= ch && ch <= 'F')
    {
      result = ch - 'A' + 10;
      return true;
    }

    if ('a' <= ch && ch <= 'f')
    {
      result = ch - 'a' + 10;
      return true;
    }

    return false;
  }

  bool isValidStrArgs(const char* str, size_t len)
  {
    return str != nullptr && len > 0 && str[0] != 0;
  }

  template<typename T>
  bool parseHex(const char* str, size_t len, bool ignore0x, T& result)
  {
    static_assert(T(-1) > T(0) && "Result should be unsigned type");

    if (!isValidStrArgs(str, len))
    {
      return false;
    }

    if (len >= 2 && ignore0x)
    {
      if (strncmp(str, "0x", 2) == 0)
      {
        str += 2;
        len -= 2;
        if (!isValidStrArgs(str, len))
        {
          return false;
        }
      }
    }

    if (len > sizeof(T) * 2)
    {
      return false;
    }

    T res = 0;
    for (size_t i = 0; i < len; ++i)
    {
      uint8_t val = 0;
      if (hexCharToU(str[i], val) == false)
      {
        return false;
      }
      res = (res << 4) | val;
    }

    result = res;

    return true;
  }

  template<typename T>
  bool parseFilledHex(const char* str, T& result)
  {
    static_assert(T(-1) > T(0) && "Result should be unsigned type");

    if (!isValidStrArgs(str, sizeof(T) * 2))
    {
      return false;
    }

    T res = 0;
    for (size_t i = 0; i < sizeof(T) * 2; ++i)
    {
      const auto ch = str[i];
      if (ch == 0)
      {
        break;
      }

      uint8_t val = 0;
      if (hexCharToU(ch, val) == false)
      {
        return false;
      }

      res = (res << 4) | val;
    }

    result = res;

    return true;
  }

  template<typename T>
  size_t formatFilledHex(T val, char* out)
  {
    static_assert(T(-1) > T(0) && "Result should be unsigned type");

    return static_cast<size_t>(sprintf(out, hexFmt<T>(), val));
  }

  template<typename Out, typename T>
  size_t printFilledHex(Out& out, T val)
  {
    static_assert(T(-1) > T(0) && "Result should be unsigned type");

    static constexpr size_t size = sizeof(T) * 2;
    char buf[size + 1] = {0};
    formatFilledHex(val, buf);
    return out.write(reinterpret_cast<const uint8_t*>(&*buf), size);
  }

  template <typename T>
  bool parseUnsigned(const char* str, size_t len, T& result)
  {
    static_assert(T(-1) > T(0) && "Result should be unsigned type");

    if (!isValidStrArgs(str, len))
    {
      return false;
    }

    static constexpr T max1 = (~T(0)) / 10;
    static constexpr T max2 = (~T(0)) % 10;

    T res = 0;

    for (auto end = str + len; str != end; ++str)
    {
      const auto ch = *str;
      if (ch < '0' || '9' < ch)
      {
        return false;
      }

      const T d = ch - '0';
      if (res > max1 || (res == max1 && d >  max2))
      {
        return false;
      }

      res = res * 10 + d;
    }

    result = res;

    return true;
  }

  bool parseBool(const char* str, size_t len, bool& result)
  {
    if (!isValidStrArgs(str, len))
    {
      return false;
    }

    const auto ch = str[0];
    if (ch != '0' && ch != '1')
    {
      return false;
    }

    result = ch == '1';

    return true;
  }
}

#endif
