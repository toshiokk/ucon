/*****************************************************************************
	mydebug.c -- Debug printf
 *****************************************************************************/

#include "utilincs.h"

#ifdef ENABLE_DEBUG
#warning "!!!! ENABLE_DEBUG defined !!!!"
#endif // ENABLE_DEBUG

void tflf_debug_printf(int usec, const char *file, int line, const char *func,
 const char *tag, const char *format, ...)
{
	char buffer[DEBUG_BUF_LEN+1];
	va_list list;

	// stderr_tflf_printf_() will be called from signal handler.
	va_start(list, format);
	tflft_vsnprintf(buffer, DEBUG_BUF_LEN, usec, file, line, func, tag, format, list);
	va_end(list);
	fputs(buffer, stderr);
	fflush(stderr);
}
//------------------------------------------------------------------------------
void tflft_vsnprintf(char *buffer, int buf_len, int usec, const char *file, int line,
 const char *func, const char *tag, const char *format, va_list ap)
{
	int len_tflf;

	get_tflft_string(buffer, usec, file, line, func, tag);
	len_tflf = strnlen(buffer, buf_len);
	vsnprintf(&buffer[len_tflf], buf_len - len_tflf, format, ap);
}

// usec file     line func   tag ==> output
// ---- -------- ---- ------ ----- -------------------------------------------
//   0  NULL        0 NULL   NULL  ""
//   1  NULL        0 NULL   NULL  "10/09/16 14:00:00, "
//   2  NULL        0 NULL   NULL  "10/09/16 14:00:00.123, "
//   3  NULL        0 NULL   NULL  "14:00:00.123456, "
//   0  "file.c"   10 NULL   NULL  "file.c 10, "
//   1  "file.c"   10 NULL   NULL  "10/09/16 14:00:00, file.c 10, "
//   2  "file.c"   10 NULL   NULL  "10/09/16 14:00:00.123, file.c 10, "
//   3  "file.c"   10 NULL   NULL  "14:00:00.123456, file.c 10, "
//   0  "file.c"   10 "func" NULL  "file.c 10, func(), "
//   1  "file.c"   10 "func" NULL  "10/09/16 14:00:00, file.c 10, func(), "
//   2  "file.c"   10 "func" NULL  "10/09/16 14:00:00.123, file.c 10, func(), "
//   3  "file.c"   10 "func" NULL  "14:00:00.123456, file.c 10, func(), "
//   0  NULL        0 "func" NULL  "func(), "
//   1  NULL        0 "func" NULL  "10/09/16 14:00:00, func(), "
//   2  NULL        0 "func" NULL  "10/09/16 14:00:00.123, func(), "
//   3  NULL        0 "func" NULL  "14:00:00.123456, func(), "
//   2  NULL        0 "func" "tag" "10/09/16 14:00:00.123, func(), tag: "
char *get_tflft_string(char *buffer, int usec, const char *file, int line,
 const char *func, const char *tag)
{
	char buf_file_line[DEBUG_BUF_LEN+1];
	char buf_time[DEBUG_BUF_LEN+1];
	char buf_func[DEBUG_BUF_LEN+1];
	char buf_tag[DEBUG_BUF_LEN+1];
	char buf[DEBUG_BUF_LEN+1];

	if (file == NULL) {
		buf_file_line[0] = '\0';
	} else {
		snprintf(buf_file_line, DEBUG_BUF_LEN, "%s, ",
		 mk_file_line_str_buf(file, line, buf));	// "File Line, "
	}
	switch (usec) {
	case LOG_TIME_NONE:
		buf_time[0] = '\0';
		break;
	default:
		switch (usec) {
		default:	// FALLTHROUGH
		case LOG_TIME_YMD_HMS:
			get_cur_yysmmsdd_hhcmmcss(buf);		// "YY/MM/DD hh:mm:ss, "
			break;
		case LOG_TIME_YMD_HMSM:
			get_cur_yysmmsdd_hhcmmcsspmmm(buf);	// "YY/MM/DD hh:mm:ss.mmm, "
			break;
		case LOG_TIME_HMSU:
			get_cur_hhcmmcsspuuuuuu(buf);		// "hh:mm:ss.uuuuuu, "
			break;
		case LOG_TIME_HMSM:
			get_cur_hhcmmcsspmmm(buf);			// "hh:mm:ss.mmm, "
			break;
		}
		snprintf(buf_time, DEBUG_BUF_LEN, "%s, ", buf);
		break;
	}
	if (func == NULL) {
		buf_func[0] = '\0';
	} else {
		snprintf(buf_func, DEBUG_BUF_LEN, "%s(), ", func);	// "func(), "
	}
	if (tag == NULL) {
		buf_tag[0] = '\0';
	} else {
		snprintf(buf_tag, DEBUG_BUF_LEN, "%s: ", tag);	// "tag: "
	}
	snprintf(buffer, DEBUG_BUF_LEN, "%s%s%s%s", buf_time, buf_file_line, buf_func, buf_tag);
	return buffer;
}

