#ifndef __UTILS_RESULT_H__
#define __UTILS_RESULT_H__

#include <inttypes.h>
#include <WString.h>
#include <utility>

template <typename DataType>
struct GenericResult
{
  using Data  = DataType;
  using Error = uint16_t;

  static constexpr Error OK            = Error(0);
  static constexpr Error UNKNOWN_ERROR = Error(1);

  static GenericResult ok()                 { return GenericResult();                    }
  static GenericResult ok(const Data& data) { return GenericResult(OK, data);            }
  static GenericResult ok(Data&& data)      { return GenericResult(OK, std::move(data)); }

  static GenericResult error(Error error)                   { return GenericResult(error);                          }
  static GenericResult error(Error error, const Data& data) { return GenericResult(error, data);                    }
  static GenericResult error(Error error, Data&& data)      { return GenericResult(error, std::move(data));         }
  static GenericResult error(const Data& data)              { return GenericResult(UNKNOWN_ERROR, data);            }
  static GenericResult error(Data&& data)                   { return GenericResult(UNKNOWN_ERROR, std::move(data)); }

  Error err = OK;
  Data  data;

  GenericResult() = default;

  GenericResult(Error e)
    : err(e)
  {
  }

  GenericResult(Error e, const Data& d)
    : err(e)
    , data(d)
  {
  }

  GenericResult(Error e, Data&& d)
    : err(e)
    , data(std::move(d))
  {
  }

  bool isError() const noexcept
  {
    return err;
  }

  bool isOk() const noexcept
  {
    return !isError();
  }

  String toHumanReadableString() const
  {
    if (isOk())
    {
      return data;
    }
    return String(F("Error 0x")) + String(err, 16) + ": " + data;
  }
};

using StringResult = GenericResult<String>;

#endif
