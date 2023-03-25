#define D_CHANGE_KB
#define INPUT_ONE_CHUNK
//#define SMALL_INPUT 4
//#define SMALL_INPUT 8
#define SMALL_INPUT 512
//#define TRACEHL1
#define TRACEHL1
//#define TRY_200324_01
//#define TRY_200324_02
#define TRY_200325_01
#define TRY_200325_02
#define TRY_200325_03
//#define TRY_200401_01
#define TRY_200402_01
#define TRY_200403_01
#define TRY_200403_02
#define TRY_200403_03
#define TRY_200405_01
#define TRY_200405_02
#define TRY_200405_03
//#define TRY_200405_04
#define TRY_200406_01
//#define TRY_200406_02
#ifdef KBDEBUG01
#define OUTPUT_SHORT
//#define DEBUG_110213_01
//#define DEBUG_110306_01
#define DEF_OUTPUT_LEN 32
#endif
#ifndef KBDEBUG01
#define DEF_OUTPUT_LEN (2 * 1024)
#endif
// +----------------------------------------------------------+
// |                                                          |
// | PROGRAM NAME: xbt-cdf-comp-3                             |
// | -------------                                            |
// |  Test Compression-Program, compression and decompression |
// |  24.01.11 JB                                             |
// |  21.03.20 KB                                             |
// |                                                          |
// | COPYRIGHT:                                               |
// | ----------                                               |
// |  Copyright (C) HOB Germany 2011                          |
// |  Copyright (C) Klaus Brandstaetter Germany 2020          |
// |                                                          |
// +----------------------------------------------------------+

#define MAX_GATHER 16
#ifndef SMALL_INPUT
#define LEN_INPUT  2048
#endif
#ifdef SMALL_INPUT
#define LEN_INPUT  SMALL_INPUT
#endif
#define LEN_OUTPUT 2048
#define MIN_LEN_B  8

// +----------------------------------------------------------+
// | System and library header files.                         |
// +----------------------------------------------------------+

#ifndef HL_WINALL1
#ifdef WIN32
#define HL_WINALL1
#endif
#ifndef HL_WINALL1
#ifdef WIN64
#define HL_WINALL1
#endif
#endif
#endif

#ifdef HL_WINALL1
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>
#include <crtdbg.h>
#include <aclapi.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef HL_UNIX
#include <conio.h>
#endif
#ifndef HL_WINALL1
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#endif
#ifdef HLMACOSX
#include <time.h>
#include <sys/time.h>
#endif

#define MIN_OUTPUT 8

#ifdef HL_UNIX
#define APIRET int
#define PVOID void *
#define PPVOID void **
#define BOOL  int
#define FALSE 0
#define TRUE  1
#define ULONG unsigned long int
#define UCHAR unsigned char
#ifndef HL_LONGLONG
#define HL_LONGLONG long long int
#endif
#include <sys/time.h>
#else
#ifndef HL_LONGLONG
#define HL_LONGLONG LONGLONG
#endif
#endif

#ifdef HL_WINALL1
#define HFILE HANDLE
#define APIRET long int
#define PPVOID void **
#endif

#include <hob-cd-file-2.h>

#ifdef TRACEHL1
#define TRACEHL_DUMP
#endif
#ifndef TRACEHL_DUMP
#ifdef DEBUG_110306_01
#define TRACEHL_DUMP
#endif
#endif

#ifndef HL_UNIX
#define MEASURE_TIME(AILL_DEST) \
   if(ill_freq){\
      if(QueryPerformanceCounter((LARGE_INTEGER *) AILL_DEST) == FALSE){ \
         printf("xbt-cdf-comp-3.cpp-l%05d QueryPerformanceCounter() measure Error %d.\n", __LINE__, GetLastError()); \
         return -1;\
      }\
      *AILL_DEST = (*AILL_DEST * 1000000) / ill_freq;\
   }
#else
#define MEASURE_TIME(AILL_DEST) \
   gettimeofday( &dsl_timeval, NULL ); \
   *AILL_DEST = ((HL_LONGLONG) dsl_timeval.tv_sec) * 1000 * 1000 + dsl_timeval.tv_usec;
#endif



// +----------------------------------------------------------+
// | Internal function prototypes.                            |
// +----------------------------------------------------------+

static BOOL m_cdaux( void * vpp_struct, int imp_func, void * achp_addr, int imp_length );
static int m_get_random_number( int impmax );
#ifdef TRACEHL_DUMP
static void m_console_out( char *achp_buff, int implength );
#endif

// +----------------------------------------------------------+
// | Static global variables and local constants.             |
// +----------------------------------------------------------+

static BOOL bos_abend = FALSE;              /* abnormal end of program */

#ifndef HL_UNIX
static HFILE dss_h_file;
#endif
#ifndef HL_UNIX
static int ims_fd_file;
#endif

//static const int IMS_MAX_LOOPS = 3;
static const int IMS_MAX_LOOPS = 10000;

static struct dsd_gather_i_1 dsrs_gai1_enc[ MAX_GATHER ];
static struct dsd_gather_i_1 dsrs_gai1_dec[ MAX_GATHER ];

static char chrs_compare_area[ 256 * LEN_OUTPUT ];

static struct dsd_cdf_ctrl dss_enc;
static struct dsd_cdf_ctrl dss_dec;

static char chrs_out_enc[ LEN_OUTPUT ];

static HL_LONGLONG ill_count_mem_enc = 0;

static char *achs_area_enc;
static char *achs_area_dec;

static BOOL bos_area_full;

