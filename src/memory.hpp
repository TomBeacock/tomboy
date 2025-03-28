#pragma once

#include "types.hpp"

#include <array>

namespace Tomboy {
constexpr u16 memory_size = 0xFFFF;
constexpr u16 io_start = 0xFF00;

class Memory {
  public:
    Memory();

    [[nodiscard]] auto read(u16 address) const -> u8;
    [[nodiscard]] auto read_io(u8 offset) const -> u8;

    auto write(u16 address, u8 value) -> void;
    auto write_io(u8 offset, u8 value) -> void;

  private:
    std::array<u8, memory_size> memory;
};

auto Memory::read(u16 address) const -> u8
{
    return memory.at(address);
}

auto Memory::read_io(u8 offset) const -> u8
{
    return memory.at(io_start + offset);
}

auto Memory::write(u16 address, u8 value) -> void
{
    memory.at(address) = value;
}

auto Memory::write_io(u8 offset, u8 value) -> void
{
    memory.at(io_start + offset) = value;
}
} // namespace Tomboy
