///-----------------------------------------------------------------------------
#ifndef LOADER_H
#define LOADER_H
///-----------------------------------------------------------------------------
#include <stdio.h>
///-----------------------------------------------------------------------------
#include "params.h"
#include "coff.h"
///-----------------------------------------------------------------------------
namespace LOADER_SPACE
{
///-----------------------------------------------------------------------------
/// SDB HARDWARE INTERFACE PARAMETERS
///-----------------------------------------------------------------------------
#define CONPORT         0x0339          /// CONTROL REGISTER
#define MAPORT          0x0338          /// MAP REGISTER
#define XINTCLR         0x02            /// IF 1, ENABLE INTERRUPTS TO PC
#define DPSEL           0x04            /// EANBLE DUAL-PORT RAM
#define SWRESET         0x08            /// IF 0, HOLD C30 IN RESET
#define DEFPAGE         0xC9000         /// DEFAULT ADDRESS OF DP RAM
#define SETADDR         0xFF
#define SETEXE          0xFE
#define OKSDB           0x07
#define TEST            0x55
#define TESTRESP        0xAA
#define BUSY            0x330           /// CONTROLLING SIDE IS PROCESSING
#define IDLE            0x331           /// CONTROLLING SIDE IS WAITING
#define RUN             0x332           /// SDB IS RUNNING
#define LATCH(s)        (outp(s,0))     /// LATCH IN A REQUEST FOR CONTROL
#define UNLOCK(s)       (outp(s,1))     /// FREE CONTROL OF THE SEMAPHORE
#define IS_LOCKED(s)    (inp(s) != 0)   /// FREE CONTROL OF THE SEMAPHORE
#define TIMEOUT  2







    #ifndef TRUE
        #define TRUE 1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif

    #define SECT_HDR(i)     ((SCNHDR *)(_sect_hdrs + (i) * SCNHSZ))
    #define MIN(a,b)        ((a)<(b)?(a):(b))
    #define MAX(a,b)        ((a)>(b)?(a):(b))
    #define MAXNSCNS        255             /// MAXIMUM NUMBER OF SECTIONS
    #define WORDSZ          sizeof(T_DATA)  /// SIZE OF DATA UNITS IN OBJ FILE
    #define RELOC_TAB_START 128             /// STARTING SIZE OF TABLE
    #define RELOC_GROW_SIZE 128             /// REALLOC AMOUNT FOR TABLE

    typedef struct
    {
        short         _rt_index;            /// INDEX OF SYMBOL IN SYMBOL TABLE
        char          _rt_index_align[2];
        short         _rt_scnum;            /// SECTION NUMBER SYMBOL DEFINED IN
        char          _rt_scnum_align[2];
        unsigned long _rt_value;            /// RELOCATED VALUE OF SYMBOL
    } RELOC_TAB;

    /**
     * @brief The LoaderClass class
     */
    class LoaderClass
    {
        private :
            unsigned char*  _pageloc;
            unsigned long   _reloc;
            int _quiet;
            int _sflag;
            int _iflag;
            int _pflag;
            int _packtoflash;
            int _firstdata;
            FILE*   _fin;                       /// INPUT FILE
            int     _verbose;                   /// PRINT PROGRESS INFO
            int     _need_symbols;              /// APPLICATION NEEDS SYMBOL TABLE
            int     _clear_bss;                 /// CLEAR BSS SECTION
            FILHDR  _file_hdr;                  /// FILE HEADER STRUCTURE
            AOUTHDR _o_filehdr;                 /// OPTIONAL (A.OUT) FILE HEADER
            T_ADDR  _entry_point;               /// ENTRY POINT OF MODULE
            T_ADDR  _reloc_amount[MAXNSCNS];    /// AMOUNT OF RELOCATION PER SECTION
            char*   _sect_hdrs;                 /// ARRAY OF SECTION HEADERS
            char*   _str_buf;                   /// STRING TABLE
            int     _n_sections;                /// NUMBER OF SECTIONS IN THE FILE
            int     _big_e_target;              /// TARGET DATA IN BIG-ENDIAN FORMAT

            static T_SIZE       _init_size;
            static RELOC_TAB*   _reloc_tab;
            static int          _need_reloc;
            static int          _byte_swapped;
            static int          _reloc_tab_size;
            static int          _reloc_sym_index;
            static char         _loadbuf[LOADBUFSIZE];

            int     cload();
            int     cload_headers();
            int     cload_data();
            int     cload_sect_data(int s);
            int     cload_cinit(SCNHDR* sptr,int* packet_size,int* excess);
            int     cload_symbols();
            int     sym_read(unsigned int index,SYMENT* sym,AUXENT* aux);
            char*   sym_name(SYMENT *symptr);
            void    reloc_add(unsigned int index,SYMENT *sym);
            int     relocate(RELOC *rp,unsigned char *data,int s);
            int     reloc_read(RELOC *rptr);
            int     reloc_size(short type);
            int     reloc_sym(int index);
            unsigned long unpack(unsigned char* data,int fieldsz,int wordsz,int bit_addr);
            void    repack(unsigned long objval,unsigned char* data,int fieldsz,int wordsz,int bit_addr);
            int     cload_lineno(long filptr,int count,LINENO* lptr,int scnum);
            void    swap4byte(long* addr);
            void    swap2byte(unsigned short* addr);
            void    set_reloc_amount();
            void    lookup_sym(SYMENT* sym,AUXENT* aux,short indx);
            int     load_syms(int need_reloc);
            char*   myalloc(int size);
            char*   mralloc(char* p,int size);
            void    load_msg(char* a,long b,long c,long d,long e,long f,long g);

        public :
            LoaderClass();

            int mem_write(unsigned char* buffer,int nbytes,T_ADDR addr,int page);
            ///int LoadCoff(MOTION_DIRECT_SPACE::MotionDirectClass *MotionDirect,const char * Name,unsigned int *EntryPoint,int PackToFlash);
            int load(const char* Name,unsigned int* EntryPoint,int PackToFlash);
    };
} /// end LOADER_SPACE
///-----------------------------------------------------------------------------
#endif /// LOADER_H
///-----------------------------------------------------------------------------
