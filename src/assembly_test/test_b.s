.global _start

_start:

    ADDI x1, x1, 0x014
    ADDI x2, x2, 0x019

    BNE x1, x2, 0x08

    ADDI x3, x3, 0x01

    ADDI x4, x4, 0x05

    ADDI x5, x5, 0x05

    BEQ x5, x4, 0x08 

    ADDI x6, x6, 0x0F

    ADDI x7, x7, 0x07 

    BGE x1, x7, 0x08

    ADDI x8, x8, 0x09

    ADDI x9, x9, 0x09

    BLT x1, x6, 0x08

    ADDI x10, x10, 0x0B
    
    loop:
    ADDI x11, x11, 1
    BNE x11, x4, -0x04

    ADDI x12, x12, 0x01

    RET