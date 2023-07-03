//
// Simple Japanese Input Method - Character-based User Interface
//

#include "imjcui.h"

// definition of IM key settings
#define CTRL_CODE(chr)				((chr) - '@')
#define IMCUI_K_ON					K_M_TILDE
#define IMCUI_K_ON2					CTRL_CODE('@')
#define IMCUI_K_OFF					K_M_TILDE
#define IMCUI_K_OFF2				CTRL_CODE('@')
#define IMCUI_K_OFF3				K_M_q		// CTRL_CODE('@')
#define IMCUI_K_CONVERT				' '
#define IMCUI_K_CLEAR				K_ESC		// CTRL_CODE('Q')
#define IMCUI_K_BACKSPACE			CTRL_CODE('H')
#define IMCUI_K_DELETE				CTRL_CODE('G')
#define IMCUI_K_DEL					'\x7f'		// CTRL_CODE('G')
#define IMCUI_K_DC					K_DC		// CTRL_CODE('G')
#define IMCUI_K_CURSOR_LEFT			K_LEFT		// CTRL_CODE('S')
#define IMCUI_K_CURSOR_RIGHT		K_RIGHT	// CTRL_CODE('D')
#define IMCUI_K_NO_CONV				K_F05		// 
#define IMCUI_K_HIRAGANA			K_F06		// 
#define IMCUI_K_KATAKANA			K_F07		// 
#define IMCUI_K_HANKAKU_KATAKANA	K_F08		// 
#define IMCUI_K_ZENKAKU_ALPHA_LOWER	K_F09		// 
#define IMCUI_K_HANKAKU_ALPHA_LOWER	K_F10		// 
#define IMCUI_K_ZENKAKU_ALPHA_UPPER	K_F11		// 
#define IMCUI_K_HANKAKU_ALPHA_UPPER	K_F12		// 

#define IMCUI_K_COMMIT_ONE			K_DOWN		// CTRL_CODE('X')
#define IMCUI_K_COMMIT_ALL			'\r'
#define IMCUI_K_SEPARATE_LEFT		K_LEFT		// CTRL_CODE('S')
#define IMCUI_K_SEPARATE_RIGHT		K_RIGHT	// CTRL_CODE('D')
#define IMCUI_K_SELECT_PREV			K_UP		// CTRL_CODE('E')
#define IMCUI_K_SELECT_NEXT			' '
#define IMCUI_K_SELECT_FIRST		K_HOME		// CTRL_CODE('W')
#define IMCUI_K_SELECT_LAST			K_END		// CTRL_CODE('Z')
#define IMCUI_K_SELECT_PREV_PAGE	K_PPAGE	// CTRL_CODE('W')
#define IMCUI_K_SELECT_NEXT_PAGE	K_NPAGE	// CTRL_CODE('Z')

// internal codes of meta keys
#define K_M(chr)		(0xc0 | (chr))
#define K_ESC			0x1b
#define K_UP			K_M(0x00)
#define K_DOWN			K_M(0x01)
#define K_LEFT			K_M(0x02)
#define K_RIGHT			K_M(0x03)
#define K_IC			K_M(0x04)
#define K_DC			K_M(0x05)
#define K_HOME			K_M(0x06)
#define K_END			K_M(0x07)
#define K_PPAGE			K_M(0x08)
#define K_NPAGE			K_M(0x09)
#define K_F01			K_M(0x20)
#define K_F02			K_M(0x21)
#define K_F03			K_M(0x22)
#define K_F04			K_M(0x23)
#define K_F05			K_M(0x24)
#define K_F06			K_M(0x25)
#define K_F07			K_M(0x26)
#define K_F08			K_M(0x27)
#define K_F09			K_M(0x28)
#define K_F10			K_M(0x29)
#define K_F11			K_M(0x2a)
#define K_F12			K_M(0x2b)
#define K_SF01			K_M(0x2c)
#define K_SF02			K_M(0x2d)
#define K_SF03			K_M(0x2e)
#define K_SF04			K_M(0x2f)
#define K_SF05			K_M(0x30)
#define K_SF06			K_M(0x31)
#define K_SF07			K_M(0x32)
#define K_SF08			K_M(0x33)
#define K_SF09			K_M(0x34)
#define K_SF10			K_M(0x35)
#define K_SF11			K_M(0x36)
#define K_SF12			K_M(0x37)
#define K_M_q			K_M(0x3d)
#define K_M_TILDE		K_M(0x3e)
#define K_M_UNUSABLE	K_M(0x3f)

