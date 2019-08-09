///-----------------------------------------------------------------------------
#ifndef LOADER_H
#define LOADER_H
///-----------------------------------------------------------------------------
#include "params.h"
///-----------------------------------------------------------------------------
/// THIS MACRO IS USED TO FACILIATE ACCESS TO THE SECTION HEADERS
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// EXTERNAL FUNCTIONS (DEFINED BY APPLICATION).
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
namespace LOADER_SPACE
{

#define SECT_HDR(i) ((SCNHDR *)(sect_hdrs + (i) * SCNHSZ))
///-----------------------------------------------------------------------------
/// VARIABLES DEFINED BY THE LOADER.
///-----------------------------------------------------------------------------
extern int     verbose;        /// PRINT PROGRESS INFO
extern int     need_symbols;   /// READ IN SYMBOL TABLE
extern int     clear_bss;      /// CLEAR BSS SECTION

extern FILE   *fin;            /// COFF INPUT FILE
extern FILHDR  file_hdr;       /// FILE HEADER STRUCTURE
extern AOUTHDR o_filehdr;      /// OPTIONAL (A.OUT) FILE HEADER
extern T_ADDR  entry_point;    /// ENTRY POINT OF MODULE
extern T_ADDR  reloc_amount[]; /// AMOUNT OF RELOCATION PER SECTION
extern char   *sect_hdrs;      /// ARRAY OF SECTION HEADERS
extern char   *str_buf;        /// STRING TABLE
extern int     n_sections;     /// NUMBER OF SECTIONS IN THE FILE
extern int     big_e_target;   /// OBJECT DATA IS STORED MSB FIRST


    class LoaderClass
    {
        public:
            LoaderClass();

            /// MAIN DRIVER FOR COFF LOADER ------------------------------------
            int cload();
            /// LOAD FILE AND SECTION HEADERS
            int cload_headers();
            /// LOAD RAW DATA FROM FILE
            int cload_data();
            /// LOAD DATA FROM ONE SECTION
            int cload_sect_data(int s);
            /// PROCESS BUFFER OF C INITIALIZATION
            /// *sptr        - CURRENT SECTION
            /// *packet_size - POINTER TO BUFFER SIZE
            /// *excess      - RETURNED NUMBER OF UNUSED BYTES
            int cload_cinit(SCNHDR *sptr,int *packet_size,int *excess);
            /// LOAD AND PROCESS COFF SYMBOL TABLE
            int cload_symbols();
            /// READ, RELOCATE, AND SWAP A SYMBOL
            int sym_read(unsigned int index,SYMENT *sym, AUXENT *aux);
            /// RETURN REAL NAME OF A SYMBOL
            char *sym_name();
            /// ADD SYMBOL TO RELOC SYMBOL TABLE
            void reloc_add(unsigned int index,SYMENT *sym);
            /// PERFORM A RELOCATION PATCH
            /// *rp   - RELOCATION ENTRY
            /// *data - DATA BUFFER
            /// s     - INDEX OF CURRENT SECTION
            int relocate(RELOC *rp,unsigned char *data,int s);
            /// READ AND SWAP A RELOC ENTRY
            int reloc_read(RELOC *rptr);
            /// RETURN SIZE IN BYTES OF RELOC FIELD
            int reloc_size(short type);
            /// LOOKUP A SYMBOL IN THE RELOC SYMTAB
            int reloc_sym(int index);
            /// EXTRACT RELOC FIELD FROM DATA
            unsigned long unpack(unsigned char *data,int fieldsz,int wordsz,int bit_addr);                              /* BIT ADDRESS WITHIN BYTE      */
            /// INSERT RELOCATED FIELD INTO DATA
            void repack(unsigned long objval,unsigned char *data,int fieldsz,int wordsz,int bit_addr);
            /// READ AND RELOCATE LINENO ENTRIES
            int  cload_lineno();
            /// BYTE-SWAP A 32-BIT VALUE
            void swap4byte(long *addr);
            /// BYTE-SWAP A 16-BIT VALUE
            void swap2byte(unsigned short *addr);

            /// DETERMINE RELOCATION FOR ALL SECTIONS --------------------------
            void set_reloc_amount();
            /// WRITE TO TARGET MEMORY
            int mem_write(unsigned char *buffer,int nbytes,T_ADDR addr,int page);
            /// LOOKUP AN UNDEFINED SYMBOL AND DEF IT
            void lookup_sym(SYMENT *sym, AUXENT *aux, short indx);
            /// BUILD THE SYMBOL TABLE
            int load_syms(int need_reloc);
            /// MEMORY ALLOCATION FUNCTIONS
            char *myalloc(int size);
            char *mralloc(char *p,int size);
            /// WRITE OUT LOADER DEBUG INFO
            void load_msg(char *a,long b,long c,long d,long e,long f,long g);

    };
} /// end LOADER_SPACE
///-----------------------------------------------------------------------------
#endif /// LOADER_H
///-----------------------------------------------------------------------------
