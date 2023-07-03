/*****************************************************************************
	mystring.c
 *****************************************************************************/

#include "utilincs.h"

int utf8s_chars(const char *string, int bytes)
{
	int chars = 0;
	int idx;
	int len;

	for (idx = 0; idx < bytes && string[idx]; ) {
		len = utf8c_bytes(&string[idx]);
		idx += len;
		chars++;
	}
	return chars;
}
int tranc_utf8s_columns(char *string, int columns)
{
	int cols;
	char *str;

	for (str = string, cols = 0; *string; ) {
		cols += utf8c_columns(str);
		if (cols >= columns)
			break;
		str += utf8c_bytes(str);
	}
	*str = '\0';
	return str - string;
}
int utf8s_len_le(const char *string, int prev_len)
{
	int utf8_len;
	int len;

	for (utf8_len = 0; *string && utf8_len < prev_len; ) {
		len = utf8c_bytes(string);
//flf_d_printf("mblen(%s): %d\n", string, len);
		if (utf8_len + len > prev_len)
			break;
		string += len;
		utf8_len += len;
	}
	return utf8_len;
}
int utf8s_len_ge(const char *string, int prev_len)
{
	int utf8_len;
	int len;

	for (utf8_len = 0; *string && utf8_len < prev_len; ) {
		len = utf8c_bytes(string);
//flf_d_printf("mblen(%s): %d\n", string, len);
		string += len;
		utf8_len += len;
	}
	return utf8_len;
}

int utf8s_columns(const char *utf8s, int bytes)
{
	const char *ptr;
	int columns;

	for (ptr = utf8s, columns = 0; *ptr && ptr - utf8s < bytes; ) {
		columns += utf8c_columns(ptr);
		ptr += utf8c_bytes(ptr);
	}
	return columns;
}
int utf8c_columns(const char *utf8c)
{
	int columns;

	columns = my_mbwidth(utf8c, MAX_UTF8C_BYTES);
//flf_d_printf("wcwidth(%04x)==>%d\n", (int)wc, columns);
	return columns;
}

// UTF8 character byte length
int utf8c_bytes(const char *utf8c)
{
	return my_mblen(utf8c, MAX_UTF8C_BYTES);
}

int wchar_columns(wchar_t wc)
{
	return my_wcwidth(wc);
}

//-----------------------------------------------------------------------------

// buffer-over-run prevented strcat()
char *utf8strcat_blen(char *buf, int buf_len, const char *str)
{
	return utf8strcat_blen_slen(buf, buf_len, str, strlen(str));
}
char *strcat_blen(char *buf, int buf_len, const char *str)
{
	return strcat_blen_slen(buf, buf_len, str, strlen(str));
}
char *utf8strcat_blen_slen(char *buf, int buf_len, const char *str, int len)
{
	int prev_len;

	prev_len = strlen(buf);
//flf_d_printf("strcat_blen_slen([%s], %d, [%s], %d) prev_len:%d\n",
// buf, buf_len, str, len, prev_len);
	if (prev_len + len > buf_len) {
		len = buf_len - prev_len;
	}
	if (len > 0) {
		utf8strcpy_blen_slen(&buf[prev_len], len, str, len);
	}
	return buf;
}
char *strcat_blen_slen(char *buf, int buf_len, const char *str, int len)
{
	int prev_len;

	prev_len = strlen(buf);
//flf_d_printf("strcat_blen_slen([%s], %d, [%s], %d) prev_len:%d\n",
// buf, buf_len, str, len, prev_len);
	if (prev_len + len > buf_len) {
		len = buf_len - prev_len;
	}
	if (len > 0) {
		strcpy_blen_slen(&buf[prev_len], len, str, len);
	}
	return buf;
}
// buffer-over-run prevented strcpy()
char *utf8strcpy_blen_slen(char *buf, int buf_len, const char *str, int len)
{
	if (len > buf_len)
		len = buf_len;
	return strcpy_blen_slen(buf, buf_len, str, utf8s_len_le(str, len));
}
char *strcpy_blen_slen(char *buf, int buf_len, const char *str, int len)
{
	if (len > buf_len)
		len = buf_len;
	return strcpy_blen(buf, len, str);
}
char *utf8strcpy_blen(char *buf, int buf_len, const char *str)
{
	return strcpy_blen(buf, utf8s_len_le(str, buf_len), str);
}
char *strcpy_blen(char *buf, int buf_len, const char *str)
{
	strncpy(buf, str, buf_len);
	buf[buf_len] = '\0';	// make sure of null termination
	return buf;
}

//------------------------------------------------------------------------------

char *strlcat(char *buf, const char *str, int buf_len)
{
	int len;

	len = strlen(buf);
	if (len < buf_len) {
		strlcpy(&buf[len], str, buf_len - len);
	}
	return buf;
}
char *strlcpy(char *buf, const char *str, int buf_len)
{
	strncpy(buf, str, buf_len);
	buf[buf_len] = '\0';	// make sure of null termination
	return buf;
}
char *strcpy_ol(char *dest, const char *src)
{
	memmove(dest, src, strlen(src)+1);
	return dest;
}
int strlcmp(const char *str1, const char *str2)
{
	return strncmp(str1, str2, strlen(str2));
}

void *memcpy_ol(void *dest, void *src, size_t bytes)
{
	return memmove(dest, src, bytes);
}

// End of mystring.c