#ifdef TRACEHL_DUMP
static const char chrstrans[]
     = { '0', '1', '2', '3', '4', '5', '6', '7',
         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
#endif

// +----------------------------------------------------------+
// | Main control procedure.                                  |
// +----------------------------------------------------------+

int main( int impargc, char *achrpargv[]) {
#ifndef HL_UNIX
   BOOL       bol_rc;                       /* return code             */
   unsigned long int uml_read;
#endif
#ifdef HL_UNIX
   int        iml_rc;                       /* return code             */
#endif
#ifdef TRACEHL1   /* 29.03.11 KB */
   int        iml1;                         /* working variable        */
   int        iml_ret_dec;                  /* returned from decode    */
#endif
   int        iml_w1, iml_w2, iml_w3, iml_w4;  /* working variables    */
   int        iml_read;                     /* length for read()       */
   char       *achl_w1, *achl_w2;           /* working variables       */
   char       *achl_area;                   /* address area            */
   char       *achl_compare;                /* area to compare         */
#ifdef XYZ1
   char       *achl_compressed;             /* compressed data         */
#endif
   char       *achl_input_dec;              /* input to decode         */
   struct dsd_gather_i_1 *adsl_gai1_w1;
#ifdef TRACEHL1
   int        imt_compressed_1;
   int        imt_compressed_2;
   int        imt_compressed_3;
   char       *acht_compressed;
#endif

   if (impargc < (1 + 1)) {
     printf("xbt-cdf-comp-3\n");
     printf("--------------\n\n");

     printf("usage:\n");
     printf("xbt-cdf-comp-3.exe file options\n");

     return 1;
   }
   char* ach_filename = achrpargv[1];
   setbuf( stdout, 0 );

   // +-----------+
   // | open file |
   // +-----------+

#ifdef HL_WINALL1
   dss_h_file = CreateFileA( ach_filename, GENERIC_READ, FILE_SHARE_READ, 0,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
   if (dss_h_file == INVALID_HANDLE_VALUE) {
     printf("CreateFile Error: return code=%d filename=%s.\n", GetLastError(), ach_filename);
     return -1; /* I did not manage to check access to file from Ruby. The ruby-function always returns TRUE, even if the file is not accessable. */
   }

   unsigned long ill_filesize_h = 0;
   HL_LONGLONG ill_filesize = GetFileSize(dss_h_file, &ill_filesize_h);
   if(ill_filesize !=INVALID_FILE_SIZE){
      ill_filesize |= ((HL_LONGLONG) ill_filesize_h) << (8 * 4);
      printf("Checking %s, size 0X%llX\n", ach_filename, ill_filesize);
   }

#endif
#ifdef HL_UNIX
   ims_fd_file = open( ach_filename, O_RDONLY );
   if (ims_fd_file < 0) {
     printf( "Input Open Error: return code = %d.\n", errno );
     return -1;
   }
#endif

   // +------------------------+
   // | Initialize Compression |
   // +------------------------+
   memset( &dss_enc, 0, sizeof(struct dsd_cdf_ctrl) );
#ifdef XYZ1
   dss_enc.boc_eof = FALSE;
#endif
   dss_enc.imc_func = DEF_IFUNC_START;
   dss_enc.amc_aux = &m_cdaux;
   dss_enc.vpc_userfld = &ill_count_mem_enc;
   dss_enc.imc_param_2 = -15;               /* deflate                 */
   m_cdf_enc( &dss_enc );
   char* ach_output_enc = NULL;

#ifdef XYZ1
   // copy eye-catcher
   dsd_gather_i_1 ds_gather_eye_catcher;
   ds_gather_eye_catcher.achc_ginp_cur = (char*) chr_workarea_enc;
   memcpy(ds_gather_eye_catcher.achc_ginp_cur, dss_enc.chrc_eye_catcher, sizeof(dss_enc.chrc_eye_catcher));
   ds_gather_eye_catcher.achc_ginp_end = ds_gather_eye_catcher.achc_ginp_cur + sizeof(dss_enc.chrc_eye_catcher);
   ds_gather_eye_catcher.adsc_next = NULL;
#endif
   if(dss_enc.imc_return > 0){
     printf( "Error! l%05d Error first call m_cdf_enc dss_enc.imc_return = %d.\n", __LINE__, dss_enc.imc_return);
     goto close_file;
   }

   // +--------------------------+
   // | Initialize Decompression |
   // +--------------------------+
   HL_LONGLONG ill_count_mem_dec = 0;
   memset( &dss_dec, 0, sizeof(struct dsd_cdf_ctrl) );
   dss_dec.amc_aux = &m_cdaux;
   dss_dec.vpc_userfld = &ill_count_mem_dec;
   dss_dec.imc_func = DEF_IFUNC_START;
   dss_dec.imc_param_2 = -15;               /* deflate                 */
   {
#ifdef XYZ1
      dss_dec.adsc_gai1_in = &ds_gather_eye_catcher;
      memcpy(dss_dec.chrc_eye_catcher, ds_gather_eye_catcher.achc_ginp_cur, sizeof(dss_enc.chrc_eye_catcher));
      ds_gather_eye_catcher.achc_ginp_cur += sizeof(dss_enc.chrc_eye_catcher);
#endif
      m_cdf_dec(&dss_dec);
   }

   // Initialize statistical counters
   HL_LONGLONG ill_sum_checked = 0;
   HL_LONGLONG ill_sum_compressed = 0;
   HL_LONGLONG ill_sum_read = 0;
   HL_LONGLONG ill_sum_decoded = 0;
#ifdef TRACEHL1
   HL_LONGLONG ill_input_decode = 0;
#endif
   HL_LONGLONG ill_call_enc = 0;
   HL_LONGLONG ill_call_dec = 0;
   int iml_count_it = 0;
   int iml_count_it_dec = 0;
   int iml_count_it_enc = 0;
   HL_LONGLONG ill_time_start;
   HL_LONGLONG ill_time_end;
   HL_LONGLONG ill_time_in_read = 0;
   HL_LONGLONG ill_time_in_compression = 0;
   HL_LONGLONG ill_time_in_decompression = 0;
   HL_LONGLONG ill_time_in_gatheralloc = 0;

   HL_LONGLONG ill_measure_1;
#ifndef HL_UNIX
   HL_LONGLONG ill_freq = 0;
   if(QueryPerformanceFrequency((LARGE_INTEGER *) &ill_freq) == FALSE){
     printf("xbt-cdf-comp-3.cpp-l%05d QueryPerformanceFrequency() Error %d.\n", __LINE__, GetLastError());
     return -1;
   }
   if(ill_freq){
      if(QueryPerformanceCounter((LARGE_INTEGER *) &ill_measure_1) == FALSE){
         printf("xbt-cdf-comp-3.cpp-l%05d QueryPerformanceCounter() measure 1 Error %d.\n", __LINE__, GetLastError());
         return -1;
      }
      ill_measure_1 = (ill_measure_1 * 1000000) / ill_freq;
   }
#else
   struct timeval dsl_timeval;
   gettimeofday( &dsl_timeval, NULL );
   ill_measure_1 = ((HL_LONGLONG) dsl_timeval.tv_sec) * 1000 * 1000 + dsl_timeval.tv_usec;
#endif

#ifdef TRACEHL1
   iml_ret_dec = 0;                         /* returned from decode    */
#endif
   // evaluate eventual errors
   if(dss_enc.imc_return > 0){
     printf( "Error! l%05d Error first call m_cdr_enc dss_enc.imc_return = %d.\n", __LINE__, dss_enc.imc_return);
     goto close_file;
   }
   if(dss_dec.imc_return > 0){
     printf( "Error! l%05d Error first call m_cdr_dec dss_dec.imc_return = %d.\n", __LINE__, dss_dec.imc_return);
     goto close_file;
   }

   achs_area_enc = chrs_compare_area + (sizeof(chrs_compare_area) >> 1);
   achl_compare = achs_area_enc;
   achs_area_dec = chrs_compare_area;
   bos_area_full = FALSE;

   p_enc_20:                                /* continue encode / compression */
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (achs_area_enc == achl_area) {
     achs_area_enc = chrs_compare_area;
   }
   if (achs_area_enc < achs_area_dec) {
     achl_area = achs_area_dec;
   }
   iml_w1 = achl_area - achs_area_enc;
   if (iml_w1 == 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d compare area too small\n",
             __LINE__ );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }
   if (iml_w1 > LEN_INPUT) iml_w1 = LEN_INPUT;
#ifndef TRY_200325_02
   iml_read = m_get_random_number( iml_w1 ) + 1;
   iml_w1 = (achl_area - achs_area_enc) - MIN_LEN_B - iml_read;
   if (iml_w1 < 0) iml_read += iml_w1;
#endif
#ifdef TRY_200325_02
   iml_read = iml_w1;
#endif
   achl_area = achs_area_enc;

   // measure time
   MEASURE_TIME( &ill_time_start );

#ifdef HL_WINALL1
   bol_rc = ReadFile( dss_h_file, achl_area, iml_read, &uml_read, 0 );
   if (bol_rc == FALSE) {                   /* error occured           */
     printf( "xbt-cdf-comp-3.cpp-l%05d Data Read Error: %d.\n",
             __LINE__, GetLastError() );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }
   iml_w1 = uml_read;
#endif
#ifdef HL_UNIX
   iml_rc = read( ims_fd_file, achl_area, iml_read );
   if (iml_rc < 0) {                        /* error occured           */
     printf( "xbt-cdf-comp-3.cpp-l%05d Input Read Error: return code = %d.\n",
             __LINE__, errno );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }
   iml_w1 = iml_rc;
#endif
   achs_area_enc += iml_w1;
   ill_sum_checked += iml_w1;
   if (   (achl_area < achs_area_dec)
       && (iml_w1 > 0)
       && (achs_area_enc == achs_area_dec)) {
     bos_area_full = TRUE;
   }
   if (iml_w1 == 0) {                       /* no data read            */
     dss_enc.boc_eof = TRUE;
#ifdef TRACEHL1
     printf( "xbt-cdf-comp-3.cpp-l%05d read file returned E-O-F.\n",
             __LINE__ );
#endif
   }

   // measure time
   MEASURE_TIME( &ill_time_end );
   ill_time_in_read += ill_time_end - ill_time_start;

#ifndef INPUT_ONE_CHUNK
   iml_w2 = (sizeof(dsrs_gai1_enc) / sizeof(dsrs_gai1_enc[0])) - 1;
   achl_w1 = achl_area;
   adsl_gai1_w1 = &dsrs_gai1_enc[0];
   iml_w3 = iml_w1;
   while (TRUE) {
     adsl_gai1_w1->achc_ginp_cur = achl_w1;
     adsl_gai1_w1->adsc_next = adsl_gai1_w1 + 1;
     iml_w4 = iml_w1 >> 2;
     if (iml_w4 > 0) {
       iml_w4 = m_get_random_number( iml_w4 );
     }
     iml_w4++;
     if (   (iml_w4 >= iml_w3)
         || (iml_w2 == 0)) {
       adsl_gai1_w1->achc_ginp_end = achl_w1 + iml_w3;
       adsl_gai1_w1->adsc_next = NULL;
       break;
     }
     adsl_gai1_w1->achc_ginp_end = achl_w1 + iml_w4;
     achl_w1 += iml_w3;
     iml_w4 -= iml_w4;
     adsl_gai1_w1++;
     iml_w2--;
   };
#endif
#ifdef INPUT_ONE_CHUNK
   dsrs_gai1_enc[0].adsc_next = NULL;
   dsrs_gai1_enc[0].achc_ginp_cur = achl_area;
   dsrs_gai1_enc[0].achc_ginp_end = achl_area + iml_w1;
#endif

   dss_enc.adsc_gai1_in = &dsrs_gai1_enc[0];

   p_enc_40:                                /* call sub-routine encode */

   dss_enc.achc_out_cur = chrs_out_enc;
#ifndef TRY_200325_03
   iml_w1 = m_get_random_number( sizeof(chrs_out_enc) ) + 1;
#ifdef XYZ1
   if (iml_w1 < MIN_LEN_B) iml_w1 += MIN_LEN_B;
#endif
#endif
#ifdef TRY_200325_03
   iml_w1 = sizeof(chrs_out_enc);
#endif
   dss_enc.achc_out_end = chrs_out_enc + iml_w1;

   // measure time
   MEASURE_TIME( &ill_time_start );

   // compress
   m_cdf_enc( &dss_enc );

   // measure time
   MEASURE_TIME( &ill_time_end );
   ill_time_in_compression += ill_time_end - ill_time_start;

   ill_call_enc++;

   while (dss_enc.adsc_gai1_in) {
     if (dss_enc.adsc_gai1_in->achc_ginp_cur < dss_enc.adsc_gai1_in->achc_ginp_end) {
       break;
     }
     dss_enc.adsc_gai1_in = dss_enc.adsc_gai1_in->adsc_next;
   }

   if (dss_enc.imc_return != DEF_IRET_NORMAL) {  /* continue processing */
#ifdef TRACEHL1
     printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_enc() DEF_IRET_END.\n",
             __LINE__ );
#endif
     if (   (dss_enc.imc_return != DEF_IRET_END)  /* subroutine has ended processing */
         || (dss_enc.boc_eof == FALSE)
         || (dss_enc.adsc_gai1_in != NULL)) {
       printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_enc() imc_return %d.\n",
               __LINE__, dss_enc.imc_return );
       printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
               __LINE__, ill_call_enc, ill_call_dec );
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
     }