#define MAX_KEY_SEQ		7	// ESC ?? ?? ?? ?? ?? ??

PRIVATE int imcui_process_key_and_send(int key);
PRIVATE int imcui_process_one_key(int key);
PRIVATE int imcui_k_to_ke(int key);

PRIVATE char send_buf[IM_INPUT_LINE_LEN+1];
PRIVATE int send_buf_bytes = 0;
PRIVATE void send_keys(char *key_seq, int key_seq_len);

PRIVATE int get_candidate_list_str(char *buffer, int buf_len);
PRIVATE const char *get_candid_string(int candid_idx, int cur_candid_idx, char *buffer, int buf_len);

PRIVATE int convert_key_seq(int key, char *key_seq, int *key_seq_len);


imj imj__;

int imcui_filter(char *key_seq, int input_len, int *output_len, int buf_len)
{
	int idx;
	int ret = 0;

//flf_d_printf("input_len: %d, [%02x]\n", input_len, key_seq[0]);
	send_buf_bytes = 0;
	if (input_len <= 0)
		ret = imcui_process_key_and_send(-1);
	for (idx = 0; idx < input_len; idx++) {
		if (imcui_process_key_and_send(key_seq[idx]) < 0)
			ret = -1;	// key eaten
	}
	*output_len = send_buf_bytes <= buf_len ? send_buf_bytes : buf_len;
	memcpy_ol(key_seq, send_buf, *output_len);
	key_seq[*output_len] = '\0';	// null terminate buffer
//flf_d_printf("output_len: %d, [%02x]\n", *output_len, key_seq[0]);
	return ret;
}
PRIVATE int imcui_process_key_and_send(int key)
{
	static char key_seq[MAX_KEY_SEQ+1];	// 1 for null-termination
	static int key_seq_len = 0;
	int idx;
	char buf[IM_INPUT_LINE_LEN+1];

	key = convert_key_seq(key, key_seq, &key_seq_len);
	if (key >= 0) {			// converted (key sequence solved)
		// process converted key code
		if (imcui_process_one_key(key) >= 0) {
			// key not eaten, send raw-key-codes
			send_keys(key_seq, key_seq_len);
		} else {
			// key eaten, send committed string if exist.
			imj__.im_get_committed_string(buf, IM_INPUT_LINE_LEN);
			send_keys(buf, strlen(buf));
			key = -1;	// key eaten
		}
		key_seq_len = 0;
	} else if (key == -1) {	// not converted
		key = 0;
		// process raw key codes
		for (idx = 0; idx < key_seq_len; idx++) {
			if (imcui_process_one_key((u_char)key_seq[idx]) >= 0) {
				// key not eaten, send raw-key-codes
				send_keys(&key_seq[idx], 1);
			} else {
				// key eaten, send committed string if exist.
				imj__.im_get_committed_string(buf, IM_INPUT_LINE_LEN);
				send_keys(buf, strlen(buf));
				key = -1;	// key eaten
			}
		}
		key_seq_len = 0;
	}
	return key;
}
PRIVATE int imcui_process_one_key(int key)
{
	int key_event;

	key_event = imcui_k_to_ke(key);
//d_printf("key:%02x, key_event:%04x\n", key, key_event);
	if (imj__.im_process_key_event(key, key_event) < 0) {
		key = -1;
	}
//d_printf("=> key:%02x, key_event:%04x\n", key, key_event);
	return key;
}
PRIVATE int imcui_k_to_ke(int key)
{
	struct k_ke_table {
		int key;
		int key_event;
	} k_ke_table[] = {
	 { IMCUI_K_ON,					IM_KE_ON					},
	 { IMCUI_K_ON2,					IM_KE_ON					},
	 { IMCUI_K_OFF,					IM_KE_OFF					},
	 { IMCUI_K_OFF2,				IM_KE_OFF					},
	 { IMCUI_K_OFF3,				IM_KE_OFF					},
///	 { IMCUI_K_ABORT_INPUT,			IM_KE_ABORT_INPUT			},
	 { IMCUI_K_CONVERT,				IM_KE_CONVERT				},
	 { IMCUI_K_BACKSPACE,			IM_KE_BACKSPACE				},
	 { IMCUI_K_CLEAR,				IM_KE_CLEAR					},
	 { IMCUI_K_DELETE,				IM_KE_DELETE				},
///	 { IMCUI_K_DEL,					IM_KE_DELETE				},
	 { IMCUI_K_DEL,					IM_KE_BACKSPACE				},
	 { IMCUI_K_DC,					IM_KE_DELETE				},
	 { IMCUI_K_CURSOR_LEFT,			IM_KE_CURSOR_LEFT			},
	 { IMCUI_K_CURSOR_RIGHT,		IM_KE_CURSOR_RIGHT			},
	 { IMCUI_K_NO_CONV,				IM_KE_NO_CONV				},
	 { IMCUI_K_HIRAGANA,			IM_KE_HIRAGANA				},
	 { IMCUI_K_KATAKANA,			IM_KE_ZENKAKU_KATAKANA		},
	 { IMCUI_K_HANKAKU_KATAKANA,	IM_KE_HANKAKU_KATAKANA		},
	 { IMCUI_K_ZENKAKU_ALPHA_LOWER,	IM_KE_ZENKAKU_ALPHA_LOWER	},
	 { IMCUI_K_HANKAKU_ALPHA_LOWER,	IM_KE_HANKAKU_ALPHA_LOWER	},
	 { IMCUI_K_ZENKAKU_ALPHA_UPPER,	IM_KE_ZENKAKU_ALPHA_UPPER	},
	 { IMCUI_K_HANKAKU_ALPHA_UPPER,	IM_KE_HANKAKU_ALPHA_UPPER	},
	 { IMCUI_K_COMMIT_ONE,			IM_KE_COMMIT_ONE			},
	 { IMCUI_K_COMMIT_ALL,			IM_KE_COMMIT_ALL			},
	 { IMCUI_K_SEPARATE_LEFT,		IM_KE_SEPARATE_LEFT			},
	 { IMCUI_K_SEPARATE_RIGHT,		IM_KE_SEPARATE_RIGHT		},
	 { IMCUI_K_SELECT_PREV,			IM_KE_SELECT_PREV			},
	 { IMCUI_K_SELECT_NEXT,			IM_KE_SELECT_NEXT			},
	 { IMCUI_K_SELECT_FIRST,		IM_KE_SELECT_FIRST			},
	 { IMCUI_K_SELECT_LAST,			IM_KE_SELECT_LAST			},
	 { -1,							-1,							},
	};
	int idx;

	for (idx = 0; k_ke_table[idx].key >= 0; idx++) {
		if (k_ke_table[idx].key == key) {
//d_printf("key %02x ==> %02x\n", key, k_ke_table[idx].key_event);
			return k_ke_table[idx].key_event;
		}
	}
//d_printf("key %02x ==> %02x\n", key, key);
	return key;
}
PRIVATE void send_keys(char *key_seq, int key_seq_len)
{
	if (send_buf_bytes + key_seq_len >= IM_INPUT_LINE_LEN) {
		key_seq_len = MAX_(0, IM_INPUT_LINE_LEN - send_buf_bytes);
	}
	memcpy_ol(&send_buf[send_buf_bytes], key_seq, key_seq_len);
	send_buf_bytes += key_seq_len;
}

