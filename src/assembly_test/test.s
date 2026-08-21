.global _start

_start:

    li x1, 0x0F
    li x2, 0x110E8

    add x3, x2, x1

    li x15, 0xFFFFFFFF

    xori x4, x15, 64

    # Jump to the instruction at 0x110E8
    # x5 = PC + 4
    # PC = x2 + 0
    jalr x5, x2, 0

    # This should NOT execute because JALR jumps over it
    jal x1, 0x4

    ret