#ifdef XYZ1
     dss_dec.boc_eof = TRUE;
#endif
   }

   achl_input_dec = chrs_out_enc;           /* input to decode         */

   iml_w1 = dss_enc.achc_out_cur - chrs_out_enc;
   if (iml_w1 == 0) {                       /* no output               */
     if (dss_enc.imc_return == DEF_IRET_NORMAL) {  /* continue processing */
#ifdef XYZ1
       goto p_enc_40;                       /* call sub-routine encode */
#endif
       if (dss_enc.adsc_gai1_in == NULL) {
         goto p_enc_20;                     /* continue encode / compression */
       }
       printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_enc() returned no output.\n",
               __LINE__ );
       printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
               __LINE__, ill_call_enc, ill_call_dec );
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
     }
#ifdef TRY_200401_01
     achl_area = chrs_compare_area + sizeof(chrs_compare_area);
     if (achs_area_dec == achl_area) {
       achs_area_dec = chrs_compare_area;
     }
     iml_w2 = achl_area - achs_area_dec;
     if (iml_w2 <= 0) {
       printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
               __LINE__ );
       *((char *) iml_w2) = 0;              /* crash                   */
     }
#endif
     goto p_dec_40;                         /* call sub-routine decode */
   }

   ill_sum_compressed += iml_w1;

