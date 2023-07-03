//
// Simple Japanese Input Method - CUI test program
// imjcuitest.cpp
//

#include "imjcui.h"

int auto_conv_mode;
PRIVATE char answer[IM_INPUT_LINE_LEN+1];
PRIVATE int no_match_count = 0;
PRIVATE int total_key_action_count = 0;
PRIVATE int key_left_count = 0;
PRIVATE int key_next_count = 0;
PRIVATE int key_commit_count = 0;

char *read_pronun(void);
int convert_auto(const char *pronun);
int search_in_candids(void);
int imcuit_process_key_event(int key, int event);
void show_wait(void);
void dump_key_count(void);
char *send_key_and_recv_conversion(int key);

void imcuit_display(void);
PRIVATE int imcuit_printf(const char *format, ...);

int main(int argc, char *argv[])
{
	int key;
	char buffer[IM_INPUT_LINE_LEN+1];
	int len;
	char *pronun;

	flf_d_printf("start\r\n");
	if (argc > 2) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		fprintf(stderr, "Usage: %s - <input.txt\n", argv[0]);
		fprintf(stderr, "Usage: %s + <input.txt\n", argv[0]);
		exit(0);
	}
	imj__.im_open(".");
	auto_conv_mode = 0;
	if (argc > 1) {
		if (strcmp(argv[1], "-") == 0) {
			auto_conv_mode = -1;
		}
		if (strcmp(argv[1], "+") == 0) {
			auto_conv_mode = 1;		// slow mode
		}
	}
	if (auto_conv_mode) {
		flf_d_printf("start auto conversion mode\r\n");
	} else {
		flf_d_printf("start manual mode\r\n");
	}
	// IM on
	memcpy(buffer, "\0", 1);		// Ctrl-@
	imcui_filter(buffer, 1, &len, IM_INPUT_LINE_LEN);
	imcuit_display();
///	usleep(100000);
	for ( ; ; ) {
		if (auto_conv_mode == 0) {
			key = fgetc(stdin);
			if (key >= 0)
				fcntl(0, F_SETFL, O_NONBLOCK);		// Not block in read(0,...)
			else
				fcntl(0, F_SETFL, 0);
			if (send_key_and_recv_conversion(key) == NULL)
				break;
			imcuit_display();
		} else {
			pronun = read_pronun();
			if (pronun == NULL)
				break;
			if (strlen(pronun)) {
				convert_auto(pronun);
			}
		}
	}
	if (auto_conv_mode) {
		flf_d_printf("auto conversion mode complete\r\n");
	} else {
		flf_d_printf("manual mode ended\r\n");
	}
	dump_key_count();
	imj__.im_close();
	flf_d_printf("exit\r\n");
	return 0;
}

char *read_pronun(void)
{
	char buf[IM_INPUT_LINE_LEN+1];
	static char pronun[IM_INPUT_LINE_LEN+1];

	strcpy(pronun, "");
	for ( ; ; ) {
		if (fgets(buf, sizeof(buf)-1, stdin) == NULL) {
//			_FLF_
			return NULL;
		}
		cut_line_tail_crlf(buf);
		if (buf[0] == '-') {
d_printf("COMMENT:[%s]\n", buf);
			dump_key_count();
		} else if (buf[0] == '#') {
			strcpy(answer, &buf[1]);
///d_printf("\nOBJECT:[%s]\n", answer);
		} else if (buf[0]) {
			strcpy(pronun, buf);
d_printf("INPUT:[%s]\n", pronun);
			break;
		}
	}
	return pronun;
}
int convert_auto(const char *pronun)
{
flf_d_printf("pronun: [%s]\n", pronun);
	imj__.im_set_pronun(pronun);
	imcuit_process_key_event(IM_KE_CONVERT, IM_KE_CONVERT);
	for ( ; imj__.im_get_pronun_len(); ) {
d_printf("ANSWER: [%s]\n", answer);
//flf_d_printf("im_pronun: [%s]:%d\n", imj__.im_get_pronun(NULL, IM_INPUT_LINE_LEN), imj__.im_get_pronun_len_to_match());
		for ( ; ; ) {
///			imj__._renbunsetsu._renbunsetsu[0]._candidates.dump_candids();
			if (imj__.im_c_get_num_of_candids()) {
				if (search_in_candids())
					break;
			}
			if (imj__.im_get_pronun_len_to_match() <= utf8c_bytes(imj__.im_get_pronun(NULL, IM_INPUT_LINE_LEN))) {
				no_match_count++;
flf_d_printf("NO_MATCH at all yomi length\n");
				return -1;
			}
			if (imj__.im_c_get_num_of_candids()) {
flf_d_printf("candid[0]: [%s]\n", imj__.im_c_get_one_candid(0, NULL, MAX_CANDID_LEN));
			}
			imcuit_process_key_event(IM_KE_SEPARATE_LEFT, IM_KE_SEPARATE_LEFT);
//flf_d_printf("im_pronun: [%s]:%d\n", imj__.im_get_pronun(NULL, IM_INPUT_LINE_LEN), imj__.im_get_pronun_len_to_match());
		}
	}
	return 0;
}
int search_in_candids(void)
{
	int candid_idx;
	char committed_string[IM_INPUT_LINE_LEN+1];

	for (candid_idx = 0; candid_idx < imj__.im_c_get_num_of_candids(); candid_idx++) {
		if (strlcmp(answer, imj__.im_c_get_one_candid(candid_idx, NULL, MAX_CANDID_LEN)) == 0)
			break;
	}
	if (candid_idx >= imj__.im_c_get_num_of_candids()) {
		return 0;
	}
	for ( ; ; ) {
		if (strlcmp(answer, imj__.im_c_get_cur_candid(NULL, MAX_CANDID_LEN)) == 0) {
			imcuit_process_key_event(IM_KE_COMMIT_ONE, IM_KE_COMMIT_ONE);
			break;
		}
flf_d_printf("cur_candid: [%s]\n", imj__.im_c_get_cur_candid(NULL, MAX_CANDID_LEN));
		imcuit_process_key_event(IM_KE_SELECT_NEXT, IM_KE_SELECT_NEXT);
		if (imj__.im_c_get_cur_cand_idx() == 0) {
			// one cycle rounded
			break;
		}
	}
	if (imj__.im_get_committed_string(committed_string, IM_INPUT_LINE_LEN)[0]) {
		strcpy_ol(answer, &answer[strlen(committed_string)]);
d_printf("OUTPUT:[%s]\n\n", committed_string);
		return 1;
	}
	return 0;
}

