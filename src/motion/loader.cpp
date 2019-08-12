///-----------------------------------------------------------------------------
#include "loader.h"
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

#include <QDebug>
///-----------------------------------------------------------------------------
using namespace LOADER_SPACE;

int LoaderClass::_byte_swapped; /// BYTE ORDERING OPPOSITE OF HOST

T_SIZE       LoaderClass::_init_size = 0;         /// CURRENT SIZE OF C INITIALIZATION
char         LoaderClass::_loadbuf[LOADBUFSIZE];  /// LOAD BUFFER
int          LoaderClass::_need_reloc = FALSE;    /// RELOCATION REQUIRED
RELOC_TAB*   LoaderClass::_reloc_tab = nullptr;   /// RELOCATION SYMBOL TABLE
int          LoaderClass::_reloc_tab_size;        /// CURRENT ALLOCATED AMOUNT
int          LoaderClass::_reloc_sym_index;       /// CURRENT SIZE OF TABLE
///-----------------------------------------------------------------------------
LoaderClass::LoaderClass()
{
    _verbose      = FALSE;
    _need_symbols = TRUE;
    _clear_bss    = FALSE;
    _sect_hdrs    = nullptr;

    _pageloc = (unsigned char*)(DEFPAGE);

    _reloc = 0;      /// RELOCATION AMOUNT
    _quiet = 0;      /// NO BANNER
    _sflag = 0;      /// SRAM MAPPED IN FOR EPROM
    _iflag = 0;      /// ALLOW SDB TO INTERRUPT PC
    _pflag = 0;      /// PAUSE BEFORE RUNNING
    _packtoflash = 0;/// PACK to Flash Memory image
    _firstdata = 1;  /// flag so we can clear flash image

}
///-----------------------------------------------------------------------------
/**
 * @brief cload - Main driver for COFF loader.
 * @return
 */
