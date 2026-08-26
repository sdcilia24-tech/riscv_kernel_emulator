#include <cpu/cpu.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/*
A file defining the disassembler which will print the given assembly instruction to 
the terminal upon calling the function.
*/
void disassemble(uint32_t ins, cpu_t *cpu)
{
    uint32_t opcode = (ins & 0x07F); 
    char buffer[256] = {0};

    switch (opcode)
    {
        default:
            char code[] = "Unkown opcode";
            strcat_s(buffer, sizeof(code), code);
            break;
        case R_TYPE:
            uint32_t funct3 = (ins >> 12) & 0x07;
            uint32_t funct7 = (ins >> 25) & 0x07F;

            uint32_t rd = (ins >> 7) & 0x01F;
            uint32_t rs1 = (ins >> 15) & 0x01F;
            uint32_t rs2 = (ins >> 20) & 0x01F;
            switch (funct3)
            {
                default:
                    snprintf(buffer, sizeof(buffer), "Unknown");
                    break;
                case 0x0:

                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "ADD x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);
                    }
                    else if (funct7 == 0x20)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SUB x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);
                    }
                    else
                    {
                        snprintf(buffer, sizeof(buffer), "Unknown");
                    }
                    break;
                case 0x4:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "XOR x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);

                    }
                    else 
                    {
                        snprintf(buffer, sizeof(buffer), "Unknown");
                    }
                    break;
                case 0x6:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "OR x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);
                    }
                    else 
                    {
                        char str[] = "unknown";
                        strcat_s(buffer, sizeof(buffer), str);
                    }
                    break;
                case 0x7:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "AND x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);
                    }
                    else 
                    {
                        snprintf(buffer, sizeof(buffer), "Unknown");
                    }
                    break;
                case 0x1:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SLL x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);            
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "Unknown");
                    }
                    break;
                case 0x5:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SRL x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);                      
                    }
                    else if (funct7 == 0x20)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SRA x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);                    
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "Unknown");                       
                    }
                    break;
                case 0x2:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SLT x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);                      
                    }
                    else
                    {
                        snprintf(buffer, sizeof(buffer), "Unknown");  
                    }
                    break;
                case 0x3:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SLTU x%ld, x%ld, x%ld", (long)rd, (long)rs1, (long)rs2);  
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "Unknown");                  
                    }
            }
            break;
        case I_TYPE:
            funct3 = (ins >> 12) & 0x07;
            rd = (ins >> 7) & 0x01F;
            rs1 = (ins >> 15) & 0x01F;
            int32_t imm = extend_n_bit_sign(ins >> 20, 12);
            funct7 = (ins >> 25) & 0x07F;
            int32_t low_bits = imm & 0x01F;
        
            switch (funct3)
            {
                default:
                    snprintf(buffer, sizeof(buffer), "Unknown");
                    break;
                case 0x0:
                    snprintf(buffer, sizeof(buffer), 
                    "ADDI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm); 
                    break;
                case 0x4:
                    snprintf(buffer, sizeof(buffer), 
                    "XORI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm);
                    break;
                case 0x6:
                    snprintf(buffer, sizeof(buffer), 
                    "ORI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm);
                    break;
                case 0x7:
                    snprintf(buffer, sizeof(buffer), 
                    "ANDI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm);
                    break;
                case 0x1:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SLLI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)low_bits);
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "Unknown");                    
                    }
                    break;
                case 0x5:
                    if (funct7 == 0x0)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SRLI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)low_bits);
                    }
                    else if (funct7 == 0x20)
                    {
                        snprintf(buffer, sizeof(buffer), 
                        "SRAI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)low_bits);                      
                    }
                    else {
                        snprintf(buffer, sizeof(buffer), "Unknown");                     
                    }
                    break;
                case 0x2:
                        snprintf(buffer, sizeof(buffer), 
                        "SLTI x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm);
                        break;
                case 0x3:
                        snprintf(buffer, sizeof(buffer), 
                        "SLTIU x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)imm);
                        break;                                                         
            }
            break;
        case LOAD_MEMORY_TYPE:
            funct3 = (ins >> 12) & 0x07;
            rd = (ins >> 7) & 0x01F;
            rs1 = (ins >> 15) & 0x01F;
            imm = extend_n_bit_sign(ins >> 20, 12);
            uint32_t rs1_unsigned_val = cpu -> registers[rs1];

            funct7 = (ins >> 25) & 0x07F;
            switch (funct3)
            {
                default:
                        snprintf(buffer, sizeof(buffer), "Unknown");
                        break;                   
                case 0x0:
                        snprintf(buffer, sizeof(buffer), 
                        "LB x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break;
                case 0x1:
                        snprintf(buffer, sizeof(buffer), 
                        "LH x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break;     
                case 0x2:
                        snprintf(buffer, sizeof(buffer), 
                        "LW x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break; 
                case 0x4:
                        snprintf(buffer, sizeof(buffer), 
                        "LBU x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break; 
                case 0x5:
                        snprintf(buffer, sizeof(buffer), 
                        "LHU x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break; 
            }
            break;
        case STORE_MEMORY_TYPE:
            uint32_t low = (ins >> 7) & 0x01F;
            funct3 = (ins >> 12) & 0x07;
            rs1 = (ins >> 15) & 0x01F;
            rs2 = (ins >> 20) & 0x01F;
            uint32_t high = ((ins >> 25) & 0x07F) << 5;
            imm = extend_n_bit_sign(high | low, 12);
            rs1_unsigned_val = cpu -> registers[rs1];

            switch (funct3)
            {
                default:
                    snprintf(buffer, sizeof(buffer), "Unknown");
                    break;
                case 0x0:
                        snprintf(buffer, sizeof(buffer), 
                        "SB x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));
                        break;
                case 0x1:
                        snprintf(buffer, sizeof(buffer), 
                        "SH x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));;
                        break;
                case 0x2:
                        snprintf(buffer, sizeof(buffer), 
                        "SW x%ld, %ld(%ld)", (long)rs2, (long)(imm), (long)(rs1));;
                        break;                  
            }
            break;

            case B_TYPE:
                funct3 = (ins >> 12) & 0x07;
                rs1 = (ins >> 15) & 0x01F;
                rs2 = (ins >> 20) & 0x01F;
                uint32_t twelve_bit = ((ins >> 31) & 0x01) << 12;
                uint32_t ten_to_five = ((ins >> 25) & 0x03F) << 5;
                uint32_t four_to_one = ((ins >> 8) & 0x0F) << 1;
                uint32_t eleven_bit = ((ins >> 7) & 0x01) << 11;

                imm = extend_n_bit_sign(twelve_bit | eleven_bit | ten_to_five | four_to_one, 13);
                switch (funct3)
                {
                    default:
                        snprintf(buffer, sizeof(buffer), "Unknown");
                        break;
                    case 0x0:
                            snprintf(buffer, sizeof(buffer), 
                            "BEQ x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;     
                    case 0x1:
                            snprintf(buffer, sizeof(buffer), 
                            "BNE x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;   
                    case 0x4:
                            snprintf(buffer, sizeof(buffer), 
                            "BLT x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;
                    case 0x5:
                            snprintf(buffer, sizeof(buffer), 
                            "BGE x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;
                    case 0x6:
                            snprintf(buffer, sizeof(buffer), 
                            "BLTU x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;
                    case 0x7:
                            snprintf(buffer, sizeof(buffer), 
                            "BGEU x%ld, x%ld, %ld", (long)rs1, (long)rs2, (long)(imm));
                            break;  
                }
                break;    
            case JUMP_AND_LINK_TYPE:
                rd = (ins >> 7) & 0x1F;
                uint32_t twentieth = ((ins >> 30) & 0x01) << 20;
                uint32_t ten_to_one = ((ins >> 21) & 0x03FF) << 1;
                uint32_t eleven = ((ins >> 20) & 0x01) << 11;
                uint32_t nineteen_to_twelve = ((ins >> 12) & 0x0FF) << 12;

                extend_n_bit_sign(twentieth | eleven | nineteen_to_twelve | ten_to_one, 21);
                snprintf(buffer, sizeof(buffer), 
                "JAL x%ld, %ld", (long)rd, (long)(imm));
                break;
            case JUMP_AND_LINK_REG_TYPE:
                funct3 = (ins >> 12) & 0x07;
                rd = (ins >> 7) & 0x01F;
                rs1 = (ins >> 15) & 0x01F;
                imm = extend_n_bit_sign(ins >> 20, 12);
                funct7 = (ins >> 25) & 0x07F;
                if (funct3 == 0x0)
                {
                snprintf(buffer, sizeof(buffer), 
                "JALR x%ld, x%ld, %ld", (long)rd, (long)rs1, (long)(imm)); 
                break;
                }
                else{
                    snprintf(buffer, sizeof(buffer), "Unknown");
                }
            case U_TYPE_NO_PC_INC:
                imm = (ins >> 12) & 0xFFFFF;
                rd = (ins >> 7) & 0x01F;
                snprintf(buffer, sizeof(buffer), 
                "LUI x%ld, %ld", (long)rd, (long)(imm));
                break;
            case U_TYPE_W_PC_INC:
                imm = (ins >> 12) & 0xFFFFF;
                rd = (ins >> 7) & 0x01F;
                snprintf(buffer, sizeof(buffer), 
                "AUIPC x%ld, %ld", (long)rd, (long)(imm));
                break;
    }
    printf("%s\n", buffer);
}