#ifdef TRACEHL1
   imt_compressed_1 = iml_w1;
   imt_compressed_2 = 0;
   imt_compressed_3 = 0;
   acht_compressed = achs_area_dec;
   printf( "xbt-cdf-comp-3.cpp-l%05d compressed=%p.\n",
           __LINE__, imt_compressed_1 );
#endif

#ifdef TRACEHL1
   m_console_out( chrs_out_enc, iml_w1 );
#endif

   achl_w1 = achl_input_dec;

#ifdef TRY_200401_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (achs_area_dec == achl_area) {
     achs_area_dec = chrs_compare_area;
   }
   iml_w2 = achl_area - achs_area_dec;
   if (iml_w2 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w2) = 0;                /* crash                   */
   }
#endif

   p_dec_20:                                /* continue decode / de-compression */

#ifdef TRY_200401_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (achs_area_dec == achl_area) {
     achs_area_dec = chrs_compare_area;
   }
   iml_w2 = achl_area - achs_area_dec;
   if (iml_w2 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w2) = 0;                /* crash                   */
   }
#endif

#ifndef TRY_200325_02
   iml_w2 = m_get_random_number( iml_w1 ) + 1;
#ifndef TRY_200325_01
   if (iml_w2 >= MIN_LEN_B) {
     iml_w1 = iml_w2;
   }
#endif
#ifdef TRY_200325_01
   if ((iml_w1 - iml_w2) > MIN_LEN_B) {
     iml_w1 = iml_w2;
   }
#endif
#endif
   achl_input_dec += iml_w1;                /* compressed data         */
#ifdef TRACEHL1
   ill_input_decode += iml_w1;
#endif

#ifndef INPUT_ONE_CHUNK
   iml_w2 = (sizeof(dsrs_gai1_dec) / sizeof(dsrs_gai1_dec[0])) - 1;
   achl_w1 = chrs_out_enc;
   adsl_gai1_w1 = &dsrs_gai1_dec[0];
   iml_w3 = iml_w1;
   while (TRUE) {
     adsl_gai1_w1->achc_ginp_cur = achl_w1;
     adsl_gai1_w1->adsc_next = adsl_gai1_w1 + 1;
     iml_w4 = iml_w1 >> 2;
     if (iml_w4 > 0) {
       iml_w4 = m_get_random_number( iml_w4 );
     }
     iml_w4++;
     if (   (iml_w4 >= iml_w3)
         || (iml_w2 == 0)) {
       adsl_gai1_w1->achc_ginp_end = achl_w1 + iml_w3;
       adsl_gai1_w1->adsc_next = NULL;
       break;
     }
     adsl_gai1_w1->achc_ginp_end = achl_w1 + iml_w4;
     achl_w1 += iml_w3;
     iml_w4 -= iml_w4;
     adsl_gai1_w1++;
     iml_w2--;
   };
