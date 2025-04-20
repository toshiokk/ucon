#ifndef mydebug_h
#define mydebug_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

// debug macros

#define DEBUG_BUF_LEN		2048

#ifdef ENABLE_DEBUG

///#define FORCE_DEBUG

#define _FL		fprintf(stderr, "%s %d ", __FILE__, __LINE__)
#define _FL_	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, NULL, NULL, "\n");
#define _FLF	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __FUNCTION__, NULL, "")
#define _FLF_	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __FUNCTION__, NULL, "\n");
#define _TFLF_	tflf_debug_printf(LOG_TIME_YMD_HMS, __FILE__, __LINE__, __FUNCTION__, NULL, "\n");
#define _MFLF_	tflf_debug_printf(LOG_TIME_HMSM, __FILE__, __LINE__, __FUNCTION__, NULL, "\n");
#define _UFLF_	tflf_debug_printf(LOG_TIME_HMSU, __FILE__, __LINE__, __FUNCTION__, NULL, "\n");
#define _FLCF	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __PRETTY_FUNCTION__, NULL, "")
#define _FLCF_	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __PRETTY_FUNCTION__, NULL, "\n");
#define _FLF0_	tflf_debug_printf(LOG_TIME_HMSM, __FILE__, __LINE__, __FUNCTION__, NULL, "{\n");
#define _FLF9_	tflf_debug_printf(LOG_TIME_HMSM, __FILE__, __LINE__, __FUNCTION__, NULL, "}\n");
#define _ERR_	fprintf(stderr, "%s %d %s() !! ERROR !!\n", __FILE__, __LINE__, __FUNCTION__);
#define d_printf(args...)		tflf_debug_printf(LOG_TIME_NONE, NULL, 0, NULL, NULL, args)
#define mflf_d_printf(args...)	tflf_debug_printf(LOG_TIME_HMSM, __FILE__, __LINE__, __FUNCTION__, NULL, args)
#define flcf_d_printf(args...)	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __PRETTY_FUNCTION__, NULL, args)
#define flf_d_printf(args...)	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __FUNCTION__, NULL, args)
#define fl_d_printf(args...)	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, NULL, NULL, args)
#define f_d_printf(args...)		tflf_debug_printf(LOG_TIME_NONE, NULL, 0, __FUNCTION__, NULL, args)
#define _d_printf(args...)		tflf_debug_printf(LOG_TIME_NONE, NULL, 0, NULL, NULL, args)
#define warn_printf(args...)	tflf_debug_printf(LOG_TIME_NONE, __FILE__, __LINE__, __FUNCTION__, "WARN", args)
#define v_printf(args...)		tflf_debug_printf(LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, NULL, args)
#define se_printf(args...)		tflf_debug_printf(LOG_STRERR, __FILE__, __LINE__, __FUNCTION__, NULL, args)
#define er_printf(args...)		tflf_debug_printf(LOG_STDERR, __FILE__, __LINE__, __FUNCTION__, NULL, args)

#else // ENABLE_DEBUG

#define _FL
#define _FL_
#define _FLF
#define _FLF_
#define _TFLF_
#define _MFLF_
#define _UFLF_
#define _FLCF
#define _FLCF_
#define _FLF0_
#define _FLF9_
#define _ERR_
#define d_printf(args...)
#define mflf_d_printf(args...)
#define flcf_d_printf(args...)
#define flf_d_printf(args...)
#define fl_d_printf(args...)
#define f_d_printf(args...)
#define warn_printf(args...)
#define v_printf(args...)
#define se_printf(args...)
#define er_printf(args...)

#endif // ENABLE_DEBUG

#define LOG_TIME_NONE		0
#define LOG_TIME_YMD_HMS	1
#define LOG_TIME_YMD_HMSM	2
#define LOG_TIME_HMSU		3
#define LOG_TIME_HMSM		4

#define LOG_VERBOSE			7
#define LOG_STRERR			8
#define LOG_STDERR			9

void tflf_debug_printf(int usec, const char *file, int line, const char *func,
 const char *tag, const char *format, ...);
void tflft_vsnprintf(char *buffer, int buf_len, int usec, const char *file, int line,
 const char *func, const char *tag, const char *format, va_list ap);
char *get_tflft_string(char *buffer, int usec, const char *file, int line,
 const char *func, const char *tag);
const char *mk_file_line_str_s_(const char *file, int line);
const char *mk_file_line_str_buf(const char *file, int line, char *buf);

char *get_cur_yysmmsdd_hhcmmcsspmmm(char *buffer);
char *get_cur_hhcmmcsspmmm(char *buffer);
char *get_cur_yyyysmmsdd_hhcmmcss(char *buffer);
char *get_cur_yysmmsdd_hhcmmcss(char *buffer);
char *get_cur_hhcmmcsspuuuuuu(char *buffer);
char *get_cur_yyyysmmsdd(char *buf);
char *get_cur_hhcmmcss(char *buffer);
char *get_hhcmmcss(time_t abs_time, char *buf);
char *get_yyyysmmsdd(time_t abs_time, char *buf);
char *get_yysmmsdd(time_t abs_time, char *buf);
char *get_hhcmmcss_yysmmsdd(time_t abs_time, char *buf);
char *get_yysmmsdd_hhcmm(time_t abs_time, char *buf);
char *get_yysmmsdd_hhcmmcss(time_t abs_time, char *buf);
char *get_yyyysmmsdduhhcmmcss(time_t abs_time, char *buf);
char *get_yyyysmmsdd_hhcmmcss(time_t abs_time, char *buf);
char *get_yymmdd_hhmmss(time_t abs_time, char *buf);

time_t get_cur_abs_sec(void);
unsigned long get_cur_abs_msec(void);

const char *dump_string_to_static_buf(const char *string, int bytes);
const char *dump_string(const char *string, int bytes);

void verbose_printf(const char *format, ...);
void strerror_printf(const char *format, ...);
void stderr_printf(const char *format, ...);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // mydebug_h

// End of mydebug.h
