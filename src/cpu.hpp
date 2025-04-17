#pragma nce

#include "register.hpp"
#include "types.hpp"

#include <cmath>

namespace tomboy {
class Memory;
}

namespace tomboy {
class Cpu {
  public:
    Cpu(Memory *memory);

    auto step() -> u8;

  private:
    enum class Flag : u8 {
        Zero = 7,
        Subtraction = 6,
        HalfCarry = 5,
        Carry = 4,
    };

    struct FetchResult {
        u8 opcode;
        bool has_prefix;
    };

    struct ExecuteResult {
        u16 new_pc;
        u8 cycles_used;
    };

  private:
    [[nodiscard]] auto fetch() const -> FetchResult;
    [[nodiscard]] auto decode_execute(u8 opcode, bool has_prefix)
        -> ExecuteResult;

    // ===== Load instructions =====

    /// Load 8-bit register into a 8-bit register
    auto ld_r8_r8(Register8 &reg1, const Register8 &reg2) -> ExecuteResult;
    /// Load immediate 8-bit value into a 8-bit register
    auto ld_r8_n8(Register8 &reg) -> ExecuteResult;
    /// Load immediate 16-bit value into a 16-bit register
    auto ld_r16_n16(Register16 &reg) -> ExecuteResult;
    /// Load register A into register address
    auto ld_ra16_a(const Register16 &reg) -> ExecuteResult;
    /// Load value at register address into register A
    auto ld_a_ra16(const Register16 &reg) -> ExecuteResult;
    /// Loag 8-bit register into HL address
    auto ld_hla_r8(const Register8 &reg) -> ExecuteResult;
    /// Load value at HL address into register
    auto ld_r8_hla(Register8 &reg) -> ExecuteResult;
    /// Load register A into HL address
    auto ld_hla_n8() -> ExecuteResult;
    /// Load register A into HL address and increment HL
    auto ld_hlai_a() -> ExecuteResult;
    /// Load value at HL address into register A and increment HL
    auto ld_a_hlai() -> ExecuteResult;
    /// Load register A into HL address and decrement HL
    auto ld_hlad_a() -> ExecuteResult;
    /// Load value at HL address into register A and decrement HL
    auto ld_a_hlad() -> ExecuteResult;
    /// Load register A into 0xFF00 + immediate 8-bit address
    auto ldh_a8_a() -> ExecuteResult;
    /// Load value at 0xFF00 + immediate 8-bit address into register A
    auto ldh_a_a8() -> ExecuteResult;
    /// Load register A into 0xFF00 + C address
    auto ldh_c_a() -> ExecuteResult;
    /// Load value at 0xFF00 + C address into register A
    auto ldh_a_c() -> ExecuteResult;
    /// Load register A into immediate 16-bit address
    auto ldh_a16_a() -> ExecuteResult;
    /// Load value at immediate 16-bit address into register A
    auto ldh_a_a16() -> ExecuteResult;

    // ===== Arithmetic instructions =====

    /// Add 8-bit immediate value to accumulator
    auto add_n8() -> ExecuteResult;
    /// Add 8-bit register to accumulator
    auto add_r8(const Register8 &reg) -> ExecuteResult;
    /// Add the value at HL address to accumulator
    auto add_hla() -> ExecuteResult;
    /// Add 16-bit register to another
    auto add_hl_r16(const Register16 &reg) -> ExecuteResult;
    /// Add 8-bit immediate value to accumulator with carry
    auto adc_n8() -> ExecuteResult;
    /// Add 8-bit register to accumulator with carry
    auto adc_r8(const Register8 &reg) -> ExecuteResult;
    /// Add the value at HL address to accumulator with carry
    auto adc_hla() -> ExecuteResult;
    /// Sub 8-bit immediate value from accumulator
    auto sub_n8() -> ExecuteResult;
    /// Sub 8-bit register from accumulator
    auto sub_r8(const Register8 &reg) -> ExecuteResult;
    /// Sub the value at HL address from accumulator
    auto sub_hla() -> ExecuteResult;
    /// Sub 8-bit immediate value from accumulator with carry
    auto sbc_n8() -> ExecuteResult;
    /// Sub 8-bit register from accumulator with carry
    auto sbc_r8(const Register8 &reg) -> ExecuteResult;
    /// Sub the value at HL address from accumulator with carry
    auto sbc_hla() -> ExecuteResult;
    /// Compare immediate 8-bit value with accumulator
    auto cp_n8() -> ExecuteResult;
    /// Compare 8-bit register with accumulator
    auto cp_r8(const Register8 &reg) -> ExecuteResult;
    /// Compare value at HL address with accumulator
    auto cp_hla() -> ExecuteResult;
    /// Increment 8-bit register
    auto inc_r8(Register8 &reg) -> ExecuteResult;
    /// Increment 16-bit register
    auto inc_r16(Register16 &reg) -> ExecuteResult;
    /// Increment value at HL address
    auto inc_hla() -> ExecuteResult;
    /// Decrement 8-bit register
    auto dec_r8(Register8 &reg) -> ExecuteResult;
    /// Decrement 16-bit register
    auto dec_r16(Register16 &reg) -> ExecuteResult;
    /// Decrement value at HL address
    auto dec_hla() -> ExecuteResult;