// 0: "漢字|変換|候補"
// 1: "<漢字>監事[1/2]変換|候補"
// 2: "漢字(<漢字>監事 [1/2])変換|候補"
int imcui_get_converting_line_str(int mode, char *buffer, int buf_len)
{
	char first_bunsetsu[MAX_CANDID_LEN+1];
	char remaining_bunsetsu[IM_INPUT_LINE_LEN+1];
	char all_bunsetsu[IM_INPUT_LINE_LEN+1];
	char candid_list[IM_INPUT_LINE_LEN+1];
	char candid_num[IM_INPUT_LINE_LEN+1];
	char buf[IM_INPUT_LINE_LEN+1];

	imj__.im_c_get_first_bunsetsu(first_bunsetsu, MAX_CANDID_LEN);
	imj__.im_c_get_remaining_bunsetsu(remaining_bunsetsu, IM_INPUT_LINE_LEN, "|");
	imj__.im_c_get_all_bunsetsu(all_bunsetsu, IM_INPUT_LINE_LEN, "|");
	get_candidate_list_str(candid_list, IM_INPUT_LINE_LEN / 2);
	snprintf(candid_num, IM_INPUT_LINE_LEN, "[%d/%d]",
	 imj__.im_c_get_cur_cand_idx()+1, imj__.im_c_get_num_of_candids());
//flf_d_printf("[[%s]][[%s]]\n", first_bunsetsu, remaining_bunsetsu);

	switch (mode) {
	default:
	case 0:
		snprintf(buf, IM_INPUT_LINE_LEN, "%s",
		 all_bunsetsu);
		break;
	case 1:
		snprintf(buf, IM_INPUT_LINE_LEN, "%s%s%s",
		 candid_list, candid_num, remaining_bunsetsu);
		break;
	case 2:
		snprintf(buf, IM_INPUT_LINE_LEN, "%s(%s%s)%s",
		 first_bunsetsu, candid_list, candid_num, remaining_bunsetsu);
		break;
	}
	utf8strcpy_blen(buffer, buf_len, buf);
//flf_d_printf("[%s]\n", buffer);
	return strlen(buffer);
}
PRIVATE int get_candidate_list_str(char *buffer, int buf_len)
{
	static int prev_start_candid_idx = 0;
	static int prev_candids_listed = 0;
	int num_of_candids;
	int cur_candid_idx;
	int candids_listed = 0;
	int candid_idx;
	char buf[IM_INPUT_LINE_LEN+1];
	int str_len;

	strcpy(buffer, "");
	num_of_candids = imj__.im_c_get_num_of_candids();
	if (num_of_candids <= 0)
		return 0;
	cur_candid_idx = imj__.im_c_get_cur_cand_idx();

	if (cur_candid_idx == 0) {
		prev_start_candid_idx = cur_candid_idx;
		prev_candids_listed = 1;
	}
	if (cur_candid_idx < prev_start_candid_idx) {
		str_len = 0;
		for (candid_idx = prev_start_candid_idx-1; candid_idx >= 0; candid_idx--) {
			get_candid_string(candid_idx, cur_candid_idx, buf, IM_INPUT_LINE_LEN);
			if (str_len + strlen(buf) > buf_len) {
				break;
			}
			str_len += strlen(buf);
		}
		prev_start_candid_idx = candid_idx + 1;
	} else if (cur_candid_idx < prev_start_candid_idx + prev_candids_listed) {
		// prev_start_candid_idx = prev_start_candid_idx;
	} else /* if (prev_start_candid_idx + prev_candids_listed <= cur_candid_idx) */ {
		prev_start_candid_idx = prev_start_candid_idx + prev_candids_listed;
	}
	candids_listed = 0;
	for (candid_idx = prev_start_candid_idx; candid_idx < num_of_candids; candid_idx++) {
		get_candid_string(candid_idx, cur_candid_idx, buf, IM_INPUT_LINE_LEN);
		if (strlen(buffer) + strlen(buf) > buf_len)
			break;
		utf8strcat_blen(buffer, buf_len, buf);
		candids_listed++;
	}
	prev_candids_listed = candids_listed;

	return num_of_candids;
}
PRIVATE const char *get_candid_string(int candid_idx, int cur_candid_idx, char *buffer, int buf_len)
{
	const char *template__;
	char buf[MAX_CANDID_LEN+1];

	imj__.im_c_get_one_candid(candid_idx, buf, MAX_CANDID_LEN);
	if (strcmp(buf, " ") == 0 || strcmp(buf, "　") == 0) {
		if (cur_candid_idx == candid_idx)
			template__ = "<'%s'>";
		else
			template__ = " '%s' ";
	} else {
		if (cur_candid_idx == candid_idx)
			template__ = "<%s>";
		else
			template__ = " %s ";
	}
	snprintf(buffer, buf_len, template__, buf);
	return buffer;
}

