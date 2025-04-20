//=============================================================================
// Simple Japanese Input Method implementation
//=============================================================================

// [Input Method layer description]
// ui: User Interface
//   keyboard and mouse input.
//   display output.
//   send KANJI to application
// imj: HIRAGANA to KANJI conversion management
//   separating YOMI into BUNSETSU(sentences)
//   converting BUSETSU to KANJI using dictionary
//   selecting one from several candidates gotten from imjdichst
// imjdichst: Dictionary(imjdic) and History(imjhst) combined conversion.
//   imjatoh: ASCII to HIRAGANA(YOMI/Pronunciation) conversion(ROMA-JI-HENKAN)
//     inputing and editing of YOMI and converting between hankaku/zenkaku
//   imjdic: HIRAGANA/KANJI conversion using dictionary file
//     converting from YOMI to KANJI
//       searching in dictionary of YOMI and getting candidates of KANJI
//   imjhst: Input/Conversion history
//     imjihst: Input history
//     imjchst: Conversion history
//   imjcandid: Candidate list management
//     list candidates and select one

// atoj (ASCII to Japanese)
// atoh (ASCII to Hiragana)
// htok (Hiragana to Kanji)
// htoa (Hiragana to ASCII)

#include "imj.h"

#define MAX_PATH_LEN	256

//-----------------------------------------------------------------------------

PRIVATE int imjihst_get_histories(void);
PRIVATE int imjihst_get_nth_history(int hist_idx, char *history, int buf_len);
PRIVATE int imjihst_get_nth_history_idx(int hist_idx);
PRIVATE int imjihst_append(const char *pronun);
PRIVATE void imjihst_dump(void);

PRIVATE int imjchst_get_num_of_matches(const char *pronun, int len_to_match);
PRIVATE int imjchst_get_nth_match(int match_idx, const char *pronun, int len_to_match,
 char *conversion, int buf_len);
PRIVATE int imjchst_search_longest_match_len(const char *pronun);
PRIVATE int imjchst_append(const char *pronun, const char *conversion);

PRIVATE void imjhst_clear_all(void);
PRIVATE void imjhst_clear_save(void);
PRIVATE int imjhst_get_entries(void);
PRIVATE int imjhst_is_updated(void);
PRIVATE int imjhst_set_updated(void);
PRIVATE int imjhst_clear_updated(void);
PRIVATE int imjhst_append_entry_line(const char *pronun_conversion);
PRIVATE int imjhst_append_new(const char *pronun, const char *conversion);
PRIVATE int imjhst_append_new_line(const char *pronun_conversion);
PRIVATE int imjhst_delete_entry(int entry_idx);
PRIVATE int imjhst_search_by_pronun_from_last(int start_entry_idx,
 const char *pronun, int len_to_match, const char *conversion);
PRIVATE int imjhst_search_the_same_entry_line(const char *pronun_conversion);
PRIVATE int imjhst_put_entry_line(int entry_idx, const char *pronun_conversion);
PRIVATE char *imjhst_make_line(const char *pronun, const char *conversion, char *buffer);
PRIVATE void imjhst_dump(void);

//-----------------------------------------------------------------------------

PRIVATE const char *expand_regexp(const char *regexp, char *buf_ret, int buf_len);
PRIVATE const char *parse_re_parallel(const char *regexp, char *buf_ret, int buf_len);
PRIVATE const char *parse_re_bunsetsu(const char *regexp, char *buf_ret, int buf_len);
PRIVATE const char *parse_re_chars(const char *regexp, char *buf_ret, int buf_len);
PRIVATE char *concat_bunsetsu(const char *bunsetsu1, const char *bunsetsu2,
 char *buffer, int buf_len);
PRIVATE int re_word_len(const char *str);
PRIVATE void add_space_separator(char *buf, int buf_len);

//-----------------------------------------------------------------------------

PRIVATE char *imjhira_conv_by_table(const char *str, char *buf, int buf_len,
 const char *table_from, const char *table_to);

PRIVATE int imjhira_contains(const char *string, const char *table);
PRIVATE char *imjhira_conv_by_table_ol(const char *str, char *buf, int buf_len,
 const char *table_from, const char *table_to);

PRIVATE const char *skip_han(const char *ptr);
PRIVATE int is_han(const char *chr);
PRIVATE int is_chr_in_table(const char *chr, const char *table);

#ifdef IMJ_BUILT_IN_TEST
PRIVATE const char *remove_space(const char *src, char *buf, int buf_len);
#endif // IMJ_BUILT_IN_TEST

//-----------------------------------------------------------------------------

int imj::im_open(const char *dir)
{
	char buf[MAX_PATH_LEN+1];
	char buffer[MAX_PATH_LEN+1];
	char *dic_path = NULL;
	char *hst_path = NULL;
	int ret = 0;

_FLF_
#ifdef IMJ_BUILT_IN_TEST
	test_imjatoh_functions();
#endif // IMJ_BUILT_IN_TEST
	im_init();

	if (dir) {
		strcpy_blen(buf, MAX_PATH_LEN, dir);
		strcat_blen(buf, MAX_PATH_LEN, "/.imj/");	// "/home/user/.imj/"
		dir = buf;
	}

	if (dir) {
		strcpy(buffer, dir);
		strcat_blen(buffer, MAX_PATH_LEN, DIC_FILE_NAME);
		dic_path = buffer;
	}
	if (imjdic::imjdic_open(dic_path)) {
		ret = (ret == 0) ? 1 : ret;
	}

	if (dir) {
		strcpy(buffer, dir);
		strcat_blen(buffer, MAX_PATH_LEN, CNVHST_FILE_NAME);
		hst_path = buffer;
	}
	if (imjhst_load(hst_path)) {
		ret = (ret == 0) ? 2 : ret;
	}

//	ret |= imjatoh_load_table(NULL);

#ifdef IMJ_BUILT_IN_TEST
///	test_hira_kata_han_conversion();
///	imjdic_test_katsuyou();
///	test_my_mbwidth();
#endif // IMJ_BUILT_IN_TEST
	return ret;
}
int imj::im_close(void)
{
	int ret = 0;

_FLF_
	ret |= imjhst_save_if_updated(NULL);
	ret |= imjdic::imjdic_close();
	return ret;
}

int imj::im_process_key(int key)
{
	return im_process_key_event(key, key);
}
int imj::im_process_key_event(int key, int event)
{
	int key_eaten = 1;

///flf_d_printf("key:%04x, event:%04x\n", key, event);
	switch(_im_mode) {
	case IM_MODE_OFF:
		if (event == IM_KE_ON) {
			im_set_mode(IM_MODE_INPUT);
			key = -1;
		}
		break;
	case IM_MODE_INPUT:
	case IM_MODE_CONVERT:
		if (event == IM_KE_ON || event == IM_KE_OFF) {
			im_set_mode(IM_MODE_OFF);
			key = -1;
		}
		break;
	}
	switch(_im_mode) {
	case IM_MODE_OFF:
		key_eaten = 0;
		break;
	case IM_MODE_INPUT:
		if (strlen(im_pronun) == 0) {
			if (isprint(key) == 0) {
				key_eaten = 0;
				break;
			} else if (key == ' ') {
				if (im_i_input_romaji(key) < 0) {
					im_i_commit_all();	// commit "　"
					break;
				}
			}
		}
		switch(event) {
		case IM_KE_SELECT_PREV:
		case IM_KE_COMMIT_ONE:
		case IM_KE_SELECT_FIRST:
		case IM_KE_SELECT_LAST:
			if (strlen(im_pronun) == 0) {
				key_eaten = 0;
				break;
			}
			switch(event) {
			default:
			case IM_KE_SELECT_PREV:
				im_i_select_prev();
				break;
			case IM_KE_COMMIT_ONE:
				im_i_select_next();
				break;
			case IM_KE_SELECT_FIRST:
				im_i_select_first();
				break;
			case IM_KE_SELECT_LAST:
				im_i_select_last();
				break;
			}
			break;
		case IM_KE_COMMIT_ALL:
			im_i_commit_all();
			break;
		case IM_KE_CURSOR_LEFT:
			im_i_cursor_left();
			break;
		case IM_KE_CURSOR_RIGHT:
			im_i_cursor_right();
			break;
		case IM_KE_CONVERT:
			im_c_start_conversion();
			break;
		case IM_KE_NO_CONV:
			im_i_no_conv();
			break;
		case IM_KE_HIRAGANA:
			im_i_hiragana();
			break;
		case IM_KE_ZENKAKU_KATAKANA:
			im_i_zenkaku_katakana();
			break;
		case IM_KE_HANKAKU_KATAKANA:
			im_i_hankaku_katakana();
			break;
		case IM_KE_ZENKAKU_ALPHA_LOWER:
			im_i_zenkaku_alpha_lower();
			break;
		case IM_KE_HANKAKU_ALPHA_LOWER:
			im_i_hankaku_alpha_lower();
			break;
		case IM_KE_ZENKAKU_ALPHA_UPPER:
			im_i_zenkaku_alpha_upper();
			break;
		case IM_KE_HANKAKU_ALPHA_UPPER:
			im_i_hankaku_alpha_upper();
			break;
		case IM_KE_CLEAR:
			im_i_restart();
			break;
		case IM_KE_BACKSPACE:
			im_i_backspace();
			break;
		case IM_KE_DELETE:
			im_i_delete();
			break;
		default:
			if (im_i_input_romaji(key) >= 0)
				key_eaten = 0;
			break;
		}
		break;
	case IM_MODE_CONVERT:
		switch(event) {
		case IM_KE_CLEAR:
			im_i_restart();
			break;
		case IM_KE_SELECT_PREV:
			im_c_select_prev();
			break;
		case IM_KE_CONVERT:
		case IM_KE_SELECT_NEXT:
			im_c_select_next();
			break;
		case IM_KE_SELECT_FIRST:
			im_c_select_first();
			break;
		case IM_KE_SELECT_LAST:
			im_c_select_last();
			break;
		case IM_KE_BACKSPACE:
			im_set_mode(IM_MODE_INPUT);
			break;
		case IM_KE_DELETE:
			imjhst_clear_save();
			im_set_mode(IM_MODE_INPUT);
			break;
		case IM_KE_COMMIT_ONE:
///_FLF_
			im_c_commit_one();
			break;
		case IM_KE_COMMIT_ALL:
///_FLF_
			im_c_commit_all();
			break;
		case IM_KE_NO_CONV:
			im_c_no_conv();
			break;
		case IM_KE_HIRAGANA:
			im_c_hiragana();
			break;
		case IM_KE_ZENKAKU_KATAKANA:
			im_c_zenkaku_katakana();
			break;
		case IM_KE_HANKAKU_KATAKANA:
			im_c_hankaku_katakana();
			break;
		case IM_KE_ZENKAKU_ALPHA_LOWER:
			im_c_zenkaku_alpha_lower();
			break;
		case IM_KE_HANKAKU_ALPHA_LOWER:
			im_c_hankaku_alpha_lower();
			break;
		case IM_KE_ZENKAKU_ALPHA_UPPER:
			im_c_zenkaku_alpha_upper();
			break;
		case IM_KE_HANKAKU_ALPHA_UPPER:
			im_c_hankaku_alpha_upper();
			break;
		case IM_KE_SEPARATE_LEFT:
			im_c_separation_left();
			break;
		case IM_KE_SEPARATE_RIGHT:
			im_c_separation_right();
			break;
		default:
			if (isgraph(key)) {
_FLF_
				im_c_commit_all();
				if (im_i_input_romaji(key) >= 0)
					key_eaten = 0;
			} else {
				key_eaten = 0;
			}
			break;
		}
		if (strlen(im_pronun) == 0)
			im_i_restart();
		break;
	}
//_FLF_
	if (key_eaten)
		key = -1;
//flf_d_printf("=>key:%04x\n", key);
	return key;
}
void imj::im_init(void)
{
	_im_mode = IM_MODE_OFF;
	strcpy(im_romaji, "");
	strcpy(im_pronun, "");
	im_i_cursor_offset = 0;

	im_i_cur_history_idx = -1;

	im_c_len_to_match = 0;
	im_c_cur_cand_idx = 0;
	strcpy(im_c_cur_candid, "");
	strcpy(im_commit_buffer, "");
	clear_commit_conversion();
}
void imj::im_set_mode(int im_mode)
{
	_im_mode = im_mode;
	switch(_im_mode) {
	case IM_MODE_OFF:
		break;
	case IM_MODE_INPUT:
		im_c_cur_cand_idx = 0;
		strcpy(im_c_cur_candid, "");
		break;
	case IM_MODE_CONVERT:
		break;
	}
}
int imj::im_get_mode(void)
{
	return _im_mode;
}
void imj::im_set_romaji(const char *string)
{
	utf8strcpy_blen(im_romaji, IM_INPUT_LINE_LEN, string);
	imjatoh_romaji_to_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
}
const char *imj::im_get_romaji(char *buf, int buf_len)
{
	if (buf == NULL)
		return im_romaji;
	utf8strcpy_blen(buf, buf_len, im_romaji);
	return buf;
}
void imj::im_set_pronun(const char *pronun)
{
	im_set_romaji("");		// no romaji
	utf8strcpy_blen(im_pronun, IM_INPUT_LINE_LEN, pronun);
	im_i_cursor_offset = strlen(im_pronun);	// set cursor pos to tail of the pronun
flf_d_printf("[%s]\n", im_pronun);
}
const char *imj::im_get_pronun(char *buf, int buf_len)
{
	if (buf == NULL)
		return im_pronun;
	utf8strcpy_blen(buf, buf_len, im_pronun);
	return buf;
}
int imj::im_get_pronun_len(void)
{
	return strlen(im_pronun);
}
int imj::im_get_pronun_len_to_match(void)
{
	return im_c_len_to_match;
}
int imj::im_get_cursor_offset()
{
	return im_i_cursor_offset;
}
//-----------------------------------------------------------------------------
void imj::im_i_clear_cur_history_idx(void)
{
	im_i_cur_history_idx = -1;
}
void imj::im_i_set_cur_history_idx(int hist_idx)
{
	im_i_cur_history_idx = hist_idx;
}
int imj::im_i_get_cur_history_idx(void)
{
	return im_i_cur_history_idx;
}
int imj::im_i_get_num_of_histories(void)
{
//imjihst_dump();
	return imjihst_get_histories();
}
void imj::im_i_select_prev(void)
{
_FLF_
	im_i_check_set_history_idx(im_i_get_cur_history_idx()-1);
}
void imj::im_i_select_next(void)
{
_FLF_
	im_i_check_set_history_idx(im_i_get_cur_history_idx()+1);
}
void imj::im_i_select_first(void)
{
_FLF_
	im_i_check_set_history_idx(0);
}
void imj::im_i_select_last(void)
{
_FLF_
	im_i_check_set_history_idx(im_i_get_num_of_histories()-1);
}
void imj::im_i_check_set_history_idx(int hist_idx)
{
flf_d_printf("hist_idx: %d\n", hist_idx);
	if (hist_idx < 0)
		hist_idx = im_i_get_num_of_histories()-1;
	if (hist_idx >= im_i_get_num_of_histories())
		hist_idx = 0;
flf_d_printf("hist_idx: %d\n", hist_idx);
	im_i_set_cur_history_idx(hist_idx);
	im_i_get_cur_history();
}
const char *imj::im_i_get_cur_history(void)
{
	imjihst_get_nth_history(im_i_cur_history_idx, im_pronun, IM_INPUT_LINE_LEN);
	strcpy_blen(im_romaji, IM_INPUT_LINE_LEN, im_pronun);
	im_i_cursor_offset = strlen(im_pronun);
	return im_pronun;
}

//-----------------------------------------------------------------------------
// im_i_: Input-Method Inputing

int imj::im_i_restart(void)
{
	restart_commit_conversion();
	im_set_mode(IM_MODE_INPUT);
	return im_i_input_romaji(IM_KE_CLEAR);
}
int imj::im_i_backspace(void)
{
	return im_i_input_romaji(IM_KE_BACKSPACE);
}
int imj::im_i_delete(void)
{
	return im_i_input_romaji(IM_KE_DELETE);
}

// Workaround for NKF
#define ZEN_HYP			"\xef\xbc\x8d"	// Zenkaku Hyphen character (U+FF0D)
#define ZEN_YEN			"\xef\xbf\xa5"	// Zenkaku Yen character    (U+FFE5)
#define ZEN_CHOUON		"\xe3\x83\xbc"	// Zenkaku Chouon Kigou character (U+30FC)

// Invisible separator character
///#define IM_INVIS_SEP_CHR			'\x1f'
#define IM_INVIS_SEP_CHR			'\x7f'

