#pragma once

#include "types.hpp"

#include <array>

namespace tomboy {
class Memory {
  public:
    Memory() = default;

    [[nodiscard]] auto read(u16 address) const -> u8;
    [[nodiscard]] auto read_io(u8 offset) const -> u8;

    auto write(u16 address, u8 value) -> void;
    auto write_io(u8 offset, u8 value) -> void;

  private:
    std::array<u8, 0xFFFF> memory_;
};

inline auto Memory::read(u16 address) const -> u8
{
    return memory_.at(address);
}

inline auto Memory::read_io(u8 offset) const -> u8
{
    return read(0xFF00 + offset);
}

inline auto Memory::write(u16 address, u8 value) -> void
{
    memory_.at(address) = value;
}

inline auto Memory::write_io(u8 offset, u8 value) -> void
{
    write(0xFF00 + offset, value);
}
} // namespace tomboy
