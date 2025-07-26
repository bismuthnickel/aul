#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int hex_digit_to_int(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    else if ('a' <= c && c <= 'f') return c - 'a' + 10;
    else if ('A' <= c && c <= 'F') return c - 'A' + 10;
    else return 0;
}

int hex_chars_to_byte(char high, char low) {
    int hi = hex_digit_to_int(high);
    int lo = hex_digit_to_int(low);
    return (hi << 4) | lo;
}

uint8_t length_of_instruction_at(char* program, uint32_t i) {
    switch (program[i]) {
        case 'a':
        case 'b':
        case 'l':
        case 'g':
        case 'G':
        case 'C':
            return 3;
        case 'S':
            return 2;
        default:
            return 1;
    }
}

int main() {
    char* program = malloc(1);
    int size = 1;
    program[0] = 0;
    int ch;
    while ((ch = getchar()) != EOF) {
        program = realloc(program, ++size);
        program[size-2] = (char)ch;
        program[size-1] = 0;
    }
    uint8_t a = 0;
    uint8_t b = 0;
    uint16_t s = 0;
    uint8_t stack[65536];
    uint16_t cs = 0;
    uint8_t callStack[65536];
    uint32_t i;
    uint32_t labelPositions[256];
    uint8_t lastResult = 0;
    for (int j = 0; j < 256; j++) {
        labelPositions[j] = -1;
    }
    for (int j = 0; j < size; ) {
        if (program[j] == 'l') {
            labelPositions[hex_chars_to_byte(program[j+1], program[j+2])] = j;
        }
        j += length_of_instruction_at(program, j);
    }
    bool comment = false;
    for (i = 0; i < size && i >= 0; ) {
        if (comment) {
            if (program[i] == 0xa || program[i] == 0xd) {
                comment = false;
            } else {
                continue;
            }
        }
        bool jumped = false;
        switch (program[i]) {
            case 'a':
                a = hex_chars_to_byte(program[i+1], program[i+2]);
                break;
            case 'b':
                b = hex_chars_to_byte(program[i+1], program[i+2]);
                break;
            case 's': {
                uint8_t tmp = b;
                b = a;
                a = tmp;
                break; 
            }
            case 'c':
                b = a;
                break;
            case 'i':
                a = getchar();
                break;
            case 'p':
                putchar(a);
                break;
            case 'P':
                printf("%u", a);
                break;
            case 'g':
                i = labelPositions[hex_chars_to_byte(program[i+1], program[i+2])];
                jumped = true;
                break;
            case 'G':
                if (!(lastResult == 0)) {
                    break;
                }
                i = labelPositions[hex_chars_to_byte(program[i+1], program[i+2])];
                jumped = true;
                break;
            case '+':
                a += b;
                lastResult = a;
                break;
            case '-':
                a -= b;
                lastResult = a;
                break;
            case '*':
                a *= b;
                lastResult = a;
                break;
            case '/':
                a /= b;
                lastResult = a;
                break;
            case 'S':
                switch (program[i+1]) {
                    case 'A':
                        stack[s++] = a;
                        break;
                    case 'B':
                        stack[s++] = b;
                        break;
                    case 'a':
                        a = stack[--s];
                        break;
                    case 'b':
                        b = stack[--s];
                        break;
                }
                break;
            case 'C':
                callStack[cs++] = labelPositions[hex_chars_to_byte(program[i+1], program[i+2])];
                break;
            case 'r':
                callStack[cs++] = i + length_of_instruction_at(program, i);
                i = labelPositions[hex_chars_to_byte(program[i+1], program[i+2])];
                jumped = true;
                break;
            case 'R':
                i = callStack[--cs];
                break;
            case ';':
                comment = true;
                break;
        }
        if (!jumped) {
            i += length_of_instruction_at(program, i);
        }
    }
    return 0;
}