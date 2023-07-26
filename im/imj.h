//=============================================================================
// Simple Japanese Input Method implementation
//=============================================================================

#ifndef imj_h
#define imj_h

#include "utilincs.h"

#ifdef ENABLE_DEBUG
///
#define IMJ_BUILT_IN_TEST
#endif // ENABLE_DEBUG

#define IS_CTRL(chr)		('\0' < (chr) && (chr) < ' ')
#define IS_SP_CTRL(chr)		('\0' < (chr) && (chr) <= ' ')
#define IS_GRAPH(chr)		(' ' < (unsigned char)(chr))

#define IM_MODE_OFF			0
#define IM_MODE_INPUT		1
#define IM_MODE_CONVERT		2

// key event code
#define IM_KE(key)					(0x0100 | (key))
#define CTRL_CODE(chr)				((chr) - '@')

#define IM_KE_ON					IM_KE(CTRL_CODE('@'))
#define IM_KE_OFF					IM_KE('`')
#define IM_KE_CONVERT				IM_KE(' ')
#define IM_KE_CLEAR					IM_KE('\x1b')
#define IM_KE_BACKSPACE				IM_KE(CTRL_CODE('H'))
#define IM_KE_DELETE				IM_KE(CTRL_CODE('G'))
#define IM_KE_CURSOR_LEFT			IM_KE(CTRL_CODE('S'))
#define IM_KE_CURSOR_RIGHT			IM_KE(CTRL_CODE('D'))
#define IM_KE_NO_CONV				IM_KE('%')
#define IM_KE_HIRAGANA				IM_KE('^')
#define IM_KE_ZENKAKU_KATAKANA		IM_KE('&')
#define IM_KE_HANKAKU_KATAKANA		IM_KE('*')
#define IM_KE_ZENKAKU_ALPHA_LOWER	IM_KE('(')
#define IM_KE_HANKAKU_ALPHA_LOWER	IM_KE(')')
#define IM_KE_ZENKAKU_ALPHA_UPPER	IM_KE('-')
#define IM_KE_HANKAKU_ALPHA_UPPER	IM_KE('=')

#define IM_KE_COMMIT_ONE			IM_KE('\n')
#define IM_KE_COMMIT_ALL			IM_KE('\r')
#define IM_KE_SEPARATE_LEFT			IM_KE(CTRL_CODE('S'))
#define IM_KE_SEPARATE_RIGHT		IM_KE(CTRL_CODE('D'))
#define IM_KE_SELECT_PREV			IM_KE(CTRL_CODE('E'))
#define IM_KE_SELECT_NEXT			IM_KE(CTRL_CODE('X'))
#define IM_KE_SELECT_FIRST			IM_KE(CTRL_CODE('A'))
#define IM_KE_SELECT_LAST			IM_KE(CTRL_CODE('Z'))

#define IM_MAX_INPUT_CHARS		60
										// 連文節入力の最大長(3*60=180 bytes)
#define IM_INPUT_LINE_LEN		(MAX_UTF8C_BYTES * IM_MAX_INPUT_CHARS)

// Dictionary file
#define MAX_DIC_PRONUN_CHARS	16		// 活用を除いた辞書登録読みの最大文字数
										// 活用を除いた辞書登録読みの最大長("朝霧高原"...)
#define MAX_DIC_PRONUN_LEN		(MAX_UTF8C_BYTES * MAX_DIC_PRONUN_CHARS)
//#define ROM_FILE_NAME		"atok.rom"
#define DIC_FILE_NAME		"imj.dic"		// dictionary file name
#define CNVHST_FILE_NAME	"imj.hst"		// conversion hisory file name
#define MAX_DIC_LINE_LEN	4096
#define MAX_DIC_RECORDS		1000000			// 1,000,000
#define MAX_CANNA_HINSHI_CODE_LEN	(1+10+1+3)		// "#ABCDEFGHIJ*500"

										// 変換候補の最大長("演じなければ"...)
#define MAX_CANDID_LEN		(MAX_UTF8C_BYTES * IM_MAX_INPUT_CHARS)

//#define MAX_CONVERSION_HISTORIES	1000
#define MAX_CONVERSION_HISTORIES	2000
///#define MAX_CONVERSION_HISTORIES	10000
										// "えくすてんでっどえでぃしょん"