#endif
#ifdef INPUT_ONE_CHUNK
   dsrs_gai1_dec[0].adsc_next = NULL;
   dsrs_gai1_dec[0].achc_ginp_cur = achl_w1;
   dsrs_gai1_dec[0].achc_ginp_end = achl_w1 + iml_w1;
#endif

#ifdef TRACEHL1
   printf( "xbt-cdf-comp-3.cpp-l%05d old=%p add=%p sum=%p.\n",
           __LINE__, imt_compressed_2, iml_w1, imt_compressed_1 );
   imt_compressed_2 += iml_w1;
   if ((imt_compressed_2 + (dss_enc.achc_out_cur - achl_input_dec)) != imt_compressed_1) {
     iml_w2 = dss_enc.achc_out_cur - achl_input_dec;
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p iml_w1=%p iml_w2=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2, iml_w1, iml_w2 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

#ifdef TRY_200401_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   iml_w2 = achl_area - achs_area_dec;
   if (iml_w2 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w2) = 0;                /* crash                   */
   }
#endif

   dss_dec.adsc_gai1_in = &dsrs_gai1_dec[0];

   achl_area = chrs_compare_area + sizeof(chrs_compare_area);

   if (achl_input_dec == achl_area) {
     achl_input_dec = chrs_compare_area;
   }

   p_dec_40:                                /* call sub-routine decode */

#ifdef TRY_200401_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
#endif
#ifdef TRY_200402_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
#endif

#ifdef TRY_200403_02
   if (achs_area_dec > achl_area) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
   if (achs_area_dec < chrs_compare_area) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too low.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

   if (achs_area_dec == achl_area) {
     achs_area_dec = chrs_compare_area;
   }

   iml_w1 = achl_area - achs_area_dec;
#ifndef TRY_200403_03
   if (achl_compare < achs_area_dec) {
     iml_w1 = achl_compare - achs_area_dec;
   }
#endif
#ifdef TRY_200403_03
   if (achl_compare > achs_area_dec) {
     iml_w1 = achl_compare - achs_area_dec;
   }
#endif

#ifndef TRY_200403_02
#ifndef TRY_200403_03
   if (achl_compare < achs_area_dec) {
     achl_area = achl_compare;
   }
   iml_w1 = achl_area - achs_area_dec;
#endif
#ifdef TRY_200402_01
   if (iml_w1 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#endif
   if (iml_w1 > LEN_INPUT) iml_w1 = LEN_INPUT;
   dss_dec.achc_out_cur = achs_area_dec;
#ifndef TRY_200403_02
#ifndef TRY_200403_01
#ifndef TRY_200402_01
#ifndef TRY_200401_01
   iml_w1 = m_get_random_number( iml_w1 ) + 1;
#endif
#endif
   iml_w2 = (achl_area - achs_area_dec) - iml_w1 - MIN_LEN_B;
   if (iml_w2 < 0) iml_w1 = MIN_LEN_B;
   iml_w2 = (achl_area - achs_area_dec) - iml_w1;
#ifdef XYZ1
#ifdef TRY_200401_01
   if (iml_w2 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w2) = 0;                /* crash                   */
   }
#endif
#endif
   if (iml_w2 < MIN_LEN_B) iml_w1 += iml_w2;
