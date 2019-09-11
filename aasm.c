#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define MAX_NAME_SIZE 256
#define MAX_READ_SIZE 256

#if defined(__x86_64__)
    #define REGISTER "r"
#elif defined(__i386__)
    #define REGISTER "e"
#else
    #error "Arch is not supported"
#endif

uint8_t init_args(int argc, char const *argv[]);
void parse_code(void);

void action_by_code(uint8_t * buffer, uint8_t abstract_code);
uint8_t has_abstract(uint8_t * buffer);

_Bool has_contain(uint8_t * string, uint8_t * pattern);
_Bool has_suffix(uint8_t * string, uint8_t * pattern);

void clear_suffix(uint8_t * string);
void pop_values(void);

void run_instruction(void);

static inline void print_help(void);
static inline void switchr(_Bool * reg);

// Global variables
FILE * File_Read  = NULL;
FILE * File_Write = NULL;

uint16_t Instruction = 0;;

struct {
    _Bool eax;
    _Bool ebx;
    _Bool ecx;
    _Bool edx;
} Register = { 0, 0, 0, 0 };

int main(int argc, char const *argv[]) {
    uint8_t exit_code = 0;
    if (exit_code = init_args(argc, argv)) {
        return exit_code;
    }
    parse_code();
    return exit_code;
}

void parse_code(void) {
    uint8_t buffer[MAX_READ_SIZE];
    uint8_t abstract_code = 0;

    #ifdef __linux__
        fprintf(File_Write, "%s\n%s\n\n", "format ELF64", "public _start");
    #elif _WIN32
        fprintf(File_Write, "%s\n%s\n\n", "format PE64", "public _start");
    #else
        // another OS
    #endif

    while (fgets(buffer, MAX_READ_SIZE, File_Read) != NULL) {
        if (abstract_code = has_abstract(buffer)) {
            action_by_code(buffer, abstract_code);
        } else {
            fprintf(File_Write, "%s", buffer);
        }
    }
}

void action_by_code(uint8_t * buffer, uint8_t abstract_code) {
    switch (abstract_code) {
        // register
        case 1: {
            clear_suffix(buffer);

            // exit
            if (has_suffix(buffer, "10")) {
                #ifdef __linux__
                    pop_values();
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "ax");
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "bx");
                    fprintf(File_Write, "\t%s\n", "mov eax, 1");
                    fprintf(File_Write, "\t%s\n", "xor ebx, ebx");
                    switchr(&Register.eax);
                    switchr(&Register.ebx);
                #elif _WIN32
                    pop_values();
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "ax");
                    fprintf(File_Write, "\t%s\n", "mov eax, 4C00");
                    switchr(&Register.eax);
                #else
                    // another OS
                #endif
            }

            // write stdout
            else if (has_suffix(buffer, "41")) {
                Instruction = 41;
                #ifdef __linux__
                    pop_values();
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "ax");
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "bx");
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "cx");
                    fprintf(File_Write, "\t%s\n", "push " REGISTER "dx");
                    fprintf(File_Write, "\t%s\n", "mov eax, 4");
                    fprintf(File_Write, "\t%s\n", "mov ebx, 1");
                    switchr(&Register.eax);
                    switchr(&Register.ebx);
                    switchr(&Register.ecx);
                    switchr(&Register.edx);
                #elif _WIN32
                    // for another OS
                #else
                    // another OS
                #endif
            }
        }
        break;

        // system call
        case 5: {
            run_instruction();
            #ifdef __linux__
                fprintf(File_Write, "\t%s\n", "int 0x80");
            #elif _WIN32
                fprintf(File_Write, "\t%s\n", "int 0x21");
            #else
                // another OS
            #endif
            pop_values();
        }
        break;
    }
}

void run_instruction(void) {
    if (Instruction == 41) {
        #ifdef __linux__
            fprintf(File_Write, "\t%s\n", "pop " REGISTER "dx");
            fprintf(File_Write, "\t%s\n", "pop " REGISTER "cx");
        #elif _WIN32
            // Windows OS
        #else
            // another OS
        #endif
    } 
    Instruction = 0;
}