PRIVATE int convert_key_seq(int key, char *key_seq, int *key_seq_len)
{
	struct key_seq_table {
		short key;
		const char *sequence;
	} key_seq_table[] = {
	//           1---2---3---4---5---6---7---
	// linux console
	K_M_q,		"\x1bq",
	K_M_TILDE,	"\x1b`",
	K_F01,		"\x1b\x5b\x5b\x41",
	K_F02,		"\x1b\x5b\x5b\x42",
	K_F03,		"\x1b\x5b\x5b\x43",
	K_F04,		"\x1b\x5b\x5b\x44",
	K_F05,		"\x1b\x5b\x5b\x45",
	K_F06,		"\x1b\x5b\x31\x37\x7e",
	K_F07,		"\x1b\x5b\x31\x38\x7e",
	K_F08,		"\x1b\x5b\x31\x39\x7e",
	K_F09,		"\x1b\x5b\x32\x30\x7e",
	K_F10,		"\x1b\x5b\x32\x31\x7e",
	K_F11,		"\x1b\x5b\x32\x33\x7e",
	K_F12,		"\x1b\x5b\x32\x34\x7e",
	K_SF01,		"\x1b\x5b\x32\x35\x7e",
	K_SF02,		"\x1b\x5b\x32\x36\x7e",
	K_SF03,		"\x1b\x5b\x32\x38\x7e",
	K_SF04,		"\x1b\x5b\x32\x39\x7e",
	K_SF05,		"\x1b\x5b\x33\x31\x7e",
	K_SF06,		"\x1b\x5b\x33\x32\x7e",
	K_SF07,		"\x1b\x5b\x33\x33\x7e",
	K_SF08,		"\x1b\x5b\x33\x34\x7e",
	K_IC,		"\x1b\x5b\x32\x7e",
	K_DC,		"\x1b\x5b\x33\x7e",
	K_HOME,		"\x1b\x5b\x31\x7e",
	K_END,		"\x1b\x5b\x34\x7e",
	K_PPAGE,	"\x1b\x5b\x35\x7e",
	K_NPAGE,	"\x1b\x5b\x36\x7e",
	K_UP,		"\x1b\x5b\x41",
	K_DOWN,		"\x1b\x5b\x42",
	K_LEFT,		"\x1b\x5b\x44",
	K_RIGHT,	"\x1b\x5b\x43",
	// xterm
	K_F01,		"\x1b\x4f\x50",
	K_F02,		"\x1b\x4f\x51",
	K_F03,		"\x1b\x4f\x52",
	K_F04,		"\x1b\x4f\x53",
	K_F05,		"\x1b\x5b\x31\x35\x7e",
	K_F06,		"\x1b\x5b\x31\x37\x7e",
	K_F07,		"\x1b\x5b\x31\x38\x7e",
	K_F08,		"\x1b\x5b\x31\x39\x7e",
	K_F09,		"\x1b\x5b\x32\x30\x7e",
	K_F10,		"\x1b\x5b\x32\x31\x7e",
	K_F11,		"\x1b\x5b\x32\x33\x7e",
	K_F12,		"\x1b\x5b\x32\x34\x7e",
	K_SF01,		"\x1b\x4f\x32\x50",
	K_SF02,		"\x1b\x4f\x32\x51",
	K_SF03,		"\x1b\x4f\x32\x52",
	K_SF04,		"\x1b\x4f\x32\x53",
	K_SF05,		"\x1b\x5b\x31\x35\x3b\x32\x7e",
	K_SF06,		"\x1b\x5b\x31\x37\x3b\x32\x7e",
	K_SF07,		"\x1b\x5b\x31\x38\x3b\x32\x7e",
	K_SF08,		"\x1b\x5b\x31\x39\x3b\x32\x7e",
	K_SF09,		"\x1b\x5b\x32\x30\x3b\x32\x7e",
	K_SF10,		"\x1b\x5b\x32\x31\x3b\x32\x7e",
	K_SF11,		"\x1b\x5b\x32\x33\x3b\x32\x7e",
	K_SF12,		"\x1b\x5b\x32\x34\x3b\x32\x7e",
	K_HOME,		"\x1b\x5b\x48",
	K_END,		"\x1b\x5b\x46",
	-1,			"",
	};
	int idx;
	int partial_match = 0;

///d_printf("key: %02x\n", key);
///d_printf("*key_seq_len: %d\n", *key_seq_len);
	if (key < 0)
		return -1;			// not converted, return raw key codes
	key_seq[(*key_seq_len)++] = key;
	key_seq[*key_seq_len] = '\0';
///d_printf("*key_seq_len: %d\n", *key_seq_len);
///	if (*key_seq_len == 1 && key != 0x1b) {
///		return key;
///	}
	for (idx = 0; key_seq_table[idx].key >= 0; idx++) {
///d_printf("%02x\n", key_seq_table[idx].key);
		if (strncmp(key_seq_table[idx].sequence, key_seq, *key_seq_len) == 0) {
///_FLF_
			if (strncmp(key_seq_table[idx].sequence, key_seq,
			 strlen(key_seq_table[idx].sequence)) == 0) {
///_FLF_
///d_printf("converted: %02x\n", key_seq_table[idx].key);
				return key_seq_table[idx].key;	// converted
			}
///_FLF_
			partial_match = 1;
			break;
		}
	}
	if (partial_match) {
///d_printf("converting:-2\n");
		return -2;		// converting
	}
///d_printf("notconverted:-1\n");
	return -1;			// not converted, return raw key codes
}

// End of imjcui.cpp
