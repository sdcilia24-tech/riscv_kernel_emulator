.global _start

    _start:
        ADDI x1, x1, 0x01 # starting point 1 for easy starting point 
        ADDI x2, x2, 0x01
        ADDI x30, x30, 0x0B
        ADDI x31, x31, 1

        loop:
            ADD x3, x2, x1

            ADD x1, x2, x0

            ADD x2, x3, x0

            BNE x30, x31, loop
            ADDI x31, x31, 1
        ADDI x5, x5, 0x08
