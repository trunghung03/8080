#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct ConditionCodes {
    uint8_t     z:1;
    uint8_t     s:1;
    uint8_t     p:1;
    uint8_t     cy:1;
    uint8_t     ac:1; //TODO implement AC flag
    uint8_t     pad:3;
} ConditionCodes;

typedef struct State8080 {
    uint8_t     a;
    uint8_t     b;
    uint8_t     c;
    uint8_t     d;
    uint8_t     e;
    uint8_t     h;
    uint8_t     l;
    uint16_t    sp;
    uint16_t    pc;
    uint8_t     *memory;
    struct      ConditionCodes      cc;
    uint8_t     int_enable;
} State8080;

void UnimplementedInstruction(State8080* state);
int disassemble8080Op(unsigned char *codebuffer, int pc);
void Emulate8080Op(State8080* state);
int parity(uint16_t n);

void UnimplementedInstruction(State8080* state) {
    printf("Error: Unimplemented Instruction: %02x", state->memory[state->pc]);
    exit(1);
}

int disassemble8080Op(unsigned char *codebuffer, int pc) {
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04x\t", pc);
    switch(*code) {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI      B, #%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x02: printf("STAX     B"); break;
        case 0x03: printf("INX      B"); break;
        case 0x04: printf("INR      B"); break;
        case 0x05: printf("DCR      B"); break;
        case 0x06: printf("MVI      B, #%02x", code[1]); opbytes=2; break;
        case 0x07: printf("RLC"); break;
        case 0x08: printf("-"); break;
        case 0x09: printf("DAD      B"); break;
        case 0x0a: printf("LDAX     B"); break;
        case 0x0b: printf("DCX      B"); break;
        case 0x0c: printf("INR      C"); break;
        case 0x0d: printf("DCR      C"); break;
        case 0x0e: printf("MVI      C, #%02x", code[1]); opbytes=2; break;
        case 0x0f: printf("RRC"); break;

        case 0x10: printf("-"); break;
        case 0x11: printf("LXI      D, #%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x12: printf("STAX     D"); break;
        case 0x13: printf("INX      D"); break;
        case 0x14: printf("INR      D"); break;
        case 0x15: printf("DCR      D"); break;
        case 0x16: printf("MVI      D, #%02x", code[1]); opbytes=2; break;
        case 0x17: printf("RAL"); break;
        case 0x18: printf("-"); break;
        case 0x19: printf("DAD      D"); break;
        case 0x1a: printf("LDAX     D"); break;
        case 0x1b: printf("DCX      D"); break;
        case 0x1c: printf("INR      E"); break;
        case 0x1d: printf("DCR      E"); break;
        case 0x1e: printf("MVI      E, #%02x", code[1]); opbytes=2; break;
        case 0x1f: printf("RAR"); break;

        case 0x20: printf("-"); break;
        case 0x21: printf("LXI      H, #%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x22: printf("SHLD     $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x23: printf("INX      H"); break;
        case 0x24: printf("INR      H"); break;
        case 0x25: printf("DCR      H"); break;
        case 0x26: printf("MVI      H, #%02x", code[1]); opbytes=2; break;
        case 0x27: printf("DAA"); break;
        case 0x28: printf("-"); break;
        case 0x29: printf("DAD      H"); break;
        case 0x2a: printf("LHLD     $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x2b: printf("DCX      H"); break;
        case 0x2c: printf("INR      L"); break;
        case 0x2d: printf("DCR      L"); break;
        case 0x2e: printf("MVI      L, #%02x", code[1]); opbytes=2; break;
        case 0x2f: printf("CMA"); break;
        
        case 0x30: printf("-"); break;
        case 0x31: printf("LXI      SP, #%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x32: printf("STA      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x33: printf("INX      SP"); break;
        case 0x34: printf("INR      M"); break;
        case 0x35: printf("DCR      M"); break;
        case 0x36: printf("MVI      M, #%02x", code[1]); opbytes=2; break;
        case 0x37: printf("STC"); break;
        case 0x38: printf("-"); break;
        case 0x39: printf("DAD      SP"); break;
        case 0x3a: printf("LDA      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0x3b: printf("DCX      SP"); break;
        case 0x3c: printf("INR      A"); break;
        case 0x3d: printf("DCR      A"); break;
        case 0x3e: printf("MVI      A, #%02x", code[1]); opbytes=2; break;
        case 0x3f: printf("CMC"); break;
        
        case 0x40: printf("MOV      B, B"); break;
        case 0x41: printf("MOV      B, C"); break;
        case 0x42: printf("MOV      B, D"); break;
        case 0x43: printf("MOV      B, E"); break;
        case 0x44: printf("MOV      B, H"); break;
        case 0x45: printf("MOV      B, L"); break;
        case 0x46: printf("MOV      B, M"); break;
        case 0x47: printf("MOV      B, A"); break;
        case 0x48: printf("MOV      C, B"); break;
        case 0x49: printf("MOV      C, C"); break;
        case 0x4a: printf("MOV      C, D"); break;
        case 0x4b: printf("MOV      C, E"); break;
        case 0x4c: printf("MOV      C, H"); break;
        case 0x4d: printf("MOV      C, L"); break;
        case 0x4e: printf("MOV      C, M"); break;
        case 0x4f: printf("MOV      C, A"); break;

        case 0x50: printf("MOV      D, B"); break;
        case 0x51: printf("MOV      D, C"); break;
        case 0x52: printf("MOV      D, D"); break;
        case 0x53: printf("MOV      D, E"); break;
        case 0x54: printf("MOV      D, H"); break;
        case 0x55: printf("MOV      D, L"); break;
        case 0x56: printf("MOV      D, M"); break;
        case 0x57: printf("MOV      D, A"); break;
        case 0x58: printf("MOV      E, B"); break;
        case 0x59: printf("MOV      E, C"); break;
        case 0x5a: printf("MOV      E, D"); break;
        case 0x5b: printf("MOV      E, E"); break;
        case 0x5c: printf("MOV      E, H"); break;
        case 0x5d: printf("MOV      E, L"); break;
        case 0x5e: printf("MOV      E, M"); break;
        case 0x5f: printf("MOV      E, A"); break;

        case 0x60: printf("MOV      H, B"); break;
        case 0x61: printf("MOV      H, C"); break;
        case 0x62: printf("MOV      H, D"); break;
        case 0x63: printf("MOV      H, E"); break;
        case 0x64: printf("MOV      H, H"); break;
        case 0x65: printf("MOV      H, L"); break;
        case 0x66: printf("MOV      H, M"); break;
        case 0x67: printf("MOV      H, A"); break;
        case 0x68: printf("MOV      L, B"); break;
        case 0x69: printf("MOV      L, C"); break;
        case 0x6a: printf("MOV      L, D"); break;
        case 0x6b: printf("MOV      L, E"); break;
        case 0x6c: printf("MOV      L, H"); break;
        case 0x6d: printf("MOV      L, L"); break;
        case 0x6e: printf("MOV      L, M"); break;
        case 0x6f: printf("MOV      L, A"); break;

        case 0x70: printf("MOV      M, B"); break;
        case 0x71: printf("MOV      M, C"); break;
        case 0x72: printf("MOV      M, D"); break;
        case 0x73: printf("MOV      M, E"); break;
        case 0x74: printf("MOV      M, H"); break;
        case 0x75: printf("MOV      M, L"); break;
        case 0x76: printf("HLT"); break;
        case 0x77: printf("MOV      M, A"); break;
        case 0x78: printf("MOV      A, B"); break;
        case 0x79: printf("MOV      A, C"); break;
        case 0x7a: printf("MOV      A, D"); break;
        case 0x7b: printf("MOV      A, E"); break;
        case 0x7c: printf("MOV      A, H"); break;
        case 0x7d: printf("MOV      A, L"); break;
        case 0x7e: printf("MOV      A, M"); break;
        case 0x7f: printf("MOV      A, A"); break;

        case 0x80: printf("ADD      B"); break;
        case 0x81: printf("ADD      C"); break;
        case 0x82: printf("ADD      D"); break;
        case 0x83: printf("ADD      E"); break;
        case 0x84: printf("ADD      H"); break;
        case 0x85: printf("ADD      L"); break;
        case 0x86: printf("ADD      M"); break;
        case 0x87: printf("ADD      A"); break;
        case 0x88: printf("ADC      B"); break;
        case 0x89: printf("ADC      C"); break;
        case 0x8a: printf("ADC      D"); break;
        case 0x8b: printf("ADC      E"); break;
        case 0x8c: printf("ADC      H"); break;
        case 0x8d: printf("ADC      L"); break;
        case 0x8e: printf("ADC      M"); break;
        case 0x8f: printf("ADC      A"); break;

        case 0x90: printf("SUB      B"); break;
        case 0x91: printf("SUB      C"); break;
        case 0x92: printf("SUB      D"); break;
        case 0x93: printf("SUB      E"); break;
        case 0x94: printf("SUB      H"); break;
        case 0x95: printf("SUB      L"); break;
        case 0x96: printf("SUB      M"); break;
        case 0x97: printf("SUB      A"); break;
        case 0x98: printf("SBB      B"); break;
        case 0x99: printf("SBB      C"); break;
        case 0x9a: printf("SBB      D"); break;
        case 0x9b: printf("SBB      E"); break;
        case 0x9c: printf("SBB      H"); break;
        case 0x9d: printf("SBB      L"); break;
        case 0x9e: printf("SBB      M"); break;
        case 0x9f: printf("SBB      A"); break;

        case 0xa0: printf("ANA      B"); break;
        case 0xa1: printf("ANA      C"); break;
        case 0xa2: printf("ANA      D"); break;
        case 0xa3: printf("ANA      E"); break;
        case 0xa4: printf("ANA      H"); break;
        case 0xa5: printf("ANA      L"); break;
        case 0xa6: printf("ANA      M"); break;
        case 0xa7: printf("ANA      A"); break;
        case 0xa8: printf("XRA      B"); break;
        case 0xa9: printf("XRA      C"); break;
        case 0xaa: printf("XRA      D"); break;
        case 0xab: printf("XRA      E"); break;
        case 0xac: printf("XRA      H"); break;
        case 0xad: printf("XRA      L"); break;
        case 0xae: printf("XRA      M"); break;
        case 0xaf: printf("XRA      A"); break;
        
        case 0xb0: printf("ORA      B"); break;
        case 0xb1: printf("ORA      C"); break;
        case 0xb2: printf("ORA      D"); break;
        case 0xb3: printf("ORA      E"); break;
        case 0xb4: printf("ORA      H"); break;
        case 0xb5: printf("ORA      L"); break;
        case 0xb6: printf("ORA      M"); break;
        case 0xb7: printf("ORA      A"); break;
        case 0xb8: printf("CMP      B"); break;
        case 0xb9: printf("CMP      C"); break;
        case 0xba: printf("CMP      D"); break;
        case 0xbb: printf("CMP      E"); break;
        case 0xbc: printf("CMP      H"); break;
        case 0xbd: printf("CMP      L"); break;
        case 0xbe: printf("CMP      M"); break;
        case 0xbf: printf("CMP      A"); break;
        
        case 0xc0: printf("RNZ"); break;
        case 0xc1: printf("POP      B"); break;
        case 0xc2: printf("JNZ      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xc3: printf("JMP      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xc4: printf("CNZ      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xc5: printf("PUSH     B"); break;
        case 0xc6: printf("ADI      #%02x", code[1]); opbytes=2; break;
        case 0xc7: printf("RST      0"); break;
        case 0xc8: printf("RZ"); break;
        case 0xc9: printf("RET"); break;
        case 0xca: printf("JZ       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xcb: printf("-"); break;
        case 0xcc: printf("CZ       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xcd: printf("CALL     $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xce: printf("ACI      #%02x", code[1]); opbytes=2; break;
        case 0xcf: printf("RST      1"); break;
        
        case 0xd0: printf("RNC"); break;
        case 0xd1: printf("POP      D"); break;
        case 0xd2: printf("JNC      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xd3: printf("OUT      #%02x", code[1]); opbytes=2; break;
        case 0xd4: printf("CNC      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xd5: printf("PUSH     D"); break;
        case 0xd6: printf("SUI      #%02x", code[1]); opbytes=2; break;
        case 0xd7: printf("RST      2"); break;
        case 0xd8: printf("RC"); break;
        case 0xd9: printf("-"); break;
        case 0xda: printf("JC       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xdb: printf("IN       #%02x", code[1]); opbytes=2; break;
        case 0xdc: printf("CC       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xdd: printf("-"); break;
        case 0xde: printf("SBI      #%02x", code[1]); opbytes=2; break;
        case 0xdf: printf("RST      3"); break;
        
        case 0xe0: printf("RPO"); break;
        case 0xe1: printf("POP      H"); break;
        case 0xe2: printf("JPO      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xe3: printf("XTHL"); break;
        case 0xe4: printf("CPO      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xe5: printf("PUSH     H"); break;
        case 0xe6: printf("ANI      #%02x", code[1]); opbytes=2; break;
        case 0xe7: printf("RST      4"); break;
        case 0xe8: printf("RPE"); break;
        case 0xe9: printf("PCHL"); break;
        case 0xea: printf("JPE      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xeb: printf("XCHG"); break;
        case 0xec: printf("CPE      $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xed: printf("-"); break;
        case 0xee: printf("XRI      #%02x", code[1]); opbytes=2; break;
        case 0xef: printf("RST      5"); break;
        
        case 0xf0: printf("RP"); break;
        case 0xf1: printf("POP      PSW"); break;
        case 0xf2: printf("JP       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xf3: printf("DI"); break;
        case 0xf4: printf("CP       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xf5: printf("PUSH     PSW"); break;
        case 0xf6: printf("ORI      #%02x", code[1]); opbytes=2; break;
        case 0xf7: printf("RST      6"); break;
        case 0xf8: printf("RM"); break;
        case 0xf9: printf("SPHL"); break;
        case 0xfa: printf("JM       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xfb: printf("EI"); break;
        case 0xfc: printf("CM       $%02x%02x", code[2], code[1]); opbytes=3; break;
        case 0xfd: printf("-"); break;
        case 0xfe: printf("CPI      #%02x", code[1]); opbytes=2; break;
        case 0xff: printf("RST      7"); break;

        default: puts("Unknown instruction found, please report back to developer.\n"); break;
    }
    putchar('\n');

    return opbytes;
}

void Emulate8080Op(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];
    bool skipPcInc = false;

    disassemble8080Op(state->memory, state->pc);
    switch(*opcode) {
        //Data Transfer Group
        //MOV r1, r2 (Generated with .py script included)
        case 0x40: //MOV B, B
        {
            state->b = state->b;
            break;
        }
        case 0x41: //MOV B, C
        {
            state->b = state->c;
            break;
        }
        case 0x42: //MOV B, D
        {
            state->b = state->d;
            break;
        }
        case 0x43: //MOV B, E
        {
            state->b = state->e;
            break;
        }
        case 0x44: //MOV B, H
        {
            state->b = state->h;
            break;
        }
        case 0x45: //MOV B, L
        {
            state->b = state->l;
            break;
        }
        case 0x47: //MOV B, A
        {
            state->b = state->a;
            break;
        }
        case 0x48: //MOV C, B
        {
            state->c = state->b;
            break;
        }
        case 0x49: //MOV C, C
        {
            state->c = state->c;
            break;
        }
        case 0x4a: //MOV C, D
        {
            state->c = state->d;
            break;
        }
        case 0x4b: //MOV C, E
        {
            state->c = state->e;
            break;
        }
        case 0x4c: //MOV C, H
        {
            state->c = state->h;
            break;
        }
        case 0x4d: //MOV C, L
        {
            state->c = state->l;
            break;
        }
        case 0x4f: //MOV C, A
        {
            state->c = state->a;
            break;
        }
        case 0x50: //MOV D, B
        {
            state->d = state->b;
            break;
        }
        case 0x51: //MOV D, C
        {
            state->d = state->c;
            break;
        }
        case 0x52: //MOV D, D
        {
            state->d = state->d;
            break;
        }
        case 0x53: //MOV D, E
        {
            state->d = state->e;
            break;
        }
        case 0x54: //MOV D, H
        {
            state->d = state->h;
            break;
        }
        case 0x55: //MOV D, L
        {
            state->d = state->l;
            break;
        }
        case 0x57: //MOV D, A
        {
            state->d = state->a;
            break;
        }
        case 0x58: //MOV E, B
        {
            state->e = state->b;
            break;
        }
        case 0x59: //MOV E, C
        {
            state->e = state->c;
            break;
        }
        case 0x5a: //MOV E, D
        {
            state->e = state->d;
            break;
        }
        case 0x5b: //MOV E, E
        {
            state->e = state->e;
            break;
        }
        case 0x5c: //MOV E, H
        {
            state->e = state->h;
            break;
        }
        case 0x5d: //MOV E, L
        {
            state->e = state->l;
            break;
        }
        case 0x5f: //MOV E, A
        {
            state->e = state->a;
            break;
        }
        case 0x60: //MOV H, B
        {
            state->h = state->b;
            break;
        }
        case 0x61: //MOV H, C
        {
            state->h = state->c;
            break;
        }
        case 0x62: //MOV H, D
        {
            state->h = state->d;
            break;
        }
        case 0x63: //MOV H, E
        {
            state->h = state->e;
            break;
        }
        case 0x64: //MOV H, H
        {
            state->h = state->h;
            break;
        }
        case 0x65: //MOV H, L
        {
            state->h = state->l;
            break;
        }
        case 0x67: //MOV H, A
        {
            state->h = state->a;
            break;
        }
        case 0x68: //MOV L, B
        {
            state->l = state->b;
            break;
        }
        case 0x69: //MOV L, C
        {
            state->l = state->c;
            break;
        }
        case 0x6a: //MOV L, D
        {
            state->l = state->d;
            break;
        }
        case 0x6b: //MOV L, E
        {
            state->l = state->e;
            break;
        }
        case 0x6c: //MOV L, H
        {
            state->l = state->h;
            break;
        }
        case 0x6d: //MOV L, L
        {
            state->l = state->l;
            break;
        }
        case 0x6f: //MOV L, A
        {
            state->l = state->a;
            break;
        }
        case 0x78: //MOV A, B
        {
            state->a = state->b;
            break;
        }
        case 0x79: //MOV A, C
        {
            state->a = state->c;
            break;
        }
        case 0x7a: //MOV A, D
        {
            state->a = state->d;
            break;
        }
        case 0x7b: //MOV A, E
        {
            state->a = state->e;
            break;
        }
        case 0x7c: //MOV A, H
        {
            state->a = state->h;
            break;
        }
        case 0x7d: //MOV A, L
        {
            state->a = state->l;
            break;
        }
        case 0x7f: //MOV A, A
        {
            state->a = state->a;
            break;
        }

        //MOV r, M
        case 0x46: //MOV B, M
        {
            state->b = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x4e: //MOV C, M
        {
            state->c = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x56: //MOV D, M
        {
            state->d = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x5e: //MOV E, M
        {
            state->e = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x66: //MOV H, M
        {
            state->h = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x6e: //MOV L, M
        {
            state->l = state->memory[(state->h<<8) | state->l];
            break;
        }
        case 0x7e: //MOV A, M
        {
            state->a = state->memory[(state->h<<8) | state->l];
            break;
        }

        //MOV M, r
        case 0x70: //MOV M, B
        {
            state->memory[(state->h<<8) | state->l] = state->b;
            break;
        }
        case 0x71: //MOV M, C
        {
            state->memory[(state->h<<8) | state->l] = state->c;
            break;
        }
        case 0x72: //MOV M, D
        {
            state->memory[(state->h<<8) | state->l] = state->d;
            break;
        }
        case 0x73: //MOV M, E
        {
            state->memory[(state->h<<8) | state->l] = state->e;
            break;
        }
        case 0x74: //MOV M, H
        {
            state->memory[(state->h<<8) | state->l] = state->h;
            break;
        }
        case 0x75: //MOV M, L
        {
            state->memory[(state->h<<8) | state->l] = state->l;
            break;
        }
        case 0x77: //MOV M, A
        {
            state->memory[(state->h<<8) | state->l] = state->a;
            break;
        }

        //MVI r, data
        case 0x06: //MVI B, data
        {
            state->b = opcode[1];
            state->pc++;
            break;
        }
        case 0x0e: //MVI C, data
        {
            state->c = opcode[1];
            state->pc++;
            break;
        }
        case 0x16: //MVI D, data
        {
            state->d = opcode[1];
            state->pc++;
            break;
        }
        case 0x1e: //MVI D, data
        {
            state->e = opcode[1];
            state->pc++;
            break;
        }
        case 0x26: //MVI H, data
        {
            state->h = opcode[1];
            state->pc++;
            break;
        }
        case 0x2e: //MVI L, data
        {
            state->l = opcode[1];
            state->pc++;
            break;
        }

        //MVI M, data
        case 0x36: 
        {
            state->memory[(state->h<<8) | state->l] = opcode[1];
            state->pc++;
            break;
        }

        //LXI rp, data 16
        case 0x01: //LXI BC, data 16
        {
            state->b = opcode[2];
            state->c = opcode[1];
            state->pc += 2;
            break;
        }
        case 0x11: //LXI DE, data 16
        {
            state->d = opcode[2];
            state->e = opcode[1];
            state->pc += 2;
            break;
        }
        case 0x21: //LXI HL, data 16
        {
            state->h = opcode[2];
            state->l = opcode[1];
            state->pc += 2;
            break;
        }
        case 0x31: //LXI SP, data 16
        {
            state->sp = (opcode[2]<<8) | opcode[1];
            state->pc += 2;
            break;
        }

        //LDA addr
        case 0x3a:
        {
            state->a = state->memory[(opcode[2]<<8) | opcode[1]];
            state->pc += 2;
            break;
        }

        //STA addr
        case 0x32:
        {
            state->memory[(opcode[2]<<8) | opcode[1]] = state->a;
            state->pc += 2;
            break;
        }

        //LHLD addr
        case 0x2a:
        {
            state->l = state->memory[(opcode[2]<<8) | opcode[1]];
            state->h = state->memory[ ((opcode[2]<<8) | opcode[1]) + 1 ];
            state->pc += 2;
            break;
        }

        //SHLD addr
        case 0x22:
        {
            state->memory[(opcode[2]<<8) | opcode[1]] = state->l;
            state->memory[ ((opcode[2]<<8) | opcode[1]) + 1 ] = state->h;
            state->pc += 2;
            break;
        }

        //LDAX rp
        case 0xa: //LDAX BC
        {
            state->a = state->memory[(state->b<<8) | state->c];
            break;
        }
        case 0x1a: //LDAX DE
        {
            state->a = state->memory[(state->d<<8) | state->e];
            break;
        }

        //STAX rp
        case 0x02: //STAX BC
        {
            state->memory[(state->b<<8) | state->c] = state->a;
            break;
        }
        case 0x12: //STAX DE
        {
            state->memory[(state->d<<8) | state->e] = state->a;
            break;
        }

        //XCHG
        case 0xeb:
        {
            uint8_t x;

            x = state->h;
            state->h = state->d;
            state->d = x;

            x = state->l;
            state->l = state->e;
            state->e = x;
            break;
        }

        // Arithmetic Group
        // ADD r
        case 0x80: //ADD B
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x81: //ADD C
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x82: //ADD D
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->d;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x83: //ADD E
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->e;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break; 
        }
        case 0x84: //ADD H
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->h;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x85: //ADD L
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->l;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x87: //ADD A
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->a;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //ADD M
        case 0x86: 
        {
            uint16_t offset = (state->h<<8) | (state->l);
            uint16_t answer = (uint16_t) state->a + state->memory[offset];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //ADI data
        case 0xc6:
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) opcode[1];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            state->pc++;
            break;
        }

        //ADC r
        case 0x88: //ADC B
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->b + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x89: //ADC C
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->c + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x8a: //ADC D
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->d + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x8b: //ADC e
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->e + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x8c: //ADC H
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->h + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x8f: //ADC A
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->a + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }

        //ADC M
        case 0x8e:
        {
            uint16_t offset = state->memory[(state->h<<8) | state->l] + (uint16_t) state->cc.cy;
            uint16_t answer = (uint16_t) state->a + state->memory[offset];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //ACI data
        case 0xce:
        {
            uint16_t answer = (uint16_t) state->a + (uint16_t) opcode[1] + (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            state->pc++;
            break;
        }

        //SUB r
        case 0x90: //SUB B
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->b;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x91: //SUB C
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->c;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x92: //SUB D
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->d;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x93: //SUB E
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->e;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x94: //SUB H
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->h;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x95: //SUB L
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->l;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x97: //SUB A
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->a;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //SUB M
        case 0x96: 
        {
            uint16_t offset = (state->h<<8) | state->l;
            uint16_t answer = (uint16_t) state->a - state->memory[offset];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }

        //SUI data
        case 0xd6:
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) opcode[1];
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            state->pc++;
            break;
        }
        
        //SBB r
        case 0x98: //SBB B
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->b - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x99: //SBB C
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->c - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x9a: //SBB D
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->d - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x9b: //SBB E
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->e - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x9c: //SBB H
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->h - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x9d: //SBB L
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->l - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        case 0x9f: //SBB A
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) state->a - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //SBB M
        case 0x9e: 
        {
            uint16_t offset = (state->h<<8) | state->l;
            uint16_t answer = (uint16_t) state->a - state->memory[offset] - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            break;
        }
        
        //SBI data
        case 0xde:
        {
            uint16_t answer = (uint16_t) state->a - (uint16_t) opcode[1] - (uint16_t) state->cc.cy;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.p = (parity(answer));
            state->cc.cy = (answer > 0xff);
            state->a = answer & 0xff;
            state->pc++;
            break;
        }
        
        //INR r
        case 0x04: //INR B
        {
            state->b++;
            state->cc.z = ((state->b & 0xff) == 0);
            state->cc.s = ((state->b & 0x80) != 0);
            state->cc.p = (parity(state->b));
            break;
        }
        case 0x0c: //INR C
        {
            state->c++;
            state->cc.z = ((state->c & 0xff) == 0);
            state->cc.s = ((state->c & 0x80) != 0);
            state->cc.p = (parity(state->c));
            break;
        }
        case 0x14: //INR D
        {
            state->d++;
            state->cc.z = ((state->d & 0xff) == 0);
            state->cc.s = ((state->d & 0x80) != 0);
            state->cc.p = (parity(state->d));
            break;
        }
        case 0x1c: //INR E
        {
            state->e++;
            state->cc.z = ((state->e & 0xff) == 0);
            state->cc.s = ((state->e & 0x80) != 0);
            state->cc.p = (parity(state->e));
            break;
        }
        case 0x24: //INR H
        {
            state->h++;
            state->cc.z = ((state->h & 0xff) == 0);
            state->cc.s = ((state->h & 0x80) != 0);
            state->cc.p = (parity(state->h));
            break;
        }
        case 0x2c: //INR L
        {
            state->l++;
            state->cc.z = ((state->l & 0xff) == 0);
            state->cc.s = ((state->l & 0x80) != 0);
            state->cc.p = (parity(state->l));
            break;
        }
        
        //INR M
        case 0x34:
        {
            uint16_t offset = (state->h<<8) | state->l;
            state->memory[offset]++;
            state->cc.z = ((state->memory[offset] & 0xff) == 0);
            state->cc.s = ((state->memory[offset] & 0x80) != 0);
            state->cc.p = (parity(state->memory[offset]));
            break;
        }
        
        //DCR r
        case 0x05: //DCR B
        {
            state->b--;
            state->cc.z = ((state->b & 0xff) == 0);
            state->cc.s = ((state->b & 0x80) != 0);
            state->cc.p = (parity(state->b));
            break;
        }
        case 0x0d: //DCR C
        {
            state->c--;
            state->cc.z = ((state->c & 0xff) == 0);
            state->cc.s = ((state->c & 0x80) != 0);
            state->cc.p = (parity(state->c));
            break;
        }
        case 0x15: //DCR D
        {
            state->d--;
            state->cc.z = ((state->d & 0xff) == 0);
            state->cc.s = ((state->d & 0x80) != 0);
            state->cc.p = (parity(state->d));
            break;
        }
        case 0x1d: //DCR E
        {
            state->e--;
            state->cc.z = ((state->e & 0xff) == 0);
            state->cc.s = ((state->e & 0x80) != 0);
            state->cc.p = (parity(state->e));
            break;
        }
        case 0x25: //DCR H
        {
            state->h--;
            state->cc.z = ((state->h & 0xff) == 0);
            state->cc.s = ((state->h & 0x80) != 0);
            state->cc.p = (parity(state->h));
            break;
        }
        case 0x2d: //DCR L
        {
            state->l--;
            state->cc.z = ((state->l & 0xff) == 0);
            state->cc.s = ((state->l & 0x80) != 0);
            state->cc.p = (parity(state->l));
            break;
        }
        case 0x3d: //DCR A
        {
            state->a--;
            state->cc.z = ((state->a & 0xff) == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = (parity(state->a));
            break;
        }

        //DCR M
        case 0x35: 
        {
            uint16_t offset = (state->h<<8) | state->l;
            state->memory[offset]--;
            state->cc.z = ((state->memory[offset] & 0xff) == 0);
            state->cc.s = ((state->memory[offset] & 0x80) != 0);
            state->cc.p = (parity(state->memory[offset]));
            break;
        }

        //INX rp
        case 0x03: //INX BC
        {
            uint16_t rpair = (state->b<<8) | state->c;
            rpair++;
            state->b = rpair>>8;
            state->c = rpair & 0xff;
            break;
        }
        case 0x13: //INX DE
        {
            uint16_t rpair = (state->d<<8) | state->e;
            rpair++;
            state->d = rpair>>8;
            state->e = rpair & 0xff;
            break;
        }
        case 0x23: //INX HL
        {
            uint16_t rpair = (state->h<<8) | state->l;
            rpair++;
            state->h = rpair>>8;
            state->l = rpair & 0xff;
            break;
        }
        case 0x33: //INX SP
        {
            state->sp++;
            break;
        }

        //DCX rp
        case 0x0b: //DCX BC
        {
            uint16_t rpair = (state->b<<8) | state->c;
            rpair--;
            state->b = rpair & 0xff;
            state->c = rpair>>8;
            break;
        }
        case 0x1b: //DCX DE
        {
            uint16_t rpair = (state->d<<8) | state->e;
            rpair--;
            state->d = rpair & 0xff;
            state->e = rpair>>8;
            break;
        }
        case 0x2b: //DCX HL
        {
            uint16_t rpair = (state->h<<8) | state->l;
            rpair--;
            state->h = rpair & 0xff;
            state->l = rpair>>8;
            break;
        }
        case 0x3b: //DCX SP 
        {
            state->sp++;
            break;
        }

        //DAD rp
        case 0x09: //DAD BC
        {
            uint16_t pairhl = (state->h<<8) | state->l;
            uint16_t pairbc = (state->b<<8) | state->c;
            uint32_t answer = (uint32_t) pairhl + (uint32_t) pairbc;
            state->cc.cy = (answer > 0xffff);
            state->l = answer & 0xff;
            state->h = (answer>>8) & 0xff;
            break;
        }
        case 0x19: //DAD DE
        {
            uint16_t pairhl = (state->h<<8) | state->l;
            uint16_t pairde = (state->d<<8) | state->e;
            uint32_t answer = (uint32_t) pairhl + (uint32_t) pairde;
            state->cc.cy = (answer > 0xffff);
            state->l = answer & 0xff;
            state->h = (answer>>8) & 0xff;
            break;
        }
        case 0x29: //DAD HL
        {
            uint16_t pairhl = (state->h<<8) | state->l;
            uint32_t answer = (uint32_t) pairhl + (uint32_t) pairhl;
            state->cc.cy = (answer > 0xffff);
            state->l = answer & 0xff;
            state->h = (answer>>8) & 0xff;
            break;
        }
        case 0x39: //DAD SP 
        {
            uint16_t pairhl = (state->h<<8) | state->l;
            uint32_t answer = (uint32_t) pairhl + (uint32_t) state->sp;
            state->cc.cy = (answer > 0xffff);
            state->l = answer & 0xff;
            state->h = (answer>>8) & 0xff;
            break;
        }

        //DAA (TODO: Learn more what this is. Probably something along the line of Binary-Coded-Decimal)
        case 0x27: 
        {
            /*state->a += ( (((state->a & 0xf) > 9) || state->cc.ac) ? 6 : 0 );
            state->cc.z = ((state->a & 0xff) == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = (parity(state->a));
            state->cc.cy = (state->a > 0xff);

            state->a += ( (((state->a & 0xf0) > 9) || state->cc.cy) ? 0x60 : 0);
            state->cc.z = ((state->a & 0xff) == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = (parity(state->a));
            state->cc.cy = (state->a > 0xff);
            state->a = state->a & 0xff; */
            break;
        }

        //Branch group
        //JMP addr
        case 0xc3:
        {
            state->pc = (opcode[2]<<8) | opcode[1];
            skipPcInc = true;
            break;
        }

        //Jcondition addr
        case 0xc2: //JNZ addr
        {
            if (!state->cc.z) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xca: //JZ addr
        {
            if (state->cc.z) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        
        case 0xd2: //JNC addr
        {
            if (!state->cc.cy) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xda: //JC addr
        {
            if (state->cc.cy) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xe2: //JPO addr
        {
            if (!state->cc.p) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xea: //JPE addr
        {
            if (state->cc.p) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xf2: //JP addr
        {
            if (!state->cc.s) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }
        case 0xfa: //JM addr
        {
            if (state->cc.s) {
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else { 
                state->pc += 2;
            }
            break;
        }

        //CALL addr
        case 0xcd: 
        {
            uint16_t ret = state->pc+3; //might be pc+2
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp-=2;
            state->pc = (opcode[2]<<8) | opcode[1];
            skipPcInc = true;
            skipPcInc = true;
            break;
        }

        //Ccondition addr
        case 0xc4: //CNZ addr
        {
            if (!state->cc.z) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xcc: //CZ addr
        {
            if (state->cc.z) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xd4: //CNC addr
        {
            if (!state->cc.cy) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xdc: //CC addr
        {
            if (state->cc.cy) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xe4: //CPO addr
        {
            if (!state->cc.p) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xec: //CPE addr
        {
            if (state->cc.p) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xf4: //CP addr
        {
            if (!state->cc.s) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }
        case 0xfc: //CM addr
        {
            if (state->cc.s) {
                uint16_t ret = state->pc+3; 
                state->memory[state->sp-1] = (ret>>8) & 0xff;
                state->memory[state->sp-2] = ret & 0xff;
                state->sp-=2;
                state->pc = (opcode[2]<<8) | opcode[1];
                skipPcInc = true;
            } else {
                state->pc += 2;
            }
            break;
        }

        //RET
        case 0xc9:
        {
            state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
            skipPcInc = true;
            state->sp += 2;
            break;
        }
        //Rcondition
        case 0xc0: //RNZ
        {
            if (!state->cc.z) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xc8: //RZ
        {
            if (state->cc.z) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xd0: //RNC
        {
            if (!state->cc.cy) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xd8: //RC
        {
            if (state->cc.cy) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xe0: //RPO
        {
            if (!state->cc.p) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xe8: //RPE
        {
            if (state->cc.p) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xf0: //RP
        {
            if (!state->cc.s) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }
        case 0xf8: //RM
        {
            if (state->cc.s) {
                state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                skipPcInc = true;
                state->sp += 2;
            } else {
                state->pc++;
            }
            break;
        }

        //RST n
        case 0xc7: //RST 0
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 0;
            skipPcInc = true;
            break;
        }
        case 0xcf: //RST 1
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 8;
            skipPcInc = true;
            break;
        }
        case 0xd7: //RST 2
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 16;
            skipPcInc = true;
            break;
        }
        case 0xdf: //RST 3
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 24;
            skipPcInc = true;
            break;
        }
        case 0xe7: //RST 4
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 32;
            skipPcInc = true;
            break;
        }
        case 0xef: //RST 5
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 40;
            skipPcInc = true;
            break;
        }
        case 0xf7: //RST 6
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 48;
            skipPcInc = true;
            break;
        }
        case 0xff: //RST 7
        {
            uint16_t ret = state->pc+1;
            state->memory[state->sp-1] = (ret>>8) & 0xff;
            state->memory[state->sp-2] = ret & 0xff;
            state->sp += 2;
            state->pc = 56;
            skipPcInc = true;
            break;
        }

        //PCHL
        case 0xe9:
        {
            state->pc = (state->h<<8) | state->l;
            skipPcInc = true;
            break;
        }

        //Logical group
        //ANA r
        case 0xa0: //ANA B
        {
            state->a = state->a & state->b;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa1: //ANA C
        {
            state->a = state->a & state->c;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa2: //ANA D
        {
            state->a = state->a & state->d;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa3: //ANA E
        {
            state->a = state->a & state->e;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa4: //ANA H
        {
            state->a = state->a & state->h;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa5: //ANA L
        {
            state->a = state->a & state->l;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }
        case 0xa7: //ANA A
        {
            state->a = state->a & state->a;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }

        //ANA M
        case 0xa6:
        {
            uint16_t offset = (state->h<<8) | state->l;
            state->a = state->a & state->memory[offset];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            break;
        }

        //ANI data
        case 0xe6:
        {
            state->a = state->a & opcode[1];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            state->pc++;
            break;
        }

        //XRA r
        case 0xa8: //XRA B
        {
            state->a = state->a ^ state->b;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xa9: //XRA C
        {
            state->a = state->a ^ state->c;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xaa: //XRA D
        {
            state->a = state->a ^ state->d;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xab: //XRA E
        {
            state->a = state->a ^ state->e;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xac: //XRA H
        {
            state->a = state->a ^ state->h;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xad: //XRA L
        {
            state->a = state->a ^ state->l;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        
        //XRA M
        case 0xae: 
        {
            uint16_t offset = (state->h<<8) | state->l;
            state->a = state->a ^ state->memory[offset];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }

        //XRI data
        case 0xee:
        {
            state->a = state->a ^ opcode[1];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            state->pc++;
            break;
        }

        //ORA r
        case 0xb0: //ORA B
        {
            state->a = state->a | state->b;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xb1: //ORA C
        {
            state->a = state->a | state->c;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xb2: //ORA D
        {
            state->a = state->a | state->d;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xb3: //ORA E
        {
            state->a = state->a | state->e;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xb4: //ORA H
        {
            state->a = state->a | state->h;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }
        case 0xb5: //ORA L
        {
            state->a = state->a | state->l;
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }

        //ORA M
        case 0xb6:
        {
            uint16_t offset = (state->h<<8) | state->l;
            state->a = state->a | state->memory[offset];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            break;
        }

        //ORI data
        case 0xf6:
        {
            state->a = state->a | opcode[1];
            state->cc.z = (state->a == 0);
            state->cc.s = ((state->a & 0x80) != 0);
            state->cc.p = parity(state->a);
            state->cc.cy = 0;
            state->cc.ac = 0;
            state->pc++;
            break;
        }

        //CMP r
        case 0xb8: //CMP B
        {
            uint8_t x = state->a - state->b;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }
        case 0xb9: //CMP C
        {
            uint8_t x = state->a - state->c;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }
        case 0xba: //CMP D
        {
            uint8_t x = state->a - state->d;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }
        case 0xbb: //CMP E
        {
            uint8_t x = state->a - state->e;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }
        case 0xbc: //CMP H
        {
            uint8_t x = state->a - state->h;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }
        case 0xbd: //CMP L
        {
            uint8_t x = state->a - state->l;
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->b);
            state->cc.ac = 0;
            break;
        }

        //CMP M
        case 0xbe: 
        {
            uint16_t offset = (state->h<<8) | state->l;
            uint8_t x = state->a - state->memory[offset];
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < state->memory[offset]);
            state->cc.ac = 0;
            break;
        }

        //CPI data
        case 0xfe:
        {
            uint8_t x = state->a - opcode[1];
            state->cc.z = (x == 0);
            state->cc.s = ((x & 0x80) != 0);
            state->cc.p = parity(x);
            state->cc.cy = (state->a < opcode[1]);
            state->cc.ac = 0;
            state->pc++;
            break;
        }
        
        //RLC 
        case 0x07:
        {
            uint8_t extract = (state->a>>7) & 0x01; //extract high bit
            state->a = (state->a<<1) | extract;
            state->cc.cy = extract;
            break;
        }

        //RRC
        case 0x0f:
        {
            uint8_t extract = state->a & 0x01; //extract low bit
            state->a = (state->a>>1) | (extract<<7);
            state->cc.cy = extract;
            break;
        }

        //RAL
        case 0x17:
        {
            uint8_t extract = state->cc.cy;
            state->cc.cy = state->a>>7;
            state->a = (state->a<<1) | extract;
            break;
        }
        
        //RAR
        case 0x1f:
        {
            uint8_t extract = state->cc.cy;
            state->cc.cy = state->a & 0x01;
            state->a = (state->a>>1) | (extract<<7);
            break;
        }
        
        //CMA
        case 0x2f:
        {
            state->a = ~state->a;
            break;
        }
        
        //CMC
        case 0x3f:
        {
            state->cc.cy = ~state->cc.cy;
            break;
        }

        //STC
        case 0x37:
        {
            state->cc.cy = 0x01;
            break;
        }

        //Stack, I/O, and Machine Control Group
        //PUSH rp (no SP)
        case 0xc5: //PUSH BC
        {
            state->memory[state->sp-1] = state->b;
            state->memory[state->sp-2] = state->c;
            state->sp -= 2;
            break;
        }
        case 0xd5: //PUSH DE
        {
            state->memory[state->sp-1] = state->d;
            state->memory[state->sp-2] = state->e;
            state->sp -= 2;
            break;
        }
        case 0xe5: //PUSH HL
        {
            state->memory[state->sp-1] = state->h;
            state->memory[state->sp-2] = state->l;
            state->sp -= 2;
            break;
        }

        //PUSH PSW
        case 0xf5:
        {
            state->memory[state->sp-1] = state->a;
            state->memory[state->sp-2] = 0x0 & \
                ( (state->cc.cy<<7) | (1<<6) | (state->cc.p<<5) | (state->cc.ac<<3) | (state->cc.z<<1) | state->cc.s ); //PSW
            break;
        }

        //POP rp (no SP)
        case 0xc1: //POP BC
        {
            state->c = state->memory[state->sp];
            state->b = state->memory[state->sp+1];
            state->sp += 2;
            break;
        }
        case 0xd1: //POP DE
        {
            state->e = state->memory[state->sp];
            state->d = state->memory[state->sp+1];
            state->sp += 2;
            break;
        }
        case 0xe1: //POP HL
        {
            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp+1];
            state->sp += 2;
            break;
        }

        //POP PSW
        case 0xf1:
        {
            state->cc.cy = state->memory[state->sp]>>7 & 1;
            state->cc.p = state->memory[state->sp]>>5 & 1;
            state->cc.ac = state->memory[state->sp]>>3 & 1;
            state->cc.z = state->memory[state->sp]>>1 & 1;
            state->cc.s = state->memory[state->sp] & 1;
            state->a = state->memory[state->sp+1];
            state->sp += 2;
            break;
        }

        //XTHL
        case 0xe3:
        {
            uint8_t x;

            x = state->l;
            state->l = state->memory[state->sp];
            state->memory[state->sp] = x;

            x = state->h;
            state->h = state->memory[state->sp+1];
            state->memory[state->sp+1] = x;
            break;
        }

        //SPHL
        case 0xf9:
        {
            state->sp = (state->h<<8) | state->l;
            break;
        }

        //IN port (TODO: implement this)
        case 0xdb:
        {
            state->pc++;
            break;
        }

        //OUT port (TODO: implement this)
        case 0xd3:
        {
            state->pc++;
            break;
        }

        //EI
        case 0xfb:
        {
            state->int_enable = 1;
            break;
        }

        //DI
        case 0xf3:
        {
            state->int_enable = 0;
            break;
        }

        //HLT
        case 0x76:
        {
            exit(0);
            break;
        }

        //NOP
        case 0x00:
        {
            break;
        }

        default: UnimplementedInstruction(state);
    }
    printf("\tC = %d, P = %d, S = %d, Z = %d\n", state->cc.cy, state->cc.p, state->cc.s, state->cc.z);
    printf("\tA %02x BC %02x%02x DE %02x%02x HL %02x%02x SP %04x\n",\
    state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->sp);
    if (!skipPcInc) state->pc++;
}

int parity(uint16_t n) {
    printf("make sure to implement parity!!!, %d\n", n);
    return 0;
}

void initEmulator(State8080* state, unsigned char *sourcefile) {
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->sp = 0;
    state->pc = 0;
    state->memory = sourcefile;

    state->cc.z = 0;
    state->cc.s = 0;
    state->cc.p = 0;
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.pad = 0;

    state->int_enable = 0;
}

int main(int argc, char **argv) {
    FILE *fp = fopen(argv[1], "rb");
    if (argc < 2) {
        puts("Error: no file specified");
        exit(1);
    } else if (fp == NULL) {
        puts("Error: unable to open file");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char *buffer = malloc(fsize);
    if (!fread(buffer, 1, fsize, fp) && ferror(fp)) { //if fread() returns 0 and error happens
        fputs("Error reading file", stderr);
    } else fclose(fp);

    State8080 cpu;
    State8080 *cpuptr = &cpu;
    initEmulator(cpuptr, buffer);
    while (1) Emulate8080Op(cpuptr);

    return 0;
}