    // ===== Bit shift instructions =====

    /// Swap 8-bit register nibbles
    auto swap_r8(Register8 &reg) -> ExecuteResult;
    /// Swap value at HL address nibbles
    auto swap_hla() -> ExecuteResult;
    /// Shift 8-bit register left arithmetically
    auto sla_r8(Register8 &reg) -> ExecuteResult;
    /// Shift value at HL address left arithmetically
    auto sla_hla() -> ExecuteResult;
    /// Shift 8-bit register right arithmetically
    auto sra_r8(Register8 &reg) -> ExecuteResult;
    /// Shift value at HL address right arithmetically
    auto sra_hla() -> ExecuteResult;
    /// Shift 8-bit register right logically
    auto srl_r8(Register8 &reg) -> ExecuteResult;
    /// Shift value at HL address right logically
    auto srl_hla() -> ExecuteResult;
    /// Rotate 8-bit register left through carry
    auto rl_r8(Register8 &reg) -> ExecuteResult;
    /// Rotate register A left through carry
    auto rl_a() -> ExecuteResult;
    /// Rotate value at HL address left through carry
    auto rl_hla() -> ExecuteResult;
    /// Rotate 8-bit register left
    auto rlc_r8(Register8 &reg) -> ExecuteResult;
    /// Rotate register A left
    auto rlc_a() -> ExecuteResult;
    /// Rotate value at HL address left
    auto rlc_hla() -> ExecuteResult;
    /// Rotate 8-bit register right through carry
    auto rr_r8(Register8 &reg) -> ExecuteResult;
    /// Rotate register A right through carry
    auto rr_a() -> ExecuteResult;
    /// Rotate value at HL address right through carry
    auto rr_hla() -> ExecuteResult;
    /// Rotate 8-bit register right
    auto rrc_r8(Register8 &reg) -> ExecuteResult;
    /// Rotate register A right
    auto rrc_a() -> ExecuteResult;
    /// Rotate value at HL address right
    auto rrc_hla() -> ExecuteResult;

    // ===== Bitwise instructions =====

    /// And 8-bit immediate value accumulator
    auto and_n8() -> ExecuteResult;
    /// And 8-bit register with accumulator
    auto and_r8(const Register8 &reg) -> ExecuteResult;
    /// And value at HL address with accumulator
    auto and_hla() -> ExecuteResult;
    /// Or 8-bit immediate value with accumulator
    auto or_n8() -> ExecuteResult;
    /// Or 8-bit register with accumulator
    auto or_r8(const Register8 &reg) -> ExecuteResult;
    /// Or value at HL address with accumulator
    auto or_hla() -> ExecuteResult;
    /// Xor 8-bit immediate value with accumulator
    auto xor_n8() -> ExecuteResult;
    /// Xor 8-bit register with accumulator
    auto xor_r8(const Register8 &reg) -> ExecuteResult;
    /// Xor value at HL address with accumulator
    auto xor_hla() -> ExecuteResult;
    /// Complement register A
    auto cpl() -> ExecuteResult;

    // ===== Bit flag instructions ====

    /// Test bit in 8-bit register
    auto bit_r8(u8 bit, const Register8 &reg) -> ExecuteResult;
    /// Test bit in value at HL address
    auto bit_hla(u8 bit) -> ExecuteResult;
    /// Reset bit in 8-bit register
    auto res_r8(u8 bit, Register8 &reg) -> ExecuteResult;
    /// Reset bit in value at HL address
    auto res_hla(u8 bit) -> ExecuteResult;
    /// Set bit in 8-bit register
    auto set_r8(u8 bit, Register8 &reg) -> ExecuteResult;
    /// Set bit in value at HL address
    auto set_hla(u8 bit) -> ExecuteResult;

    // ===== Jump and subroutine instructions =====