int imj::im_i_input_romaji(int chr)
{
	char insert[2+1];
	int off_from;
	int off_to;
//	int offset_r;
	int key_eaten = 1;

//flf_d_printf("chr[%02x]\n", chr);
	im_i_clear_cur_history_idx();
	if (chr == IM_KE_CLEAR) {				// Ctrl-L
		strcpy(im_romaji, "");
		im_i_cursor_offset = strlen(im_pronun);
	} else if (chr == IM_KE_DELETE) {		// Ctrl-G
		off_from = im_i_cursor_offset;
		off_to = utf8s_len_ge(im_pronun, im_i_cursor_offset+1);
		im_i_cursor_offset = imjatoh_delete_romaji(im_romaji, IM_INPUT_LINE_LEN,
		 im_pronun, IM_INPUT_LINE_LEN, off_from, off_to);
flf_d_printf("[%s]\n[%s]\n", im_romaji, im_pronun);
	} else if (chr == IM_KE_BACKSPACE) {	// Ctrl-H
		off_from = utf8s_len_le(im_pronun, im_i_cursor_offset-1);
		off_to = im_i_cursor_offset;
		im_i_cursor_offset = imjatoh_delete_romaji(im_romaji, IM_INPUT_LINE_LEN,
		 im_pronun, IM_INPUT_LINE_LEN, off_from, off_to);
flf_d_printf("[%s]\n[%s]\n", im_romaji, im_pronun);
	} else if (chr < 0x0100) {
		if (isprint(chr)) {
//flf_d_printf("add[%02x]\n", chr);
			if (im_pronun[im_i_cursor_offset] == '\0') {
				insert[0] = chr;
				insert[1] = '\0';
			} else {
				insert[0] = chr;
				insert[1] = IM_INVIS_SEP_CHR;
				insert[2] = '\0';
			}
			im_i_cursor_offset = imjatoh_insert_romaji(im_romaji, IM_INPUT_LINE_LEN,
			 im_pronun, IM_INPUT_LINE_LEN, im_i_cursor_offset, insert);
flf_d_printf("romaji[%s], pronun[%s]\n", im_romaji, im_pronun);
			imjatoh_limit_romaji_len_by_pronun_len(im_romaji, im_pronun);
flf_d_printf("romaji[%s], pronun[%s]\n", im_romaji, im_pronun);
			// adjust cursor offset
//			offset_r = imjatoh_romaji_offset(im_romaji, im_i_cursor_offset) + strlen(insert);
//flf_d_printf("offset_r:%d\n", offset_r);
//			im_i_cursor_offset = imjatoh_hira_offset(im_romaji, im_pronun, offset_r);
			return -1;	// key eaten
		} else {
			key_eaten = 0;
		}
	} else {
		key_eaten = 0;
	}
	if (key_eaten == 0)
		return chr;		// return key not eaten
	imjatoh_romaji_to_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	return -1;			// key eaten
}

