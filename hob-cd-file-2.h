/**
  hob-cd-file-2.h * HOB Compression File Oriented
  derived from ucddef.h hob-cd-file-1.h
  15.07.10 KB
  24.11.15 KB
*/

#ifndef DEF_HL_STR_G_I_1
#define DEF_HL_STR_G_I_1
struct dsd_gather_i_1 {                     /* gather input data       */
   struct dsd_gather_i_1 *adsc_next;        /* next in chain           */
   char *     achc_ginp_cur;                /* current position        */
   char *     achc_ginp_end;                /* end of input data       */
};
#endif

struct dsd_cdf_ctrl {                       /* compress data file oriented control */
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
#ifndef DEF_IRET_ERREY
#define DEF_IRET_ERREY         3            /* eyecather invalid       */
#endif
#ifndef DEF_IRET_ERRNE
#define DEF_IRET_ERRNE         4            /* no end-of-file found    */
#endif
#ifndef DEF_IRET_INVDA
#define DEF_IRET_INVDA         5            /* invalid data found      */
#endif

   BOOL       boc_eof;                      /* end of file input       */

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
   BOOL       boc_callagain;                /* call again this function */
   int        imc_param_1;                  /* parameter value 1       */
   int        imc_param_2;                  /* parameter value 2       */
   int        imc_param_3;                  /* parameter value 3       */
   int        imc_param_4;                  /* parameter value 4       */
};

#ifndef UCDPROG

#ifndef PTYPE
#ifdef __cplusplus
#define PTYPE "C"
#else
#define PTYPE
#endif
#endif

extern PTYPE void m_cdf_enc( struct dsd_cdf_ctrl * );
extern PTYPE void m_cdf_dec( struct dsd_cdf_ctrl * );

#endif