void pop_values(void) {
    if (Register.edx) {
        fprintf(File_Write, "\t%s\n", "pop " REGISTER "dx");
        switchr(&Register.edx);
    }
    if (Register.ecx) {
        fprintf(File_Write, "\t%s\n", "pop " REGISTER "cx");
        switchr(&Register.ecx);
    }
    if (Register.ebx) {
        fprintf(File_Write, "\t%s\n", "pop " REGISTER "bx");
        switchr(&Register.ebx);
    }
    if (Register.eax) {
        fprintf(File_Write, "\t%s\n", "pop " REGISTER "ax");
        switchr(&Register.eax);
    }
}

uint8_t has_abstract(uint8_t * buffer) {
    if (has_contain(buffer, "!ax")) {
        return 1;
    }
    if (has_contain(buffer, "!syscall")) {
        return 5;
    }
    return 0;
}

void clear_suffix(uint8_t * string) {
    const size_t length = strlen(string);
    for (ssize_t i = length - 1; i >= 0; --i) {
        if (string[i] == ' ' || string[i] == '\n' || string[i] == '\t') {
            continue;
        }
        string[i+1] = '\0';
        break;
    }
}

_Bool has_suffix(uint8_t * string, uint8_t * pattern) {
    const size_t length = strlen(string);
    const size_t patt_length = strlen(pattern);

    if (length < patt_length) {
        return 0;
    }

    for (ssize_t j = patt_length - 1, i = length - 1; i >= 0; --i) {
        if (j == 0) {
            return 1;
        }
        if (string[i] != pattern[j]) {
            return 0;
        }
        --j;
    }

    return 1;
}

_Bool has_contain(uint8_t * string, uint8_t * pattern) {
    const size_t length = strlen(string);
    const size_t patt_length = strlen(pattern);

    if (length < patt_length) {
        return 0;
    }

    for (size_t j, i = 0; i < length; ++i) {
        j = 0;
        for (j = 0; j < patt_length; ++j) {
            if (string[i+j] != pattern[j]) {
                break;
            }
        }
        if (j == patt_length) {
            return 1;
        }
    }

    return 0;
}

uint8_t init_args(int argc, char const *argv[]) {
    _Bool is_in_file = 0, has_in_file = 0;
    _Bool is_out_file = 0, has_out_file = 0;

    uint8_t input_file[MAX_NAME_SIZE];
    uint8_t output_file[MAX_NAME_SIZE] = "out.asm";

    if (argc == 1) {
        print_help();
        return 1;
    }

    for (size_t i = 1; i < argc; ++i) {
        if ((strcmp("-i", argv[i])) == 0 || strcmp("--input", argv[i]) == 0) {
            is_in_file = 1;
            continue;   
        }
        if ((strcmp("-o", argv[i])) == 0 || strcmp("--output", argv[i]) == 0) {
            is_out_file = 1;
            continue;   
        }

        if (is_in_file && !has_in_file) {
            strcpy(input_file, argv[i]);
            has_in_file = 1;
            is_in_file = 0;
            continue;
        }
        if (is_out_file && !has_out_file) {
            strcpy(output_file, argv[i]);
            has_out_file = 1;
            is_out_file = 0;
            continue;
        }
    }

    if (!has_in_file) {
        printf("[!] ASM file is undefined\n");
        print_help();
        return 2;
    }

    File_Read = fopen(input_file, "r");
    if (File_Read == NULL) {
        printf("[!] Can't get descriptor from '%s'\n", input_file);
        return 3;
    }

    File_Write = fopen(output_file, "w");
    if (File_Write == NULL) {
        printf("[!] Can't get descriptor from '%s'\n", output_file);
        return 4;
    }

    return 0;
}

static inline void switchr(_Bool * reg) {
    *reg = !*reg;
}

static inline void print_help(void) {
    printf(
        "| AASM Abstract Assembler |\n"
        "[-i, --input] = input assembly file\n"
        "[-o, --output] = output assembly file\n"
    );
}