    /// Jump to immediate 16-bit address
    auto jp_a16() -> ExecuteResult;
    /// Jump to immediate 16-bit address if condition
    auto jp_cc_a16(Flag flag, bool value) -> ExecuteResult;
    /// Jump to HL address
    auto jp_hl() -> ExecuteResult;
    /// Relative jump to immediate 8-bit signed offset
    auto jr_s8() -> ExecuteResult;
    /// Relative jump to immediate 8-bit signed offset if condition
    auto jr_cc_s8(Flag flag, bool value) -> ExecuteResult;
    /// Call immediate 16-bit address
    auto call_a16() -> ExecuteResult;
    /// Call immediate 16-bit address if condition
    auto call_cc_a16(Flag flag, bool value) -> ExecuteResult;
    /// Return
    auto ret() -> ExecuteResult;
    /// Return if condition
    auto ret_cc(Flag flag, bool value) -> ExecuteResult;
    /// Return and enable interrupts
    auto reti() -> ExecuteResult;
    /// Call address vec
    auto rst(u8 vec) -> ExecuteResult;

    // ===== Stack instructions =====

    /// Add immediate 8-bit signed value to SP
    auto add_sp_s8() -> ExecuteResult;
    /// Load SP into memory address pointed to by 16-bit immediate value
    auto ld_a16_sp() -> ExecuteResult;
    /// Load HL into SP
    auto ld_sp_hl() -> ExecuteResult;
    /// Load SP + immediate 8-bit signed value into HL
    auto ld_hl_sp_s8() -> ExecuteResult;
    /// Push 16-bit register onto stack
    auto push_r16(const Register16 &reg) -> ExecuteResult;
    /// Pop stack into 16-bit register
    auto pop_r16(Register16 &reg) -> ExecuteResult;

    // ===== Carry flag instructions =====

    /// Complement the carry flag
    auto ccf() -> ExecuteResult;
    /// Set the carry flag
    auto scf() -> ExecuteResult;

    // ===== Interrupt instructions =====

    /// Enable interrupts
    auto ei() -> ExecuteResult;
    /// Disable interrupts
    auto di() -> ExecuteResult;
    /// Halt
    auto halt() -> ExecuteResult;

    // ===== Misc instructions =====

    /// Decimal adjust accumulator
    auto daa() -> ExecuteResult;
    /// No operation
    auto nop() -> ExecuteResult;
    /// Stop
    auto stop() -> ExecuteResult;

    // ===== Flag operations =====

    [[nodiscard]] auto get_flag(Flag flag) const -> bool;
    auto set_flag(Flag flag, bool value) -> void;

    // ===== Shared operations =====

    /// Add two 8-bit values
    [[nodiscard]] auto add(u8 lhs, u8 rhs) -> u8;
    /// Add two 16-bit values
    [[nodiscard]] auto add(u16 lhs, u16 rhs) -> u16;
    /// Add two 8-bit values with carry
    [[nodiscard]] auto adc(u8 lhs, u8 rhs) -> u8;
    /// Subtract two 8-bit values
    [[nodiscard]] auto sub(u8 lhs, u8 rhs) -> u8;
    /// Subtract two 8-bit values with carry
    [[nodiscard]] auto sbc(u8 lhs, u8 rhs) -> u8;
    /// Increment 8-bit value
    [[nodiscard]] auto inc(u8 lhs) -> u8;
    /// Increment 8-bit value
    [[nodiscard]] auto dec(u8 lhs) -> u8;
    /// Bitwise and two 8-bit values
    [[nodiscard]] auto bitwise_and(u8 lhs, u8 rhs) -> u8;
    /// Bitwise xor two 8-bit values
    [[nodiscard]] auto bitwise_xor(u8 lhs, u8 rhs) -> u8;
    /// Bitwise or two 8-bit values
    [[nodiscard]] auto bitwise_or(u8 lhs, u8 rhs) -> u8;
    /// Test bit
    auto bit(u8 bit, u8 value) -> void;
    /// Reset bit
    [[nodiscard]] static auto res(u8 bit, u8 value) -> u8;
    /// Set bit
    [[nodiscard]] static auto set(u8 bit, u8 value) -> u8;
    /// Swap nibbles
    [[nodiscard]] auto swap(u8 lhs) -> u8;
    /// Shift left arithmetically
    [[nodiscard]] auto sla(u8 lhs) -> u8;
    /// Shift right arithmetically
    [[nodiscard]] auto sra(u8 lhs) -> u8;
    /// Shift right logically
    [[nodiscard]] auto srl(u8 lhs) -> u8;
    /// Rotate left through carry bit
    [[nodiscard]] auto rl(u8 lhs) -> u8;
    /// Rotate left
    [[nodiscard]] auto rlc(u8 lhs) -> u8;
    /// Rotate right through carry bit
    [[nodiscard]] auto rr(u8 lhs) -> u8;
    /// Rotate right
    [[nodiscard]] auto rrc(u8 lhs) -> u8;
    /// Compare two 8-bit values
    auto cp(u8 lhs, u8 rhs) -> void;

  private:
    Register16 af_;
    Register16 bc_;
    Register16 de_;
    Register16 hl_;
    Register16 sp_;
    Register16 pc_;
    bool halted_;
    bool ime_;
    Memory *memory_;
};
} // namespace tomboy
