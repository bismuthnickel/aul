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
        case 'r':
            return 3;
        case 'S':
            return 2;
        default:
            return 1;
    }
}

char* read_file(const char* filename, uint32_t* size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        puts("Failed to open file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        puts("fseek failed");
        return NULL;
    }

    long filesize = ftell(file);
    if (filesize < 0) {
        fclose(file);
        puts("ftell failed");
        return NULL;
    }

    rewind(file);

    char *buffer = malloc(filesize + 1);
    if (!buffer) {
        fclose(file);
        puts("Memory allocation failed");
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, filesize, file);
    if (read_bytes != filesize) {
        free(buffer);
        fclose(file);
        puts("Failed to read entire file");
        return NULL;
    }

    buffer[filesize] = '\0';

    fclose(file);

    *size = filesize+1;

    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        return -1;
    uint32_t size;
    char* program = read_file(argv[1], &size);
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
                i++;
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
            case 'x':
                printf("%02x", a);
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
                jumped = true;
                break;
            case ';':
                comment = true;
                break;
        }
        if (!jumped) {
            i += length_of_instruction_at(program, i);
        }
    }
    free(program);
    return 0;
}