const char *mk_file_line_str_s_(const char *file, int line)
{
	static char buf[DEBUG_BUF_LEN+1];
	return mk_file_line_str_buf(file, line, buf);
}
const char *mk_file_line_str_buf(const char *file, int line, char *buf)
{
	snprintf(buf, DEBUG_BUF_LEN, "%s %d", file, abs(line));	// File Line
	return buf;
}

//------------------------------------------------------------------------------

// 012345678901234567890123456789
// 05-11-10 15:34:29.123
char *get_cur_yysmmsdd_hhcmmcsspmmm(char *buffer)
{
	struct timeval tv;
	char buf[DEBUG_BUF_LEN+1];

	gettimeofday(&tv, NULL);
	snprintf(buffer, DEBUG_BUF_LEN, "%s.%03ld",
	 get_yysmmsdd_hhcmmcss(tv.tv_sec, buf), tv.tv_usec / 1000);
	return buffer;
}
// 012345678901234567890123456789
// 15:34:29.123
char *get_cur_hhcmmcsspmmm(char *buffer)
{
	struct timeval tv;
	char buf[DEBUG_BUF_LEN+1];

	gettimeofday(&tv, NULL);
	snprintf(buffer, DEBUG_BUF_LEN, "%s.%03ld",
	 get_hhcmmcss(tv.tv_sec, buf), tv.tv_usec / 1000);
	return buffer;
}
// 012345678901234567890123456789
// 2005-11-10 15:34:29
char *get_cur_yyyysmmsdd_hhcmmcss(char *buffer)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return get_yyyysmmsdd_hhcmmcss(tv.tv_sec, buffer);
}
// 012345678901234567890123456789
// 05-11-10 15:34:29
char *get_cur_yysmmsdd_hhcmmcss(char *buffer)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return get_yysmmsdd_hhcmmcss(tv.tv_sec, buffer);
}
// 012345678901234567890123456789
// 15:34:29.999999
char *get_cur_hhcmmcsspuuuuuu(char *buffer)
{
	struct timeval tv;
	char buf[DEBUG_BUF_LEN+1];

	gettimeofday(&tv, NULL);
	snprintf(buffer, DEBUG_BUF_LEN, "%s.%06ld",
	 get_hhcmmcss(tv.tv_sec, buf), tv.tv_usec);
	return buffer;
}
// 012345678901234567890123456789
// 2005-11-10
char *get_cur_yyyysmmsdd(char *buf)
{
	return get_yyyysmmsdd(get_cur_abs_sec(), buf);
}
// 012345678901234567890123456789
// 15:34:29
char *get_cur_hhcmmcss(char *buffer)
{
	struct timeval tv;
	char buf[DEBUG_BUF_LEN+1];

	gettimeofday(&tv, NULL);
	snprintf(buffer, DEBUG_BUF_LEN, "%s",
	 get_hhcmmcss(tv.tv_sec, buf));
	return buffer;
}
// 0123456789012345678
// yyyy/mm/dd hh:mm:ss
//            --------
char *get_hhcmmcss(time_t abs_time, char *buf)
{
	get_yyyysmmsdd_hhcmmcss(abs_time, buf);
	return &buf[11];
}
// 01234567890123456
// yyyy/mm/dd hh:mm:ss
// ----------
char *get_yyyysmmsdd(time_t abs_time, char *buf)
{
	get_yyyysmmsdd_hhcmmcss(abs_time, buf);
	buf[10] = '\0';
	return buf;
}
// 01234567890123456
// yyyy/mm/dd hh:mm:ss
//   --------
char *get_yysmmsdd(time_t abs_time, char *buf)
{
	get_yyyysmmsdd_hhcmmcss(abs_time, buf);
	buf[10] = '\0';
	return &get_yyyysmmsdd_hhcmmcss(abs_time, buf)[2];
}
// 01234567890123456
// hh:mm:ss yy/mm/dd
char *get_hhcmmcss_yysmmsdd(time_t abs_time, char *buf)
{
	char buffer[DEBUG_BUF_LEN+1];

	if (abs_time == 0) {
		strcpy(buf, "");
	} else {
		get_yyyysmmsdd_hhcmmcss(abs_time, buffer);
		buffer[10] = '\0';
		snprintf(buf, DEBUG_BUF_LEN, "%s %s", &buffer[11], &buffer[2]);
	}
	return buf;
}
// 0123456789012345
//   --------------
// yyyy/mm/dd hh:mm
char *get_yysmmsdd_hhcmm(time_t abs_time, char *buf)
{
	get_yyyysmmsdd_hhcmmcss(abs_time, buf);
	buf[16] = '\0';
	return &buf[2];
}
// 01234567890123456
// yy/mm/dd hh:mm:ss
char *get_yysmmsdd_hhcmmcss(time_t abs_time, char *buf)
{
	return &get_yyyysmmsdd_hhcmmcss(abs_time, buf)[2];
}
//  0123456789012345678
// "yyyy/mm/dd_hh:mm:ss"
char *get_yyyysmmsdduhhcmmcss(time_t abs_time, char *buf)
{
	get_yyyysmmsdd_hhcmmcss(abs_time, buf);
	buf[10] = '_';
	return buf;
}
//  0123456789012345678
// "yyyy/mm/dd hh:mm:ss"
char *get_yyyysmmsdd_hhcmmcss(time_t abs_time, char *buf)
{
	struct tm tm_;
	struct tm *tm;

	if (abs_time == 0) {
//		strcpy(buf, "???\?/?\?/?? ??:??:??");
//		strcpy(buf, "0000/00/00 00:00:00");
		strcpy(buf, "----/--/-- --:--:--");
	} else {
		tm = localtime_r(&abs_time, &tm_);		// THREAD_SAFE
		snprintf(buf, DEBUG_BUF_LEN, "%04d/%02d/%02d %02d:%02d:%02d",
		 1900 + tm->tm_year, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
	return buf;
}
//  0123456789012345678
// "yymmdd_hhmmss"
char *get_yymmdd_hhmmss(time_t abs_time, char *buf)
{
	struct tm tm_;
	struct tm *tm;

	if (abs_time == 0) {
		strcpy(buf, "------_------");
	} else {
		tm = localtime_r(&abs_time, &tm_);		// THREAD_SAFE
		snprintf(buf, DEBUG_BUF_LEN, "%02d%02d%02d_%02d%02d%02d",
		 tm->tm_year % 100, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
	return buf;
}

//------------------------------------------------------------------------------

time_t get_cur_abs_sec(void)
{
	return time(NULL);
}

unsigned long get_cur_abs_msec(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//------------------------------------------------------------------------------

const char *dump_string_to_static_buf(const char *string, int bytes)
{
#define	DUMP_STRING_BUF_LEN		(256*3)
	static char buffer[DUMP_STRING_BUF_LEN+1];
	const char *str;
	char buf[3+1];

	strcpy(buffer, "");
	for (str = string; (str - string < bytes) && *str; str++) {
		snprintf(buf, 3+1, "%02x ", *(unsigned char *)str);
		strlcat__(buffer, buf, DUMP_STRING_BUF_LEN);
	}
	return buffer;
}
const char *dump_string(const char *string, int bytes)
{
	int idx;

	mflf_d_printf(" %d{", bytes);
	for (idx = 0; idx < bytes; idx++) {
		if (isprint(string[idx]) || ((unsigned char)string[idx]) >= 0x80) {
			d_printf("%c", string[idx]);
		} else {
			d_printf("{%02x}", (unsigned char)string[idx]);
		}
	}
	d_printf("}\n");
}

// End of mydebug.c