int imcuit_process_key_event(int key, int event)
{
	total_key_action_count++;
	switch (key) {
	case IM_KE_CONVERT:
d_printf("IM_KE_CONVERT\r");
///		printf("\n");
		break;
	case IM_KE_SEPARATE_LEFT:
d_printf("IM_KE_SEPARATE_LEFT\r");
		key_left_count++;
		break;
	case IM_KE_SELECT_NEXT:
d_printf("IM_KE_SELECT_NEXT\r");
		key_next_count++;
		break;
	case IM_KE_COMMIT_ONE:
d_printf("IM_KE_COMMIT_ONE\r");
		key_commit_count++;
		break;
	}
	imj__.im_process_key_event(key, event);
	imcuit_display();
	switch (key) {
	case IM_KE_CONVERT:
		printf("\n");
		break;
	case IM_KE_SEPARATE_LEFT:
		printf("\n");
		break;
	case IM_KE_SELECT_NEXT:
		printf("\n");
		break;
	case IM_KE_COMMIT_ONE:
		printf("\n");
		break;
	}
	show_wait();
	return 0;
}
void show_wait(void)
{
	if (auto_conv_mode > 0) {
		usleep(100000);
	}
}
void dump_key_count(void)
{
	if (total_key_action_count) {
		flf_d_printf("---------------------------------------------------------\r\n");
		d_printf("NO_MATCH count: %d\r\n", no_match_count);
		d_printf("TOTAL_KEY count: %d\r\n", total_key_action_count);
		d_printf("IM_KE_SEPARATE_LEFT count: %d\r\n", key_left_count);
		d_printf("IM_KE_SELECT_NEXT count: %d\r\n", key_next_count);
		d_printf("IM_KE_COMMIT_ONE count: %d\r\n", key_commit_count);
		flf_d_printf("---------------------------------------------------------\r\n");
	}
}
char *send_key_and_recv_conversion(int key)
{
	static char buffer[IM_INPUT_LINE_LEN+1];
	int len;

flf_d_printf("%02x\r\n", key);
	buffer[0] = key;
	buffer[1] = '\0';
	imcui_filter(buffer, 1, &len, IM_INPUT_LINE_LEN);
	if (len > 0) {
		buffer[len] = '\0';
		if (buffer[0] == CTRL_CODE('C'))
			return NULL;
		printf("\r\n  <<%s>>\r\n", buffer);
	}
	return buffer;
}

void imcuit_display(void)
{
	static int prev_im_mode = IM_MODE_OFF;
	int im_mode;
	char buffer[IM_INPUT_LINE_LEN+1];
	const char clear_line[] =
// 1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
"\r                                                                                                    \r";

	im_mode = imj__.im_get_mode();
	switch(im_mode) {
	case IM_MODE_OFF:
		if (im_mode != prev_im_mode)
			imcuit_printf(clear_line);
		break;
	case IM_MODE_INPUT:
		imj__.im_get_pronun(buffer, IM_INPUT_LINE_LEN);
		imcuit_printf(clear_line);
		imcuit_printf("\rI: %s", buffer);
		buffer[imj__.im_get_cursor_offset()] = '\0';
		imcuit_printf("\rI: %s", buffer);
		break;
	case IM_MODE_CONVERT:
//		imcui_get_converting_line_str(0, buffer, IM_INPUT_LINE_LEN);
//		imcui_get_converting_line_str(1, buffer, IM_INPUT_LINE_LEN);
		imcui_get_converting_line_str(2, buffer, IM_INPUT_LINE_LEN);
		imcuit_printf(clear_line);
		imcuit_printf("\rC: %s", buffer);
		break;
	}
	prev_im_mode = im_mode;
}
PRIVATE int imcuit_printf(const char *format, ...)
{
#define	IMUI_INPUT_LINE_LEN		(IM_INPUT_LINE_LEN+4)
	va_list args;
	char buffer[IMUI_INPUT_LINE_LEN+1];
	int len;

	va_start(args, format);
	len = vsnprintf(buffer, IMUI_INPUT_LINE_LEN+1, format, args);
	va_end(args);
	fputs(buffer, stdout);
	fflush(stdout);
	return len;
}

// End of imjcuitest.cpp