int LoaderClass::cload()
{
    int result;
    _reloc_tab = nullptr;
    _sect_hdrs = nullptr;
    result = !cload_headers() || !cload_symbols() || !cload_data();
    if(_reloc_tab)
        free(_reloc_tab);
    if(_sect_hdrs)
        free(_sect_hdrs);

    if(result)
        return FALSE;
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_headers - Read in the various headers of the COFF file.
 * @return
 */
int LoaderClass::cload_headers()
{
    int i;
    _byte_swapped = FALSE;
    if(!fread(&_file_hdr,FILHSZ,1,_fin))
        return FALSE;
    /// MAKE SURE THIS IS REALLY A COFF FILE. CHECK FOR SWAPPED FILES.
    /// DETERMINE BYTE ORDERING OF OBJECT DATA.
    if(_file_hdr.f_magic != MAGIC)
    {
        swap2byte(&_file_hdr.f_magic);
        if(_file_hdr.f_magic != MAGIC)
            return FALSE;
        _byte_swapped = TRUE;
        swap2byte(&_file_hdr.f_nscns);
        swap4byte(&_file_hdr.f_timdat);
        swap4byte(&_file_hdr.f_symptr);
        swap4byte(&_file_hdr.f_nsyms);
        swap2byte(&_file_hdr.f_opthdr);
        swap2byte(&_file_hdr.f_flags);
    }
    _big_e_target = (_file_hdr.f_flags & F_BIG);
    /// READ IN OPTIONAL HEADER, IF THERE IS ONE, AND SWAP IF NEEDED.
    if(_file_hdr.f_opthdr == AOUTSZ)
    {
        if(fread(&_o_filehdr,_file_hdr.f_opthdr,1,_fin) != 1)
            return FALSE;
        if(_byte_swapped)
        {
            swap2byte((unsigned short*)&_o_filehdr.magic);
            swap2byte((unsigned short*)&_o_filehdr.vstamp);
            swap4byte(&_o_filehdr.tsize);
            swap4byte(&_o_filehdr.dsize);
            swap4byte(&_o_filehdr.bsize);
            swap4byte(&_o_filehdr.entrypt);
            swap4byte(&_o_filehdr.text_start);
            swap4byte(&_o_filehdr.data_start);
        }
        _entry_point = static_cast<unsigned long>(_o_filehdr.entrypt);
    }
    else if(_file_hdr.f_opthdr && fseek(_fin,static_cast<long>(_file_hdr.f_opthdr), 1) == -1)
        return FALSE;
    /// READ IN SECTION HEADERS.
    if(
        !(_sect_hdrs = myalloc((_n_sections = _file_hdr.f_nscns) * SCNHSZ)) ||
        fread(_sect_hdrs,
              SCNHSZ,
              static_cast<size_t>(_n_sections),_fin) !=
        static_cast<unsigned>(_n_sections)
    )
        return FALSE;
    /// SWAP SECTION HEADERS IF REQUIRED.
    if(_byte_swapped)
    {
        for(i = 0;i < _n_sections;i++)
        {
            SCNHDR *sptr = SECT_HDR(i);
            swap4byte(&sptr->s_paddr);
            swap4byte(&sptr->s_vaddr);
            swap4byte(&sptr->s_size);
            swap4byte(&sptr->s_scnptr);
            swap4byte(&sptr->s_relptr);
            swap4byte(&sptr->s_lnnoptr);
            swap4byte((long *)&sptr->s_nreloc);
            swap4byte((long *)&sptr->s_nlnno);
            swap4byte((long *)&sptr->s_flags);
        }
    }
    /// CALL AN EXTERNAL ROUTINE TO DETERMINE THE RELOCATION AMOUNTS FOR EACH SECTION.                                                           */
    for(i = 0;i < _n_sections;i++)
        _reloc_amount[i] = 0;
    set_reloc_amount();
    for(i = 0;i < _n_sections;i++)
        _need_reloc |= (_reloc_amount[i] != 0);
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_data - Read in the raw data and load it into memory.
 * @return
 */
int LoaderClass::cload_data()
{
    int s;
    int ok = TRUE;
    /// LOOP THROUGH THE SECTIONS AND LOAD THEM ONE AT A TIME.
    for(s = 0;s < _n_sections && ok;s++)
    {
        SCNHDR *sptr = SECT_HDR(s);
        int     bss_flag;
        /// IF THIS IS THE TEXT SECTION, RELOCATE THE ENTRY POINT.
        if((sptr->s_flags & STYP_TEXT) && !strcmp(sptr->s_name, ".text"))
            _entry_point += _reloc_amount[s];
        /// SET A FLAG INDICATING A BSS SECTION THAT MUST BE LOADED.
        bss_flag = (_clear_bss && (sptr->s_flags & STYP_BSS) && !strcmp(sptr->s_name,".bss"));
        /// IGNORE EMPTY SECTIONS OR SECTIONS WHOSE FLAGS INDICATE THE
        /// SECTION IS NOT TO BE LOADED.  NOTE THAT THE CINIT SECTION,
        /// ALTHOUGH IT HAS THE STYP_COPY FLAG SET, MUST BE LOADED.
        if(!sptr->s_scnptr && !bss_flag                                ||
           !sptr->s_size                                               ||
           (sptr->s_flags & STYP_DSECT)                                ||
           (sptr->s_flags & STYP_COPY) && strcmp(sptr->s_name, CINIT)  ||
           (sptr->s_flags & STYP_NOLOAD)
        )
            continue;
        /// LOAD ALL OTHER SECTIONS.
        ok = cload_sect_data(s);
    }
    return ok;
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_sect_data - Read, relocate, and write out one section.
 * @param s
 * @return
 */
int LoaderClass::cload_sect_data(int s)
{
    SCNHDR* sptr    = SECT_HDR(s);
    T_ADDR addr = static_cast<T_ADDR>(sptr->s_vaddr); /* CURRENT ADDRESS IN SECTION       */
    unsigned long nbytes;                  /* BYTE COUNT WITHIN SECTION        */
    int           packet_size;             /* SIZE OF CURRENT DATA BUFFER      */
    int           excess  = 0;             /* BYTES LEFT FROM PREVIOUS BUFFER  */
    unsigned int  n_reloc = 0;             /* COUNTER FOR RELOCATION ENTRIES   */
    RELOC         reloc;                   /* RELOCATION ENTRY                 */
    int           bss_flag;

    /// READ THE FIRST RELOCATION ENTRY, IF THERE ARE ANY.
    /// IF THIS IS A BSS SECTION, CLEAR THE LOAD BUFFER.
    if(_need_reloc && sptr->s_nreloc &&(fseek(_fin, sptr->s_relptr, 0) == -1 || !reloc_read(&reloc)))
        return FALSE;
    if(bss_flag = (sptr->s_flags & STYP_BSS) && !strcmp(sptr->s_name, ".bss"))
        for(nbytes = 0;nbytes < LOADBUFSIZE;++nbytes)
            _loadbuf[nbytes] = 0;
    /// COPY ALL THE DATA IN THE SECTION.
    for(nbytes = 0;nbytes < (unsigned)(LOCTOBYTE(sptr->s_size));nbytes += packet_size)
    {
        int j, ok;
        /// READ IN A BUFFER OF DATA.  IF THE PREVIOUS RELOCATION SPANNED        */
        /// ACCROSS THE END OF THE LAST BUFFER, COPY THE LEFTOVER BYTES INTO     */
        /// THE BEGINNING OF THE NEW BUFFER.                                     */
        for(j = 0;j < excess;++j)
            _loadbuf[j] = _loadbuf[packet_size + j];
        packet_size = MIN(LOCTOBYTE(sptr->s_size) - nbytes, LOADBUFSIZE);
        if(!bss_flag &&
            (
                fseek(_fin,sptr->s_scnptr + nbytes + excess,0) == -1 ||
                fread(_loadbuf + excess,packet_size - excess,1,_fin) != 1
            )
        )
            return FALSE;
        excess = 0;
        /// READ AND PROCESS ALL THE RELOCATION ENTRIES THAT AFFECT DATA CURRENTLY IN THE BUFFER.                                             */
        if(_need_reloc)
            while(n_reloc < sptr->s_nreloc && (unsigned long)reloc.r_vaddr < addr + BYTETOLOC(packet_size))
            {
                int i = LOCTOBYTE(reloc.r_vaddr - addr); /// BYTE INDEX OF FIELD
                /// IF THIS RELOCATION SPANS PAST THE END OF THE BUFFER,
                /// SET 'EXCESS' TO THE NUMBER OF REMAINING BYTES AND FLUSH THE
                /// BUFFER.  AT THE NEXT FILL, THESE BYTES WILL BE COPIED FROM
                /// THE END OF THE BUFFER TO THE BEGINNING AND THEN RELOCATED.
                if(i + static_cast<int>(MAX(WORDSZ,reloc_size(reloc.r_type))) > packet_size)
                {
                    i          -= i % LOADWORDSIZE; /// DON'T BREAK WITHIN A WORD
                    excess      = packet_size - i;
                    packet_size = i;
                    break;
                }
                /// PERFORM THE RELOCATION AND READ IN THE NEXT RELOCATION ENTRY.
                relocate(&reloc,(unsigned char *)(_loadbuf + i),s);
                if(n_reloc++ < sptr->s_nreloc &&
                    (
                        fseek(_fin,sptr->s_relptr + ((long)n_reloc * RELSZ),0) == -1 ||
                        !reloc_read(&reloc)
                    )
                )
                    return FALSE;
            }
            /// WRITE OUT THE RELOCATED DATA TO THE TARGET DEVICE.  IF THIS IS A
            /// CINIT SECTION, CALL A SPECIAL FUNCTION TO HANDLE IT.
            ok = (sptr->s_flags & STYP_COPY) && !strcmp(sptr->s_name, CINIT) ?
            cload_cinit(sptr,&packet_size,&excess) :
            mem_write((unsigned char *)_loadbuf,packet_size,addr + _reloc_amount[s],sptr->s_page);
            if(!ok)
                return FALSE;
            /// KEEP TRACK OF THE ADDRESS WITHIN THE SECTION.
            addr += BYTETOLOC(packet_size);
    }
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_cinit - Process one buffer of C initialization records.
 * @param sptr        - CURRENT SECTION
 * @param packet_size - POINTER TO BUFFER SIZE
 * @param excess      - RETURNED NUMBER OF UNUSED BYTES
 * @return
 */
int LoaderClass::cload_cinit(SCNHDR *sptr,int *packet_size,int *excess)
{
    int i;                   /// BYTE COUNTER
    int init_packet_size;    /// SIZE OF CURRENT INITIALIZATION
    static T_ADDR init_addr; ///ADDRESS OF CURRENT INITIALIZATION
    /// PROCESS ALL THE INITIALIZATION RECORDS IN THE BUFFER.
    for(i = 0;i < *packet_size;i += init_packet_size)
    {
        /// IF STARTING A NEW INITIALIZATION, READ THE SIZE AND ADDRESS FROM THE TABLE.                                                           */
        if(_init_size == 0)
        {
            T_SIZE temp;
            /// IF THE SIZE AND ADDRESS ARE NOT FULLY CONTAINED IN THIS BUFFER,
            /// STOP HERE.  SET THE 'EXCESS' COUNTER TO THE NUMBER OF UNPROCESSED
            /// BYTES - THESE WILL BE COPIED TO THE HEAD OF THE NEXT BUFFER.
            if(i + (int)sizeof(T_SIZE) > *packet_size)
            {
                *excess += *packet_size - i;
                *packet_size = i;
                break;
            }
            /// IF THE NEXT SIZE FIELD IS ZERO, BREAK.
            temp = (T_SIZE)unpack((unsigned char *)(_loadbuf + i),sizeof(T_SIZE),sizeof(T_SIZE),0);
            if(temp == 0) /// tktk skip 0 length pads
            {
                i += sizeof(T_SIZE);
                init_packet_size=0;
            }
            else
            { /// READ THE ADDRESS FIELD, IF IT'S ALL HERE.
                if(i + (signed)sizeof(T_SIZE) + (signed)sizeof(T_ADDR) > *packet_size)
                {
                    *excess += *packet_size - i;
                    *packet_size = i;
                    break;
                }
                i         += sizeof(T_SIZE);
                _init_size  = temp;
                init_addr  = unpack((unsigned char *)(_loadbuf + i),sizeof(T_ADDR),sizeof(T_ADDR),0);
                i         += sizeof(T_ADDR);
            }
        }
        if(_init_size > 0) /// tktk skip 0 length pads
        {
            /// WRITE OUT THE CURRENT PACKET, UP TO THE END OF THE BUFFER.
            if(init_packet_size = MIN(*packet_size - i,(int)(_init_size * WORDSZ)))
            {
                if(!mem_write((unsigned char *)(_loadbuf + i),init_packet_size,init_addr,sptr->s_page))
                    return FALSE;
                init_addr += BYTETOLOC(init_packet_size);
                _init_size -= init_packet_size / WORDSZ;
            }
        }
    }
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_symbols - Read in the symbol table.
 * @return
 */
int LoaderClass::cload_symbols()
{
    long str_size;
    int first, next;
    SYMENT sym;
    AUXENT aux;

    if(!_need_symbols && !_need_reloc)
        return TRUE;
    /// ALLOCATE THE RELOCATION SYMBOL TABLE.
    if(_need_reloc)
    {
        _reloc_tab_size = MIN(RELOC_TAB_START,_file_hdr.f_nsyms);
        _reloc_tab = (RELOC_TAB *)(myalloc(_reloc_tab_size * sizeof(RELOC_TAB)));
        _reloc_sym_index = 0;
    }
    /// SEEK TO THE END OF THE SYMBOL TABLE AND READ IN THE SIZE OF THE STRING
    /// TABLE. THEN, ALLOCATE AND READ THE STRING TABLE.
    if(fseek(_fin,_file_hdr.f_symptr + (_file_hdr.f_nsyms * SYMESZ), 0) == -1 ||
       fread(&str_size, sizeof(long), 1,_fin) != 1
    )
        str_size = 0;
    else
    {
        if(_byte_swapped)
            swap4byte(&str_size);
        _str_buf = myalloc(static_cast<int>(str_size + 4));
        if(fread(_str_buf + 4,static_cast<size_t>(static_cast<int>(str_size - 4)),1,_fin) != 1)
            return FALSE;
    }
    /// IF THE APPLICATION NEEDS THE SYMBOL TABLE, LET IT READ IT IN.
    /// PASS NEED_RELOC TO THE APPLICATION SO THAT IT CAN CALL RELOC_ADD()
    if(_need_symbols)
        return load_syms(_need_reloc);
    ///  READ THE SYMBOL TABLE AND BUILD THE RELOCATION SYMBOL TABLE
    ///  FOR SYMBOLS THAT CAN BE USED IN RELCOATION, STORE THEM IN A
    ///  SPECIAL SYMBOL TABLE THAT CAN BE SEARCHED QUICKLY DURING
    ///  RELOCATION.
    for(first = 0;first < _file_hdr.f_nsyms;first = next)
    {
        if(!(next = sym_read(first,&sym,&aux)))
            return FALSE;
        if(sym.n_sclass == C_EXT   || sym.n_sclass == C_EXTDEF ||
           sym.n_sclass == C_STAT  || sym.n_sclass == C_LABEL  ||
           sym.n_sclass == C_BLOCK || sym.n_sclass == C_FCN
        )
            reloc_add(first, &sym);
    }
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief sym_read - Read and relocate a symbol and its aux entry.
 * @param index
 * @param sym
 * @param aux
 * @return Return the index of the next symbol.
 */
int LoaderClass::sym_read(unsigned int index,SYMENT *sym, AUXENT *aux)
{
    /// READ IN A SYMBOL AND ITS AUX ENTRY.
    if(
        fseek(_fin,_file_hdr.f_symptr + ((long)index * SYMESZ), 0) == -1 ||
        fread(sym,SYMESZ,1,_fin) != 1                                 ||
        (sym->n_numaux && fread(aux, SYMESZ, 1,_fin) != 1)
    )
        return FALSE;
    if(_byte_swapped)
    {
        /// SWAP THE SYMBOL TABLE ENTRY.
        if(sym->n_zeroes == 0)
            swap4byte((long *)(&sym->n_nptr));
        swap4byte(&sym->n_value);
        swap2byte((unsigned short *)&sym->n_scnum);
        swap2byte((unsigned short *)&sym->n_type);
        /// SWAP THE AUX ENTRY, BASED ON THE STORAGE CLASS.
        if(sym->n_numaux)
            switch(sym->n_sclass)
            {
                case C_FILE :
                    break;
                case C_STRTAG :
                case C_UNTAG  :
                case C_ENTAG  :
                    swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                    swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                    break;
                case C_FCN   :
                case C_BLOCK :
                    swap2byte(&aux->x_sym.x_misc.x_lnsz.x_lnno);
                    swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                    break;
                case C_EOS :
                    swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                    swap4byte(&aux->x_sym.x_tagndx);
                    break;
                default :
                    /// HANDLE FUNCTION DEFINITION SYMBOL
                    if(((sym->n_type >> 4) & 3) == DT_FCN)
                    {
                        swap4byte(&aux->x_sym.x_tagndx);
                        swap4byte(&aux->x_sym.x_misc.x_fsize);
                        swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_lnnoptr);
                        swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                        swap2byte(&aux->x_sym.x_regcount);
                    }
                    /// HANDLE ARRAYS.
                    else if(((sym->n_type >> 4) & 3) == DT_ARY)
                    {
                        swap4byte(&aux->x_sym.x_tagndx);
                        swap2byte(&aux->x_sym.x_misc.x_lnsz.x_lnno);
                        swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                        swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[0]);
                        swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[1]);
                        swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[2]);
                        swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[3]);
                    }
                    /// HANDLE SECTION DEFINITIONS
                    else if(sym->n_type == 0)
                    {
                        swap4byte(&aux->x_scn.x_scnlen);
                        swap2byte(&aux->x_scn.x_nreloc);
                        swap2byte(&aux->x_scn.x_nlinno);
                    }
                    /// HANDLE MISC SYMBOL RECORD
                    else
                    {
                        swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                        swap4byte(&aux->x_sym.x_tagndx);
                    }
            }
    }
    /// RELOCATE THE SYMBOL, BASED ON ITS STORAGE CLASS.
    switch(sym->n_sclass)
    {
        case C_EXT     :
        case C_EXTDEF  :
        case C_STAT    :
        case C_LABEL   :
        case C_BLOCK   :
        case C_FCN     :
        /// IF THE SYMBOL IS UNDEFINED, CALL AN APPLICATION ROUTINE TO LOOK
        /// IT UP IN AN EXTERNAL SYMBOL TABLE.  IF THE SYMBOL IS DEFINED,
        /// RELOCATE IT ACCORDING TO THE SECTION IT IS DEFINED IN.
        if(sym->n_scnum == 0)
            // lookup_sym((struct syment *)index, (union auxent *)sym, (short)aux);  // tktk ??
            lookup_sym((struct syment *)index, (union auxent *)sym, 0);  // tktk ??
        else if(sym->n_scnum > 0)
            sym->n_value += _reloc_amount[sym->n_scnum - 1];
    }
    return (index + sym->n_numaux + 1);
}
///-----------------------------------------------------------------------------
/**
 * @brief sym_name
 * @return Return a pointer to the name of a symbol.
 */
char* LoaderClass::sym_name(SYMENT *symptr)
{
    static char temp[9];
    if(symptr->n_zeroes == 0)
        return(_str_buf + symptr->n_offset);
    strncpy(temp, symptr->n_name, 8);
    temp[8] = 0;
    return temp;
}
///-----------------------------------------------------------------------------
/**
 * @brief reloc_add - Add a symbol to the relocation symbol table.
 * @param index
 * @param sym
 */
void LoaderClass::reloc_add(unsigned int index,SYMENT *sym)
{
    if(!_need_reloc)
        return;
    if(_reloc_sym_index >= _reloc_tab_size)
    {
        _reloc_tab_size += RELOC_GROW_SIZE;
        _reloc_tab = (RELOC_TAB * )mralloc((char *)_reloc_tab,_reloc_tab_size * sizeof(RELOC_TAB));
    }
    _reloc_tab[_reloc_sym_index  ]._rt_index = index;
    _reloc_tab[_reloc_sym_index  ]._rt_scnum = sym->n_scnum;
    _reloc_tab[_reloc_sym_index++]._rt_value = sym->n_value;
}
///-----------------------------------------------------------------------------
/**
 * @brief relocate - Perform a single relocation.
 * @param rp   - RELOCATION ENTRY
 * @param data - DATA BUFFER
 * @param s    - INDEX OF CURRENT SECTION
 * @return
 */
int LoaderClass::relocate(RELOC* rp,unsigned char* data,int s)
{
    int fieldsz = reloc_size(rp->r_type);     /* SIZE OF ACTUAL PATCH VALUE    */
    int wordsz  = MAX(fieldsz, WORDSZ);       /* SIZE OF CONTAINING FIELD      */
    long objval;                              /* FIELD TO BE PATCHED           */
    long reloc_amt;                           /* AMOUNT OF RELOCATION          */
    /// LOOK UP THE SYMBOL BEING REFERENCED IN THE RELOCATION SYMBOL TABLE.
    /// USE THE SYMBOL VALUE TO CALCULATE THE RELOCATION AMOUNT:
    /// 1) IF THE SYM INDEX IS -1 (INTERNAL RELOCATION) USE THE RELOCATION
    ///    AMOUNT OF THE CURRENT SECTION.
    /// 2) IF THE SYMBOL WAS UNDEFINED (DEFINED IN SECTION 0), USE THE
    ///    SYMBOL'S VALUE.
    /// 3) IF THE SYMBOL HAS A POSITIVE SECTION NUMBER, USE THE RELOCATION
    ///    AMOUNT FOR THE SECTION IN WHICH THE SYMBOL IS DEFINED.
    /// 4) OTHERWISE, THE SYMBOL IS ABSOLUTE, SO THE RELOCATION AMOUNT IS 0.
        if(rp->r_symndx == -1)
            reloc_amt = _reloc_amount[s];
        else
        {
            int rt_index = reloc_sym(rp->r_symndx);       /* INDEX IN RELOC TABLE   */
            int sect_ref = _reloc_tab[rt_index]._rt_scnum;  /* SECTION WHERE DEFINED  */
            reloc_amt    = sect_ref == 0 ? _reloc_tab[rt_index]._rt_value
               : sect_ref >  0 ? _reloc_amount[sect_ref - 1] : 0;
        }
        /// EXTRACT THE RELOCATABLE FIELD FROM THE OBJECT DATA.
        objval = unpack(data,fieldsz,wordsz,BIT_OFFSET(rp->r_vaddr));
        /// MODIFY THE FIELD BASED ON THE RELOCATION TYPE.
        switch(rp->r_type)
        {
            /// NORMAL RELOCATIONS: ADD IN THE RELOCATION AMOUNT.
            case R_RELBYTE:
            case R_RELWORD:
            case R_REL24:
            case R_RELLONG:
            case R_PARTLS16:
                objval += reloc_amt;
                break;
            /// 34010 ONE'S COMPLEMENT RELOCATION.  SUBTRACT INSTEAD OF ADD.
            case R_OCRLONG:
                objval -= reloc_amt;
                break;
            /// 34020 WORD-SWAPPED RELOCATION.  SWAP BEFORE RELOCATING.
            case R_GSPOPR32:
                objval  = ((objval >> 16) & 0xFFFF) | (objval << 16);
                objval += reloc_amt;
                objval  = ((objval >> 16) & 0xFFFF) | (objval << 16);
                break;
            /// PC-RELATIVE RELOCATIONS.  IN THIS CASE THE RELOCATION AMOUNT
            /// IS ADJUSTED BY THE PC DIFFERENCE.   IF THIS IS AN INTERNAL
            /// RELOCATION TO THE CURRENT SECTION, NO ADJUSTMENT IS NEEDED.
            case R_PCRBYTE:
            case R_PCRWORD:
            case R_GSPPCR16:
            case R_PCRLONG:
                if(rp->r_symndx != -1) /// IF NOT INTERNAL RELOCATION
                {
                    int shift  = 8 * (4 - fieldsz);
                    objval     = (long)(objval << shift) >> shift; /// SIGN EXTEND
                    reloc_amt -= _reloc_amount[s];
                    if(rp->r_type == R_GSPPCR16)
                        reloc_amt >>= 4; /// BITS TO WORDS
                    objval += reloc_amt;
                }
                break;
            /// 320C30 PAGE-ADDRESSING RELOCATION.  CALCULATE THE ADDRESS FROM
            /// THE 8-BIT PAGE VALUE IN THE FIELD, THE 16-BIT OFFSET IN THE RELOC
            /// ENTRY, AND THE RELOCATION AMOUNT.  THEN, STORE THE 8-BIT PAGE
            /// VALUE OF THE RESULT BACK IN THE FIELD.
            case R_PARTMS8:
                objval = ((objval << 16) + rp->r_disp + reloc_amt) >> 16;
                break;
            /// DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS FROM THE 16-BIT
            /// VALUE IN THE RELOCATION FIELD PLUS THE RELOCATION AMOUNT.  OR THE
            /// TOP 9 BITS OF THIS RESULT INTO THE RELOCATION FIELD.
            case R_PARTMS9:
                objval = (objval & 0xFE00) |
                    (((rp->r_disp + reloc_amt) >> 7) & 0x1FF);
                break;
            /// DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS AS ABOVE, AND OR THE
            /// 7-BIT DISPLACEMENT INTO THE FIELD.
            case R_PARTLS7:
                objval = (objval & 0x80) | ((rp->r_disp + reloc_amt) & 0x7F);
                break;
            /// DSP(320) 13-BIT CONSTANT.  RELOCATE ONLY THE LOWER 13 BITS OF THE FIELD.                                                               */
            case R_REL13:
                objval = (objval & 0xE000) | ((objval + reloc_amt) & 0x1FFF);
                break;
        }
        /// PACK THE RELOCATED FIELD BACK INTO THE OBJECT DATA.
        repack(objval,data,fieldsz,wordsz,0);
        return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief reloc_read - Read in and swap one relocation entry.
 * @param rptr
 * @return
 */
int LoaderClass::reloc_read(RELOC *rptr)
{
    if(fread(rptr,RELSZ,1,_fin) != 1)
        return FALSE;
    if(_byte_swapped)
    {
        swap4byte(&rptr->r_vaddr);
        swap2byte((unsigned short *)&rptr->r_symndx);
        swap2byte(&rptr->r_disp);
        swap2byte(&rptr->r_type);
    }
    return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief reloc_size - Return the field size of a relocation type.
 * @param type
 * @return
 */
int LoaderClass::reloc_size(short type)
{
    switch (type)
    {
        case R_RELBYTE:
        case R_PCRBYTE:
        case R_PARTMS8:
        case R_PARTLS7:
            return 1;
        case R_RELWORD:
        case R_PCRWORD:
        case R_GSPPCR16:
        case R_PARTLS16:
        case R_PARTMS9:
        case R_REL13:
            return 2;
        case R_REL24:
            return 3;
        case R_GSPOPR32:
        case R_RELLONG:
        case R_PCRLONG:
        case R_OCRLONG:
            return 4;
        default:
            return 0;
    }
}
///-----------------------------------------------------------------------------
/**
 * @brief reloc_sym - Search the relocation symbol table for a symbol.
 * @param index
 * @return
 */
int LoaderClass::reloc_sym(int index)
{
    int i = 0,

    j = _reloc_sym_index - 1;

    /// THIS IS A SIMPLE BINARY SEARCH (THE RELOC TABLE IS ALWAYS SORTED).
    while(i <= j)
    {
        int m = (i + j) / 2;
        if(_reloc_tab[m]._rt_index < index)
            i = m + 1;
        else
            if(_reloc_tab[m]._rt_index > index)
                j = m - 1;
        else
            return m;
    }
 //   assert(FALSE);
   return 0;
}
///-----------------------------------------------------------------------------
///                               /* BIT ADDRESS WITHIN BYTE      */
/**
 * @brief unpack - Extract a relocation field from object bytes.
 * @param data
 * @param fieldsz
 * @param wordsz
 * @param bit_addr
 * @return
 */
unsigned long LoaderClass::unpack(unsigned char* data,
                                  int fieldsz,
                                  int wordsz,
                                  int bit_addr)
{
    register int i;
    int r = bit_addr;                          /* RIGHT SHIFT VALUE            */
    int l = 8 - r;                             /* LEFT SHIFT VALUE             */
    unsigned long objval = 0;

    if(!_big_e_target)
    {
        /// TAKE THE FIRST 'fieldsz' BYTES AND CONVERT
        if(r == 0)
            for(i = fieldsz - 1;i >= 0;--i)
                objval = (objval << 8) | data[i];
            else
            {
                for(i = fieldsz;i > 0;--i)
                objval = (objval << 8) | (data[i] << l) | (data[i-1] >> r);

                data[0]       = (data[0]       << l) >> l;
                data[fieldsz] = (data[fieldsz] >> r) << r;
            }
    }
    else      /* MSBFIRST */
    {
        /// TAKE THE LAST 'fieldsz' BYTES.
        if(r == 0)
            for(i = wordsz - fieldsz;i < wordsz;++i)
                objval = (objval << 8) | data[i];
            else
            {
                int firstbyte = wordsz - fieldsz;
                for(i = firstbyte;i < wordsz;++i)
                    objval = (objval << 8) | (data[i] << r) | (data[i+1] >> l);

                data[firstbyte] = (data[firstbyte] >> l) << l;
                data[wordsz]    = (data[wordsz]    << r) >> r;
            }
    }
    return objval;
}
///-----------------------------------------------------------------------------
/**
 * @brief repack - Encode a relocated field into object bytes.
 * @param objval
 * @param data
 * @param fieldsz
 * @param wordsz
 * @param bit_addr
 */
void LoaderClass::repack(unsigned long objval,
                         unsigned char*  data,
                         int fieldsz,
                         int wordsz,
                         int bit_addr)
{
    register int i;
    int r = bit_addr;                          /* RIGHT SHIFT VALUE            */
    int l = 8 - r;                             /* LEFT SHIFT VALUE             */

    if(!_big_e_target)
    {
        /// ENCODE LSB OF VALUE FIRST
        if(r == 0)
          for(i = 0;i < fieldsz;objval >>= 8)
              data[i++] = (unsigned char)objval;
        else
        {
            data[0] |= (objval << r);
            objval  >>= l;
            for(i = 1;i < fieldsz;objval >>= 8)
                data[i++] = (unsigned char)objval;
            data[fieldsz] |= objval;
        }
    }
    else   /* MSBFIRST */
    {
        /// ENCODE MSB OF VALUE FIRST
        if(r == 0)
            for(i = wordsz - 1;i >= wordsz - fieldsz;objval >>= 8)
                data[i--] = (unsigned char)objval;
        else
        {
            int firstbyte = wordsz - fieldsz;

            data[wordsz] |= (unsigned char)(objval << l);
            objval >>= r;
            for(i = wordsz - 1;i > firstbyte;objval >>= 8)
                data[i--] = (unsigned char)(unsigned char)objval;
            data[firstbyte] |= objval;
        }
    }
}
///-----------------------------------------------------------------------------
/**
 * @brief cload_lineno - Read in & swap line number entries.
 * @param filptr - WHERE TO READ FROM
 * @param count  - HOW MANY TO READ
 * @param lptr   - WHERE TO PUT THEM
 * @param scnum  - SECTION NUMBER OF THESE ENTRIES
 * @return
 */
int LoaderClass::cload_lineno(long filptr,
                              int count,
                              LINENO* lptr,
                              int scnum)
{
     int i;

     /// READ IN THE REQUESTED NUMBER OF LINE NUMBER ENTRIES AS A BLOCK.
     if(fseek(_fin, filptr, 0) == -1 || fread(lptr, count, LINESZ, _fin) != LINESZ)
         return FALSE;
     /// SWAP AND RELOCATE EACH ENTRY, AS NECESSARY.
     if(_byte_swapped || _reloc_amount[scnum - 1])
         for(i = 0;i < count;i++)
         {
             if(_byte_swapped)
             {
                 swap2byte(&lptr->l_lnno);
                 swap4byte(&lptr->l_addr.l_paddr);
             }

             if(lptr->l_lnno != 0)
                 lptr->l_addr.l_paddr += _reloc_amount[scnum - 1];

             lptr = (LINENO *) (((char *)lptr) + LINESZ);
         }
     return TRUE;
}
///-----------------------------------------------------------------------------
/**
 * @brief swap4byte - Swap the order of bytes in a long.
 * @param addr
 */
void LoaderClass::swap4byte(long* addr)
{
    long temp1;
    long temp2;
    long temp3;
    long temp4;

    temp1 = (*addr)       & 0xFF;
    temp2 = (*addr >> 8)  & 0xFF;
    temp3 = (*addr >> 16) & 0xFF;
    temp4 = (*addr >> 24) & 0xFF;

    *addr = (temp1 << 24) | (temp2 << 16) | (temp3 << 8) | temp4;
}
///-----------------------------------------------------------------------------
/**
 * @brief swap2byte - Swap the order of bytes in a short.
 * @param addr
 */
void LoaderClass::swap2byte(unsigned short* addr)
{
    unsigned short temp1;
    unsigned short temp2;

    temp1 = *addr;
    temp2 = *addr;
    *addr = ((temp2 & 0xFF) << 8) | ((temp1 >> 8) & 0xFF);
}
///-----------------------------------------------------------------------------
/**
 * @brief set_reloc_amount
 */
void LoaderClass::set_reloc_amount()
{
    int i;
    for(i = 0;i < _n_sections;++i)
    {
        _reloc_amount[i] = _reloc;
    }
}
///-----------------------------------------------------------------------------
/**
 * @brief lookup_sym
 * @param sym
 * @param aux
 * @param indx
 */
void LoaderClass::lookup_sym(SYMENT* sym,AUXENT* aux,short indx)
{
}
///-----------------------------------------------------------------------------
/**
 * @brief load_syms
 * @param need_reloc
 * @return
 */
int LoaderClass::load_syms(int need_reloc)
{
    return 1;
}
///-----------------------------------------------------------------------------
/**
 * @brief myalloc
 * @param size
 * @return
 */
char* LoaderClass::myalloc(int size)
{
    char *p = static_cast<char*>(malloc(static_cast<size_t>(size)));
    if(p) return p;
    ///errorMessageBox("out of memory");
    exit(1);
}
///-----------------------------------------------------------------------------
char* LoaderClass::mralloc(char* p,int size)
{
    p = static_cast<char*>(realloc(p,static_cast<size_t>(size)));
    if(p) return p;
    ///errorMessageBox("out of memory");
    exit(1);
}
///-----------------------------------------------------------------------------
/**
 * @brief load - main load coff file method
 * @param Name
 * @param EntryPoint
 * @param PackToFlash
 * @return
 */
int LoaderClass::load(const char* Name,unsigned int* EntryPoint,int PackToFlash)
{
    int  files   = 0;
    int  nostart = 0;

    _need_symbols = 0;

    //Motion = MotionDirect;  // save globally so memwrite can use it

    /*----------------------------------------------------------------------*/
    /* PROCESS COMMAND LINE ARGUMENTS                                       */
    /*----------------------------------------------------------------------*/

    _clear_bss = 1;
    _quiet     = 1;
    nostart    = 1;
    _sflag     = 1;
    _iflag     = 1;
    _pflag     = 1;
    ++_verbose;
    _reloc   = 0x00000000;
    _pageloc = 0x00000000;

    _packtoflash = PackToFlash;
    _firstdata = 1;              /* flag so we can clear flash image*/

    _fin = fopen(Name,"rb");
    if(!_fin)
    {
        char s2[300]="can't open file : ";
        strcat(s2,Name);
///		AfxMessageBox(s2,MB_OK|MB_SYSTEMMODAL);
        qDebug() << s2;
        return 1;
    }
    if (!cload())
    {
///		AfxMessageBox("error loading file",MB_OK|MB_SYSTEMMODAL);
        qDebug() << "error loading file";
        fclose(_fin);
        return 1;
    }
    fclose(_fin);
    *EntryPoint = _entry_point;
    return 0;
}
///-----------------------------------------------------------------------------
/**
 * @brief mem_write
 * @param buffer
 * @param nbytes
 * @param addr
 * @param page
 * @return
 */
#define ARGBUFSIZE 256
#define N_BYTES_PER_LINE 64			  // number of bytes/line for coff downloads
int BytesPerLine = N_BYTES_PER_LINE;
int LoaderClass::mem_write(unsigned char* buffer,int nbytes,T_ADDR addr,int page)
{
    char s[256];
    char x[256];
    int i;
    int k;

    if(nbytes < 1) return 1;

    if(_packtoflash != 0)
    {
        if(_firstdata && _packtoflash != 2)
        {
//            if(Motion->firmwareVersion()==1)
                BytesPerLine = 16;
//            if(Motion->writeLine("CLEARFLASHIMAGE")) return 0;
            _firstdata = 0;
        }
        if(BytesPerLine == 16)
        {
            // SCRAMBLE DATA to Make compatible OLD VERSION of FLASH Layout
            if(addr >= 0x80000000 && addr < 0x80000000+13*0x400)
            {
                addr += 3*0x400 - 0x80000000;
            }
            else if(addr >= 3*0x400 && addr < 64*0x400)
            {
                addr += 13*0x400;
                if(addr>64*0x400)
                    addr += 0x80000000 - 64*0x400;
            }
        }
        sprintf(s,"LOADFLASH %X %X",addr,nbytes);
    }
    else
    {
        sprintf(s,"LOADDATA %X %X",addr,nbytes);
    }


    qDebug() << s;
//    if(Motion->waitToken("memwritecoff")) return 0;
//    if(Motion->writeLine(s))
//    {
//        Motion->releaseToken();
//        return 0;
//    }

    for(i = 0;i < nbytes;i += BytesPerLine)
    {
        s[0] = 0;
        for(k = 0;k < BytesPerLine && i+k < nbytes;k++)
        {
            if(k+1 < BytesPerLine && i+k+1 < nbytes)
                sprintf(x,"%02X ",buffer[i+k]);
            else
                sprintf(x,"%02X",buffer[i+k]);
            strcat(s,x);
        }

        qDebug() << s;

//        if(Motion->writeLine(s))
//        {
//            Motion->releaseToken();
//            return 0;
//        }
    }
//    Motion->releaseToken();
    return 1;
}
///-----------------------------------------------------------------------------