#define MAX_HISTORY_PRONUN_LEN		(MAX_UTF8C_BYTES * IM_MAX_INPUT_CHARS)
										// "エクステンデッドエディション"
#define MAX_HISTORY_CONVERTED_LEN	(MAX_UTF8C_BYTES * IM_MAX_INPUT_CHARS)
										// "えくすてんでっどえでぃしょん"
#define MAX_HISTORY_INPUT_LEN		(MAX_HISTORY_PRONUN_LEN+MAX_HISTORY_CONVERTED_LEN)
										// "かんじ 漢字"
#define MAX_CONVERSION_HISTORY_LEN	(MAX_HISTORY_PRONUN_LEN+1+MAX_HISTORY_CONVERTED_LEN)
										// " えくすてんでっどえでぃしょん"
#define MAX_INPUT_HISTORY_LEN		(1+MAX_HISTORY_INPUT_LEN)
										// "かんじ 漢字\n"
#define MAX_HISTORY_BUF_LEN			(MAX_HISTORY_PRONUN_LEN+1+MAX_HISTORY_CONVERTED_LEN+1)

// 変換候補 -------------------------------------------------------------------
class imjcandidates
{
	struct _candidate_ {
		int pronun_len;
		char candidate[MAX_CANDID_LEN+1];
	};
#define MAX_CANDIDS_LIST		300
	struct _candidate_ _candidates[MAX_CANDIDS_LIST];

public:
	void clear(void);
	int get_num_of_candids(void);
	int append_one_candid(char *candid);
	int insert_one_candid(char *candid);
	int is_there_the_same_candid(char *candid);
	int delete_one_candid(int candid_idx);
	int compare_candid(int candid_idx, char *candid);
	void put_candid(int candid_idx, char *candid, int len);
	char *get_candid(int candid_idx, char *buf, int buf_len);
	void dump_candids(void);
};

// 辞書検索 -------------------------------------------------------------------
class imjdic
{
// かんじ #KJ*500 漢字 幹事 ...
#define	HINSHI_PREF_CHR		'#'
#define	HINSHI_PREF_STR		"#"
#define	HINSHI_CODE_KJ		"#KJ"
	static FILE *fp_dic;	// dic file
	static int dic_num_of_records;
	static long dic_record_offsets[MAX_DIC_RECORDS];

	static long bsearch_match_offset;

	// Dictionary search statistics
	static int dic_search_words;
	static int dic_search_records;
	static int dic_max_search_records;
	static int dic_total_search_records;
	static int dic_avg_search_records;	// average records searched per word

public:
	static int imjdic_open(const char *file_path);
	static int imjdic_close(void);

	static int imjdic_read_record(long offset, char *buffer, int buf_len);
	static int imjdic_get_offsets_of_all_records(void);
	static int imjdic_sort(void);
	static int imjdic_qsort_compare(const void *aa, const void *bb);
	static void imjdic_dump_all_tangos(void);

	static int imjdic_search_total_len(char *pronun, int len_to_match, imjcandidates *_candidates);
	static int imjdic_search_gokan_katsuyou(char *pronun,
	 int gokan_len_to_match, int gok_kat_len_to_match, imjcandidates *_candidates);
	static int imjdic_disp_search_stats(char *pronun);
	static int imjdic_bsearch_compare(const void *key, const void *element);
	static int imjdic_compare_katsuyou(char *dic_rec, char *pronun,
	 int gokan_len, int expanded_len, imjcandidates *_candidates);

	static int imjdic_get_pronun(char *dic_rec, char *pronun, int buf_len);
	static int imjdic_get_num_of_candids(char *dic_record);
	static int imjdic_get_one_gokan_hinshi(char *dic_record, int candid_idx,
	 char *gokan, int buf_len, char *hinshi_code);
	static int imjdic_next_candidate(char *dic_record);
	static int imjdic_candidate_len(const char *dic_record);
	static char *get_hinshi_code(char *hinshi_code);
};

