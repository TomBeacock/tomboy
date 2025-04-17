#pragma once

#include "types.hpp"

namespace tomboy {
using Register8 = u8;

class Register16 {
  public:
    Register16() = default;
    Register16(u16 value);

    auto hi() -> Register8 &;
    [[nodiscard]] auto hi() const -> Register8;

    auto lo() -> Register8 &;
    [[nodiscard]] auto lo() const -> Register8;

    operator u16() const;
    auto operator=(u16 value) -> Register16 &;
    auto operator+=(u16 value) -> Register16 &;
    auto operator-=(u16 value) -> Register16 &;

  private:
    Register8 hi_;
    Register8 lo_;
};

Register16::Register16(u16 value)
  : hi_(static_cast<Register8>(value >> 8)),
    lo_(static_cast<Register8>(value))
{
}

inline auto Register16::hi() -> Register8 &
{
    return hi_;
}

inline auto Register16::hi() const -> Register8
{
    return hi_;
}

inline auto Register16::lo() -> Register8 &
{
    return lo_;
}

inline auto Register16::lo() const -> Register8
{
    return lo_;
}

inline Register16::operator u16() const
{
    return static_cast<u16>(hi_) << 8 | lo_;
}

inline auto Register16::operator=(u16 value) -> Register16 &
{
    hi_ = static_cast<Register8>(value >> 8);
    lo_ = static_cast<Register8>(value);
    return *this;
}

inline auto Register16::operator+=(u16 value) -> Register16 &
{
    return (*this = *this + 1);
}

inline auto Register16::operator-=(u16 value) -> Register16 &
{
    return (*this = *this - 1);
}
} // namespace tomboy