#ifdef TRY_200401_01
   if (iml_w1 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#endif
#endif
   dss_dec.achc_out_end = achs_area_dec + iml_w1;
#ifdef TRY_200402_01
   if (achs_area_dec > (chrs_compare_area + sizeof(chrs_compare_area))) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
   if (achs_area_dec < chrs_compare_area) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too low.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#ifdef TRACEHL1
   if (iml_w1 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d dss_dec.achc_out_end - cur too low.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
   if (dss_dec.achc_out_cur == dss_dec.achc_out_end) {
     printf( "xbt-cdf-comp-3.cpp-l%05d dss_dec.achc_out_cur == end invalid.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
   if (dss_dec.achc_out_cur > dss_dec.achc_out_end) {
     printf( "xbt-cdf-comp-3.cpp-l%05d dss_dec.achc_out_cur greater end invalid.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (dss_dec.achc_out_end > achl_area) {
     printf( "xbt-cdf-comp-3.cpp-l%05d dss_dec.achc_out_end too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

   // measure time
   MEASURE_TIME( &ill_time_start );

   // Call Decompression
   m_cdf_dec( &dss_dec );

   // measure time
   MEASURE_TIME( &ill_time_end );
   ill_time_in_decompression += (ill_time_end - ill_time_start);

   ill_call_dec++;

#ifdef TRACEHL1
   if (dss_dec.achc_out_cur > dss_dec.achc_out_end) {
     printf( "xbt-cdf-comp-3.cpp-l%05d dss_dec.achc_out_cur greater end invalid.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

   if (dss_dec.imc_return != DEF_IRET_NORMAL) {  /* continue processing */
//#ifdef XYZ1
     if (dss_dec.imc_return != DEF_IRET_END) {  /* subroutine has ended processing */
       printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() imc_return %d.\n",
               __LINE__, dss_dec.imc_return );
       printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
               __LINE__, ill_call_enc, ill_call_dec );
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
     }
//#endif
   }

#ifdef TRACEHL1
   imt_compressed_3 = dss_dec.achc_out_cur - acht_compressed;
   printf( "xbt-cdf-comp-3.cpp-l%05d len=%p add=%p end=%p.\n",
           __LINE__, acht_compressed, imt_compressed_3, dss_dec.achc_out_cur );
   m_console_out( acht_compressed, imt_compressed_3 );
   acht_compressed = dss_dec.achc_out_cur;
   if ((imt_compressed_2 + (dss_enc.achc_out_cur - achl_input_dec)) != imt_compressed_1) {
     iml_w2 = dss_enc.achc_out_cur - achl_input_dec;
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p iml_w1=%p iml_w2=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2, iml_w1, iml_w2 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

   while (dss_dec.adsc_gai1_in) {
     if (dss_dec.adsc_gai1_in->achc_ginp_cur < dss_dec.adsc_gai1_in->achc_ginp_end) {
       break;
     }
     dss_dec.adsc_gai1_in = dss_dec.adsc_gai1_in->adsc_next;
   }

   achl_area = chrs_compare_area + sizeof(chrs_compare_area);

   iml_w1 = dss_dec.achc_out_cur - achs_area_dec;
   if (iml_w1 == 0) {                       /* no output               */
     if (dss_dec.adsc_gai1_in == NULL) {
       if (   (dss_enc.boc_eof == FALSE)
           && (dss_enc.imc_return == DEF_IRET_NORMAL)) {  /* continue processing */
         achl_w1 = achl_input_dec;
         iml_w1 = dss_enc.achc_out_cur - achl_input_dec;
         if (iml_w1 > 0) {
#ifdef TRY_200401_01
           achl_area = chrs_compare_area + sizeof(chrs_compare_area);
           if (achs_area_dec == achl_area) {
             achs_area_dec = chrs_compare_area;
           }
           iml_w2 = achl_area - achs_area_dec;
           if (iml_w2 <= 0) {
             printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
                     __LINE__ );
             *((char *) iml_w2) = 0;        /* crash                   */
           }
#endif
           goto p_dec_20;                   /* continue decode / de-compression */
         }
         if (dss_enc.adsc_gai1_in != NULL) {
           goto p_enc_40;                   /* call sub-routine encode */
         }
         goto p_enc_20;                     /* continue encode / compression */
       }
#ifdef XYZ1
       printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() returned no bytes.\n",
               __LINE__ );
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
#endif
     }
#ifdef TRY_200401_01
     achl_area = chrs_compare_area + sizeof(chrs_compare_area);
     if (achs_area_dec == achl_area) {
       achs_area_dec = chrs_compare_area;
     }
     iml_w2 = achl_area - achs_area_dec;
     if (iml_w2 <= 0) {
       printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
               __LINE__ );
       *((char *) iml_w2) = 0;              /* crash                   */
     }
#endif
     if (dss_dec.imc_return == DEF_IRET_NORMAL) {  /* continue processing */
       goto p_dec_40;                       /* call sub-routine decode */
     }
     printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() returned too less bytes.\n",
             __LINE__ );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }

   if (iml_w1 == 0) {                       /* no data returned        */
     goto p_dec_60;                         /* data have been compared */
   }

#ifdef TRY_200324_01
   iml_w2 = achs_area_enc - achl_compare;
   if (iml_w2 != iml_w1) {
     iml_w3 = dss_enc.achc_out_cur - achl_input_dec;
     if (iml_w3 > 0) {
       iml_w1 = iml_w3;
       achl_w1 = achl_input_dec;
#ifdef TRY_200401_01
       achl_area = chrs_compare_area + sizeof(chrs_compare_area);
       if (achs_area_dec == achl_area) {
         achs_area_dec = chrs_compare_area;
       }
       iml_w2 = achl_area - achs_area_dec;
       if (iml_w2 <= 0) {
         printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
                 __LINE__ );
         *((char *) iml_w2) = 0;            /* crash                   */
       }
#endif
       goto p_dec_20;                       /* continue decode / de-compression */
     }
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p iml_w1=%p iml_w3=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2, iml_w1, iml_w3 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#ifdef TRY_200324_02
   if (imt_compressed_2 != imt_compressed_1) {
     iml_w3 = dss_enc.achc_out_cur - achl_input_dec;
     if (iml_w3 > 0) {
       iml_w1 = iml_w3;
       achl_w1 = achl_input_dec;
#ifdef TRY_200401_01
       achl_area = chrs_compare_area + sizeof(chrs_compare_area);
       if (achs_area_dec == achl_area) {
         achs_area_dec = chrs_compare_area;
       }
       iml_w2 = achl_area - achs_area_dec;
       if (iml_w2 <= 0) {
         printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
                 __LINE__ );
         *((char *) iml_w2) = 0;            /* crash                   */
       }
#endif
       goto p_dec_20;                       /* continue decode / de-compression */
     }
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p iml_w1=%p iml_w3=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2, iml_w1, iml_w3 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#ifdef TRACEHL1
   if ((imt_compressed_2 + (dss_enc.achc_out_cur - achl_input_dec)) != imt_compressed_1) {
     iml_w2 = dss_enc.achc_out_cur - achl_input_dec;
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p iml_w1=%p iml_w2=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2, iml_w1, iml_w2 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#ifdef XYZ1
   if (imt_compressed_2 != imt_compressed_1) {
     printf( "xbt-cdf-comp-3.cpp-l%05d data lost / imt_compressed_1=%p imt_compressed_2=%p.\n",
             __LINE__, imt_compressed_1, imt_compressed_2 );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#endif

   /* compare original with after de-compression                       */

   if (achl_compare == achl_area) {
     achl_compare = chrs_compare_area;
   }
   do {
     iml_w2 = achs_area_enc - achl_compare;
     if (achl_compare >= achs_area_enc) {
       iml_w2 = achl_area - achl_compare;
     }
     if (iml_w2 > iml_w1) iml_w2 = iml_w1;  /* only size output decode */
#ifndef TRY_200405_01
     iml_w3 = achl_area - achs_area_dec;
     if (iml_w2 > iml_w3) iml_w2 = iml_w3;  /* only size till end area */
#endif
     iml_w3 = memcmp( achs_area_dec, achl_compare, iml_w2 );
     if (iml_w3 != 0) {                     /* not equal               */
       printf( "xbt-cdf-comp-3.cpp-l%05d data do not compare\n",
               __LINE__ );
       printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
               __LINE__, ill_call_enc, ill_call_dec );
#ifdef TRACEHL1
       printf( "xbt-cdf-comp-3.cpp-l%05d output-encode=%llx input-decode=%llx.\n",
               __LINE__, ill_sum_compressed, ill_input_decode );
#endif
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
     }
     achl_compare += iml_w2;
     achs_area_dec += iml_w2;
     iml_w1 -= iml_w2;
     if (achl_compare == achl_area) {
       achl_compare = chrs_compare_area;
     }
     if (achs_area_dec == achl_area) {
       achs_area_dec = chrs_compare_area;
     }
   } while (iml_w1 > 0);

   bos_area_full = FALSE;

#ifdef TRY_200401_01
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (achs_area_dec == achl_area) {
     achs_area_dec = chrs_compare_area;
   }
   iml_w1 = achl_area - achs_area_dec;
   if (iml_w1 <= 0) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif
#ifdef TRACEHL1
   achl_area = chrs_compare_area + sizeof(chrs_compare_area);
   if (achs_area_dec > achl_area) {
     printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
             __LINE__ );
     *((char *) iml_w1) = 0;                /* crash                   */
   }
#endif

   p_dec_60:                                /* data have been compared */

#ifdef TRY_200405_02
   if (dss_dec.adsc_gai1_in != NULL) {
     goto p_dec_40;                         /* call sub-routine decode */
   }
   iml_w1 = dss_enc.achc_out_cur - achl_input_dec;
   achl_w1 = achl_input_dec;
   if (iml_w1 > 0) {
     goto p_dec_20;                         /* continue decode / de-compression */
   }
#endif

#ifdef TRY_200406_01
   if (dss_dec.adsc_gai1_in != NULL) {
     goto p_dec_40;                         /* call sub-routine decode */
   }
   iml_w1 = dss_enc.achc_out_cur - achl_input_dec;
   achl_w1 = achl_input_dec;
   if (iml_w1 > 0) {
     goto p_dec_20;                       /* continue decode / de-compression */
   }
#endif

   if (   (dss_enc.boc_eof == FALSE)
       && (dss_enc.imc_return == DEF_IRET_NORMAL)) {  /* continue processing */
#ifndef TRY_200406_01
#ifndef TRY_200405_02
     if (dss_dec.adsc_gai1_in != NULL) {
#ifdef TRY_200401_01
       achl_area = chrs_compare_area + sizeof(chrs_compare_area);
       if (achs_area_dec == achl_area) {
         achs_area_dec = chrs_compare_area;
       }
       iml_w2 = achl_area - achs_area_dec;
       if (iml_w2 <= 0) {
         printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
                 __LINE__ );
         *((char *) iml_w2) = 0;            /* crash                   */
       }
#endif
       goto p_dec_40;                       /* call sub-routine decode */
     }
     iml_w1 = dss_enc.achc_out_cur - achl_input_dec;
     achl_w1 = achl_input_dec;
     if (iml_w1 > 0) {
#ifdef TRY_200401_01
       achl_area = chrs_compare_area + sizeof(chrs_compare_area);
       if (achs_area_dec == achl_area) {
         achs_area_dec = chrs_compare_area;
       }
       iml_w2 = achl_area - achs_area_dec;
       if (iml_w2 <= 0) {
         printf( "xbt-cdf-comp-3.cpp-l%05d achs_area_dec too high.\n",
                 __LINE__ );
         *((char *) iml_w2) = 0;            /* crash                   */
       }
#endif
       goto p_dec_20;                       /* continue decode / de-compression */
     }
#endif
#endif
     if (dss_dec.imc_return != DEF_IRET_NORMAL) {  /* do not continue processing */
       printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() returned END but still data.\n",
               __LINE__ );
       printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
               __LINE__, ill_call_enc, ill_call_dec );
       bos_abend = TRUE;                    /* abnormal end of program */
       goto close_file;
     }
     if (dss_enc.adsc_gai1_in != NULL) {
       goto p_enc_40;                       /* call sub-routine encode */
     }
     goto p_enc_20;                         /* continue encode / compression */
   }
#ifdef XYZ1
#ifdef TRY_200405_02
   if (dss_dec.imc_return == DEF_IRET_NORMAL) {  /* do not continue processing */
     printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() did not return END but no more data.\n",
             __LINE__ );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }
