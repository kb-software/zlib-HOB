/**
  hob-cd-record-1.h * HOB Compression Record Oriented
  derived from hob-cdrdef1.h
  15.07.10 KB
*/

#ifndef DEF_HL_STR_G_I_1
#define DEF_HL_STR_G_I_1
struct dsd_gather_i_1 {                     /* gather input data       */
   struct dsd_gather_i_1 *adsc_next;        /* next in chain           */
   char *     achc_ginp_cur;                /* current position        */
   char *     achc_ginp_end;                /* end of input data       */
};
#endif

struct dsd_cdr_ctrl {
   int        imc_func;                     /* called function         */
#ifndef DEF_IFUNC_START
#define DEF_IFUNC_START        0            /* start of processing, initialize */
#endif
#ifndef DEF_IFUNC_CONT
#define DEF_IFUNC_CONT         1            /* process data as specified */
                                            /* by buffer pointers      */
#endif
#ifndef DEF_IFUNC_RESET
#define DEF_IFUNC_RESET        2
#endif
#ifndef DEF_IFUNC_END
#define DEF_IFUNC_END          3
#endif
   int        imc_return;                   /* return code             */
#ifndef DEF_IRET_NORMAL
#define DEF_IRET_NORMAL        0            /* continue processing     */
#endif
#ifndef DEF_IRET_END
#define DEF_IRET_END           1            /* subroutine has ended processing */
#endif
#ifndef DEF_IRET_ERRAU
#define DEF_IRET_ERRAU         2            /* error in auxiliary prog */
#endif
#ifndef DEF_IRET_INVDA
#define DEF_IRET_INVDA         5            /* invalid data found      */
#endif
   BOOL       boc_mp_flush;                 /* end-of-record input     */
                                            /* set by main-program     */
   BOOL       boc_sr_flush;                 /* end-of-record output    */
                                            /* set by subroutine       */
   BOOL       boc_maybe_uncompressed;       /* subroutine may req unco */
                                            /* set by subroutine       */
   BOOL       boc_compressed;               /* use compressed output   */
                                            /* set by subroutine       */
   struct dsd_gather_i_1 *adsc_gai1_in;     /* input data              */

   char *     achc_out_cur;                 /* current end of output data */
   char *     achc_out_end;                 /* end of buffer for output data */

   BOOL (* amc_aux) ( void *, int, void *, int );  /* auxiliary helper routine pointer */
                                            /* callback                */
#ifndef DEF_AUX_MEMGET
#define DEF_AUX_MEMGET             0        /* get / acquire a block of memory */
#endif
#ifndef DEF_AUX_MEMFREE
#define DEF_AUX_MEMFREE            1        /* free / release a block of memory */
#endif
   void *     ac_ext;                       /* attached buffer pointer */
   void *     vpc_userfld;                  /* User Field Subroutine   */

   /* for compression header as needed by MPPC                         */
   int        imc_len_header;               /* length of header data   */
   char       chrc_header[ 8 ];             /* content of header       */

   int        imc_save_mp_needed;           /* length save-area needed */
                                            /* set by subroutine       */
   char       *achc_save_mp;                /* save-area from main pr  */
                                            /* set by main-program     */
   int        imc_save_mp_given;            /* length save-area given  */
                                            /* set by main-program     */
   int        imc_param_1;                  /* parameter value 1       */
   int        imc_param_2;                  /* parameter value 2       */
   int        imc_param_3;                  /* parameter value 3       */
   int        imc_param_4;                  /* parameter value 4       */
};

#ifndef UCDRPROG
#ifndef HL_CDR_SPEC

#ifndef PTYPE
#ifdef __cplusplus
#define PTYPE "C"
#else
#define PTYPE
#endif
#endif

#ifndef HL_COMP_MULTI
extern PTYPE void m_cdr_enc( struct dsd_cdr_ctrl * );  /* encode = compression */
extern PTYPE void m_cdr_dec( struct dsd_cdr_ctrl * );  /* decode = decompression */
#endif
#endif
#endif
