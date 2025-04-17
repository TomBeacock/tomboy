#include "cpu.hpp"

#include "memory.hpp"
#include "types.hpp"

#include <sys/stat.h>

#include <iostream>
#include <print>
#include <type_traits>

namespace tomboy {

constexpr auto lo_nibble(u8 byte) -> u8
{
    return byte & 0x0F;
}

Cpu::Cpu(Memory *memory)
  : af_(),
    bc_(),
    de_(),
    hl_(),
    sp_(),
    pc_(),
    halted_(false),
    ime_(true),
    memory_(memory)
{
}

auto Cpu::step() -> u8
{
    u8 cycles = 0;
    const auto [opcode, has_prefix] = fetch();

    return cycles;
}

auto Cpu::fetch() const -> FetchResult
{
    const u8 opcode = memory_->read(pc_);
    const bool has_prefix = opcode == 0xCB;
    return {
        .opcode = has_prefix ? memory_->read(pc_ + 1) : opcode,
        .has_prefix = has_prefix,
    };
}

auto Cpu::decode_execute(u8 opcode, bool has_prefix) -> ExecuteResult
{
    // 8-bit opcode
    if (!has_prefix) {
        switch (opcode) {
        case 0x00: return nop();
        case 0x01: return ld_r16_n16(bc_);
        case 0x02: return ld_ra16_a(bc_);
        case 0x03: return inc_r16(bc_);
        case 0x04: return inc_r8(bc_.hi());
        case 0x05: return dec_r8(bc_.hi());
        case 0x06: return ld_r8_n8(bc_.hi());
        case 0x07: return rlc_a();
        case 0x08: return ld_a16_sp();
        case 0x09: return add_hl_r16(bc_);
        case 0x0A: return ld_a_ra16(bc_);
        case 0x0B: return dec_r16(bc_);
        case 0x0C: return inc_r8(bc_.lo());
        case 0x0D: return dec_r8(bc_.lo());
        case 0x0E: return ld_r8_n8(bc_.lo());
        case 0x0F: return rrc_a();
        case 0x10: return stop();
        case 0x11: return ld_r16_n16(de_);
        case 0x12: return ld_ra16_a(de_);
        case 0x13: return inc_r16(de_);
        case 0x14: return inc_r8(de_.hi());
        case 0x15: return dec_r8(de_.hi());
        case 0x16: return ld_r8_n8(de_.hi());
        case 0x17: return rl_a();
        case 0x18: return jr_s8();
        case 0x19: return add_hl_r16(de_);
        case 0x1A: return ld_a_ra16(de_);
        case 0x1B: return dec_r16(de_);
        case 0x1C: return inc_r8(de_.lo());
        case 0x1D: return dec_r8(de_.lo());
        case 0x1E: return ld_r8_n8(de_.lo());
        case 0x1F: return rr_a();
        case 0x20: return jr_cc_s8(Flag::Zero, false);
        case 0x21: return ld_r16_n16(hl_);
        case 0x22: return ld_hlai_a();
        case 0x23: return inc_r16(hl_);
        case 0x24: return inc_r8(hl_.hi());
        case 0x25: return dec_r8(hl_.hi());
        case 0x26: return ld_r8_n8(hl_.hi());
        case 0x27: return daa();
        case 0x28: return jr_cc_s8(Flag::Zero, true);
        case 0x29: return add_hl_r16(hl_);
        case 0x2A: return ld_a_hlai();
        case 0x2B: return dec_r16(hl_);
        case 0x2C: return inc_r8(hl_.lo());
        case 0x2D: return dec_r8(hl_.lo());
        case 0x2E: return ld_r8_n8(hl_.lo());
        case 0x2F: return cpl();
        case 0x30: return jr_cc_s8(Flag::Carry, false);
        case 0x31: return ld_r16_n16(sp_);
        case 0x32: return ld_hlad_a();
        case 0x33: return inc_r16(sp_);
        case 0x34: return inc_hla();
        case 0x35: return dec_hla();
        case 0x36: return ld_hla_n8();
        case 0x37: return scf();
        case 0x38: return jr_cc_s8(Flag::Carry, true);
        case 0x39: return add_hl_r16(sp_);
        case 0x3A: return ld_a_hlad();
        case 0x3B: return dec_r16(sp_);
        case 0x3C: return inc_r8(af_.hi());
        case 0x3D: return dec_r8(af_.hi());
        case 0x3E: return ld_r8_n8(af_.lo());
        case 0x3F: return ccf();
        case 0x40: return ld_r8_r8(bc_.hi(), bc_.hi());
        case 0x41: return ld_r8_r8(bc_.hi(), bc_.lo());
        case 0x42: return ld_r8_r8(bc_.hi(), de_.hi());
        case 0x43: return ld_r8_r8(bc_.hi(), de_.lo());
        case 0x44: return ld_r8_r8(bc_.hi(), hl_.hi());
        case 0x45: return ld_r8_r8(bc_.hi(), hl_.lo());
        case 0x46: return ld_r8_hla(bc_.hi());
        case 0x47: return ld_r8_r8(bc_.hi(), af_.hi());
        case 0x48: return ld_r8_r8(bc_.lo(), bc_.hi());
        case 0x49: return ld_r8_r8(bc_.lo(), bc_.lo());
        case 0x4A: return ld_r8_r8(bc_.lo(), de_.hi());
        case 0x4B: return ld_r8_r8(bc_.lo(), de_.lo());
        case 0x4C: return ld_r8_r8(bc_.lo(), hl_.hi());
        case 0x4D: return ld_r8_r8(bc_.lo(), hl_.lo());
        case 0x4E: return ld_r8_hla(bc_.lo());
        case 0x4F: return ld_r8_r8(bc_.lo(), af_.hi());
        case 0x50: return ld_r8_r8(de_.hi(), bc_.hi());
        case 0x51: return ld_r8_r8(de_.hi(), bc_.lo());
        case 0x52: return ld_r8_r8(de_.hi(), de_.hi());
        case 0x53: return ld_r8_r8(de_.hi(), de_.lo());
        case 0x54: return ld_r8_r8(de_.hi(), hl_.hi());
        case 0x55: return ld_r8_r8(de_.hi(), hl_.lo());
        case 0x56: return ld_r8_hla(de_.hi());
        case 0x57: return ld_r8_r8(de_.hi(), af_.hi());
        case 0x58: return ld_r8_r8(de_.lo(), bc_.hi());
        case 0x59: return ld_r8_r8(de_.lo(), bc_.lo());
        case 0x5A: return ld_r8_r8(de_.lo(), de_.hi());
        case 0x5B: return ld_r8_r8(de_.lo(), de_.lo());
        case 0x5C: return ld_r8_r8(de_.lo(), hl_.hi());
        case 0x5D: return ld_r8_r8(de_.lo(), hl_.lo());
        case 0x5E: return ld_r8_hla(de_.lo());
        case 0x5F: return ld_r8_r8(de_.lo(), af_.hi());
        case 0x60: return ld_r8_r8(hl_.hi(), bc_.hi());
        case 0x61: return ld_r8_r8(hl_.hi(), bc_.lo());
        case 0x62: return ld_r8_r8(hl_.hi(), de_.hi());
        case 0x63: return ld_r8_r8(hl_.hi(), de_.lo());
        case 0x64: return ld_r8_r8(hl_.hi(), hl_.hi());
        case 0x65: return ld_r8_r8(hl_.hi(), hl_.lo());
        case 0x66: return ld_r8_hla(hl_.hi());
        case 0x67: return ld_r8_r8(hl_.hi(), af_.hi());
        case 0x68: return ld_r8_r8(hl_.lo(), bc_.hi());
        case 0x69: return ld_r8_r8(hl_.lo(), bc_.lo());
        case 0x6A: return ld_r8_r8(hl_.lo(), de_.hi());
        case 0x6B: return ld_r8_r8(hl_.lo(), de_.lo());
        case 0x6C: return ld_r8_r8(hl_.lo(), hl_.hi());
        case 0x6D: return ld_r8_r8(hl_.lo(), hl_.lo());
        case 0x6E: return ld_r8_hla(hl_.lo());
        case 0x6F: return ld_r8_r8(hl_.lo(), af_.hi());
        case 0x70: return ld_hla_r8(bc_.hi());
        case 0x71: return ld_hla_r8(bc_.lo());
        case 0x72: return ld_hla_r8(de_.hi());
        case 0x73: return ld_hla_r8(de_.lo());
        case 0x74: return ld_hla_r8(hl_.hi());
        case 0x75: return ld_hla_r8(hl_.lo());
        case 0x76: return halt();
        case 0x77: return ld_hla_r8(af_.hi());
        case 0x78: return ld_r8_r8(af_.hi(), bc_.hi());
        case 0x79: return ld_r8_r8(af_.hi(), bc_.lo());
        case 0x7A: return ld_r8_r8(af_.hi(), de_.hi());
        case 0x7B: return ld_r8_r8(af_.hi(), de_.lo());
        case 0x7C: return ld_r8_r8(af_.hi(), hl_.hi());
        case 0x7D: return ld_r8_r8(af_.hi(), hl_.lo());
        case 0x7E: return ld_r8_hla(af_.hi());
        case 0x7F: return ld_r8_r8(af_.hi(), af_.hi());
        case 0x80: return add_r8(bc_.hi());
        case 0x81: return add_r8(bc_.lo());
        case 0x82: return add_r8(de_.hi());
        case 0x83: return add_r8(de_.lo());
        case 0x84: return add_r8(hl_.hi());
        case 0x85: return add_r8(hl_.lo());
        case 0x86: return add_hla();
        case 0x87: return add_r8(af_.hi());
        case 0x88: return adc_r8(bc_.hi());
        case 0x89: return adc_r8(bc_.lo());
        case 0x8A: return adc_r8(de_.hi());
        case 0x8B: return adc_r8(de_.lo());
        case 0x8C: return adc_r8(hl_.hi());
        case 0x8D: return adc_r8(hl_.lo());
        case 0x8E: return adc_hla();
        case 0x8F: return adc_r8(af_.hi());
        case 0x90: return sub_r8(bc_.hi());
        case 0x91: return sub_r8(bc_.lo());
        case 0x92: return sub_r8(de_.hi());
        case 0x93: return sub_r8(de_.lo());
        case 0x94: return sub_r8(hl_.hi());
        case 0x95: return sub_r8(hl_.lo());
        case 0x96: return sub_hla();
        case 0x97: return sub_r8(af_.hi());
        case 0x98: return sbc_r8(bc_.hi());
        case 0x99: return sbc_r8(bc_.lo());
        case 0x9A: return sbc_r8(de_.hi());
        case 0x9B: return sbc_r8(de_.lo());
        case 0x9C: return sbc_r8(hl_.hi());
        case 0x9D: return sbc_r8(hl_.lo());
        case 0x9E: return sbc_hla();
        case 0x9F: return sbc_r8(af_.hi());
        case 0xA0: return and_r8(bc_.hi());
        case 0xA1: return and_r8(bc_.lo());
        case 0xA2: return and_r8(de_.hi());
        case 0xA3: return and_r8(de_.lo());
        case 0xA4: return and_r8(hl_.hi());
        case 0xA5: return and_r8(hl_.lo());
        case 0xA6: return and_hla();
        case 0xA7: return and_r8(af_.hi());
        case 0xA8: return xor_r8(bc_.hi());
        case 0xA9: return xor_r8(bc_.lo());
        case 0xAA: return xor_r8(de_.hi());
        case 0xAB: return xor_r8(de_.lo());
        case 0xAC: return xor_r8(hl_.hi());
        case 0xAD: return xor_r8(hl_.lo());
        case 0xAE: return xor_hla();
        case 0xAF: return xor_r8(af_.hi());
        case 0xB0: return or_r8(bc_.hi());
        case 0xB1: return or_r8(bc_.lo());
        case 0xB2: return or_r8(de_.hi());
        case 0xB3: return or_r8(de_.lo());
        case 0xB4: return or_r8(hl_.hi());
        case 0xB5: return or_r8(hl_.lo());
        case 0xB6: return or_hla();
        case 0xB7: return or_r8(af_.hi());
        case 0xB8: return cp_r8(bc_.hi());
        case 0xB9: return cp_r8(bc_.lo());
        case 0xBA: return cp_r8(de_.hi());
        case 0xBB: return cp_r8(de_.lo());
        case 0xBC: return cp_r8(hl_.hi());
        case 0xBD: return cp_r8(hl_.lo());
        case 0xBE: return cp_hla();
        case 0xBF: return cp_r8(af_.hi());
        case 0xC0: return ret_cc(Flag::Zero, false);
        case 0xC1: return pop_r16(bc_);
        case 0xC2: return jp_cc_a16(Flag::Zero, false);
        case 0xC3: return jp_a16();
        case 0xC4: return call_cc_a16(Flag::Zero, false);
        case 0xC5: return push_r16(bc_);
        case 0xC6: return add_n8();
        case 0xC7: return rst(0x00);
        case 0xC8: return ret_cc(Flag::Zero, true);
        case 0xC9: return ret();
        case 0xCA: return jp_cc_a16(Flag::Zero, true);

        case 0xCC: return call_cc_a16(Flag::Zero, true);
        case 0xCD: return call_a16();
        case 0xCE: return adc_n8();
        case 0xCF: return rst(0x08);
        case 0xD0: return ret_cc(Flag::Carry, false);
        case 0xD1: return pop_r16(de_);
        case 0xD2: return jp_cc_a16(Flag::Carry, false);

        case 0xD4: return call_cc_a16(Flag::Carry, false);
        case 0xD5: return push_r16(de_);
        case 0xD6: return sub_n8();
        case 0xD7: return rst(0x10);
        case 0xD8: return ret_cc(Flag::Carry, true);
        case 0xD9: return reti();
        case 0xDA: return jp_cc_a16(Flag::Carry, true);

        case 0xDC: return call_cc_a16(Flag::Carry, true);

        case 0xDE: return sbc_n8();
        case 0xDF: return rst(0x18);
        case 0xE0: return ldh_a8_a();
        case 0xE1: return pop_r16(hl_);
        case 0xE2: return ldh_c_a();

        case 0xE5: return push_r16(hl_);
        case 0xE6: return and_n8();
        case 0xE7: return rst(0x20);
        case 0xE8: return add_sp_s8();
        case 0xE9: return jp_hl();
        case 0xEA: return ldh_a16_a();

        case 0xEE: return xor_n8();
        case 0xEF: return rst(0x28);
        case 0xF0: return ldh_a_a8();
        case 0xF1: return pop_r16(af_);
        case 0xF2: return ldh_a_c();
        case 0xF3: return di();

        case 0xF5: return push_r16(af_);
        case 0xF6: return or_n8();
        case 0xF7: return rst(0x30);
        case 0xF8: return ld_hl_sp_s8();
        case 0xF9: return ld_sp_hl();
        case 0xFA: return ldh_a_a16();
        case 0xFB: return ei();

        case 0xFE: return cp_n8();
        case 0xFF: return rst(0x38);
        default:
            std::println(
                std::cerr, "Decode failed. Invalid opcode: 0x{:x}", opcode);
            return {.new_pc = pc_, .cycles_used = 0};
        }
    }
    // 16-bit opcode
    else {
        switch (opcode) {
        case 0x00: return rlc_r8(bc_.hi());
        case 0x01: return rlc_r8(bc_.lo());
        case 0x02: return rlc_r8(de_.hi());
        case 0x03: return rlc_r8(de_.lo());
        case 0x04: return rlc_r8(hl_.hi());
        case 0x05: return rlc_r8(hl_.lo());
        case 0x06: return rlc_hla();
        case 0x07: return rlc_r8(af_.hi());
        case 0x08: return rrc_r8(bc_.hi());
        case 0x09: return rrc_r8(bc_.lo());
        case 0x0A: return rrc_r8(de_.hi());
        case 0x0B: return rrc_r8(de_.lo());
        case 0x0C: return rrc_r8(hl_.hi());
        case 0x0D: return rrc_r8(hl_.lo());
        case 0x0E: return rrc_hla();
        case 0x0F: return rrc_r8(af_.hi());
        case 0x10: return rl_r8(bc_.hi());
        case 0x11: return rl_r8(bc_.lo());
        case 0x12: return rl_r8(de_.hi());
        case 0x13: return rl_r8(de_.lo());
        case 0x14: return rl_r8(hl_.hi());
        case 0x15: return rl_r8(hl_.lo());
        case 0x16: return rl_hla();
        case 0x17: return rl_r8(af_.hi());
        case 0x18: return rr_r8(bc_.hi());
        case 0x19: return rr_r8(bc_.lo());
        case 0x1A: return rr_r8(de_.hi());
        case 0x1B: return rr_r8(de_.lo());
        case 0x1C: return rr_r8(hl_.hi());
        case 0x1D: return rr_r8(hl_.lo());
        case 0x1E: return rr_hla();
        case 0x1F: return rr_r8(af_.hi());
        case 0x20: return sla_r8(bc_.hi());
        case 0x21: return sla_r8(bc_.lo());
        case 0x22: return sla_r8(de_.hi());
        case 0x23: return sla_r8(de_.lo());
        case 0x24: return sla_r8(hl_.hi());
        case 0x25: return sla_r8(hl_.lo());
        case 0x26: return sla_hla();
        case 0x27: return sla_r8(af_.hi());
        case 0x28: return sra_r8(bc_.hi());
        case 0x29: return sra_r8(bc_.lo());
        case 0x2A: return sra_r8(de_.hi());
        case 0x2B: return sra_r8(de_.lo());
        case 0x2C: return sra_r8(hl_.hi());
        case 0x2D: return sra_r8(hl_.lo());
        case 0x2E: return sra_hla();
        case 0x2F: return sra_r8(af_.hi());
        case 0x30: return swap_r8(bc_.hi());
        case 0x31: return swap_r8(bc_.lo());
        case 0x32: return swap_r8(de_.hi());
        case 0x33: return swap_r8(de_.lo());
        case 0x34: return swap_r8(hl_.hi());
        case 0x35: return swap_r8(hl_.lo());
        case 0x36: return swap_hla();
        case 0x37: return swap_r8(af_.hi());
        case 0x38: return srl_r8(bc_.hi());
        case 0x39: return srl_r8(bc_.lo());
        case 0x3A: return srl_r8(de_.hi());
        case 0x3B: return srl_r8(de_.lo());
        case 0x3C: return srl_r8(hl_.hi());
        case 0x3D: return srl_r8(hl_.lo());
        case 0x3E: return srl_hla();
        case 0x3F: return srl_r8(af_.hi());
        case 0x40: return bit_r8(0, bc_.hi());
        case 0x41: return bit_r8(0, bc_.lo());
        case 0x42: return bit_r8(0, de_.hi());
        case 0x43: return bit_r8(0, de_.lo());
        case 0x44: return bit_r8(0, hl_.hi());
        case 0x45: return bit_r8(0, hl_.lo());
        case 0x46: return bit_hla(0);
        case 0x47: return bit_r8(0, af_.hi());
        case 0x48: return bit_r8(1, bc_.hi());
        case 0x49: return bit_r8(1, bc_.lo());
        case 0x4A: return bit_r8(1, de_.hi());
        case 0x4B: return bit_r8(1, de_.lo());
        case 0x4C: return bit_r8(1, hl_.hi());
        case 0x4D: return bit_r8(1, hl_.lo());
        case 0x4E: return bit_hla(1);
        case 0x4F: return bit_r8(1, af_.hi());
        case 0x50: return bit_r8(2, bc_.hi());
        case 0x51: return bit_r8(2, bc_.lo());
        case 0x52: return bit_r8(2, de_.hi());
        case 0x53: return bit_r8(2, de_.lo());
        case 0x54: return bit_r8(2, hl_.hi());
        case 0x55: return bit_r8(2, hl_.lo());
        case 0x56: return bit_hla(2);
        case 0x57: return bit_r8(2, af_.hi());
        case 0x58: return bit_r8(3, bc_.hi());
        case 0x59: return bit_r8(3, bc_.lo());
        case 0x5A: return bit_r8(3, de_.hi());
        case 0x5B: return bit_r8(3, de_.lo());
        case 0x5C: return bit_r8(3, hl_.hi());
        case 0x5D: return bit_r8(3, hl_.lo());
        case 0x5E: return bit_hla(3);
        case 0x5F: return bit_r8(3, af_.hi());
        case 0x60: return bit_r8(4, bc_.hi());
        case 0x61: return bit_r8(4, bc_.lo());
        case 0x62: return bit_r8(4, de_.hi());
        case 0x63: return bit_r8(4, de_.lo());
        case 0x64: return bit_r8(4, hl_.hi());
        case 0x65: return bit_r8(4, hl_.lo());
        case 0x66: return bit_hla(4);
        case 0x67: return bit_r8(4, af_.hi());
        case 0x68: return bit_r8(5, bc_.hi());
        case 0x69: return bit_r8(5, bc_.lo());
        case 0x6A: return bit_r8(5, de_.hi());
        case 0x6B: return bit_r8(5, de_.lo());
        case 0x6C: return bit_r8(5, hl_.hi());
        case 0x6D: return bit_r8(5, hl_.lo());
        case 0x6E: return bit_hla(5);
        case 0x6F: return bit_r8(5, af_.hi());
        case 0x70: return bit_r8(6, bc_.hi());
        case 0x71: return bit_r8(6, bc_.lo());
        case 0x72: return bit_r8(6, de_.hi());
        case 0x73: return bit_r8(6, de_.lo());
        case 0x74: return bit_r8(6, hl_.hi());
        case 0x75: return bit_r8(6, hl_.lo());
        case 0x76: return bit_hla(6);
        case 0x77: return bit_r8(6, af_.hi());
        case 0x78: return bit_r8(7, bc_.hi());
        case 0x79: return bit_r8(7, bc_.lo());
        case 0x7A: return bit_r8(7, de_.hi());
        case 0x7B: return bit_r8(7, de_.lo());
        case 0x7C: return bit_r8(7, hl_.hi());
        case 0x7D: return bit_r8(7, hl_.lo());
        case 0x7E: return bit_hla(7);
        case 0x7F: return bit_r8(7, af_.hi());
        case 0x80: return res_r8(0, bc_.hi());
        case 0x81: return res_r8(0, bc_.lo());
        case 0x82: return res_r8(0, de_.hi());
        case 0x83: return res_r8(0, de_.lo());
        case 0x84: return res_r8(0, hl_.hi());
        case 0x85: return res_r8(0, hl_.lo());
        case 0x86: return res_hla(0);
        case 0x87: return res_r8(0, af_.hi());
        case 0x88: return res_r8(1, bc_.hi());
        case 0x89: return res_r8(1, bc_.lo());
        case 0x8A: return res_r8(1, de_.hi());
        case 0x8B: return res_r8(1, de_.lo());
        case 0x8C: return res_r8(1, hl_.hi());
        case 0x8D: return res_r8(1, hl_.lo());
        case 0x8E: return res_hla(1);
        case 0x8F: return res_r8(1, af_.hi());
        case 0x90: return res_r8(2, bc_.hi());
        case 0x91: return res_r8(2, bc_.lo());
        case 0x92: return res_r8(2, de_.hi());
        case 0x93: return res_r8(2, de_.lo());
        case 0x94: return res_r8(2, hl_.hi());
        case 0x95: return res_r8(2, hl_.lo());
        case 0x96: return res_hla(2);
        case 0x97: return res_r8(2, af_.hi());
        case 0x98: return res_r8(3, bc_.hi());
        case 0x99: return res_r8(3, bc_.lo());
        case 0x9A: return res_r8(3, de_.hi());
        case 0x9B: return res_r8(3, de_.lo());
        case 0x9C: return res_r8(3, hl_.hi());
        case 0x9D: return res_r8(3, hl_.lo());
        case 0x9E: return res_hla(3);
        case 0x9F: return res_r8(3, af_.hi());
        case 0xA0: return res_r8(4, bc_.hi());
        case 0xA1: return res_r8(4, bc_.lo());
        case 0xA2: return res_r8(4, de_.hi());
        case 0xA3: return res_r8(4, de_.lo());
        case 0xA4: return res_r8(4, hl_.hi());
        case 0xA5: return res_r8(4, hl_.lo());
        case 0xA6: return res_hla(4);
        case 0xA7: return res_r8(4, af_.hi());
        case 0xA8: return res_r8(5, bc_.hi());
        case 0xA9: return res_r8(5, bc_.lo());
        case 0xAA: return res_r8(5, de_.hi());
        case 0xAB: return res_r8(5, de_.lo());
        case 0xAC: return res_r8(5, hl_.hi());
        case 0xAD: return res_r8(5, hl_.lo());
        case 0xAE: return res_hla(5);
        case 0xAF: return res_r8(5, af_.hi());
        case 0xB0: return res_r8(6, bc_.hi());
        case 0xB1: return res_r8(6, bc_.lo());
        case 0xB2: return res_r8(6, de_.hi());
        case 0xB3: return res_r8(6, de_.lo());
        case 0xB4: return res_r8(6, hl_.hi());
        case 0xB5: return res_r8(6, hl_.lo());
        case 0xB6: return res_hla(6);
        case 0xB7: return res_r8(6, af_.hi());
        case 0xB8: return res_r8(7, bc_.hi());
        case 0xB9: return res_r8(7, bc_.lo());
        case 0xBA: return res_r8(7, de_.hi());
        case 0xBB: return res_r8(7, de_.lo());
        case 0xBC: return res_r8(7, hl_.hi());
        case 0xBD: return res_r8(7, hl_.lo());
        case 0xBE: return res_hla(7);
        case 0xBF: return res_r8(7, af_.hi());
        case 0xC0: return set_r8(0, bc_.hi());
        case 0xC1: return set_r8(0, bc_.lo());
        case 0xC2: return set_r8(0, de_.hi());
        case 0xC3: return set_r8(0, de_.lo());
        case 0xC4: return set_r8(0, hl_.hi());
        case 0xC5: return set_r8(0, hl_.lo());
        case 0xC6: return set_hla(0);
        case 0xC7: return set_r8(0, af_.hi());
        case 0xC8: return set_r8(1, bc_.hi());
        case 0xC9: return set_r8(1, bc_.lo());
        case 0xCA: return set_r8(1, de_.hi());
        case 0xCB: return set_r8(1, de_.lo());
        case 0xCC: return set_r8(1, hl_.hi());
        case 0xCD: return set_r8(1, hl_.lo());
        case 0xCE: return set_hla(1);
        case 0xCF: return set_r8(1, af_.hi());
        case 0xD0: return set_r8(2, bc_.hi());
        case 0xD1: return set_r8(2, bc_.lo());
        case 0xD2: return set_r8(2, de_.hi());
        case 0xD3: return set_r8(2, de_.lo());
        case 0xD4: return set_r8(2, hl_.hi());
        case 0xD5: return set_r8(2, hl_.lo());
        case 0xD6: return set_hla(2);
        case 0xD7: return set_r8(2, af_.hi());
        case 0xD8: return set_r8(3, bc_.hi());
        case 0xD9: return set_r8(3, bc_.lo());
        case 0xDA: return set_r8(3, de_.hi());
        case 0xDB: return set_r8(3, de_.lo());
        case 0xDC: return set_r8(3, hl_.hi());
        case 0xDD: return set_r8(3, hl_.lo());
        case 0xDE: return set_hla(3);
        case 0xDF: return set_r8(3, af_.hi());
        case 0xE0: return set_r8(4, bc_.hi());
        case 0xE1: return set_r8(4, bc_.lo());
        case 0xE2: return set_r8(4, de_.hi());
        case 0xE3: return set_r8(4, de_.lo());
        case 0xE4: return set_r8(4, hl_.hi());
        case 0xE5: return set_r8(4, hl_.lo());
        case 0xE6: return set_hla(4);
        case 0xE7: return set_r8(4, af_.hi());
        case 0xE8: return set_r8(5, bc_.hi());
        case 0xE9: return set_r8(5, bc_.lo());
        case 0xEA: return set_r8(5, de_.hi());
        case 0xEB: return set_r8(5, de_.lo());
        case 0xEC: return set_r8(5, hl_.hi());
        case 0xED: return set_r8(5, hl_.lo());
        case 0xEE: return set_hla(5);
        case 0xEF: return set_r8(5, af_.hi());
        case 0xF0: return set_r8(6, bc_.hi());
        case 0xF1: return set_r8(6, bc_.lo());
        case 0xF2: return set_r8(6, de_.hi());
        case 0xF3: return set_r8(6, de_.lo());
        case 0xF4: return set_r8(6, hl_.hi());
        case 0xF5: return set_r8(6, hl_.lo());
        case 0xF6: return set_hla(6);
        case 0xF7: return set_r8(6, af_.hi());
        case 0xF8: return set_r8(7, bc_.hi());
        case 0xF9: return set_r8(7, bc_.lo());
        case 0xFA: return set_r8(7, de_.hi());
        case 0xFB: return set_r8(7, de_.lo());
        case 0xFC: return set_r8(7, hl_.hi());
        case 0xFD: return set_r8(7, hl_.lo());
        case 0xFE: return set_hla(7);
        case 0xFF: return set_r8(7, af_.hi());

        default:
            std::println(
                std::cerr, "Decode failed. Invalid opcode: 0xCB{:x}", opcode);
            return {.new_pc = pc_, .cycles_used = 0};
        }
    }
}

auto Cpu::ld_r8_r8(Register8 &reg1, const Register8 &reg2) -> ExecuteResult
{
    reg1 = reg2;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::ld_r8_n8(Register8 &reg) -> ExecuteResult
{
    reg = memory_->read(pc_ + 1);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::ld_r16_n16(Register16 &reg) -> Cpu::ExecuteResult
{
    reg.lo() = memory_->read(pc_ + 1);
    reg.hi() = memory_->read(pc_ + 2);
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 3,
    };
}

auto Cpu::ld_ra16_a(const Register16 &reg) -> ExecuteResult
{
    memory_->write(reg, af_.hi());
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_a_ra16(const Register16 &reg) -> ExecuteResult
{
    af_.hi() = memory_->read(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_hla_r8(const Register8 &reg) -> ExecuteResult
{
    memory_->write(hl_, reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_r8_hla(Register8 &reg) -> ExecuteResult
{
    reg = memory_->read(hl_);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_hla_n8() -> ExecuteResult
{
    memory_->write(hl_, memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 3,
    };
}

auto Cpu::ld_hlai_a() -> ExecuteResult
{
    memory_->write(hl_, af_.hi());
    hl_ += 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_a_hlai() -> ExecuteResult
{
    af_.hi() = memory_->read(hl_);
    hl_ += 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_hlad_a() -> ExecuteResult
{
    memory_->write(hl_, af_.hi());
    hl_ -= 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ld_a_hlad() -> ExecuteResult
{
    af_.hi() = memory_->read(hl_);
    hl_ -= 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ldh_a8_a() -> ExecuteResult
{
    memory_->write_io(memory_->read(pc_ + 1), af_.hi());
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 3,
    };
}

auto Cpu::ldh_a_a8() -> ExecuteResult
{
    af_.hi() = memory_->read_io(memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 3,
    };
}

auto Cpu::ldh_c_a() -> ExecuteResult
{
    memory_->write_io(bc_.lo(), af_.hi());
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ldh_a_c() -> ExecuteResult
{
    af_.hi() = memory_->read_io(bc_.lo());
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::ldh_a16_a() -> ExecuteResult
{
    u16 address = memory_->read(pc_ + 1) | memory_->read(pc_ + 2) << 8;
    memory_->write(address, af_.hi());
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 4,
    };
}

auto Cpu::ldh_a_a16() -> ExecuteResult
{
    u16 address = memory_->read(pc_ + 1) | memory_->read(pc_ + 2) << 8;
    af_.hi() = memory_->read(address);
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 4,
    };
}

auto Cpu::add_n8() -> ExecuteResult
{
    af_.hi() = add(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::add_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = add(af_.hi(), reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::add_hla() -> ExecuteResult
{
    af_.hi() = add(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::add_hl_r16(const Register16 &reg) -> ExecuteResult
{
    hl_ = add(hl_, reg);
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::adc_n8() -> ExecuteResult
{
    af_.hi() = adc(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::adc_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = adc(af_.hi(), reg);
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::adc_hla() -> ExecuteResult
{
    af_.hi() = adc(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::sub_n8() -> ExecuteResult
{
    af_.hi() = sub(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::sub_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = sub(af_.hi(), reg);
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::sub_hla() -> ExecuteResult
{
    af_.hi() = sub(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::sbc_n8() -> ExecuteResult
{
    af_.hi() = sbc(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::sbc_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = sbc(af_.hi(), reg);
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::sbc_hla() -> ExecuteResult
{
    af_.hi() = sbc(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::cp_n8() -> ExecuteResult
{
    cp(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u8>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::cp_r8(const Register8 &reg) -> ExecuteResult
{
    cp(af_.hi(), reg);
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::cp_hla() -> ExecuteResult
{
    cp(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u8>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::inc_r8(Register8 &reg) -> ExecuteResult
{
    reg = inc(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::inc_r16(Register16 &reg) -> ExecuteResult
{
    reg += 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::inc_hla() -> ExecuteResult
{
    memory_->write(hl_, inc(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 3,
    };
}

auto Cpu::dec_r8(Register8 &reg) -> ExecuteResult
{
    reg = dec(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::dec_r16(Register16 &reg) -> ExecuteResult
{
    reg -= 1;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::dec_hla() -> ExecuteResult
{
    memory_->write(hl_, dec(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 3,
    };
}

auto Cpu::swap_r8(Register8 &reg) -> ExecuteResult
{

    reg = swap(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::swap_hla() -> ExecuteResult
{

    memory_->write(hl_, swap(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::sla_r8(Register8 &reg) -> ExecuteResult
{

    reg = sla(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::sla_hla() -> ExecuteResult
{

    memory_->write(hl_, sla(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::sra_r8(Register8 &reg) -> ExecuteResult
{

    reg = sra(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::sra_hla() -> ExecuteResult
{

    memory_->write(hl_, sra(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::srl_r8(Register8 &reg) -> ExecuteResult
{

    reg = srl(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::srl_hla() -> ExecuteResult
{

    memory_->write(hl_, srl(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::rl_r8(Register8 &reg) -> ExecuteResult
{
    reg = rl(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::rl_a() -> ExecuteResult
{
    af_.hi() = rl(af_.hi());
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::rl_hla() -> ExecuteResult
{
    memory_->write(hl_, rl(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::rlc_r8(Register8 &reg) -> ExecuteResult
{
    reg = rlc(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::rlc_a() -> ExecuteResult
{
    af_.hi() = rlc(af_.hi());
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::rlc_hla() -> ExecuteResult
{
    memory_->write(hl_, rlc(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::rr_r8(Register8 &reg) -> ExecuteResult
{
    reg = rr(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::rr_a() -> ExecuteResult
{
    af_.hi() = rr(af_.hi());
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::rr_hla() -> ExecuteResult
{
    memory_->write(hl_, rr(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::rrc_r8(Register8 &reg) -> ExecuteResult
{
    reg = rrc(reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::rrc_a() -> ExecuteResult
{
    af_.hi() = rlc(af_.hi());
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::rrc_hla() -> ExecuteResult
{
    memory_->write(hl_, rrc(memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::and_n8() -> ExecuteResult
{
    af_.hi() = bitwise_and(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::and_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = bitwise_and(af_.hi(), reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::and_hla() -> ExecuteResult
{
    af_.hi() = bitwise_and(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::or_n8() -> ExecuteResult
{
    af_.hi() = bitwise_or(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::or_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = bitwise_or(af_.hi(), reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::or_hla() -> ExecuteResult
{
    af_.hi() = bitwise_or(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::xor_n8() -> ExecuteResult
{
    af_.hi() = bitwise_xor(af_.hi(), memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::xor_r8(const Register8 &reg) -> ExecuteResult
{
    af_.hi() = bitwise_xor(af_.hi(), reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::xor_hla() -> ExecuteResult
{
    af_.hi() = bitwise_xor(af_.hi(), memory_->read(hl_));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::cpl() -> ExecuteResult
{
    af_.hi() = ~af_.hi();
    set_flag(Flag::Subtraction, true);
    set_flag(Flag::HalfCarry, true);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::bit_r8(u8 bit, const Register8 &reg) -> ExecuteResult
{
    Cpu::bit(bit, reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::bit_hla(u8 bit) -> ExecuteResult
{
    Cpu::bit(bit, memory_->read(hl_));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 3,
    };
}

auto Cpu::res_r8(u8 bit, Register8 &reg) -> ExecuteResult
{
    reg = res(bit, reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::res_hla(u8 bit) -> ExecuteResult
{
    memory_->write(hl_, res(bit, memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::set_r8(u8 bit, Register8 &reg) -> ExecuteResult
{
    reg = set(bit, reg);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::set_hla(u8 bit) -> ExecuteResult
{
    memory_->write(hl_, set(bit, memory_->read(hl_)));
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::jp_a16() -> ExecuteResult
{
    u16 pc = memory_->read(pc_ + 1) | memory_->read(pc_ + 2) << 8;
    return {
        .new_pc = pc,
        .cycles_used = 4,
    };
}

auto Cpu::jp_cc_a16(Flag flag, bool value) -> ExecuteResult
{
    if (get_flag(flag) == value) {
        return jp_a16();
    }
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 3,
    };
}

auto Cpu::jp_hl() -> ExecuteResult
{
    return {
        .new_pc = hl_,
        .cycles_used = 1,
    };
}

auto Cpu::jr_s8() -> ExecuteResult
{
    i8 offset = static_cast<i8>(memory_->read(pc_ + 1));
    return {
        .new_pc = static_cast<u16>(pc_ + 2 + offset),
        .cycles_used = 3,
    };
}

auto Cpu::jr_cc_s8(Flag flag, bool value) -> ExecuteResult
{
    if (get_flag(flag) == value) {
        return jr_s8();
    }
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 2,
    };
}

auto Cpu::call_a16() -> ExecuteResult
{
    Register16 ret = pc_ + 3;
    sp_ -= 2;
    memory_->write(sp_ + 1, ret.hi());
    memory_->write(sp_, ret.lo());

    u16 pc = memory_->read(pc_ + 1) | memory_->read(pc_ + 2) << 8;
    return {
        .new_pc = pc,
        .cycles_used = 6,
    };
}

auto Cpu::call_cc_a16(Flag flag, bool value) -> ExecuteResult
{
    if (get_flag(flag) == value) {
        return call_a16();
    }
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 3,
    };
}

auto Cpu::ret() -> ExecuteResult
{
    u16 pc = memory_->read(sp_) | memory_->read(sp_ + 1) << 8;
    sp_ += 2;

    return {
        .new_pc = pc,
        .cycles_used = 4,
    };
}

auto Cpu::ret_cc(Flag flag, bool value) -> ExecuteResult
{
    if (get_flag(flag) == value) {
        u16 pc = memory_->read(sp_) | memory_->read(sp_ + 1) << 8;
        sp_ += 2;

        return {
            .new_pc = pc,
            .cycles_used = 5,
        };
    }
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 2,
    };
}

auto Cpu::reti() -> ExecuteResult
{
    ime_ = true;
    return ret();
}

auto Cpu::rst(u8 vec) -> ExecuteResult
{
    Register16 ret = pc_ + 1;
    sp_ -= 2;
    memory_->write(sp_ + 1, ret.hi());
    memory_->write(sp_, ret.lo());
    return {
        .new_pc = vec,
        .cycles_used = 4,
    };
}

auto Cpu::add_sp_s8() -> ExecuteResult
{
    u8 value = memory_->read(pc_ + 1);
    bool sign = static_cast<bool>(value & 0x80);
    sp_.lo() = add(sp_.lo(), value);
    if (get_flag(Flag::Carry) && !sign) {
        sp_.hi() += 1;
    }
    if (!get_flag(Flag::Carry) && sign) {
        sp_.hi() -= 1;
    }
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 4,
    };
}

auto Cpu::ld_a16_sp() -> ExecuteResult
{
    u16 address = memory_->read(pc_ + 1) | memory_->read(pc_ + 2) << 8;
    memory_->write(address, sp_.lo());
    memory_->write(address + 1, sp_.hi());
    return {
        .new_pc = static_cast<u16>(pc_ + 3),
        .cycles_used = 5,
    };
}

auto Cpu::ld_sp_hl() -> ExecuteResult
{
    sp_ = hl_;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 3,
    };
}

auto Cpu::ld_hl_sp_s8() -> ExecuteResult
{
    u8 value = memory_->read(pc_ + 1);
    bool sign = static_cast<bool>(value & 0x80);
    hl_ = sp_;
    hl_.lo() = add(hl_.lo(), value);
    if (get_flag(Flag::Carry) && !sign) {
        hl_.hi() += 1;
    }
    if (!get_flag(Flag::Carry) && sign) {
        hl_.hi() -= 1;
    }
    set_flag(Flag::Zero, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 3,
    };
}

auto Cpu::push_r16(const Register16 &reg) -> ExecuteResult
{
    sp_ -= 2;
    memory_->write(sp_ + 1, reg.hi());
    memory_->write(sp_, reg.lo());
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 4,
    };
}

auto Cpu::pop_r16(Register16 &reg) -> ExecuteResult
{
    reg = memory_->read(sp_) | memory_->read(sp_ + 1) << 8;
    sp_ += 2;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 3,
    };
}

auto Cpu::ccf() -> ExecuteResult
{
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, !get_flag(Flag::Carry));
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::scf() -> ExecuteResult
{
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, true);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::ei() -> ExecuteResult
{
    ime_ = true;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::di() -> ExecuteResult
{
    ime_ = false;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::halt() -> ExecuteResult
{
    halted_ = true;
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 0,
    };
}

auto Cpu::daa() -> ExecuteResult
{
    if (get_flag(Flag::Subtraction)) {
        if (get_flag(Flag::HalfCarry)) {
            af_.hi() -= 0x06;
        }
        if (get_flag(Flag::Carry)) {
            af_.hi() -= 0x60;
        }
    }
    else {
        if (get_flag(Flag::HalfCarry) || lo_nibble(af_.hi()) > 0x9) {
            af_.hi() += 0x06;
        }
        if (get_flag(Flag::Carry) || af_.hi() > 0x99) {
            af_.hi() += 0x60;
            set_flag(Flag::Carry, true);
        }
    }
    set_flag(Flag::Zero, af_.hi() == 0);
    set_flag(Flag::HalfCarry, false);
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::nop() -> ExecuteResult
{
    return {
        .new_pc = static_cast<u16>(pc_ + 1),
        .cycles_used = 1,
    };
}

auto Cpu::stop() -> ExecuteResult
{
    halted_ = true;
    return {
        .new_pc = static_cast<u16>(pc_ + 2),
        .cycles_used = 1,
    };
}

auto Cpu::get_flag(Flag flag) const -> bool
{
    return static_cast<bool>(
        af_.lo() & 1 << static_cast<std::underlying_type_t<Flag>>(flag));
}

auto Cpu::set_flag(Flag flag, bool value) -> void
{
    const auto offset = static_cast<std::underlying_type_t<Flag>>(flag);
    af_.lo() = af_.lo() & ~(1 << offset) | static_cast<u8>(value) << offset;
}

auto Cpu::add(u8 lhs, u8 rhs) -> u8
{
    const uint result = static_cast<uint>(lhs) + static_cast<uint>(rhs);
    set_flag(Flag::Zero, (result & 0xFF) == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) + lo_nibble(rhs) > 0xF);
    set_flag(Flag::Carry, result > 0xFF);
    return static_cast<u8>(result);
}

auto Cpu::add(u16 lhs, u16 rhs) -> u16
{
    const uint result = static_cast<uint>(lhs) + static_cast<uint>(rhs);
    set_flag(Cpu::Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, (lhs & 0xFFF) + (rhs & 0xFFF) > 0xFFF);
    set_flag(Cpu::Flag::Carry, result > 0xFFFF);
    return static_cast<u8>(result);
}

auto Cpu::adc(u8 lhs, u8 rhs) -> u8
{
    const uint carry = static_cast<uint>(get_flag(Flag::Carry));
    const uint result = static_cast<uint>(lhs) + static_cast<uint>(rhs) + carry;
    set_flag(Flag::Zero, (result & 0xFF) == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) + lo_nibble(rhs) > 0xF);
    set_flag(Flag::Carry, result > 0xFF);
    return static_cast<u8>(result);
}

auto Cpu::sub(u8 lhs, u8 rhs) -> u8
{
    const u8 result = lhs - rhs;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, true);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) < lo_nibble(rhs));
    set_flag(Flag::Carry, lhs < rhs);
    return result;
}

auto Cpu::sbc(u8 lhs, u8 rhs) -> u8
{
    const u8 carry = static_cast<u8>(get_flag(Flag::Carry));
    const u8 result = lhs - rhs - carry;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, true);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) < lo_nibble(rhs) + carry);
    set_flag(Flag::Carry, lhs < rhs + carry);
    return result;
}

auto Cpu::inc(u8 lhs) -> u8
{
    const u8 result = lhs + 1;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) == 0xF);
    return result;
}

auto Cpu::dec(u8 lhs) -> u8
{
    const u8 result = lhs - 1;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, true);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) == 0);
    return result;
}

auto Cpu::bitwise_and(u8 lhs, u8 rhs) -> u8
{
    const u8 result = lhs & rhs;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, true);
    set_flag(Flag::Carry, false);
    return result;
}

auto Cpu::bitwise_xor(u8 lhs, u8 rhs) -> u8
{
    const u8 result = lhs ^ rhs;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, false);
    return result;
}

auto Cpu::bitwise_or(u8 lhs, u8 rhs) -> u8
{
    const u8 result = lhs | rhs;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, false);
    return result;
}

auto Cpu::bit(u8 bit, u8 value) -> void
{
    set_flag(Flag::Zero, static_cast<bool>(1 << bit & ~value));
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, true);
}

auto Cpu::res(u8 bit, u8 value) -> u8
{
    return value & ~(1 << bit);
}

auto Cpu::set(u8 bit, u8 value) -> u8
{
    return value | 1 << bit;
}

auto Cpu::swap(u8 lhs) -> u8
{
    const u8 result = (lhs << 4 & 0xF0) | (lhs >> 4 & 0x0F);
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, false);
    return result;
}

auto Cpu::sla(u8 lhs) -> u8
{
    const u8 result = lhs << 1;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(lhs & 0x80));
    return result;
}

auto Cpu::sra(u8 lhs) -> u8
{
    const u8 result = lhs >> 1 | (lhs & 0x80);
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(lhs & 1));
    return result;
}

auto Cpu::srl(u8 lhs) -> u8
{
    const u8 result = lhs >> 1;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(lhs & 1));
    return result;
}

auto Cpu::rl(u8 lhs) -> u8
{
    const u8 carry = static_cast<u8>(get_flag(Flag::Carry));
    const u8 result = lhs << 1 | carry;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(lhs & 0x80));
    return result;
}

auto Cpu::rlc(u8 lhs) -> u8
{
    const u8 result = lhs << 1 | (lhs & 0x80) >> 7;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(result & 1));
    return result;
}

auto Cpu::rr(u8 lhs) -> u8
{
    const u8 carry = static_cast<u8>(get_flag(Flag::Carry));
    const u8 result = lhs >> 1 | carry << 7;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(lhs & 1));
    return result;
}

auto Cpu::rrc(u8 lhs) -> u8
{
    const u8 result = lhs >> 1 | (lhs & 1) << 7;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, false);
    set_flag(Flag::HalfCarry, false);
    set_flag(Flag::Carry, static_cast<bool>(result & 0x80));
    return result;
}

auto Cpu::cp(u8 lhs, u8 rhs) -> void
{
    const u8 result = lhs - rhs;
    set_flag(Flag::Zero, result == 0);
    set_flag(Flag::Subtraction, true);
    set_flag(Flag::HalfCarry, lo_nibble(lhs) < lo_nibble(rhs));
    set_flag(Flag::Carry, lhs < rhs);
}
} // namespace tomboy