void imj::im_i_cursor_left(void)
{
	im_i_cursor_offset = utf8s_len_le(im_pronun, im_i_cursor_offset-1);
}
void imj::im_i_cursor_right(void)
{
	im_i_cursor_offset = utf8s_len_ge(im_pronun, im_i_cursor_offset+1);
}
void imj::im_i_no_conv(void)
{
	strcpy_blen(im_pronun, IM_INPUT_LINE_LEN, im_romaji);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
}
void imj::im_i_hiragana(void)
{
	imjatoh_romaji_to_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_zenkaku_katakana(void)
{
	imjatoh_romaji_to_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_zen_kata(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_hankaku_katakana(void)
{
	imjatoh_romaji_to_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_han_kata(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_zenkaku_alpha_lower(void)
{
	imjhira_to_zen_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_lower(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_zenkaku_alpha_upper(void)
{
	imjhira_to_zen_hira(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_upper(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_hankaku_alpha_lower(void)
{
	imjhira_to_han_kata(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_lower(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_hankaku_alpha_upper(void)
{
	imjhira_to_han_kata(im_romaji, im_pronun, IM_INPUT_LINE_LEN);
	imjhira_to_upper(im_pronun, im_pronun, IM_INPUT_LINE_LEN);
	im_i_cursor_offset = strlen(im_pronun);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("im_pronun:[%s]\n", im_pronun);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_i_commit_all(void)
{
_FLF_
///	imjihst_append(im_pronun);
	im_c_commit_string(im_pronun, im_pronun);
	im_i_restart();
}

//-----------------------------------------------------------------------------
// im_c_: Input-Method Conversion

void imj::im_c_start_conversion(void)
{
//_FLF_
flf_d_printf("[%s]\n", im_pronun);
	imjihst_append(im_pronun);
	im_set_mode(IM_MODE_CONVERT);
	new_convert();
}
//-----------------------------------------------------------------------------
void imj::im_c_separation_left(void)
{
	im_c_len_to_match = utf8s_len_le(im_pronun, im_c_len_to_match-1);
	if (im_c_len_to_match == 0)
		im_c_len_to_match = utf8c_bytes(im_pronun);	// at least one char
	re_convert();
}
void imj::im_c_separation_right(void)
{
	im_c_len_to_match = utf8s_len_ge(im_pronun, im_c_len_to_match+1);
	re_convert();
}
void imj::new_convert(void)
{
	im_c_len_to_match = 0;
	re_convert();
}
void imj::re_convert(void)
{
//flf_d_printf("[%s]:[%s]\n", im_romaji, im_pronun);
	im_c_cur_cand_idx = 0;
	strcpy(im_c_cur_candid, "");
	if (strlen(im_pronun) == 0)
		return;
	im_c_len_to_match = _renbunsetsu.conv_renbunsetsu(im_romaji, im_pronun, im_c_len_to_match);
	if (im_c_len_to_match == 0)
		im_c_len_to_match = utf8c_bytes(im_pronun);	// at least one char
//flf_d_printf("num_of_candids: %d\n", im_c_get_num_of_candids());
	if (im_c_get_num_of_candids() == 0) {
		utf8strcpy_blen_slen(im_c_cur_candid, MAX_CANDID_LEN, im_pronun, im_c_len_to_match);
	} else {
		im_c_get_one_candid(im_c_cur_cand_idx, im_c_cur_candid, MAX_CANDID_LEN);
	}
//flf_d_printf("im_c_cur_candid[%s]\n", im_c_cur_candid);
}

char *imj::im_c_get_cur_candid(char *buf, int buf_len)
{
	if (buf == NULL)
		return im_c_cur_candid;
	return utf8strcpy_blen(buf, buf_len, im_c_cur_candid);
}
char *imj::im_c_get_one_candid(int candid_idx, char *buf, int buf_len)
{
	return _renbunsetsu._renbunsetsu[0]._candidates.get_candid(candid_idx, buf, buf_len);
}
//-----------------------------------------------------------------------------
int imj::im_c_get_num_of_candids(void)
{
	return _renbunsetsu._renbunsetsu[0]._candidates.get_num_of_candids();
}
void imj::im_c_select_prev(void)
{
	im_c_check_set_candid_idx(im_c_get_cur_cand_idx()-1);
}
void imj::im_c_select_next(void)
{
	im_c_check_set_candid_idx(im_c_get_cur_cand_idx()+1);
}
void imj::im_c_select_first(void)
{
	im_c_set_cur_cand_idx(0);
}
void imj::im_c_select_last(void)
{
	im_c_set_cur_cand_idx(im_c_get_num_of_candids()-1);
}
void imj::im_c_check_set_candid_idx(int candid_idx)
{
	if (candid_idx < 0)
		candid_idx = im_c_get_num_of_candids()-1;
	if (candid_idx >= im_c_get_num_of_candids())
		candid_idx = 0;
	im_c_set_cur_cand_idx(candid_idx);
}
void imj::im_c_set_cur_cand_idx(int candid_idx)
{
	im_c_cur_cand_idx = candid_idx;
	_renbunsetsu._renbunsetsu[0]._candid_idx = candid_idx;
	im_c_get_one_candid(im_c_cur_cand_idx, im_c_cur_candid, IM_INPUT_LINE_LEN);
}
int imj::im_c_get_cur_cand_idx(void)
{
	return im_c_cur_cand_idx;
}

char *imj::im_c_get_first_bunsetsu(char *buffer, int buf_len)
{
	return join_bunsetsu(0, 1, "|", buffer, buf_len);
}
char *imj::im_c_get_remaining_bunsetsu(char *buffer, int buf_len, const char *separator)
{
	return join_bunsetsu(1, MAX_BUNSETSU, separator, buffer, buf_len);
}
char *imj::im_c_get_all_bunsetsu(char *buffer, int buf_len, const char *separator)
{
	return join_bunsetsu(0, MAX_BUNSETSU, separator, buffer, buf_len);
}
char *imj::join_bunsetsu(int start_bunsetsu_idx, int end_bunsetsu_idx, const char *separator,
 char *buffer, int buf_len)
{
	int bunsetsu_idx;
	char buf[MAX_CANDID_LEN+1];

	start_bunsetsu_idx = MAX_(0, start_bunsetsu_idx);
	end_bunsetsu_idx = MIN_(_renbunsetsu._num_of_bunsetsu, end_bunsetsu_idx);
	strcpy(buffer, "");
	for (bunsetsu_idx = start_bunsetsu_idx; bunsetsu_idx < end_bunsetsu_idx; bunsetsu_idx++) {
		if (bunsetsu_idx > 0 && _renbunsetsu._renbunsetsu[bunsetsu_idx]._match_len == 0)
			break;
		if (bunsetsu_idx == 0) {
			im_c_get_cur_candid(buf, MAX_CANDID_LEN);
//flf_d_printf("%s\n", buf);
		} else {
			_renbunsetsu._renbunsetsu[bunsetsu_idx]
			 ._candidates.get_candid(0, buf, MAX_CANDID_LEN);
//flf_d_printf("%s\n", buf);
		}
		if (strlen(buffer)) {
			utf8strcat_blen(buffer, buf_len, separator);
		}
//flf_d_printf("%s\n", buffer);
		utf8strcat_blen(buffer, buf_len, buf);
//flf_d_printf("%s\n", buffer);
	}
	return buffer;
}
//-----------------------------------------------------------------------------

void imj::im_c_no_conv(void)
{
	int offset_r;

	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(im_c_cur_candid, IM_INPUT_LINE_LEN, im_romaji, offset_r);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_hiragana(void)
{
	int offset_r;
	char romaji[IM_INPUT_LINE_LEN+1];
	char hiragana[IM_INPUT_LINE_LEN+1];

	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(romaji, IM_INPUT_LINE_LEN, im_romaji, offset_r);
	imjatoh_romaji_to_hira(romaji, hiragana, IM_INPUT_LINE_LEN);
	imjhira_to_zen_hira(hiragana, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("romaji:[%s]\n", romaji);
flf_d_printf("hiragana:[%s]\n", hiragana);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_zenkaku_katakana(void)
{
	int offset_r;
	char romaji[IM_INPUT_LINE_LEN+1];
	char hiragana[IM_INPUT_LINE_LEN+1];

	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(romaji, IM_INPUT_LINE_LEN, im_romaji, offset_r);
	imjatoh_romaji_to_hira(romaji, hiragana, IM_INPUT_LINE_LEN);
	imjhira_to_zen_kata(hiragana, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("romaji:[%s]\n", romaji);
flf_d_printf("hiragana:[%s]\n", hiragana);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_hankaku_katakana(void)
{
	int offset_r;
	char romaji[IM_INPUT_LINE_LEN+1];
	char hiragana[IM_INPUT_LINE_LEN+1];

	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(romaji, IM_INPUT_LINE_LEN, im_romaji, offset_r);
	imjatoh_romaji_to_hira(romaji, hiragana, IM_INPUT_LINE_LEN);
	imjhira_to_han_kata(hiragana, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_romaji:[%s]\n", im_romaji);
flf_d_printf("romaji:[%s]\n", romaji);
flf_d_printf("hiragana:[%s]\n", hiragana);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_zenkaku_alpha_lower(void)
{
	int offset_r;

flf_d_printf("im_romaji:[%s]\n", im_romaji);
	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(im_c_cur_candid, IM_INPUT_LINE_LEN, im_romaji, offset_r);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_zen_hira(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_lower(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_zenkaku_alpha_upper(void)
{
	int offset_r;

flf_d_printf("im_romaji:[%s]\n", im_romaji);
	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(im_c_cur_candid, IM_INPUT_LINE_LEN, im_romaji, offset_r);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_zen_hira(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_upper(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_hankaku_alpha_lower(void)
{
	int offset_r;

flf_d_printf("im_romaji:[%s]\n", im_romaji);
	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(im_c_cur_candid, IM_INPUT_LINE_LEN, im_romaji, offset_r);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_han_kata(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_lower(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}
void imj::im_c_hankaku_alpha_upper(void)
{
	int offset_r;

flf_d_printf("im_romaji:[%s]\n", im_romaji);
	offset_r = imjatoh_romaji_offset(im_romaji, im_c_len_to_match);
	utf8strcpy_blen_slen(im_c_cur_candid, IM_INPUT_LINE_LEN, im_romaji, offset_r);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_han_kata(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
	imjhira_to_upper(im_c_cur_candid, im_c_cur_candid, IM_INPUT_LINE_LEN);
flf_d_printf("im_c_cur_candid:[%s]\n", im_c_cur_candid);
}

void imj::im_c_commit_one(void)
{
	// commit one bunsetsu
	im_c_commit_one__();
}
void imj::im_c_commit_all(void)
{
///_FLF_
	// commit all bunsetsu
	while (_renbunsetsu._num_of_bunsetsu > 0) {
		im_c_commit_one__();
	}
}
void imj::im_c_commit_one__(void)
{
	// commit one bunsetsu
	char pronun[IM_INPUT_LINE_LEN+1];
	char candid[IM_INPUT_LINE_LEN+1];
	int match_len;
	int offset_r;

	match_len = _renbunsetsu._renbunsetsu[0]._match_len;

	utf8strcpy_blen_slen(pronun, IM_INPUT_LINE_LEN, im_pronun, match_len);
	im_c_get_cur_candid(candid, IM_INPUT_LINE_LEN);
	im_c_commit_string(pronun, candid);

	offset_r = MIN_(strlen(im_romaji), imjatoh_romaji_offset(im_romaji, match_len));
	strcpy_ol(im_romaji, &im_romaji[offset_r]);
	strcpy_ol(im_pronun, &im_pronun[match_len]);
	im_i_cursor_offset = utf8s_len_ge(im_pronun, im_i_cursor_offset - match_len);
///flf_d_printf("[%s]:[%s]\n", im_romaji, im_pronun);

	_renbunsetsu.shift_bunsetsu();

	if (strlen(im_pronun)) {
		// continue conversion
///_FLF_
		new_convert();
	} else {
		// return to Input mode
		im_i_restart();
///_FLF_
	}
}
void imj::im_c_commit_string(const char *pronun, const char *conversion)
{
flf_d_printf("[%s]:[%s]\n", pronun, conversion);
	imjchst_append(pronun, conversion);
	utf8strcat_blen(im_commit_buffer, IM_INPUT_LINE_LEN, conversion);
	if (pronun[0])
		concat_commit_conversion(pronun, conversion);
}
const char *imj::im_get_committed_string(char *buf, int buf_len)
{
	buf = get_static_buf_if_null(buf);
	utf8strcpy_blen(buf, buf_len, im_commit_buffer);
	strcpy(im_commit_buffer, "");	// clear
flf_d_printf("[%s]\n", buf);
	return buf;
}
//-----------------------------------------------------------------------------
#define IMJ_CONFIG_CONCAT_LEARNING_2
#ifdef IMJ_CONFIG_CONCAT_LEARNING_2
// 連文節ヒストリ(分割確定した文節を繋げ連文節をヒストリに登録する)
// Ex. [ぶんかつ]:[分割], [かくてい]:[確定]
// ==> [ぶんかつかくてい]:[分割確定]
PRIVATE char commit_pronun2[MAX_INPUT_HISTORY_LEN+1];
PRIVATE char commit_conversion2[MAX_INPUT_HISTORY_LEN+1];
#endif
void clear_commit_conversion(void)
{
#ifdef IMJ_CONFIG_CONCAT_LEARNING_2
	strcpy(commit_pronun2, "");
	strcpy(commit_conversion2, "");
#endif
}
void concat_commit_conversion(const char *pronun, const char *conversion)
{
#ifdef IMJ_CONFIG_CONCAT_LEARNING_2
	if (strlen(commit_pronun2) && strlen(commit_conversion2)) {
		// "aaa" ==> "aaabbb"
		utf8strcat_blen(commit_pronun2, MAX_INPUT_HISTORY_LEN, pronun);
		utf8strcat_blen(commit_conversion2, MAX_INPUT_HISTORY_LEN, conversion);
		imjchst_append(commit_pronun2, commit_conversion2);
	}
	// "bbb"
	utf8strcpy_blen(commit_pronun2, MAX_INPUT_HISTORY_LEN, pronun);
	utf8strcpy_blen(commit_conversion2, MAX_INPUT_HISTORY_LEN, conversion);
#endif
}
void restart_commit_conversion(void)
{
#ifdef IMJ_CONFIG_CONCAT_LEARNING_2
	if (strlen(commit_pronun2) && strlen(commit_conversion2)) {
		imjchst_append(commit_pronun2, commit_conversion2);
	}
#endif
	clear_commit_conversion();
}

// class imjrenbunsetsu -----------------------------------------------------
void imjrenbunsetsu::clear(void)
{
	int bunsetsu_idx;

	strcpy(_romaji, "");
	strcpy(_pronun, "");
	for (bunsetsu_idx = 0; bunsetsu_idx < MAX_BUNSETSU; bunsetsu_idx++) {
		_renbunsetsu[bunsetsu_idx].clear();
	}
	_num_of_bunsetsu = 0;
}
int imjrenbunsetsu::conv_renbunsetsu(char *romaji, char *pronun, int len_to_match)
{
	int bunsetsu_idx;
	int romaji_idx;
	int pronun_idx;

//flf_d_printf("romaji:[%s], pronun:[%s], len_to_match:%d\n", romaji, pronun, len_to_match);
	clear();
	strcpy_blen(_romaji, IM_INPUT_LINE_LEN, romaji);
	strcpy_blen(_pronun, IM_INPUT_LINE_LEN, pronun);

	romaji_idx = 0;
	pronun_idx = 0;
	for (bunsetsu_idx = 0; bunsetsu_idx < MAX_BUNSETSU; bunsetsu_idx++) {
		if (pronun[pronun_idx] == '\0')
			break;		// no more pronun
//flf_d_printf("bunsetsu-%d, pronun: [%s]\n", bunsetsu_idx, &pronun[pronun_idx]);
		_renbunsetsu[bunsetsu_idx].conv_tanbunsetsu(
		 &romaji[romaji_idx], &pronun[pronun_idx], bunsetsu_idx == 0 ? len_to_match : 0);
		pronun_idx += _renbunsetsu[bunsetsu_idx]._match_len;
		romaji_idx += imjatoh_romaji_offset(romaji, pronun_idx);
	}
//flf_d_printf("romaji:[%s], pronun:[%s], len_to_match:%d\n", romaji, pronun, len_to_match);
	_num_of_bunsetsu = bunsetsu_idx;
//dump();
//_renbunsetsu[0].dump();
	return _renbunsetsu[0]._match_len;
}
void imjrenbunsetsu::shift_bunsetsu(void)
{
	int bunsetsu_idx;

	_num_of_bunsetsu--;
	for (bunsetsu_idx = 0; bunsetsu_idx < _num_of_bunsetsu; bunsetsu_idx++) {
		memcpy_ol(&_renbunsetsu[bunsetsu_idx], &_renbunsetsu[bunsetsu_idx+1],
		 sizeof(_renbunsetsu[bunsetsu_idx]));
	}
}
void imjrenbunsetsu::dump(void)
{
	int bunsetsu_idx;

	for (bunsetsu_idx = 0; bunsetsu_idx < MAX_BUNSETSU; bunsetsu_idx++) {
		if (_renbunsetsu[bunsetsu_idx]._match_len) {
flcf_d_printf("bunsetsu[%d]:---------------------\n", bunsetsu_idx);
			_renbunsetsu[bunsetsu_idx].dump();
		}
	}
}

// class imjtanbunsetsu -----------------------------------------------------
void imjtanbunsetsu::clear(void)
{
	strcpy(_romaji, "");
	strcpy(_pronun, "");
	_match_len = 0;
	_candid_idx = 0;
	_candidates.clear();
}
int imjtanbunsetsu::conv_tanbunsetsu(char *romaji, char *pronun, int len_to_match)
{
	int hst_match_len;		// history
	int dic_match_len;		// dictionary
	int candid_idx;
	char buf1[IM_INPUT_LINE_LEN+1];
	char buf2[IM_INPUT_LINE_LEN+1];
	int matches;
	int len;

//flf_d_printf("romaji:[%s], pronun:[%s], len_to_match:%d\n", romaji, pronun, len_to_match);
	clear();
	strcpy_blen(_romaji, IM_INPUT_LINE_LEN, romaji);
	strcpy_blen(_pronun, IM_INPUT_LINE_LEN, pronun);

	if (len_to_match <= 0) {
//flf_d_printf("romaji:[%s], pronun:[%s], len_to_match:%d\n", romaji, pronun, len_to_match);
		hst_match_len = imjchst_search_longest_match_len(pronun);
		dic_match_len = imjdic::imjdic_search_total_len(pronun, len_to_match, &_candidates);
		// if conversion-history gotten, give priority to it
		if (hst_match_len >= dic_match_len)
			len_to_match = hst_match_len;
		if (len_to_match <= 0) {
			len_to_match = MAX_(dic_match_len, hst_match_len);
//flf_d_printf("dic_match_len:%d hst_match_len:%d len_to_match:%d\n",
// dic_match_len, hst_match_len, len_to_match);
		}
		if (len_to_match <= 0) {
			// if no match, select some characters
			if (is_han(pronun)) {
				// if hankaku, select whole hankaku string
				len_to_match = skip_han(pronun) - pronun;
			} else {
				// if no match, whole pronun
				len_to_match = strlen(pronun);
			}
		}
//flf_d_printf("dic_match_len:%d hst_match_len:%d len_to_match:%d\n",
// dic_match_len, hst_match_len, len_to_match);
	}
//flf_d_printf("len_to_match: %d\n", len_to_match);
	_candidates.clear();

	// list up candidates from conversion history
	matches = imjchst_get_num_of_matches(pronun, len_to_match);
	for (candid_idx = 0; candid_idx < matches; candid_idx++) {
//	for (candid_idx = matches-1; candid_idx >= 0; candid_idx--) {
		if (imjchst_get_nth_match(candid_idx, pronun, len_to_match, buf1, MAX_CANDID_LEN))
			_candidates.append_one_candid(buf1);
	}

	// list up candidates from dictionary
	imjdic::imjdic_search_total_len(pronun, len_to_match, &_candidates);
///_candidates.dump_candids();

	utf8strcpy_blen_slen(buf1, IM_INPUT_LINE_LEN, pronun, len_to_match);
	_candidates.append_one_candid(buf1);		// "かんじ"

	imjhira_to_zen_kata(buf1, buf2, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf2);		// "カンジ"

	imjhira_to_han_kata(buf1, buf2, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf2);		// "ｶﾝｼﾞ"

	len = imjatoh_romaji_offset(romaji, len_to_match);

	utf8strcpy_blen_slen(buf1, IM_INPUT_LINE_LEN, romaji, len);
	imjhira_han_to_zen(buf1, buf2, IM_INPUT_LINE_LEN);
	imjhira_to_lower(buf2, buf1, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf1);		// ”ｋａｎｎｊｉ"

	utf8strcpy_blen_slen(buf1, IM_INPUT_LINE_LEN, romaji, len);
	imjhira_han_to_zen(buf1, buf2, IM_INPUT_LINE_LEN);
	imjhira_to_upper(buf2, buf1, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf1);		// ”ＫＡＮＮＪＩ"

	utf8strcpy_blen_slen(buf1, IM_INPUT_LINE_LEN, romaji, len);
	imjhira_to_lower(buf1, buf2, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf2);		// "kannji"

	utf8strcpy_blen_slen(buf1, IM_INPUT_LINE_LEN, romaji, len);
	imjhira_to_upper(buf1, buf2, IM_INPUT_LINE_LEN);
	_candidates.append_one_candid(buf2);		// "KANNJI"

	_match_len = len_to_match;
///dump();
	return len_to_match;
}
void imjtanbunsetsu::dump(void)
{
	char buf[MAX_CANDID_LEN+1];

flcf_d_printf("_romaji: [%s]\n", _romaji);
flcf_d_printf("_pronun: [%s]\n", _pronun);
flcf_d_printf("_match_len: %d\n", _match_len);
flcf_d_printf("_num_of_candids(): %d\n", _candidates.get_num_of_candids());
flcf_d_printf("_candid: [%s]\n", _candidates.get_candid(_candid_idx, buf, MAX_CANDID_LEN));
_candidates.dump_candids();
}

// 辞書検索 -------------------------------------------------------------------
FILE *imjdic::fp_dic = NULL;
int imjdic::dic_num_of_records = 0;
long imjdic::dic_record_offsets[MAX_DIC_RECORDS] = { 0 };
long imjdic::bsearch_match_offset = 0;
int imjdic::dic_search_words = 0;
int imjdic::dic_search_records = 0;
int imjdic::dic_max_search_records = 0;
int imjdic::dic_total_search_records = 0;
int imjdic::dic_avg_search_records = 0;

int imjdic::imjdic_open(const char *file_path)
{
	if (file_path == NULL)
		file_path = DIC_FILE_NAME;
flf_d_printf("dic-file-path:[%s]\n", file_path);
	if ((fp_dic = fopen(file_path, "r")) == NULL) {
		_ERR_
		return 1;
	}
	imjdic_get_offsets_of_all_records();
//	imjdic_sort();
//	imjdic_dump_all_tangos();
	return 0;
}
int imjdic::imjdic_close(void)
{
	int ret = 0;

	if (fp_dic) {
		if (fclose(fp_dic) < 0) {
			_FL_
			ret = 1;
		}
		fp_dic = NULL;
	}
	return ret;
}
int imjdic::imjdic_read_record(long offset, char *buffer, int buf_len)
{
	dic_search_records++;
	if (offset >= 0)
		fseek(fp_dic, offset, SEEK_SET);
	if (fgets(buffer, buf_len, fp_dic) == NULL) {
		return -1;
	}
	cut_line_tail_crlf(buffer);
	return 0;
}

int imjdic::imjdic_get_offsets_of_all_records(void)
{
	int idx;
	char dic_rec[MAX_DIC_LINE_LEN+1];

	fseek(fp_dic, 0, SEEK_SET);
	for (idx = 0; idx < MAX_DIC_RECORDS; idx++) {
		dic_record_offsets[idx] = ftell(fp_dic);
		if (imjdic_read_record(-1, dic_rec, MAX_DIC_LINE_LEN) < 0) {
			break;
		}
	}
	dic_num_of_records = idx;
//
flf_d_printf("total dic records: %d\n", dic_num_of_records);
	return 0;
}
int imjdic::imjdic_sort(void)
{
flf_d_printf("sorting dictionary %d records...\n", dic_num_of_records);
	qsort(dic_record_offsets, dic_num_of_records, sizeof(long), imjdic_qsort_compare);
flf_d_printf("sorting complete.\n");
flf_d_printf("dic_search_records: %d\n", dic_search_records);
	return 0;
}
// Sort dictionary lines by character code.
int imjdic::imjdic_qsort_compare(const void *aa, const void *bb)
{
	char line_a[MAX_DIC_LINE_LEN+1];
	char line_b[MAX_DIC_LINE_LEN+1];
	int ret;

	fseek(fp_dic, *(const long *)aa, SEEK_SET);
	if (fgets(line_a, MAX_DIC_LINE_LEN, fp_dic) == NULL) {
		return 0;
	}
	fseek(fp_dic, *(const long *)bb, SEEK_SET);
	if (fgets(line_b, MAX_DIC_LINE_LEN, fp_dic) == NULL) {
		return 0;
	}
	ret = strcmp(line_a, line_b);
//flf_d_printf("%s%s%s\n",
// line_a,
// ret > 0 ? " > " : (ret < 0 ? " < " : " = "),
// line_b);
	return ret;
}
void imjdic::imjdic_dump_all_tangos(void)
{
	int rec_idx;
	char dic_rec[MAX_DIC_LINE_LEN+1];
	int candids;
	int candid_idx;
	char pronun[MAX_CANDID_LEN+1];
	char gokan[MAX_CANDID_LEN+1];
	char hinshi[MAX_CANNA_HINSHI_CODE_LEN+1];
#define MAX_KATSUYOU_LIST_LEN	500
	char katsuyous[MAX_KATSUYOU_LIST_LEN+1];
	char buffer[MAX_DIC_LINE_LEN+1];

	for (rec_idx = 0; rec_idx < MAX_DIC_RECORDS; rec_idx++) {
		if (imjdic_read_record(dic_record_offsets[rec_idx], dic_rec, MAX_DIC_LINE_LEN) < 0) {
			break;
		}
		imjdic_get_pronun(dic_rec, pronun, MAX_CANDID_LEN);
//		flf_d_printf("$%s\n", pronun);
		candids = imjdic_get_num_of_candids(dic_rec);
		for (candid_idx = 0; candid_idx < candids; candid_idx++) {
			imjdic_get_one_gokan_hinshi(dic_rec, candid_idx, gokan, MAX_CANDID_LEN, hinshi);
			imjdic_get_katsuyous_from_hinshi_code(hinshi, katsuyous, MAX_KATSUYOU_LIST_LEN);
			concat_bunsetsu(gokan, katsuyous, buffer, MAX_DIC_LINE_LEN);
			if (*katsuyous) {
				flf_d_printf(HINSHI_PREF_STR "%s [%s]\n", hinshi, buffer);
			}
		}
	}
flf_d_printf("%d records\n", rec_idx);
}

struct bsearch_key {
	char *pronun;				// I:search key
	int len_to_match;			// I:length to compare
};

int imjdic::imjdic_search_total_len(char *pronun, int len_to_match, imjcandidates *_candidates)
{
	int gokan_len_to_match;				// len of "か"
	int match_len;
	int max_match_len;

//flf_d_printf("pronun:[%s], len_to_match: %d\n", pronun, len_to_match);
	if (strlen(pronun) == 0)
		return 0;

	dic_search_words++;
	dic_search_records = 0;
	if (len_to_match <= 0) {
		// determine len_to_match
		max_match_len = 0;
		for (gokan_len_to_match = utf8c_bytes(pronun); ;	// one char ...
		 gokan_len_to_match = utf8s_len_ge(pronun, gokan_len_to_match+1)) {
//flf_d_printf("gokan_len_to_match: %d\n", gokan_len_to_match);
			match_len = imjdic_search_gokan_katsuyou(pronun, gokan_len_to_match, 0, _candidates);
			max_match_len = MAX_(max_match_len, match_len);
			if (gokan_len_to_match >= MIN_((int)strlen(pronun), MAX_DIC_PRONUN_LEN))
				break;
		}
		len_to_match = max_match_len;
		if (len_to_match <= 0) {
			len_to_match = utf8c_bytes(pronun);	// one char
		}
//flf_d_printf("pronun:[%s], len_to_match: %d, max_match_len: %d\n",
// pronun, len_to_match, max_match_len);
	}
	max_match_len = 0;
	for (gokan_len_to_match = utf8c_bytes(pronun); ;
	 gokan_len_to_match = utf8s_len_ge(pronun, gokan_len_to_match+1)) {
		match_len = imjdic_search_gokan_katsuyou(pronun, gokan_len_to_match, len_to_match,
		 _candidates);
		max_match_len = MAX_(max_match_len, match_len);
		if (gokan_len_to_match >= MIN_((int)strlen(pronun), MAX_DIC_PRONUN_LEN))
			break;
	}
//flf_d_printf("pronun:[%s], len_to_match: %d, max_match_len: %d\n",
// pronun, len_to_match, max_match_len);
//	imjdic_disp_search_stats(pronun);
	return max_match_len;
}

int imjdic::imjdic_search_gokan_katsuyou(char *pronun,
 int gokan_len_to_match, int gok_kat_len_to_match, imjcandidates *_candidates)
{
	struct bsearch_key key;
	long *result_of_search;
	char dic_rec[MAX_DIC_LINE_LEN+1];
	int match_len = 0;

	if (fp_dic == NULL)
		return 0;
//flf_d_printf("pronun:[%s], gokan_len_to_match: %d, gok_kat_len_to_match: %d\n",
// pronun, gokan_len_to_match, gok_kat_len_to_match);
	key.pronun = pronun;
	key.len_to_match = gokan_len_to_match;
	bsearch_match_offset = 0;
	result_of_search = (long *)bsearch(&key, dic_record_offsets, dic_num_of_records, sizeof(long),
	 imjdic_bsearch_compare);
	if (result_of_search) {
		if (imjdic_read_record(bsearch_match_offset, dic_rec, MAX_DIC_LINE_LEN) >= 0) {
//flf_d_printf("dic_rec: [%s]\n", dic_rec);
			match_len = imjdic_compare_katsuyou(dic_rec, pronun,
			 gokan_len_to_match, gok_kat_len_to_match,
			 _candidates);
		}
	}
//flf_d_printf("pronun:[%s], match_len: %d\n", pronun, match_len);
	return match_len;
}
int imjdic::imjdic_bsearch_compare(const void *key, const void *element)
{
	char *pronun;
	int len_to_match;
	long match_offset;
	char dic_rec[MAX_DIC_LINE_LEN+1];
	int candid_len;
	int cmp;
//	int match_len = 0;
	int ret = 0;

	pronun = ((const struct bsearch_key *)key)->pronun;
	len_to_match = ((const struct bsearch_key *)key)->len_to_match;
	match_offset = *(const long *)element;
	if (imjdic_read_record(match_offset, dic_rec, MAX_DIC_LINE_LEN) < 0) {
		return -1;
	}
	candid_len = imjdic_candidate_len(dic_rec);
//flf_d_printf("dic_rec: %d,[%s]\n", match_len, dic_rec);
	// 指定長一致
	if (len_to_match == candid_len) {			// "ABC" "ABC"
		cmp = strncmp(pronun, dic_rec, len_to_match);
		ret = cmp;
//		match_len = len_to_match;
//flf_d_printf("%2d:[%s]%s[%s]\n", len_to_match, pronun,
// ret > 0 ? " > " : (ret < 0 ? " < " : " = "),
// dic_rec);
	} else if (len_to_match > candid_len) {	// "ABC" > "AB"
		cmp = strncmp(pronun, dic_rec, candid_len);
		ret = cmp;
		if (ret == 0) {
			ret = +1;
//			match_len = candid_len;
		}
//flf_d_printf("%2d:[%s]%s[%s]\n", candid_len, pronun,
// ret > 0 ? " > " : (ret < 0 ? " < " : " = "),
// dic_rec);
	} else /* if (len_to_match < candid_len) */ {	// "ABC" < "ABCD"
		cmp = strncmp(pronun, dic_rec, len_to_match);
		ret = cmp;
		if (ret == 0) {
			ret = -1;
//			match_len = len_to_match;
		}
//flf_d_printf("%2d:[%s]%s[%s]\n", len_to_match, pronun,
// ret > 0 ? " > " : (ret < 0 ? " < " : " = "),
// dic_rec);
	}
	bsearch_match_offset = match_offset;
	return ret;
}
int imjdic::imjdic_compare_katsuyou(char *dic_rec, char *pronun,
 int gokan_len, int gok_kat_len_to_match, imjcandidates *_candidates)
{
	int candids;
	int candid_idx;
	char gokan[MAX_CANDID_LEN+1];
	char hinshi[MAX_CANNA_HINSHI_CODE_LEN+1];
	char extended[MAX_CANDID_LEN+1];
	char katsuyous[MAX_KATSUYOU_LIST_LEN+1];
	const char *ptr_k;
	int katsuyou_len;
	int katsuyou_match_len = 0;

//flf_d_printf("pronun:[%s], katsuyou:[%s], gokan_len:%d, gok_kat_len_to_match:%d\n",
// pronun, &pronun[gokan_len], gokan_len, gok_kat_len_to_match);
///	if (strlen(&pronun[gokan_len]) == 0) {	// no katsuyou part
///_FLF_
///		return gokan_len;
///	}
	candids = imjdic_get_num_of_candids(dic_rec);
//flf_d_printf("candids: %d\n", candids);
	for (candid_idx = 0; candid_idx < candids; candid_idx++) {
		imjdic_get_one_gokan_hinshi(dic_rec, candid_idx, gokan, MAX_CANDID_LEN, hinshi);
		imjdic_get_katsuyous_from_hinshi_code(hinshi, katsuyous, MAX_KATSUYOU_LIST_LEN);
		for (ptr_k = katsuyous; ; ptr_k = skip_word_sp(ptr_k)) {
///flf_d_printf("ptr_k: [%s]\n", ptr_k);
			if (gok_kat_len_to_match <= 0) {
				// 最長一致
///flf_d_printf("[%s] ?? %d:[%s]\n", &pronun[gokan_len], word_len(ptr_k), ptr_k);
				if (strncmp(&pronun[gokan_len], ptr_k, word_len(ptr_k)) == 0) {
					katsuyou_match_len = MAX_(word_len(ptr_k), katsuyou_match_len);
				}
			} else {
				// 指定長一致
				katsuyou_len = gok_kat_len_to_match - gokan_len;
///flf_d_printf("%d:[%s] ?? [%s]\n", katsuyou_len, &pronun[gokan_len], ptr_k);
				if (word_len(ptr_k) == katsuyou_len
				 && strncmp(&pronun[gokan_len], ptr_k, word_len(ptr_k)) == 0) {
					strcpy(extended, gokan);
										// "買" + "います"
					utf8strcat_blen_slen(extended, MAX_CANDID_LEN, ptr_k, word_len(ptr_k));
					_candidates->append_one_candid(extended);
					katsuyou_match_len = katsuyou_len;
				}
			}
			if (*ptr_k == '\0')
				break;
		}
	}
///_candidates->dump_candids();
	return gokan_len + katsuyou_match_len;
}

int imjdic::imjdic_disp_search_stats(char *pronun)
{
fl_d_printf("(((((((((((((((((((((((((((((((((((((((((((((((((\n");
fl_d_printf("dic_search_records: %d:[%s]\n", dic_search_records, pronun);
	dic_max_search_records = MAX_(dic_search_records, dic_max_search_records);
fl_d_printf("dic_max_search_records: %d\n", dic_max_search_records);
	dic_total_search_records += dic_search_records;
	dic_avg_search_records = dic_total_search_records
	 / (dic_search_words == 0 ? 1 : dic_search_words);
fl_d_printf("dic_search_words: %d\n", dic_search_words);
fl_d_printf("dic_avg_search_records: %d\n", dic_avg_search_records);
fl_d_printf(")))))))))))))))))))))))))))))))))))))))))))))))))\n");
	return 0;
}

int imjdic::imjdic_get_pronun(char *dic_rec, char *pronun, int buf_len)
{
	int len;

	len = imjdic_candidate_len(dic_rec);
	utf8strcpy_blen_slen(pronun, buf_len, dic_rec, len);
	return len;
}
int imjdic::imjdic_get_num_of_candids(char *dic_rec)
{
	char *ptr;
	int candid_cnt;
	int prev_is_hinshi = 0;

	ptr = dic_rec;		// かんじ #KJ*500 漢字 幹事 ...
	ptr += imjdic_next_candidate(ptr);
	for (candid_cnt = 0; *ptr; ) {
//flf_d_printf("%d:[%s]\n", candid_cnt, ptr);
		if (prev_is_hinshi == 0 && *ptr == HINSHI_PREF_CHR) {
			// #KJ*500 漢字 幹事 ...
			prev_is_hinshi = 1;
		} else {
			// 漢字 幹事 ...
			prev_is_hinshi = 0;
			candid_cnt++;
		}
		ptr += imjdic_next_candidate(ptr);
	}
	return candid_cnt;
}
int imjdic::imjdic_get_one_gokan_hinshi(char *dic_rec, int candid_idx,
 char *gokan, int buf_len, char *hinshi_code)
{
	char *ptr;
	const char *ptr_hinshi = HINSHI_CODE_KJ;
	int candid_cnt;
	int prev_is_hinshi = 0;

	strcpy(gokan, "");
	ptr = dic_rec;
	ptr += imjdic_next_candidate(ptr);
	for (candid_cnt = 0; *ptr; ) {
//flf_d_printf("%d:[%s]\n", candid_cnt, ptr);
		if (prev_is_hinshi == 0 && *ptr == HINSHI_PREF_CHR) {
			// #KJ*500 漢字 幹事 ...
			ptr_hinshi = ptr;
			prev_is_hinshi = 1;
		} else {
			// 漢字 幹事 ...
			prev_is_hinshi = 0;
			if (candid_cnt >= candid_idx)
				break;
			candid_cnt++;
		}
		ptr += imjdic_next_candidate(ptr);
	}
	utf8strcpy_blen_slen(gokan, buf_len, ptr, imjdic_candidate_len(ptr));
	if (hinshi_code) {
		strcpy_blen_slen(hinshi_code, MAX_CANNA_HINSHI_CODE_LEN,
		 ptr_hinshi, imjdic_candidate_len(ptr_hinshi));
		strcpy(hinshi_code, get_hinshi_code(hinshi_code));
	}
//
flf_d_printf("imjdic_get_one_gokan_hinshi(%d):[%s]:[%s]\n",
//
 candid_idx, gokan, hinshi_code);
	return strlen(gokan);
}

// "#K5*500" ==> "K5"
char *imjdic::get_hinshi_code(char *hinshi_code)
{
	char *ptr;

	if (hinshi_code[0] == HINSHI_PREF_CHR)
		hinshi_code++;		// "#K5*500" ==> "K5*500"
	for (ptr = hinshi_code; *ptr; ptr++) {
		if (*ptr == '*')
			*ptr = '\0';	// "K5*500" ==> "K5"
	}
	return hinshi_code;
}

int imjdic::imjdic_next_candidate(char *dic_rec)
{
	char *ptr;

	ptr = dic_rec;
	ptr += imjdic_candidate_len(ptr);
	ptr += space_len(ptr);
	return ptr - dic_rec;
}
int imjdic::imjdic_candidate_len(const char *dic_rec)
{
	const char *ptr;

	ptr = dic_rec;
	if (*ptr)
		ptr++;		// skip one char even if it is ' '
	for ( ; *ptr; ptr++) {
		if (*ptr == ' ')
			break;
	}
	return ptr - dic_rec;
}

// 動詞/形容詞 活用 -----------------------------------------------------------
// imjkatsuyou_...

PRIVATE const char *katsuyou_table[] = {
// canna-dic品詞コード  活用RegExp
// 動詞
 "abcdefg",
 "K5   く(|な)|か(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |き(|た|たい|ま|ます|ませ|まし)|け(|ば|る|な|ない|ぬ)|こ(|う)|い(た|ちゃ|     て|と)",
 "C5r  く(|な)|か(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |き(|た|たい|ま|ます|ませ|まし)|け(|ば|る|な|ない|ぬ)|こ(|う)|っ(た|ちゃ|     て|と)",
 "G5   ぐ(|な)|が(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |ぎ(|た|たい|ま|ます|ませ|まし)|げ(|ば|る|な|ない|ぬ)|ご(|う)|い(だ|ぢゃ|じゃ|で|ど)",
 "S5   す(|な)|さ(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |し(|た|たい|ま|ます|ませ|まし)|せ(|ば|る|な|ない|ぬ)|そ(|う)|し(た|ちゃ|     て|と)",
 "T5   つ(|な)|た(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |ち(|た|たい|ま|ます|ませ|まし)|て(|ば|る|な|ない|ぬ)|と(|う)|っ(た|ちゃ|     て|と)",
 "N5   ぬ(|な)|な(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |に(|た|たい|ま|ます|ませ|まし)|ね(|ば|る|な|ない|ぬ)|の(|う)|ん(だ|ぢゃ|じゃ|で|ど)",
 "B5   ぶ(|な)|ば(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |び(|た|たい|ま|ます|ませ|まし)|べ(|ば|る|な|ない|ぬ)|ぼ(|う)|ん(だ|ぢゃ|じゃ|で|ど)",
 "M5   む(|な)|ま(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |み(|た|たい|ま|ます|ませ|まし)|め(|ば|る|な|ない|ぬ)|も(|う)|ん(だ|ぢゃ|じゃ|で|ど)",
 "R5   る(|な)|ら(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |り(|た|たい|ま|ます|ませ|まし)|れ(|ば|る|な|ない|ぬ)|ろ(|う)|っ(た|ちゃ|     て|と)",
 "W5   う(|な)|わ(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |い(|た|たい|ま|ます|ませ|まし)|え(|ば|る|な|ない|ぬ)|お(|う)|っ(た|ちゃ|     て|と)",
 "KS   る(|な)|    な|ない|なけれ|なければ|ず|ぬ|ねば|ん|(|ら)(れ|れる)|らるる|    た|たい|ま|ます|ませ|まし |れ(|ば|る|な|ない|ぬ)|よ(|う)|   た|ちゃ|     て|と|ろ",
 "SX する(|な)|さ(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |し(|た|たい|ま|ます|ませ|まし)|せ(|ば|る|な|ない|ぬ)|そ(|う)|し(た|ちゃ|     て|と)   |すれば",
 "ZX ずる(|な)|じ(|な|ない|なけれ|なければ|ず|ぬ|ねば|   (|ら)(れ|れる)|らるる|    た|たい|ま|ます|ませ|まし |れ(|ば|る|な|ない|ぬ)|よ(|う)|   た|ちゃ|     て|と|ろ)|ずれば",
 "L5   る(|な)|ら(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |り(|た|たい|ま|ます|ませ|まし)|れ(|ば|る|な|ない|ぬ)|ろ(|う)|っ(た|ちゃ|     て|と)   |い(|ま|ます|ませ|まし)",
 "U5   う(|な)|わ(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |い(|た|たい|ま|ます|ませ|まし)|え(|ば|る|な|ない|ぬ)|お(|う)|う(た|ちゃ|     て|と)",
 "aru  る(|な)|ら(|な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|るる)        |り(|た|たい|ま|ます|ませ|まし)|れ(|ば|る|な|ない|ぬ)|ろ(|う)|っ(た|ちゃ|     て|と)",
 "kxuru   |な",
 "kxure                                                                            た|     ま|ます|ませ|まし |れば  |る|な|ない|ぬ               |ちゃ|     て|と)",
 "kxi                                                                            ( た|たい|ま|ます|ませ|まし)|                                (た|ちゃ|     て|と)",
 "kxo              な|ない|なけれ|なければ|ず|ぬ|ねば|ん|(|ら)(れ|れる)|らるる|                                                     よ(|う)|い",
 "sxu2 る(|な)|                                                                                               れば",
 "sxi              な|ない|なけれ|なければ|ず                                 |    た|たい|ま|ます|ませ|まし |                      よ(|う)|   た|ちゃ|     て|と|ろ",
 "sxe                                      ず|ぬ|ねば|ん|( ら)(れ|れる)|                                                            よ",
// 形容詞
 "KY   い | く(|な|ない) | かっ(|た) | けれ(|ば) | かろ(|う) | き | かれ",
 ""
};
const char *ex_katsuyou[] = {	// 活用例
 "K5    書",
 "C5r   行",
 "G5    漕",
 "S5    消",
 "T5    勝",
 "N5    死",
 "B5    呼",
 "M5    読",
 "R5    知",
 "W5    笑",
 "KS    見",
 "SX    資",
 "ZX    演",
 "L5    なさ",
 "U5    問",
 "aru   有",
 "kxuru 見てくる",
 "kxure 見てくれ",
 "kxi   来",
 "kxo   来",
 "sxu2  す",
 "sxi   し",
 "sxe   せ",
// 形容詞
 "KY    美し",
 ""
};

#ifdef IMJ_BUILT_IN_TEST
void imjdic_test_katsuyou(void)
{
#if 0
	const char *str1;
	const char *str2;
	const char *re;
#endif
	int max_katsuyou_len = 0;
	int gokan_idx;
	char katsuyous[MAX_KATSUYOU_LIST_LEN+1];
	char buffer[MAX_KATSUYOU_LIST_LEN+1];

#if 0
	str1 = "abc ";
	str2 = "def ";
	concat_bunsetsu(str1, str2, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]+[%s]\n==>[%s]\n", str1, str2, buffer);
	str1 = "a b";
	str2 = "c d e";
	concat_bunsetsu(str1, str2, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]+[%s]\n==>[%s]\n", str1, str2, buffer);
	str1 = "a b";
	str2 = " c d e";
	concat_bunsetsu(str1, str2, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]+[%s]\n==>[%s]\n", str1, str2, buffer);
	str1 = "";
	str2 = "c d e";
	concat_bunsetsu(str1, str2, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]+[%s]\n==>[%s]\n", str1, str2, buffer);
	str1 = "a b c";
	str2 = "";
	concat_bunsetsu(str1, str2, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]+[%s]\n==>[%s]\n", str1, str2, buffer);
//	re = "(c|d)";
//	expand_regexp(re, "a b ", buffer, MAX_KATSUYOU_LIST_LEN);
//	flf_d_printf("[%s]\n==>\n[%s]\n", re, buffer);
	re = "あい うえ おか";
	expand_regexp(re, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]\n==>[%s]\n----\n", re, buffer);
	re = "(あ|い)(う|え)";
	expand_regexp(re, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]\n==>[%s]\n----\n", re, buffer);
	re = "か(|な|ない|(|ら)(れ|れる))";
	expand_regexp(re, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]\n==>[%s]\n----\n", re, buffer);
	re = "か(|な|ない|(|ら)(れ|れる))|き(|た|たい|ま|ます|ませ|まし)";
	expand_regexp(re, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]\n==>[%s]\n----\n", re, buffer);
	re = "じ((|な|ない|れ|れる|る|るる)|(|た|たい|ま|ます|ませ|まし))";
	expand_regexp(re, buffer, MAX_KATSUYOU_LIST_LEN);
	flf_d_printf("[%s]\n==>[%s]\n----\n", re, buffer);
#endif
	for (gokan_idx = 0; ex_katsuyou[gokan_idx][0]; gokan_idx++) {
		imjdic_get_katsuyous_from_hinshi_code(ex_katsuyou[gokan_idx],
		 katsuyous, MAX_KATSUYOU_LIST_LEN);
		max_katsuyou_len = MAX_(max_katsuyou_len, strlen(katsuyous));
		concat_bunsetsu(skip_word_sp(ex_katsuyou[gokan_idx]), katsuyous,
		 buffer, MAX_KATSUYOU_LIST_LEN);
		flf_d_printf("[%s]\n[%s]\n", ex_katsuyou[gokan_idx], buffer);
	}
flf_d_printf("max_katsuyou_len: %d\n", max_katsuyou_len);
}
#endif // IMJ_BUILT_IN_TEST

// "K5" ==> "く くな か かな かない ..."
char *imjdic_get_katsuyous_from_hinshi_code(const char *code, char *buffer, int buf_len)
{
	int tbl_idx;

	strcpy(buffer, "");
	for (tbl_idx = 0; katsuyou_table[tbl_idx][0]; tbl_idx++) {
//flf_d_printf("katsuyou_table[%d]:[%s]\n", tbl_idx, katsuyou_table[tbl_idx]);
		if (word_len(code) == word_len(katsuyou_table[tbl_idx])
		 && strncmp(code, katsuyou_table[tbl_idx], word_len(code)) == 0) {
			expand_regexp(&katsuyou_table[tbl_idx][next_word(katsuyou_table[tbl_idx])],
			 buffer, buf_len);
		}
	}
//flf_d_printf("katsuyou_code: [%s] ==> katsuyou: [%s]\n", code, buffer);
	return buffer;
}

#define MAX_KATSUYOU_LEN	(3*20)	// じなければ
// regexp = bunsetsu | bunsetsu ...
// bunsetsu = chars chars ...
// chars = ( exgexp )
// な|ない|なけれ|なければ|ず|ぬ|ねば|ん|れ|れる|る|るる
// か(|な|ない|(|ら)(れ|れる))
PRIVATE const char *expand_regexp(const char *regexp, char *buf_ret, int buf_len)
{
	return parse_re_parallel(regexp, buf_ret, buf_len);
}

// "abc|def|ghi" ==> "abc def ghi"
PRIVATE const char *parse_re_parallel(const char *regexp, char *buf_ret, int buf_len)
{
	const char *ptr_re;
	char bunsetsu[MAX_KATSUYOU_LIST_LEN+1];

//_FLF0_
//flf_d_printf("regexp:[%s]\n", regexp);
	strcpy(buf_ret, "");
	for (ptr_re = regexp; *ptr_re; ) {
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
		ptr_re = skip_space(ptr_re);
		ptr_re = parse_re_bunsetsu(ptr_re, bunsetsu, MAX_KATSUYOU_LIST_LEN);
		utf8strcat_blen(buf_ret, buf_len, bunsetsu);
		ptr_re = skip_space(ptr_re);
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
		if (*ptr_re == '|') {
			ptr_re++;
		} else {
			break;
		}
		add_space_separator(buf_ret, buf_len);
//flf_d_printf("buf_ret:[%s]\n", buf_ret);
	}
//flf_d_printf("[%s]\n==>\n[%s]\n", regexp, buf_ret);
//_FLF9_
	return ptr_re;
}
// "abc def ghi" ==> "abcdefghi"
// "(a|b)(c|d|e)" ==> "ac ad ae bc bd be"
PRIVATE const char *parse_re_bunsetsu(const char *regexp, char *buf_ret, int buf_len)
{
	const char *ptr_re;
	char bunsetsu[MAX_KATSUYOU_LIST_LEN+1];
	char buf_call[MAX_KATSUYOU_LIST_LEN+1];

//_FLF0_
//flf_d_printf("regexp:[%s]\n", regexp);
	strcpy(buf_ret, "");
	for (ptr_re = regexp; *ptr_re; ) {
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
		ptr_re = parse_re_chars(ptr_re, bunsetsu, MAX_KATSUYOU_LIST_LEN);
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
		concat_bunsetsu(buf_ret, bunsetsu, buf_call, MAX_KATSUYOU_LIST_LEN);
		utf8strcpy_blen(buf_ret, buf_len, buf_call);
		if (*ptr_re == '|' || *ptr_re == ')') {
			break;
		}
//flf_d_printf("[%s],[%s]\n", regexp, buf_ret);
	}
//_FLF9_
	return ptr_re;
}
// "(abc|def|ghi)" ==> "abc def ghi"
PRIVATE const char *parse_re_chars(const char *regexp, char *buf_ret, int buf_len)
{
	const char *ptr_re;
	char bunsetsu[MAX_KATSUYOU_LIST_LEN+1];
	int len;

//_FLF0_
//flf_d_printf("regexp:[%s]\n", regexp);
	strcpy(buf_ret, "");
	ptr_re = regexp;
	ptr_re = skip_space(ptr_re);
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
	if (*ptr_re == '(') {
		ptr_re++;
		ptr_re = parse_re_parallel(ptr_re, bunsetsu, MAX_KATSUYOU_LIST_LEN);
		utf8strcat_blen(buf_ret, buf_len, bunsetsu);
		if (*ptr_re == ')') {
			ptr_re++;
		}
	} else {
		len = re_word_len(ptr_re);
		utf8strcat_blen_slen(buf_ret, buf_len, ptr_re, len);
		ptr_re += len;
	}
//flf_d_printf("ptr_re:[%s]\n", ptr_re);
//flf_d_printf("[%s]\n==>\n[%s]\n", regexp, buf_ret);
//_FLF9_
	return ptr_re;
}
// "abc" + "def" ==> "abcdef"
//	"a b" + "c d e" ==> "ac ad ae bc bd be"
//	"" + "c d e" ==> "c d e"
//	"a" + " c d e" ==> "a ac ad ae"
PRIVATE char *concat_bunsetsu(const char *bunsetsu1, const char *bunsetsu2,
 char *buffer, int buf_len)
{
	const char *ptr_b1;
	const char *ptr_b2;
	char bunsetsu[MAX_KATSUYOU_LEN+1];

//_FLF0_
//flf_d_printf("[%s]+[%s]\n", bunsetsu1, bunsetsu2);
	strcpy(buffer, "");
	for (ptr_b1 = bunsetsu1; ; ) {
		for (ptr_b2 = bunsetsu2; ; ) {
//fl_d_printf("ptr_b1:[%s],%d\n", ptr_b1, word_len(ptr_b1));
//fl_d_printf("ptr_b2:[%s],%d\n", ptr_b2, word_len(ptr_b2));
//			ptr_b1 = skip_space(ptr_b1);
			utf8strcpy_blen_slen(bunsetsu, MAX_KATSUYOU_LEN, ptr_b1, word_len(ptr_b1));
//			ptr_b2 = skip_space(ptr_b2);
			utf8strcat_blen_slen(bunsetsu, MAX_KATSUYOU_LEN, ptr_b2, word_len(ptr_b2));
			utf8strcat_blen(buffer, buf_len, bunsetsu);
			strcat_blen(buffer, buf_len, " ");
//fl_d_printf("buffer:[%s]\n", buffer);
			ptr_b2 = skip_word_sp(ptr_b2);
			if (*ptr_b2 == '\0')
				break;
		}
		ptr_b1 = skip_word_sp(ptr_b1);
		if (*ptr_b1 == '\0')
			break;
	}
//flf_d_printf("[%s]+[%s]\n==>[%s]\n", bunsetsu1, bunsetsu2, buffer);
//_FLF9_
	return buffer;
}
PRIVATE int re_word_len(const char *str)
{
	const char *ptr;

	for (ptr = str; *ptr; ) {
//flf_d_printf("ptr:[%s]\n", ptr);
		switch(*ptr) {
		case ' ':
		case '(':
		case ')':
		case '|':
			goto re_word_len_break;
		default:
			ptr += utf8c_bytes(ptr);
			break;
		}
	}
re_word_len_break:;
//flf_d_printf("ptr:[%s]\n", ptr);
	return ptr - str;
}
PRIVATE void add_space_separator(char *buf, int buf_len)
{
	if (buf[0]) {
		if (buf[strlen(buf)-1] != ' ') {
			strcat_blen(buf, buf_len, " ");
		}
	}
}

// Input/Conversion History --------------------------------------------------
// imjihst_... : input history
// imjchst_... : conversion history
// imjhst_...  : history common to input and conversion

char imjhst_file_path[MAX_PATH_LEN+1] = "";

typedef struct imjhst_ {
	// input      history "\tよみ"
	// conversion history "よみ\t読み"
	char pronun_conversion[MAX_HISTORY_BUF_LEN+1];
} imjhst_t;
int imjhst_history_entries = 0;		// num of entries
int imjhst_history_updated = 0;
imjhst_t imjhst_history[MAX_CONVERSION_HISTORIES];
#define IS_HST_INUSE(pronun_conversion)		(pronun_conversion[0])
#define IS_IHST(pronun_conversion)			(IS_CTRL(pronun_conversion[0]))
#define IS_CHST(pronun_conversion)			(IS_GRAPH(pronun_conversion[0]))

int imjhst_load(const char *file_path)
{
	FILE *fp_cnvhst;
	int entry_idx;
	char line[MAX_HISTORY_BUF_LEN+1];

	imjhst_clear_all();
	if (file_path == NULL)
		file_path = CNVHST_FILE_NAME;
	strcpy_blen(imjhst_file_path, MAX_PATH_LEN, file_path);
flf_d_printf("hst-file-path:[%s]\n", file_path);
	if ((fp_cnvhst = fopen(file_path, "r")) == NULL) {
		_ERR_
		return 1;
	}
	for (entry_idx = 0; entry_idx < MAX_CONVERSION_HISTORIES; entry_idx++) {
		if (fgets(line, MAX_HISTORY_BUF_LEN+1, fp_cnvhst) == NULL) {
			break;
		}
		imjhst_append_new_line(cut_line_tail_crlf(line));
	}
flf_d_printf("history entries: %d\n", imjhst_history_entries);
	if (fclose(fp_cnvhst) < 0) {
		_FL_
		return 2;
	}
///imjhst_dump();
	return 0;
}
int imjhst_save_if_updated(char *file_path)
{
	if (imjhst_is_updated()) {
		imjhst_clear_updated();
		return imjhst_save(file_path);
	}
	return 0;
}
int imjhst_save(const char *file_path)
{
	FILE *fp_cnvhst;
	int line_idx;
	int hist_type;		// 0: input, 1: conversion
	int entry_idx;
	const char *pronun_conversion;
	char line[MAX_HISTORY_BUF_LEN+1];

///imjhst_dump();
	if (file_path == NULL) {
		if (imjhst_file_path[0])
			file_path = imjhst_file_path;
		else
			file_path = CNVHST_FILE_NAME;
	}
flf_d_printf("hst-file-path:[%s]\n", file_path);
	if ((fp_cnvhst = fopen(file_path, "w")) == NULL) {
		_ERR_
		return 1;
	}
	line_idx = 0;
	for (hist_type = 0; hist_type < 2; hist_type++) {
		for (entry_idx = 0; entry_idx < imjhst_history_entries; entry_idx++) {
			pronun_conversion = imjhst_history[entry_idx].pronun_conversion;
			if (hist_type == 0 ? IS_IHST(pronun_conversion) : IS_CHST(pronun_conversion)) {
///flf_d_printf("%d:%d:[%s]\n", line_idx, entry_idx, pronun_conversion);
				snprintf_(line, MAX_HISTORY_BUF_LEN+1, "%s\n", pronun_conversion);
				if (fputs(line, fp_cnvhst) < 0) {
					break;
				}
				line_idx++;
			}
		}
flf_d_printf("%s entries: %d\n", hist_type == 0
 ? "input      history"
 : "conversion history", line_idx);
	}
	if (fclose(fp_cnvhst) < 0) {
		_FL_
		return 3;
	}
	return 0;
}

PRIVATE int imjihst_get_histories(void)
{
	int entry_idx;
	int histories;

	histories = 0;
	for (entry_idx = 0; entry_idx < imjhst_history_entries; entry_idx++) {
		if (IS_IHST(imjhst_history[entry_idx].pronun_conversion)) {
			histories++;
		}
	}
flf_d_printf("imjihst_get_histories() ==> %d\n", histories);
	return histories;
}
PRIVATE int imjihst_get_nth_history(int hist_idx, char *history, int buf_len)
{
	int entry_idx;

flf_d_printf("imjihst_get_nth_history(%d)\n", hist_idx);
	entry_idx = imjihst_get_nth_history_idx(hist_idx);
	if (entry_idx >= 0) {
		utf8strcpy_blen(history, buf_len, &imjhst_history[entry_idx].pronun_conversion[1]);
flf_d_printf("==> [%s]\n", history);
		return strlen(history);
	}
flf_d_printf("==> []\n");
	return 0;
}
PRIVATE int imjihst_get_nth_history_idx(int hist_idx)
{
	int entry_idx;
	int histories;

//flf_d_printf("(%d)\n", hist_idx);
	histories = 0;
	for (entry_idx = 0; entry_idx < imjhst_history_entries; entry_idx++) {
		if (IS_IHST(imjhst_history[entry_idx].pronun_conversion)) {
			if (histories >= hist_idx) {
//flf_d_printf("==> %d\n", entry_idx);
				return entry_idx;
			}
			histories++;
		}
	}
//flf_d_printf("==> %d\n", -1);
	return -1;
}
PRIVATE int imjihst_append(const char *pronun)
{
//flf_d_printf("[%s]\n", pronun);
	return imjhst_append_new(pronun, "");
}
PRIVATE void imjihst_dump(void)
{
	int histories;
	int hist_idx;
	int entry_idx;

	histories = imjihst_get_histories();
	for (hist_idx = 0; hist_idx < histories; hist_idx++) {
		entry_idx = imjihst_get_nth_history_idx(hist_idx);
		if (entry_idx < 0)
			break;
		flf_d_printf("%d:[%s]\n", hist_idx, imjhst_history[entry_idx].pronun_conversion);
	}
}
//-----------------------------------------------------------------------------
PRIVATE int imjchst_append(const char *pronun, const char *conversion)
{
//flf_d_printf("[%s]:[%s]\n", pronun, conversion);
	return imjhst_append_new(pronun, conversion);
}
PRIVATE int imjchst_get_num_of_matches(const char *pronun, int len_to_match)
{
	int entry_idx;
	int matches;

	matches = 0;
	for (entry_idx = imjhst_history_entries-1; entry_idx >= 0; entry_idx--) {
		entry_idx = imjhst_search_by_pronun_from_last(entry_idx, pronun, len_to_match, "*");
		if (entry_idx < 0)
			break;
		matches++;
	}
//flf_d_printf("[%s]:%d ==> %d\n", pronun, len_to_match, matches);
	return matches;
}
PRIVATE int imjchst_get_nth_match(int match_idx, const char *pronun, int len_to_match,
 char *conversion, int buf_len)
{
	int entry_idx;
	int matches;

//flf_d_printf("imjchst_get_nth_match(%d, [%s]:%d)\n", match_idx, pronun, len_to_match);
	strcpy(conversion, "");
	matches = 0;
	for (entry_idx = imjhst_history_entries-1; entry_idx >= 0; entry_idx--) {
		entry_idx = imjhst_search_by_pronun_from_last(entry_idx, pronun, len_to_match, "*");
		if (entry_idx < 0)
			break;
		if (matches >= match_idx) {
			utf8strcpy_blen(conversion, buf_len,
			 skip_tsword_tab(imjhst_history[entry_idx].pronun_conversion));
//flf_d_printf("==> [%s]\n", conversion);
			return strlen(conversion);
		}
		matches++;
	}
//flf_d_printf("==> []\n");
	return 0;
}
PRIVATE int imjchst_search_longest_match_len(const char *pronun)
{
	int max_match_len;
	int entry_idx;
	const char *hst_pronun;
	int hst_pronun_len;

//flf_d_printf("[%s]\n", pronun);
	max_match_len = 0;
	if (strlen(pronun) == 0)
		return max_match_len;
	for (entry_idx = imjhst_history_entries-1; entry_idx >= 0; entry_idx--) {
		hst_pronun = imjhst_history[entry_idx].pronun_conversion;
		if (IS_CHST(hst_pronun)) {
			hst_pronun_len = tsword_len(hst_pronun);
			if (strncmp(pronun, hst_pronun, hst_pronun_len) == 0) {
				if (hst_pronun_len > max_match_len) {
					max_match_len = hst_pronun_len;
				}
			}
		}
	}
//flf_d_printf("[%s] ==> %d\n", pronun, max_match_len);
	return max_match_len;
}
//-----------------------------------------------------------------------------
PRIVATE void imjhst_clear_all(void)
{
	int entry_idx;

	for (entry_idx = 0; entry_idx < MAX_CONVERSION_HISTORIES; entry_idx++) {
		strcpy(imjhst_history[entry_idx].pronun_conversion, "");
	}
	imjhst_history_entries = 0;
}
PRIVATE void imjhst_clear_save(void)
{
	imjhst_clear_all();
	imjhst_set_updated();
	imjhst_save_if_updated(NULL);
}
PRIVATE int imjhst_get_entries(void)
{
	return imjhst_history_entries;
}
PRIVATE int imjhst_is_updated(void)
{
	return imjhst_history_updated;
}
PRIVATE int imjhst_set_updated(void)
{
	return imjhst_history_updated = 1;
}
PRIVATE int imjhst_clear_updated(void)
{
	return imjhst_history_updated = 0;
}
PRIVATE int imjhst_append_new(const char *pronun, const char *conversion)
{
	char pronun_conversion[MAX_INPUT_HISTORY_LEN+1];

	if (strlen(pronun) == 0)
		return imjhst_history_entries;
	return imjhst_append_new_line(imjhst_make_line(pronun, conversion, pronun_conversion));
}
PRIVATE int imjhst_append_new_line(const char *pronun_conversion)
{
	int entry_idx;

///flf_d_printf("[%s]\n", pronun_conversion);
	if (strlen(pronun_conversion) <= 1)
		return imjhst_history_entries;
	entry_idx = imjhst_search_the_same_entry_line(pronun_conversion);
	if (entry_idx >= 0) {	// if the same entry found, delete it.
		imjhst_delete_entry(entry_idx);
	}
	imjhst_append_entry_line(pronun_conversion);
	imjhst_set_updated();
//flf_d_printf("imjhst_history_entries: %d\n", imjhst_history_entries);
	return imjhst_history_entries;
}
PRIVATE int imjhst_append_entry_line(const char *pronun_conversion)
{
	if (strlen(pronun_conversion) == 0)
		return 0;
//flf_d_printf("[%s]\n", pronun_conversion);
	if (imjhst_history_entries >= MAX_CONVERSION_HISTORIES) {
		imjhst_delete_entry(0);		// delete oldest
	}
	imjhst_put_entry_line(imjhst_history_entries, pronun_conversion);
	imjhst_history_entries = MIN_(MAX_CONVERSION_HISTORIES,
	 imjhst_history_entries+1);
//flf_d_printf("imjhst_history_entries: %d\n", imjhst_history_entries);
	return imjhst_history_entries;
}
PRIVATE int imjhst_search_by_pronun_from_last(int start_entry_idx,
 const char *pronun, int len_to_match, const char *conversion)
{
	int entry_idx;
	const char *hst_pronun;
	int hst_pronun_len;

	if (strlen(pronun) == 0 || len_to_match == 0)
		return -1;
//flf_d_printf("[%s], [%s]\n", pronun, conversion);
	for (entry_idx = start_entry_idx; entry_idx >= 0; entry_idx--) {
//flf_d_printf("[%s]\n", imjhst_history[entry_idx].pronun_conversion);
		hst_pronun = imjhst_history[entry_idx].pronun_conversion;
		hst_pronun_len = tsword_len(hst_pronun);
		if (hst_pronun_len == len_to_match) {
			if (strncmp(hst_pronun, pronun, len_to_match) == 0) {
//_FLF_
				return entry_idx;
			}
		}
	}
//_FLF_
	return -1;
}
PRIVATE int imjhst_search_the_same_entry_line(const char *pronun_conversion)
{
	int entry_idx;

//flf_d_printf("[%s]\n", pronun_conversion);
	for (entry_idx = imjhst_history_entries-1; entry_idx >= 0; entry_idx--) {
		if (strcmp(imjhst_history[entry_idx].pronun_conversion, pronun_conversion) == 0) {
			return entry_idx;
		}
	}
	return -1;
}
PRIVATE int imjhst_delete_entry(int entry_idx)
{
	int last_entry_idx;		// (last entry index to be moved)+1

	if (entry_idx < imjhst_history_entries) {
///flf_d_printf("%d:[%s]\n", entry_idx, imjhst_history[entry_idx].pronun_conversion);
//	+--------+
//	| str-00 |								(Oldest)
//	+--------+ <== entry_idx
//	| str-01 | <-- entry to be deleted
//	+--------+ <== entry_idx+1
//	| str-02 |
//	+--------+
//	| str-03 |
//	+--------+
//	| str-04 |
//	+--------+
//	| str-05 |
//	+--------+
//	| str-06 |								(Newest)
//	+--------+ <== imjhst_history_entries
//	|        |
//	+--------+
		last_entry_idx = MIN_(MAX_CONVERSION_HISTORIES,
		 imjhst_history_entries);
///flf_d_printf("%d:[%s] -- %d\n",
/// entry_idx, imjhst_history[entry_idx].pronun_conversion, last_entry_idx);
		// shift all entries to 1 older (1 entry disappears)
		memmove(&imjhst_history[entry_idx], &imjhst_history[entry_idx+1],
		 (char *)&imjhst_history[last_entry_idx]
		  - (char *)&imjhst_history[entry_idx+1]);
		imjhst_history_entries = MAX_(0, imjhst_history_entries-1);
///flf_d_printf("%d:[%s]\n",
/// imjhst_history_entries, imjhst_history[imjhst_history_entries].pronun_conversion);
		strcpy(imjhst_history[imjhst_history_entries].pronun_conversion, "");
///flf_d_printf("%d:[%s]\n",
/// imjhst_history_entries, imjhst_history[imjhst_history_entries].pronun_conversion);
	}
	return entry_idx;
}
PRIVATE int imjhst_put_entry_line(int entry_idx, const char *pronun_conversion)
{
///flf_d_printf("%d:[%s]\n", entry_idx, pronun_conversion);
	if (entry_idx < MAX_CONVERSION_HISTORIES) {
		utf8strcpy_blen(imjhst_history[entry_idx].pronun_conversion, MAX_HISTORY_INPUT_LEN,
		 pronun_conversion);
	}
	return entry_idx;
}
PRIVATE char *imjhst_make_line(const char *pronun, const char *conversion, char *buffer)
{
	if (strlen(conversion) == 0) {
		// input history "\tよみ"
		strcpy(buffer, "\t");
		utf8strcat_blen(buffer, MAX_HISTORY_INPUT_LEN, pronun);
	} else {
		// conversion history "よみ\t読み"
		utf8strcpy_blen(buffer, MAX_HISTORY_PRONUN_LEN, pronun);
		utf8strcat_blen(buffer, MAX_HISTORY_INPUT_LEN, "\t");
		utf8strcat_blen(buffer, MAX_HISTORY_INPUT_LEN, conversion);
	}
	return buffer;
}
PRIVATE void imjhst_dump(void)
{
	int entry_idx;

	for (entry_idx = 0; entry_idx < imjhst_history_entries; entry_idx++) {
		if (IS_HST_INUSE(imjhst_history[entry_idx].pronun_conversion)) {
flf_d_printf("%d:[%s]\n", entry_idx, imjhst_history[entry_idx].pronun_conversion);
		}
	}
}
//-----------------------------------------------------------------------------
void imjcandidates::clear(void)
{
	int candid_idx;

	for (candid_idx = 0; candid_idx < MAX_CANDIDS_LIST; candid_idx++) {
		_candidates[candid_idx].pronun_len = 0;
		strcpy(_candidates[candid_idx].candidate, "");
	}
}
int imjcandidates::get_num_of_candids(void)
{
	int candid_idx;

	for (candid_idx = 0; candid_idx < MAX_CANDIDS_LIST; candid_idx++) {
		if (_candidates[candid_idx].pronun_len == 0)
			break;
	}
	return candid_idx;
}
int imjcandidates::append_one_candid(char *candid)
{
	int candid_idx;
	int len = utf8s_len_le(candid, MAX_CANDID_LEN);

//flf_d_printf("[%s]\n", candid);
#if 1
	if (is_there_the_same_candid(candid) < 0) {
		candid_idx = get_num_of_candids();
		if (candid_idx < MAX_CANDIDS_LIST) {
			put_candid(candid_idx, candid, len);
		}
	}
#else
	if ((candid_idx = is_there_the_same_candid(candid)) >= 0) {
		delete_one_candid(candid_idx);
	}
	candid_idx = get_num_of_candids();
	if (candid_idx < MAX_CANDIDS_LIST) {
		put_candid(candid_idx, candid, len);
	}
#endif
///dump_candids();
	return get_num_of_candids();
}
int imjcandidates::insert_one_candid(char *candid)
{
	int candid_idx;
	int len = utf8s_len_le(candid, MAX_CANDID_LEN);

flf_d_printf("[%s]\n", candid);
	if ((candid_idx = is_there_the_same_candid(candid)) >= 0) {
		delete_one_candid(candid_idx);
	}
	candid_idx = get_num_of_candids();
	if (candid_idx < MAX_CANDIDS_LIST) {
		memmove(&_candidates[1], &_candidates[0],
		 sizeof(_candidates[0]) * candid_idx);
		put_candid(0, candid, len);
	}
	return get_num_of_candids();
}
int imjcandidates::is_there_the_same_candid(char *candid)
{
	int candid_idx;

	for (candid_idx = 0; candid_idx < MAX_CANDIDS_LIST; candid_idx++) {
		if (_candidates[candid_idx].pronun_len == 0)
			break;
		if (compare_candid(candid_idx, candid) == 0)
			return candid_idx;
	}
	return -1;
}
int imjcandidates::delete_one_candid(int candid_idx)
{
//_FLF_
//flf_d_printf("%d\n", candid_idx);
	memmove(&_candidates[candid_idx], &_candidates[candid_idx+1],
	 sizeof(_candidates[candid_idx]) * (MAX_CANDIDS_LIST-candid_idx-1));
	_candidates[MAX_CANDIDS_LIST-1].pronun_len = 0;
	_candidates[MAX_CANDIDS_LIST-1].candidate[0] = '\0';
	return 0;
}
int imjcandidates::compare_candid(int candid_idx, char *candid)
{
	int len = utf8s_len_le(candid, MAX_CANDID_LEN);
	int diff;

	diff = _candidates[candid_idx].pronun_len - len;
	return diff == 0 ? strncmp(_candidates[candid_idx].candidate, candid, len) : diff;
}
void imjcandidates::put_candid(int candid_idx, char *candid, int len)
{
	_candidates[candid_idx].pronun_len = len;
	utf8strcpy_blen_slen(_candidates[candid_idx].candidate, MAX_CANDID_LEN, candid, len);
}
char *imjcandidates::get_candid(int candid_idx, char *buf, int buf_len)
{
	buf = get_static_buf_if_null(buf);
	return utf8strcpy_blen(buf, buf_len, _candidates[candid_idx].candidate);
}
void imjcandidates::dump_candids(void)
{
	int candid_idx;

	for (candid_idx = 0; candid_idx < MAX_CANDIDS_LIST; candid_idx++) {
		if (_candidates[candid_idx].pronun_len == 0)
			break;
flf_d_printf("%d:%2d[%s]\n", candid_idx,
 _candidates[candid_idx].pronun_len, _candidates[candid_idx].candidate);
	}
}

// ローマ字入力 ---------------------------------------------------------------
// imjatoh

#define MAX_ROM_ALPHA_LEN		(4)		// "ltsu", "xtsu"
#define MAX_ROM_HIRA_LEN		(3*5)	// "っう゛ぁ"
#define MAX_ROM_TABLE_LINE_LEN	(MAX_ROM_ALPHA_LEN+1+MAX_ROM_HIRA_LEN)	// "xtsu う゛ぁ"
#define MAX_ROM_TABLE_LINES		1000

PRIVATE int imjatoh_convert_one(const char *romaji, char *hira, int buf_len);
PRIVATE int imjatoh_convert_one2(const char *romaji, char *hira, int buf_len);
PRIVATE int imjatoh_convert_one3(const char *romaji, char *hira, int buf_len);
PRIVATE int imjatoh_reverse_one(const char *hira, char *romaji);
PRIVATE int imjatoh_reverse_one2(const char *hira, char *romaji);

char roman_table[MAX_ROM_TABLE_LINES+1][MAX_ROM_TABLE_LINE_LEN+1] = {
	"  　",		// " " ==> "　"
	"[ 「",
	"] 」",
	", 、",
	". 。",

	"` ｀",
	"~ 〜",
	"! ！",
	"@ ＠",
	"# ＃",
	"$ ＄",
	"% ％",
	"^ ＾",
	"& ＆",
	"* ＊",
	"( （",
	") ）",
	"- " ZEN_CHOUON,
	"_ ＿",
	"= ＝",
	"+ ＋",
	"{ ｛",
	"} ｝",
	"\\ " ZEN_YEN,
	"| ｜",
	"; ；",
	": ：",
	"' ＇",
	"\" ＂",
	"< ＜",
	"> ＞",
	"/ ／",
	"? ？",

	"a    あ",
	"ba   ば",
	"be   べ",
	"bi   び",
	"bo   ぼ",
	"bu   ぶ",
	"bya  びゃ",
	"bye  びぇ",
	"byi  びぃ",
	"byo  びょ",
	"byu  びゅ",
	"ca   か",
	"ce   せ",
	"ci   し",
	"co   こ",
	"cu   きゅ",
	"cha  ちゃ",
	"che  ちぇ",
	"chi  ち",
	"cho  ちょ",
	"chu  ちゅ",
	"cya  ちゃ",
	"cye  ちぇ",
	"cyi  ちぃ",
	"cyo  ちょ",
	"cyu  ちゅ",
	"da   だ",
	"de   で",
	"dha  でゃ",
	"dhe  でぇ",
	"dhi  でぃ",
	"dho  でょ",
	"dhu  でゅ",
	"di   ぢ",
	"do   ど",
	"du   づ",
	"dwu  どぅ",
	"dya  ぢゃ",
	"dye  ぢぇ",
	"dyi  ぢぃ",
	"dyo  ぢょ",
	"dyu  ぢゅ",
	"e    え",
	"fa   ふぁ",
	"fe   ふぇ",
	"fi   ふぃ",
	"fo   ふぉ",
	"fu   ふ",
	"fya  ふゃ",
	"fye  ふぇ",
	"fyi  ふぃ",
	"fyo  ふょ",
	"fyu  ふゅ",
	"ga   が",
	"ge   げ",
	"gi   ぎ",
	"go   ご",
	"gu   ぐ",
	"gwa  ぐぁ",
	"gya  ぎゃ",
	"gye  ぎぇ",
	"gyi  ぎぃ",
	"gyo  ぎょ",
	"gyu  ぎゅ",
	"ha   は",
	"he   へ",
	"hi   ひ",
	"ho   ほ",
	"hu   ふ",
	"hya  ひゃ",
	"hye  ひぇ",
	"hyi  ひぃ",
	"hyo  ひょ",
	"hyu  ひゅ",
	"i    い",
	"ja   じゃ",
	"je   じぇ",
	"ji   じ",
	"jo   じょ",
	"ju   じゅ",
	"jya  じゃ",
	"jye  じぇ",
	"jyi  じぃ",
	"jyo  じょ",
	"jyu  じゅ",
	"ka   か",
	"ke   け",
	"ki   き",
	"ko   こ",
	"ku   く",
	"kwa  くぁ",
	"kya  きゃ",
	"kye  きぇ",
	"kyi  きぃ",
	"kyo  きょ",
	"kyu  きゅ",
	"la   ぁ",
	"le   ぇ",
	"li   ぃ",
	"lka  ヵ",
	"lke  ヶ",
	"lo   ぉ",
	"ltsu っ",
	"ltu  っ",
	"lu   ぅ",
	"lwa  ゎ",
	"lya  ゃ",
	"lye  ぇ",
	"lyi  ぃ",
	"lyo  ょ",
	"lyu  ゅ",
	"ma   ま",
	"me   め",
	"mi   み",
	"mo   も",
	"mu   む",
	"mya  みゃ",
	"mye  みぇ",
	"myi  みぃ",
	"myo  みょ",
	"myu  みゅ",
	"nn   ん",
	"na   な",
	"ne   ね",
	"ni   に",
	"no   の",
	"nu   ぬ",
	"n    ん",
	"n'   ん",
	"nya  にゃ",
	"nye  にぇ",
	"nyi  にぃ",
	"nyo  にょ",
	"nyu  にゅ",
	"o    お",
	"pa   ぱ",
	"pe   ぺ",
	"pi   ぴ",
	"po   ぽ",
	"pu   ぷ",
	"pya  ぴゃ",
	"pye  ぴぇ",
	"pyi  ぴぃ",
	"pyo  ぴょ",
	"pyu  ぴゅ",
	"qa   くぁ",
	"qe   くぇ",
	"qi   くぃ",
	"qo   くぉ",
	"qu   くぅ",
	"ra   ら",
	"re   れ",
	"ri   り",
	"ro   ろ",
	"ru   る",
	"rya  りゃ",
	"rye  りぇ",
	"ryi  りぃ",
	"ryo  りょ",
	"ryu  りゅ",
	"sa   さ",
	"se   せ",
	"sha  しゃ",
	"she  しぇ",
	"shi  し",
	"sho  しょ",
	"shu  しゅ",
	"si   し",
	"so   そ",
	"su   す",
	"sya  しゃ",
	"sye  しぇ",
	"syi  しぃ",
	"syo  しょ",
	"syu  しゅ",
	"ta   た",
	"te   て",
	"tha  てゃ",
	"the  てぇ",
	"thi  てぃ",
	"tho  てょ",
	"thu  てゅ",
	"ti   ち",
	"to   と",
	"tsa  つぁ",
	"tse  つぇ",
	"tsi  つぃ",
	"tso  つぉ",
	"tsu  つ",
	"tu   つ",
	"twu  とぅ",
	"tya  ちゃ",
	"tye  ちぇ",
	"tyi  ちぃ",
	"tyo  ちょ",
	"tyu  ちゅ",
	"u    う",
	"va   う゛ぁ",
	"ve   う゛ぇ",
	"vi   う゛ぃ",
	"vo   う゛ぉ",
	"vu   う゛",
	"wa   わ",
	"we   うぇ",
	"wi   うぃ",
	"wo   を",
	"wu   う",
	"wye  ゑ",
	"wyi  ゐ",
	"x\"   ゛",
	"x@   ゜",
	"xa   ぁ",
	"xe   ぇ",
	"xi   ぃ",
	"xka  ヵ",
	"xke  ヶ",
	"xo   ぉ",
	"xtsu っ",
	"xtu  っ",
	"xu   ぅ",
	"xwa  ゎ",
	"xya  ゃ",
	"xye  ぇ",
	"xyi  ぃ",
	"xyo  ょ",
	"xyu  ゅ",
	"ya   や",
	"ye   いぇ",
	"yi   い",
	"yo   よ",
	"yu   ゆ",
	"za   ざ",
	"ze   ぜ",
	"zi   じ",
	"zo   ぞ",
	"zu   ず",
	"zya  じゃ",
	"zye  じぇ",
	"zyi  じぃ",
	"zyo  じょ",
	"zyu  じゅ",
	"",
};

#ifdef IMJ_BUILT_IN_TEST
void test_imjatoh_functions(void)
{
	char pronun[IM_INPUT_LINE_LEN+1];
	char roman1[] = " hiragana";
	char hira1[] = "ひらがな";
	char roman2[] = "kanji";
	int off;

_FLF_
	imjatoh_romaji_to_hira(roman1, pronun, IM_INPUT_LINE_LEN);
flf_d_printf("roman1:[%s] ==> pronun[%s]\n", roman1, pronun);
	off = imjatoh_romaji_offset(roman1, strlen("ひら"));
flf_d_printf("imjatoh_romaji_offset([%s], strlen([ひら])) ==> %d\n",
 roman1, off);
	off = imjatoh_romaji_offset(roman2, strlen("かん"));
flf_d_printf("imjatoh_romaji_offset([%s], strlen([かん])) ==> %d\n",
 roman2, off);
	off = imjatoh_hira_offset(roman1, hira1, 6);
flf_d_printf("imjatoh_hira_offset([%s], [%s], 6) ==> %d\n",
 roman1, hira1, off);
}
#endif // IMJ_BUILT_IN_TEST

#if 0
int imjatoh_load_table(char *file_path)
{
	FILE *fp = NULL;
	int idx;
	char buffer[MAX_ROM_TABLE_LINE_LEN+10+1];

	if (file_path == NULL)
		file_path = ROM_FILE_NAME;
	if ((fp = fopen(file_path, "r")) == NULL) {
		_ERR_
		return 1;
	}
	for (idx = 0 ; idx < MAX_ROM_TABLE_LINES; idx++) {
		if (fgets(buffer, MAX_ROM_TABLE_LINE_LEN+10, fp) == NULL) {
			break;
		}
		cut_line_tail_crlf(buffer);
		utf8strcpy_blen(roman_table[idx], MAX_ROM_TABLE_LINE_LEN, buffer);
	}
	roman_table[idx][0] = '\0';		// end of lines
	if (fclose(fp) < 0) {
		_FL_
	}
	fp = NULL;
	return 0;
}
#endif

// romaji     insertion                    internal
// ---------- ----------                   -------
// sakka-     さっ{a}かー  ==> さっあかー  ==> saltuaka-
// aj|k|j|i   あj{a}kjい    ==> あじゃkjい   ==> ajakji
int imjatoh_insert_romaji(char *romaji, int rom_buf_len,
 char *hira, int hira_buf_len, int off_h, char *insert)
{
	int off_r;
	char buffer_r[IM_INPUT_LINE_LEN+1];
	int off_r_after_ins;

//_FLF_
//flf_d_printf("romaji:[%s]<==[%s]\n", romaji, insert);
//flf_d_printf("hira:[%s]:%d\n", hira, off_h);
	imjatoh_split_romaji_spelling(romaji, rom_buf_len, hira, off_h);
//_FL_
	off_r = imjatoh_romaji_offset(romaji, off_h);
	utf8strcpy_blen_slen(buffer_r, IM_INPUT_LINE_LEN, romaji, off_r);
	utf8strcat_blen(buffer_r, IM_INPUT_LINE_LEN, insert);
//flf_d_printf("buf_r:[%s]\n", buffer_r);
	off_r_after_ins = strlen(buffer_r);
	utf8strcat_blen(buffer_r, IM_INPUT_LINE_LEN, &romaji[off_r]);
//flf_d_printf("buf_r:[%s]\n", buffer_r);
	utf8strcpy_blen(romaji, rom_buf_len, buffer_r);	// copy back to romaji
	imjatoh_romaji_to_hira(romaji, hira, hira_buf_len);
//flf_d_printf("romaji:[%s]\n", romaji);
	off_h = imjatoh_hira_offset(romaji, hira, off_r_after_ins);
	return off_h;
}

// romaji     hira                             internal
// ---------- --------------                   ----------
// macchinngu まっ{ち}んぐ   ==> まっ|んぐ  -> maltsunngu
// ochaya     おち{ゃ}や     ==> おち|や    -> ochiya
// ivennto    いう゛{ぇ}んと ==> いう゛んと -> ivunnto
// ivennto    いう{゛}ぇんと ==> いうぇんと -> iuxennto
// ivennto    い{う}゛ぇんと ==> い゛ぇんと -> ix"xennto
// ab|c|d|i   あbc{d}い      ==> あbcい     -> ab|c|i
// ab|c|d|i   あb{c}dい      ==> あbdい     -> ab|d|i
// ab|c|d|i   あ{b}cdい      ==> あcdい     -> ac|d|i
// aj|k|j|i   あj{k}jい      ==> あjjい     -> aj|j|i
//      +----- off_h_from
//      |  +-- off_h_to
//      |  |
// i  ve       nnto
// い う{゛}ぇ ん と
// い う    ぇ ん と
// i  u     xe nn to
int imjatoh_delete_romaji(char *romaji, int rom_buf_len,
 char *hira, int hira_buf_len, int off_h_from, int off_h_to)
{
	int off_r_from;
	int off_r_to;

flf_d_printf("off_h_from: %d, off_h_to: %d\n", off_h_from, off_h_to);
	imjatoh_split_romaji_spelling(romaji, rom_buf_len, hira, off_h_from);
	imjatoh_split_romaji_spelling(romaji, rom_buf_len, hira, off_h_to);
	off_r_from = imjatoh_romaji_offset(romaji, off_h_from);
	off_r_to = imjatoh_romaji_offset(romaji, off_h_to);
flf_d_printf("off_r_from: %d, off_r_to: %d\n", off_r_from, off_r_to);
	strcpy_ol(&romaji[off_r_from], &romaji[off_r_to]);
	strcpy_ol(&hira[off_h_from], &hira[off_h_to]);
flf_d_printf("romaji:[%s]\r\n", romaji);
flf_d_printf("hira:[%s]\r\n", hira);
	off_h_from = imjatoh_hira_offset(romaji, hira, off_r_from);
	return off_h_from;
}
// Ex.1 ===================
//      +----- split_offset
//      |
// sa kka   -
// さ っ か ー
//   |     |
//   |     +-- off_h_to
//   +-------- off_h_from
// さ っ か ー
// sa ltuka -
int imjatoh_split_romaji_spelling(char *romaji, int rom_buf_len,
 const char *hira, int split_offset)
{
	char *ptr_r;
	const char *ptr_h;
	char buf_h[IM_INPUT_LINE_LEN+1];	// buffer for one romaji conversion "っか"
	char buf_r_from_h[MAX_ROM_ALPHA_LEN+1];		// "ltu", "ka"
	char buffer_r_from_h[IM_INPUT_LINE_LEN+1];	// "ltuka"
	int off_h_from;		// offset of the begining of spelling
	int off_h_to;		// offset of the end of spelling
	const char *ptr_h_to_a;
	int len_r;
	int len_h;
	char buffer_r[IM_INPUT_LINE_LEN+1];

flf_d_printf("romaji:[%s]\n", romaji);
//flf_d_printf("hira:[%s],[%s]\n", hira, &hira[split_offset]);
	for (ptr_r = romaji, ptr_h = hira; *ptr_r && *ptr_h; ) {
		len_r = imjatoh_convert_one(ptr_r, buf_h, IM_INPUT_LINE_LEN);
		off_h_from = ptr_h - hira;
		off_h_to = off_h_from + strlen(buf_h);
///flf_d_printf("buf_h:[%s]\n", buf_h);
		if (off_h_from < split_offset && split_offset < off_h_to) {
			// romaji splitting is needed
flf_d_printf("off_h_from:%d split_off:%d off_h_to:%d\r\n",
 off_h_from, split_offset, off_h_to);
			strcpy(buffer_r_from_h, "");
flf_d_printf("ptr_h:[%s]\n", ptr_h);
			for (ptr_h_to_a = ptr_h; *ptr_h_to_a && ptr_h_to_a < &ptr_h[strlen(buf_h)]; ) {
flf_d_printf("ptr_h_to_a:[%s]\n", ptr_h_to_a);
				len_h = imjatoh_reverse_one(ptr_h_to_a, buf_r_from_h);
				utf8strcat_blen(buffer_r_from_h, IM_INPUT_LINE_LEN, buf_r_from_h);
flf_d_printf("buffer_r_from_h:[%s]\n", buffer_r_from_h);
				ptr_h_to_a += len_h;
			}
			utf8strcpy_blen_slen(buffer_r, IM_INPUT_LINE_LEN, romaji, ptr_r - romaji);
			utf8strcat_blen(buffer_r, IM_INPUT_LINE_LEN, buffer_r_from_h);
			utf8strcat_blen(buffer_r, IM_INPUT_LINE_LEN, ptr_r + len_r);
			strcpy_blen(romaji, rom_buf_len, buffer_r);		// copy back to romaji
			break;
		}
		ptr_r += len_r;
		ptr_h += strlen(buf_h);
	}
flf_d_printf("romaji:[%s]\n", romaji);
	return strlen(romaji);
}

int imjatoh_romaji_offset(const char *romaji, int offset_h)
{
	const char *ptr_r;
	int off_h;
	char buf_h_from_a[IM_INPUT_LINE_LEN+1];
	int len_r;

//flf_d_printf("romaji: [%s], off_h: %d\n", romaji, offset_h);
	off_h = 0;
	for (ptr_r = romaji; *ptr_r; ) {
		if (off_h >= offset_h)
			break;
		len_r = imjatoh_convert_one(ptr_r, buf_h_from_a, IM_INPUT_LINE_LEN);
		ptr_r += len_r;
		off_h += strlen(buf_h_from_a);
//flf_d_printf("off_h: %d\n", off_h);
	}
//flf_d_printf("romaji:[%s],%d ==> %d\r\n", romaji, offset_h, ptr_r - romaji);
	return ptr_r - romaji;
}
int imjatoh_hira_offset(const char *romaji, const char *hira, int off_r)
{
	const char *ptr_r;
	int offset_h;
	char buf_h_from_a[IM_INPUT_LINE_LEN+1];
	int len_r;
	int len_h;

//_FLF_
	len_h = strlen(hira);
	offset_h = 0;
	for (ptr_r = romaji; *ptr_r; ) {
		if (offset_h >= len_h)
			break;
		if (ptr_r - romaji >= off_r)
			break;
		len_r = imjatoh_convert_one(ptr_r, buf_h_from_a, IM_INPUT_LINE_LEN);
		ptr_r += len_r;
		offset_h += strlen(buf_h_from_a);
	}
//flf_d_printf("romaji:[%s],%d ==> %d\r\n", romaji, off_r, offset_h);
	return offset_h;
}
int imjatoh_limit_romaji_len_by_pronun_len(char *romaji, const char *hira)
{
	const char *ptr_r;
	int offset_h;
	char buf_h_from_a[IM_INPUT_LINE_LEN+1];
	int len_r;
	int len_h;
	int offset_r;

//_FLF_
	len_h = strlen(hira);
	offset_h = 0;
	for (ptr_r = romaji; *ptr_r; ) {
		if (offset_h >= len_h)
			break;
		len_r = imjatoh_convert_one(ptr_r, buf_h_from_a, IM_INPUT_LINE_LEN);
		ptr_r += len_r;
		offset_h += strlen(buf_h_from_a);
	}
	offset_r = ptr_r - romaji;
//flf_d_printf("romaji:[%s],%d ==> %d\r\n", romaji, offset_r, offset_h);
	romaji[offset_r] = '\0';		// limit romaji len
	return offset_r;
}
int imjatoh_romaji_to_hira(const char *romaji, char *hira, int buf_len)
{
	const char *ptr_r;
	char buf_h[IM_INPUT_LINE_LEN+1];
	int len_r;

//flf_d_printf("romaji:[%s]==>\r\n", romaji);
	strcpy(hira, "");
	for (ptr_r = romaji; *ptr_r; ) {
		len_r = imjatoh_convert_one(ptr_r, buf_h, IM_INPUT_LINE_LEN);
		utf8strcat_blen(hira, buf_len, buf_h);
//flf_d_printf("hira:[%s]\r\n", hira);
		ptr_r += len_r;
	}
//flf_d_printf("hira:[%s]\r\n", hira);
	return strlen(hira);
}
PRIVATE int imjatoh_convert_one(const char *romaji, char *hira, int buf_len)
{
	int len_r;

//flf_d_printf(" romaji:[%s]==>\r\n", romaji);
	if (*romaji == IM_INVIS_SEP_CHR) {
		strcpy(hira, "");
		len_r = utf8c_bytes(romaji);		// length of unconverted char
	} else {
		len_r = imjatoh_convert_one2(romaji, hira, buf_len);
		if (len_r == 0) {
			len_r = utf8c_bytes(romaji);	// copy unconverted char
			utf8strcpy_blen_slen(hira, buf_len, romaji, len_r);
		}
	}
//flf_d_printf(" hira:[%s]\r\n", hira);
	return len_r;
}
// "ka" ==> "か", 2
// "kk" ==> "っ", 1
PRIVATE int imjatoh_convert_one2(const char *romaji, char *hira, int buf_len)
{
	const char *ptr_r;
	int len_r;
	char buf_h[MAX_ROM_HIRA_LEN+1];

	strcpy(hira, "");
	ptr_r = romaji;
	len_r = imjatoh_convert_one3(ptr_r, hira, buf_len);
	if (len_r == 0) {
		if (isalpha(ptr_r[0])) {
//			for ( ; ptr_r[0] == ptr_r[1]; ptr_r++) {
//				utf8strcat_blen(hira, buf_len, "っ");	// "kkkkka" ==> "っっっっか"
//			}
			if (ptr_r[0] == ptr_r[1]) {
				utf8strcat_blen(hira, buf_len, "っ");	// "kk" ==> "っ"
				ptr_r++;
			} else {
				len_r = imjatoh_convert_one3(ptr_r, buf_h, MAX_ROM_HIRA_LEN);
				if (len_r == 0) {
					len_r = utf8c_bytes(ptr_r);				// copy unconverted char
					utf8strcat_blen_slen(hira, buf_len, ptr_r, len_r);
				} else {
					utf8strcat_blen(hira, buf_len, buf_h);	// "kk" ==> "っ"
				}
				ptr_r += len_r;
			}
			len_r = ptr_r - romaji;
		}
	}
	return len_r;
}
// ka ==> か
PRIVATE int imjatoh_convert_one3(const char *romaji, char *hira, int buf_len)
{
	int idx;
	int len_r;
	int match_idx;
	int max_len_r;

//flf_d_printf(" romaji:[%s]==>\r\n", romaji);
	match_idx = -1;
	max_len_r = 0;
	strcpy(hira, "");
	for (idx = 0 ; idx < MAX_ROM_TABLE_LINES && roman_table[idx][0]; idx++) {
		len_r = sp1_word_len(roman_table[idx]);
		if (strncmp(romaji, roman_table[idx], len_r) == 0) {
			if (len_r > max_len_r) {
				match_idx = idx;
				max_len_r = len_r;
			}
		}
	}
	if (match_idx >= 0) {
		utf8strcpy_blen(hira, buf_len, skip_word_sp(roman_table[match_idx]));
	}
//flf_d_printf(" hira:[%s]\r\n", hira);
	return max_len_r;
}
// か ==> ka
// じゃ,じゅ,じょ ==> ji
PRIVATE int imjatoh_reverse_one(const char *hira, char *romaji)
{
	int len_h;

	len_h = imjatoh_reverse_one2(hira, romaji);
	if (len_h == 0) {
		len_h = utf8c_bytes(hira);	// copy unconverted char
		utf8strcpy_blen_slen(romaji, MAX_UTF8C_BYTES, hira, len_h);
	}
//flf_d_printf("hira:[%s] ==>romaji:[%s]\n", hira, romaji);
	return len_h;
}
PRIVATE int imjatoh_reverse_one2(const char *hira, char *romaji)
{
	int idx;
	int len_h;
	const char *tbl_hira;

//flf_d_printf("hira:[%s]==>\n", hira);
	len_h = utf8c_bytes(hira);
	strcpy(romaji, "");
	for (idx = 0 ; idx < MAX_ROM_TABLE_LINES && roman_table[idx][0]; idx++) {
		tbl_hira = skip_word_sp(roman_table[idx]);
//flf_d_printf("hira:[%s], tbl_romaji:[%s], tbl_hira:[%s]\n", hira, roman_table[idx], tbl_hira);
//flf_d_printf("%d ?? %d\n", len_h, strlen(tbl_hira));
		if (len_h == (int)strlen(tbl_hira)
		 && strncmp(hira, tbl_hira, len_h) == 0) {
			utf8strcpy_blen_slen(romaji, MAX_ROM_ALPHA_LEN, roman_table[idx],
			 sp1_word_len(roman_table[idx]));
			break;
		}
	}
//flf_d_printf("romaji:[%s]\r\n", romaji);
	return len_h;
}

// 全角/半角/ひらがな/カタカナ/Upper/Lower 相互変換 ---------------------------
// imjhira_...

PRIVATE char han_kata_table[] = 
"  / , . ･ : ; ? ! ' \" \\ ^ ~ _ ( ) { } [ ] ¥ "
"0 1 2 3 4 5 6 7 8 9 + - $ % # & * @ = "
"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z "
"a b c d e f g h i j k l m n o p q r s t u v w x y z "
"ｶﾞ ｷﾞ ｸﾞ ｹﾞ ｺﾞ ｻﾞ ｼﾞ ｽﾞ ｾﾞ ｿﾞ ﾀﾞ ﾁﾞ ﾂﾞ ﾃﾞ ﾄﾞ ﾊﾞ ﾋﾞ ﾌﾞ ﾍﾞ ﾎﾞ ﾊﾟ ﾋﾟ ﾌﾟ ﾍﾟ ﾎﾟ ｳﾞ "
"ｧ ｱ ｨ ｲ ｩ ｳ ｪ ｴ ｫ ｵ ｶ ｷ ｸ ｹ ｺ "
"ｻ ｼ ｽ ｾ ｿ ﾀ ﾁ ｯ ﾂ ﾃ ﾄ ﾅ ﾆ ﾇ ﾈ ﾉ "
"ﾊ ﾋ ﾌ ﾍ ﾎ ﾏ ﾐ ﾑ ﾒ ﾓ ｬ ﾔ ｭ ﾕ ｮ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ ﾜ ｦ ﾝ "
"ヮ ヰ ヱ ヵ ヶ "
"ﾞ ﾟ ､ ｢ ｣ ｰ ｡ ";

PRIVATE char zen_kata_table[] = 
"　 ／ ， ． ・ ： ； ？ ！ ＇ ＂ ＼ ＾ 〜 ＿ （ ） ｛ ｝ ［ ］ " ZEN_YEN " "
"０ １ ２ ３ ４ ５ ６ ７ ８ ９ ＋ " ZEN_HYP " ＄ ％ ＃ ＆ ＊ ＠ ＝ "
"Ａ Ｂ Ｃ Ｄ Ｅ Ｆ Ｇ Ｈ Ｉ Ｊ Ｋ Ｌ Ｍ Ｎ Ｏ Ｐ Ｑ Ｒ Ｓ Ｔ Ｕ Ｖ Ｗ Ｘ Ｙ Ｚ "
"ａ ｂ ｃ ｄ ｅ ｆ ｇ ｈ ｉ ｊ ｋ ｌ ｍ ｎ ｏ ｐ ｑ ｒ ｓ ｔ ｕ ｖ ｗ ｘ ｙ ｚ "
"ガ ギ グ ゲ ゴ ザ ジ ズ ゼ ゾ ダ ヂ ヅ デ ド バ ビ ブ ベ ボ パ ピ プ ペ ポ ヴ "
"ァ ア ィ イ ゥ ウ ェ エ ォ オ カ キ ク ケ コ "
"サ シ ス セ ソ タ チ ッ ツ テ ト ナ ニ ヌ ネ ノ "
"ハ ヒ フ ヘ ホ マ ミ ム メ モ ャ ヤ ュ ユ ョ ヨ ラ リ ル レ ロ ワ ヲ ン "
"ヮ ヰ ヱ ヵ ヶ "
"゛ ゜ 、 「 」 ー 。 ";

PRIVATE char zen_hira_table[] = 
"　 ／ ， ． ・ ： ； ？ ！ ＇ ＂ ＼ ＾ 〜 ＿ （ ） ｛ ｝ ［ ］ " ZEN_YEN " "
"０ １ ２ ３ ４ ５ ６ ７ ８ ９ ＋ " ZEN_HYP " ＄ ％ ＃ ＆ ＊ ＠ ＝ "
"Ａ Ｂ Ｃ Ｄ Ｅ Ｆ Ｇ Ｈ Ｉ Ｊ Ｋ Ｌ Ｍ Ｎ Ｏ Ｐ Ｑ Ｒ Ｓ Ｔ Ｕ Ｖ Ｗ Ｘ Ｙ Ｚ "
"ａ ｂ ｃ ｄ ｅ ｆ ｇ ｈ ｉ ｊ ｋ ｌ ｍ ｎ ｏ ｐ ｑ ｒ ｓ ｔ ｕ ｖ ｗ ｘ ｙ ｚ "
"が ぎ ぐ げ ご ざ じ ず ぜ ぞ だ ぢ づ で ど ば び ぶ べ ぼ ぱ ぴ ぷ ぺ ぽ う゛ "
"ぁ あ ぃ い ぅ う ぇ え ぉ お か き く け こ "
"さ し す せ そ た ち っ つ て と な に ぬ ね の "
"は ひ ふ へ ほ ま み む め も ゃ や ゅ ゆ ょ よ ら り る れ ろ わ を ん "
"ヮ ヰ ヱ ヵ ヶ "
"゛ ゜ 、 「 」 ー 。 ";

PRIVATE char ascii_upper_table[] = 
"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z "
"Ａ Ｂ Ｃ Ｄ Ｅ Ｆ Ｇ Ｈ Ｉ Ｊ Ｋ Ｌ Ｍ Ｎ Ｏ Ｐ Ｑ Ｒ Ｓ Ｔ Ｕ Ｖ Ｗ Ｘ Ｙ Ｚ ";

PRIVATE char ascii_lower_table[] = 
"a b c d e f g h i j k l m n o p q r s t u v w x y z "
"ａ ｂ ｃ ｄ ｅ ｆ ｇ ｈ ｉ ｊ ｋ ｌ ｍ ｎ ｏ ｐ ｑ ｒ ｓ ｔ ｕ ｖ ｗ ｘ ｙ ｚ ";

#ifdef IMJ_BUILT_IN_TEST
int test_hira_kata_han_conversion(void)
{
	char buf_from[MAX_DIC_LINE_LEN+1];
	char buf_to[MAX_DIC_LINE_LEN+1];

	remove_space(han_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_han_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("HAN_KATA to HAN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(han_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("HAN_KATA to ZEN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(han_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_hira(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("HAN_KATA to ZEN_HIRA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

_FLF_
	remove_space(zen_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_han_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_KATA to HAN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(zen_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_KATA to ZEN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(zen_kata_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_hira(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_KATA to ZEN_HIRA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

_FLF_
	remove_space(zen_hira_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_han_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_HIRA to HAN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(zen_hira_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_kata(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_HIRA to ZEN_KATA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	remove_space(zen_hira_table, buf_from, MAX_DIC_LINE_LEN);
	imjhira_to_zen_hira(buf_from, buf_to, MAX_DIC_LINE_LEN);
flf_d_printf("ZEN_HIRA to ZEN_HIRA\r\n<[%s]\r\n>[%s]\r\n", buf_from, buf_to);

	return 0;
}
#endif // IMJ_BUILT_IN_TEST

// カタ ==> かた
// ｶﾀ   ==> かた
char *imjhira_to_zen_hira(const char *str, char *buf, int buf_len)
{
	char buffer[MAX_DIC_LINE_LEN+1];

	imjhira_han_to_zen(str, buffer, MAX_DIC_LINE_LEN);
	imjhira_kata_to_hira(buffer, buf, buf_len);
	return buf;
}
// かた ==> カタ
// ｶﾀ   ==> カタ
char *imjhira_to_zen_kata(const char *str, char *buf, int buf_len)
{
	char buffer[MAX_DIC_LINE_LEN+1];

	imjhira_han_to_zen(str, buffer, MAX_DIC_LINE_LEN);
	imjhira_hira_to_kata(buffer, buf, buf_len);
	return buf;
}
// かた ==> ｶﾀ
// カタ ==> ｶﾀ
char *imjhira_to_han_kata(const char *str, char *buf, int buf_len)
{
	imjhira_zen_to_han(str, buf, buf_len);
//flf_d_printf("buf:[%s]\n", buf);
	return buf;
}
// かた ==> ｶﾀ
// カタ ==> ｶﾀ
// ＡＢａｂ ==> ABab
char *imjhira_zen_to_han(const char *str, char *buf, int buf_len)
{
	imjhira_conv_by_table_ol(str, buf, buf_len, zen_hira_table, han_kata_table);
//flf_d_printf("buf:[%s]\n", buf);
	imjhira_conv_by_table_ol(buf, buf, buf_len, zen_kata_table, han_kata_table);
//flf_d_printf("buf:[%s]\n", buf);
	return buf;
}
// ｶﾀ   ==> カタ
// ABab ==> ＡＢａｂ
char *imjhira_han_to_zen(const char *str, char *buf, int buf_len)
{
	return imjhira_conv_by_table_ol(str, buf, buf_len, han_kata_table, zen_hira_table);
}
// かた ==> カタ
char *imjhira_hira_to_kata(const char *str, char *buf, int buf_len)
{
	return imjhira_conv_by_table_ol(str, buf, buf_len, zen_hira_table, zen_kata_table);
}
// カタ ==> かた
char *imjhira_kata_to_hira(const char *str, char *buf, int buf_len)
{
	return imjhira_conv_by_table_ol(str, buf, buf_len, zen_kata_table, zen_hira_table);
}
// AB ==> ab
// ＡＢ ==> ａｂ
char *imjhira_to_lower(const char *str, char *buf, int buf_len)
{
	return imjhira_conv_by_table_ol(str, buf, buf_len, ascii_upper_table, ascii_lower_table);
}
// ab ==> AB
// ａｂ ==> ＡＢ
char *imjhira_to_upper(const char *str, char *buf, int buf_len)
{
	return imjhira_conv_by_table_ol(str, buf, buf_len, ascii_lower_table, ascii_upper_table);
}
// "str" and "buf" can be the same buffer (Overlappable)
PRIVATE char *imjhira_conv_by_table_ol(const char *str, char *buf, int buf_len,
 const char *table_from, const char *table_to)
{
	char buffer[MAX_DIC_LINE_LEN+1];	// enough length

	imjhira_conv_by_table(str, buffer, MIN_(MAX_DIC_LINE_LEN, buf_len), table_from, table_to);
	return utf8strcpy_blen(buf, buf_len, buffer);
}
PRIVATE char *imjhira_conv_by_table(const char *str, char *buf, int buf_len,
 const char *table_from, const char *table_to)
{
	const char *ptr_str;
	const char *tbl_from;
	const char *tbl_to;

//flf_d_printf("[%s]\n", str);
//flf_d_printf("TF[%s]\r\nTT[%s]\r\n", table_from, table_to);
	strcpy(buf, "");
	for (ptr_str = str; *ptr_str; ) {
		for (tbl_from = table_from, tbl_to = table_to; *tbl_from; ) {
//flf_d_printf("ptr:[%s]\nF:[%s]\r\nT:[%s]\r\n", ptr_str, tbl_from, tbl_to);
			if (strncmp(ptr_str, tbl_from, sp1_word_len(tbl_from)) == 0) {
				break;
			}
			tbl_from = skip_word_sp(tbl_from);
			tbl_to = skip_word_sp(tbl_to);
		}
		if (*tbl_from) {
			// convert
			utf8strcat_blen_slen(buf, buf_len, tbl_to, sp1_word_len(tbl_to));
			ptr_str += sp1_word_len(tbl_from);
		} else {
			// not convert
///			if (*ptr_str != IM_INVIS_SEP_CHR) {
				// copy
				utf8strcat_blen_slen(buf, buf_len, ptr_str, utf8c_bytes(ptr_str));
///			}
			ptr_str += utf8c_bytes(ptr_str);
		}
	}
//flf_d_printf("[%s]\n==>[%s]\n", str, buf);
	return 0;
}
int imjhira_contains_lower(const char *str)
{
	return imjhira_contains(str, ascii_lower_table);
}
int imjhira_contains_upper(const char *str)
{
	return imjhira_contains(str, ascii_upper_table);
}
PRIVATE int imjhira_contains(const char *string, const char *table)
{
	const char *str;
	const char *tbl;

//flf_d_printf("[%s]\n", str);
//flf_d_printf("TF[%s]\r\nTT[%s]\r\n", table_from, table_to);
	for (str = string; *str; ) {
		for (tbl = table; *tbl; ) {
//flf_d_printf("ptr:[%s]\nF:[%s]\r\nT:[%s]\r\n", ptr_str, tbl_from, tbl_to);
			if (strncmp(str, tbl, sp1_word_len(tbl)) == 0) {
				break;
			}
			tbl = skip_word_sp(tbl);
		}
		if (*tbl) {
			return 1;
		}
		str += utf8c_bytes(str);
	}
//flf_d_printf("[%s]\n==>[%s]\n", str, buf);
	return 0;
}
PRIVATE const char *skip_han(const char *ptr)
{
	while (is_han(ptr)) {
		ptr += utf8c_bytes(ptr);
	}
	return ptr;
}
PRIVATE int is_han(const char *chr)
{
	return is_chr_in_table(chr, han_kata_table);
}
PRIVATE int is_chr_in_table(const char *chr, const char *table)
{
	const char *tbl;

	for (tbl = table; *tbl; ) {
		if (strncmp(chr, tbl, sp1_word_len(tbl)) == 0) {
			break;
		}
		tbl = skip_word_sp(tbl);
	}
	if (*tbl)
		return 1;
	return 0;
}

#ifdef IMJ_BUILT_IN_TEST
PRIVATE const char *remove_space(const char *src, char *buf, int buf_len)
{
	const char *ptr1;
	const char *ptr2;

	strcpy(buf, "");
	for (ptr1 = src; *ptr1; ) {
///flf_d_printf("[%s]\n", ptr1);
		ptr2 = skip_word(ptr1+1);
///flf_d_printf("[%s]\n", ptr2);
		utf8strcat_blen_slen(buf, buf_len, ptr1, ptr2 - ptr1);
///flf_d_printf("[%s]\n", buf);
		ptr2 = skip_space(ptr2);
		ptr1 = ptr2;
	}
	return buf;
}
#endif // IMJ_BUILT_IN_TEST

//-----------------------------------------------------------------------------
// imjutil

///int strmatchlen_utf8(const char *str1, const char *str2, int max_len)
///{
///	int match_len;
///	int utf8_len;
///
///	utf8_len = 0;
///	if ((match_len = strmatchlen(str1, str2, max_len)) > 0) {
///		utf8_len = utf8s_len_le(str1, match_len);
///	}
/////flf_d_printf("strmatchlen_utf8: %d:[%s]:[%s]\n", utf8_len, str1, str2);
///	return utf8_len;
///}
///int strmatchlen(const char *str1, const char *str2, int max_len)
///{
///	const char *ptr1, *ptr2;
///	int match_len;
///
///	ptr1 = str1;
///	ptr2 = str2;
/////flf_d_printf("[%s] ?? [%s]\n", ptr1, ptr2);
///	for (match_len = 0; match_len < max_len; match_len++) {
///		if (*ptr1 != *ptr2)
///			break;
///		ptr1++;
///		ptr2++;
///	}
/////flf_d_printf("match_len: %d\n", match_len);
///	return match_len;
///}

char *cut_line_tail_crlf(char *string)
{
	if (strlen(string)) {
		if (string[strlen(string)-1] == '\n')
			string[strlen(string)-1] = '\0';
	}
	if (strlen(string)) {
		if (string[strlen(string)-1] == '\r')
			string[strlen(string)-1] = '\0';
	}
	return string;
}
//-----------------------------------------------------------------------------
int next_word(const char *string)
{
	return skip_word_sp(string) - string;
}
int sp1_word_len(const char *string)
{
	const char *ptr;

	ptr = skip_sp1(string);
	ptr = skip_word(ptr);
	return ptr - string;
}
int word_len(const char *string)	// "abc " ==> 3, " abc" ==> 0
{
	return skip_word(string) - string;
}
// Tab-Separated-word length
int tsword_len(const char *string)	// "abc\t" ==> 3, "\tabc" ==> 0
{
	return skip_tsword(string) - string;
}
int space_len(const char *string)
{
	return skip_space(string) - string;
}
//-----------------------------------------------------------------------------
const char *skip_sp_word_sp(const char *ptr)
{
	ptr = skip_space(ptr);
	return skip_word_sp(ptr);
}
const char *skip_word_sp(const char *ptr)
{
	ptr = skip_word(ptr);
	return skip_space(ptr);
}
const char *skip_tsword_tab(const char *ptr)
{
	ptr = skip_tsword(ptr);
	ptr = skip_ctrl(ptr);
	return ptr;
}
const char *skip_ctrl(const char *ptr)
{
	for ( ; IS_CTRL(*ptr); ) {
		ptr++;
		// nothing to do
	}
	return ptr;
}
const char *skip_sp1(const char *ptr)
{
	if (IS_SP_CTRL(*ptr)) {
		ptr++;
	}
	return ptr;
}
const char *skip_space(const char *ptr)
{
	for ( ; IS_SP_CTRL(*ptr); ) {
		ptr++;
		// nothing to do
	}
	return ptr;
}
const char *skip_word(const char *ptr)
{
	for ( ; *ptr; ptr++) {
		if (IS_SP_CTRL(*ptr))
			break;
	}
	return ptr;
}
// skip Tab-Separated-word
const char *skip_tsword(const char *ptr)
{
	for ( ; *ptr; ptr++) {
		if (IS_CTRL(*ptr))
			break;
	}
	return ptr;
}
//-----------------------------------------------------------------------------
PRIVATE int is_ctrl(const char chr)
{
	return IS_CTRL(chr);
}
PRIVATE int is_sp_ctrl(const char chr)
{
	return IS_SP_CTRL(chr);
}
//-----------------------------------------------------------------------------
// This must be used only for debug.
char *get_static_buf_if_null(char *buf)
{
	static char static_buf[MAX_DIC_LINE_LEN+1];
	if (buf == NULL)
		buf = static_buf;
	return buf;
}

// End of imj.cpp