// 単文節変換(辞書+変換ヒストリ複合検索) --------------------------------------
// 1: candidates from Conversion history
//    "漢字"
// 2: candidates from Dictionary
//    "漢字", "幹事", "感じ", "完治"
// 3: Hiragana, Katakana, Hankaku-Katakana, Zenkaku-ASCII, Hankaku-ASCII
//    "かんじ", "カンジ", "ｶﾝｼﾞ", "kannji", "KANNJI", "ｋａｎｎｊｉ", "ＫＡＮＮＪＩ"
class imjtanbunsetsu
{
public:
	char _romaji[IM_INPUT_LINE_LEN+1];
	char _pronun[IM_INPUT_LINE_LEN+1];
	int _match_len;
	int _candid_idx;
	imjcandidates _candidates;

public:
	void clear(void);
	int conv_tanbunsetsu(char *romaji, char *pronun, int len_to_match);
	void dump(void);
};

// 連文節変換 -----------------------------------------------------------------
class imjrenbunsetsu
{
#define MAX_BUNSETSU	10		// for 連文節変換
public:
	char _romaji[IM_INPUT_LINE_LEN+1];
	char _pronun[IM_INPUT_LINE_LEN+1];
	imjtanbunsetsu _renbunsetsu[MAX_BUNSETSU];
	int _num_of_bunsetsu;

	void clear(void);
	int conv_renbunsetsu(char *romaji, char *pronun, int len_to_match);
	void shift_bunsetsu(void);
	void dump(void);
};

class imj
{
	int _im_mode;
	char im_romaji[IM_INPUT_LINE_LEN+1];	// ROMAJI input "kannji"
	char im_pronun[IM_INPUT_LINE_LEN+1];	// pronunciation (YOMI) "かんじ"
	int im_i_cursor_offset;					// cursor position in pronunciation
	int im_i_cur_history_idx;				// input history index currently selected

	int im_c_len_to_match;					// length of pronunciation currently being converted
	int im_c_cur_cand_idx;					// index currently selected
	char im_c_cur_candid[MAX_CANDID_LEN+1];		// currently selected candidate "漢字"
	char im_commit_buffer[IM_INPUT_LINE_LEN+1];	// buffer in which committed string is held
public:
	imjrenbunsetsu _renbunsetsu;

public:
	int im_open(const char *dir);
	int im_close(void);

	int im_process_key(int key);
	int im_process_key_event(int key, int event);

	void im_init(void);
	void im_set_mode(int im_mode);
	int im_get_mode(void);
	void im_set_romaji(const char *string);
	const char *im_get_romaji(char *buf, int buf_len);
	void im_set_pronun(const char *pronun);
	const char *im_get_pronun(char *buf, int buf_len);
	int im_get_pronun_len(void);
	int im_get_pronun_len_to_match(void);
	int im_get_cursor_offset();

	void im_i_clear_cur_history_idx(void);
	void im_i_set_cur_history_idx(int hist_idx);
	int im_i_get_cur_history_idx(void);
	int im_i_get_num_of_histories(void);
	void im_i_select_prev(void);
	void im_i_select_next(void);
	void im_i_select_first(void);
	void im_i_select_last(void);
	void im_i_check_set_history_idx(int hist_idx);
	const char *im_i_get_cur_history(void);

	int im_i_restart(void);
	int im_i_backspace(void);
	int im_i_delete(void);
	int im_i_input_romaji(int chr);

	void im_i_cursor_left(void);
	void im_i_cursor_right(void);
	void im_i_no_conv(void);
	void im_i_hiragana(void);
	void im_i_zenkaku_katakana(void);
	void im_i_hankaku_katakana(void);
	void im_i_zenkaku_alpha_lower(void);
	void im_i_zenkaku_alpha_upper(void);
	void im_i_hankaku_alpha_lower(void);
	void im_i_hankaku_alpha_upper(void);
	void im_i_commit_all(void);

	void im_c_start_conversion(void);
	void new_convert(void);
	void re_convert(void);
	char *im_c_get_cur_candid(char *buf, int buf_len);
	char *im_c_get_one_candid(int candid_idx, char *buf, int buf_len);

	void im_c_select_prev(void);
	void im_c_select_next(void);
	void im_c_select_first(void);
	void im_c_select_last(void);
	void im_c_check_set_candid_idx(int candid_idx);
	void im_c_set_cur_cand_idx(int candid_idx);
	int im_c_get_cur_cand_idx(void);
	char *im_c_get_first_bunsetsu(char *buf, int buf_len);
	char *im_c_get_remaining_bunsetsu(char *buffer, int buf_len, const char *separator);
	char *im_c_get_all_bunsetsu(char *buffer, int buf_len, const char *separator);
	char *join_bunsetsu(int start_bunsetsu_idx, int end_bunsetsu_idx, const char *separator,
	 char *buffer, int buf_len);
	int im_c_get_num_of_candids(void);