#endif
#endif
#ifndef TRY_200405_04
#ifdef TRY_200405_03
   if (   (dss_dec.imc_return == DEF_IRET_NORMAL)  /* do not continue processing */
       && (dss_dec.boc_eof == FALSE)) {
     dss_dec.boc_eof = TRUE;
     goto p_dec_40;                         /* call sub-routine decode */
   }
#endif
#endif
#ifndef TRY_200405_04
   if (achl_compare != achs_area_enc) {
     printf( "xbt-cdf-comp-3.cpp-l%05d m_cdf_dec() returned too less bytes.\n",
             __LINE__ );
     printf( "xbt-cdf-comp-3.cpp-l%05d calls_enc=0x%05llx calls_dec=0x%05llx.\n",
             __LINE__, ill_call_enc, ill_call_dec );
     bos_abend = TRUE;                      /* abnormal end of program */
     goto close_file;
   }
#endif
#ifdef TRY_200405_04
   if (   (achl_compare != achs_area_enc)
       && (dss_dec.imc_return == DEF_IRET_NORMAL)  /* do not continue processing */
       && (dss_dec.boc_eof == FALSE)) {
     dss_dec.boc_eof = TRUE;
     goto p_dec_40;                         /* call sub-routine decode */
   }
#endif
//---
   // End encoder and decoder
   dss_enc.imc_func = DEF_IFUNC_END;
   m_cdf_enc(&dss_enc);
   dss_dec.imc_func = DEF_IFUNC_END;
   m_cdf_dec(&dss_dec);

