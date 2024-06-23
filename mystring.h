/*****************************************************************************
	mystring.h
 *****************************************************************************/

#ifndef mystring_h
#define mystring_h

#include "myutf8.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define MAX_UTF8C_BYTES			4

#define	CONST_CHAR_PTR_FROM_QSTRING(qstring)	(const char *)((qstring).toUtf8().constData())
#define	CCP_FROM_QSTRING(qstring)				CONST_CHAR_PTR_FROM_QSTRING(qstring)
#define	CCP(qstring)							CCP_FROM_QSTRING(qstring)

int utf8s_chars(const char *string, int bytes);
int tranc_utf8s_columns(char *string, int columns);
int utf8s_len_le(const char *string, int prev_len);
int utf8s_len_ge(const char *string, int prev_len);
int utf8s_columns(const char *utf8s, int bytes);
int utf8c_columns(const char *utf8s);
int utf8c_bytes(const char *utf8c);
int wchar_columns(wchar_t wc);

char *utf8strcat_blen(char *buf, int buf_len, const char *str);
char *strcat_blen(char *buf, int buf_len, const char *str);
char *utf8strcat_blen_slen(char *buf, int buf_len, const char *str, int len);
char *strcat_blen_slen(char *buf, int buf_len, const char *str, int len);
char *utf8strcpy_blen_slen(char *buf, int buf_len, const char *str, int len);
char *strcpy_blen_slen(char *buf, int buf_len, const char *str, int len);
char *utf8strcpy_blen(char *buf, int buf_len, const char *str);
char *strcpy_blen(char *buf, int buf_len, const char *str);

char *strlcat__(char *buf, const char *str, int buf_len);
char *strlcpy__(char *buf, const char *str, int buf_len);
char *strcpy_ol(char *dest, const char *src);
int strlcmp__(const char *str1, const char *str2);

void *memcpy_ol(void *dest, void *src, size_t bytes);

int snprintf_(char *buffer, size_t buf_len, const char *format, ...);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // mystring_h

// End of mystring.h