	void im_c_no_conv(void);
	void im_c_hiragana(void);
	void im_c_zenkaku_katakana(void);
	void im_c_hankaku_katakana(void);
	void im_c_zenkaku_alpha_lower(void);
	void im_c_zenkaku_alpha_upper(void);
	void im_c_hankaku_alpha_lower(void);
	void im_c_hankaku_alpha_upper(void);
	void im_c_separation_left(void);
	void im_c_separation_right(void);

	void im_c_commit_one(void);
	void im_c_commit_all(void);
	void im_c_commit_one__(void);
	void im_c_commit_string(const char *pronun, const char *conversion);
	const char *im_get_committed_string(char *buf, int buf_len);
};

void clear_commit_conversion(void);
void concat_commit_conversion(const char *pronun, const char *conversion);
void restart_commit_conversion(void);

#ifdef IMJ_BUILT_IN_TEST
void imjdic_test_katsuyou(void);
#endif // IMJ_BUILT_IN_TEST
char *imjdic_get_katsuyous_from_hinshi_code(const char *code, char *buffer, int buf_len);

// conversion history
int imjhst_load(const char *file_path);
int imjhst_save_if_updated(char *file_path);
int imjhst_save(const char *file_path);

// ROMAJI conversion (atoh:ASCII to Hiragana)
#ifdef IMJ_BUILT_IN_TEST
void test_imjatoh_functions(void);
#endif // IMJ_BUILT_IN_TEST
#if 0
int imjatoh_load_table(char *file_path);
#endif

int imjatoh_insert_romaji(char *romaji, int rom_buf_len,
 char *hira, int hira_buf_len, int off_h, char *insert);
int imjatoh_delete_romaji(char *romaji, int rom_buf_len,
 char *hira, int hira_buf_len, int off_h_from, int off_h_to);
int imjatoh_split_romaji_spelling(char *romaji, int rom_buf_len,
 const char *hira, int split_offset);
int imjatoh_romaji_offset(const char *romaji, int offset_h);
int imjatoh_hira_offset(const char *romaji, const char *hira, int off_r);
int imjatoh_limit_romaji_len_by_pronun_len(char *romaji, const char *hira);

int imjatoh_romaji_to_hira(const char *romaji, char *hira, int buf_len);

// Hiragana, Katakana, Hankaku, Zenkaku conversion
#ifdef IMJ_BUILT_IN_TEST
int test_hira_kata_han_conversion(void);
#endif // IMJ_BUILT_IN_TEST
char *imjhira_to_zen_hira(const char *str, char *buf, int buf_len);
char *imjhira_to_zen_kata(const char *str, char *buf, int buf_len);
char *imjhira_to_han_kata(const char *str, char *buf, int buf_len);
char *imjhira_zen_to_han(const char *str, char *buf, int buf_len);
char *imjhira_han_to_zen(const char *str, char *buf, int buf_len);
char *imjhira_hira_to_kata(const char *str, char *buf, int buf_len);
char *imjhira_kata_to_hira(const char *str, char *buf, int buf_len);
char *imjhira_to_lower(const char *str, char *buf, int buf_len);
char *imjhira_to_upper(const char *str, char *buf, int buf_len);
int imjhira_contains_lower(const char *str);
int imjhira_contains_upper(const char *str);

///int strmatchlen_utf8(const char *str1, const char *str2, int max_len);
///int strmatchlen(const char *str1, const char *str2, int max_len);
char *cut_line_tail_crlf(char *string);

int next_word(const char *string);
int sp1_word_len(const char *string);
int word_len(const char *string);
int tsword_len(const char *string);
int space_len(const char *string);

const char *skip_sp_word_sp(const char *ptr);
const char *skip_word_sp(const char *ptr);
const char *skip_tsword_tab(const char *ptr);
const char *skip_ctrl(const char *ptr);
const char *skip_sp1(const char *ptr);
const char *skip_space(const char *ptr);
const char *skip_word(const char *ptr);
const char *skip_tsword(const char *ptr);

char *get_static_buf_if_null(char *buf);

#endif // imj_h

// End of imj.h