//---

   HL_LONGLONG ill_measure_2;
#ifndef HL_UNIX
   if(ill_freq){
      if(QueryPerformanceCounter((LARGE_INTEGER *) &ill_measure_2) == FALSE){
         printf( "xbt-cdf-comp-3.cpp-l%05d QueryPerformanceCounter() measure 2 Error %d.\n", __LINE__, GetLastError());
         return -1;
      }
      ill_measure_2 = (ill_measure_2 * 1000000) / ill_freq;
   }
#else
   gettimeofday( &dsl_timeval, NULL );
   ill_measure_2 = ((HL_LONGLONG) dsl_timeval.tv_sec) * 1000 * 1000 + dsl_timeval.tv_usec;
#endif

   HL_LONGLONG ill_whole_time = ill_measure_2 - ill_measure_1;
   printf("Checked  %s, time=%lli micro-seconds\n", ach_filename, ill_whole_time);
   printf("  ill_time_in_read         =%lli micro-seconds (%i%%)\n", ill_time_in_read, (int)((ill_time_in_read * 100) / ill_whole_time));
   printf("  ill_time_in_compression  =%lli micro-seconds (%i%%)\n", ill_time_in_compression, (int)((ill_time_in_compression * 100) / ill_whole_time));
   printf("  ill_time_in_decompression=%lli micro-seconds (%i%%)\n", ill_time_in_decompression, (int)((ill_time_in_decompression * 100) / ill_whole_time));
   HL_LONGLONG ill_resttime = ill_whole_time - ill_time_in_read - ill_time_in_compression - ill_time_in_decompression;
   printf("  ill_resttime             =%lli micro-seconds (%i%%)\n", ill_resttime, (int)((ill_resttime * 100) / ill_whole_time));

   printf(" size_uncomp=0x%05llx size_compr =0x%05llx (%i%%)\n",
      ill_sum_checked, ill_sum_compressed,
      ill_sum_checked == 0 ? 0xFF : (int) (100 * ill_sum_compressed / ill_sum_checked + 0.5));
   printf("                       calls_enc=0x%05llx   calls_dec=0x%05llx\n", ill_call_enc, ill_call_dec);
   fflush( stdout );
// bo_return = TRUE;

   close_file:

#ifdef HL_WINALL1
   if(CloseHandle(dss_h_file) == FALSE){
      printf( "CloseHandle Error: %d\n", GetLastError());
   }
#endif
#ifdef HL_UNIX
   if(close(ims_fd_file) < 0){
      printf( "Input Close Error: return code = %d\n", errno );
   }
#endif
// if(bo_return == TRUE)
      return 0;
// return -1;
} /* end main()                                                        */

/* subroutine for miscellaneous functions                              */
static BOOL m_cdaux( void * vpp_struct, int imp_func, void * achp_addr, int imp_length ) {
#define X_AUADDR  *((void **) achp_addr)
   if (imp_func == DEF_AUX_MEMGET) {
     if (vpp_struct == &dss_enc) {
     }
     if (vpp_struct == &dss_dec) {
     }
     X_AUADDR = malloc( imp_length );
     if (X_AUADDR) return TRUE;
     return FALSE;
   }
   if (imp_func == DEF_AUX_MEMFREE) {
     free( X_AUADDR );
     return TRUE;
   }
   return FALSE;
} /* end m_cdaux()                                                     */

/* this routine returns a number between zero and impmax minus one     */
/* 0 <= ret-val < impmax                                               */
static int m_get_random_number( int impmax ) {
   HL_LONGLONG ill1;

   ill1 = (HL_LONGLONG) rand() * impmax;
   ill1 /= RAND_MAX + 1;
   return (int) ill1;
} /* end m_get_random_number()                                         */

#ifdef TRACEHL_DUMP
/* subroutine to dump storage-content to console                       */
static void m_console_out( char *achp_buff, int implength ) {
   int        iml1, iml2, iml3, iml4, iml5, iml6;  /* working variable */
   char       byl1;                         /* working-variable        */
   char       chrlwork1[ 76 ];              /* buffer to print         */

   iml1 = 0;
   while (iml1 < implength) {
     iml2 = iml1 + 16;
     if (iml2 > implength) iml2 = implength;
     for ( iml3 = 4; iml3 < 75; iml3++ ) {
       chrlwork1[iml3] = ' ';
     }
     chrlwork1[58] = '*';
     chrlwork1[75] = '*';
     iml3 = 4;
     do {
       iml3--;
       chrlwork1[ iml3 ] = chrstrans[ (iml1 >> ((4 - 1 - iml3) << 2)) & 0X0F ];
     } while (iml3 > 0);
     iml4 = 6;                              /* start hexa digits here  */
     iml5 = 59;                             /* start ASCII here        */
     iml6 = 4;                              /* times normal            */
     do {
       byl1 = achp_buff[ iml1++ ];
       chrlwork1[ iml4++ ] = chrstrans[ (byl1 >> 4) & 0X0F ];
       chrlwork1[ iml4++ ] = chrstrans[ byl1 & 0X0F ];
       iml4++;
       if (byl1 > 0X20) {
         chrlwork1[ iml5 ] = byl1;
       }
       iml5++;
       iml6--;
       if (iml6 == 0) {
         iml4++;
         iml6 = 4;
       }
     } while (iml1 < iml2);
     printf( "%.*s\n", sizeof(chrlwork1), chrlwork1 );
   }
} /* end m_console_out()                                            */
#endif
