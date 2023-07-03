/****************************************************************************
	Qt based IM UI imjqt.cpp
 ****************************************************************************/

#include "stdincs.h"
#include "qincludes.h"

#include "imj.h"
#include "imjqt.h"

imjqt *__imjqt = 0;

_imj_ __imj;

imjqt::imjqt(QWidget* parent) : QDialog(parent)
{
//_TFLF_
	setupUi(this);
	__imj.im_open(NULL);
	connect(pushButtonOn, SIGNAL(clicked()), this, SLOT(im_on()));
	connect(pushButtonOff, SIGNAL(clicked()), this, SLOT(im_off()));
	connect(pushButtonConvert, SIGNAL(clicked()), this, SLOT(convert()));
	connect(pushButtonHiragana, SIGNAL(clicked()), this, SLOT(hiragana()));
	connect(pushButtonKatakana, SIGNAL(clicked()), this, SLOT(zenkaku_katakana()));
	connect(pushButtonHankakuKatakana, SIGNAL(clicked()), this, SLOT(hankaku_katakana()));
	connect(pushButtonZenkakuAlphaNumeric, SIGNAL(clicked()), this, SLOT(zenkaku_alpha_lower()));
	connect(pushButtonHankakuAlphaNumeric, SIGNAL(clicked()), this, SLOT(hankaku_alpha_lower()));
	connect(pushButtonLeft, SIGNAL(clicked()), this, SLOT(separation_left()));
	connect(pushButtonLight, SIGNAL(clicked()), this, SLOT(separation_right()));
	connect(pushButtonCommitOne, SIGNAL(clicked()), this, SLOT(commit_one()));
	connect(pushButtonCommitAll, SIGNAL(clicked()), this, SLOT(commit_all()));
	connect(pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));

//	timer_id = startTimer(1000);
	MyLineEdit1 = new MyLineEdit(this);
	MyLineEdit1->setGeometry(lineEdit1->geometry());
	lineEdit1->hide();
	MyLineEdit1->setFocus();
//	connect(MyLineEdit1, SIGNAL(keyPressEventSent(QKeyEvent *ke)),
//	 this, SLOT(receiveKeyPressEvent(QKeyEvent *ke)));
	connect(MyLineEdit1, SIGNAL(keyPressEventSent(QKeyEvent *)),
	 this, SLOT(receiveKeyPressEvent(QKeyEvent *)));

	update_conversion();
	textLabel2->setText("");
//_TFLF_
}

imjqt::~imjqt()
{
//_TFLF_
	delete MyLineEdit1;
//	killTimer(timer_id);
	__imj.im_close();
//_TFLF_
}

///void imjqt::timerEvent(QTimerEvent *te)
///{
///	static int processing_prev_timer = 0;
///	Q_UNUSED(te)
///
///	if (processing_prev_timer == 0) {
///		processing_prev_timer = 1;
///
///		processing_prev_timer = 0;
///	}
///}
void imjqt::receiveKeyPressEvent(QKeyEvent *ke)
{
///
flf_d_printf("key: %04x, text: [%s]\n", ke->key(), CCP(ke->text()));
	switch(ke->key()) {
	case Qt::Key_Space:
		if (__imj.im_get_pronun_len() == 0)
			__imj.im_process_key(ke->key());
		else
			convert();
		break;
	case Qt::Key_Backspace:	backspace();				break;
//	case Qt::Key_Insert:	im_process_key(IM_KE_INSERT);		break;
	case Qt::Key_Delete:	delete_char();				break;
	case Qt::Key_Home:
	case Qt::Key_End:
	case Qt::Key_PageUp:
	case Qt::Key_PageDown:
		break;
	case Qt::Key_Left:		separation_left();			break;
	case Qt::Key_Right:		separation_right();			break;
	case Qt::Key_Up:		select_prev();				break;
	case Qt::Key_Down:		commit_one();				break;
	case Qt::Key_Return:
	case Qt::Key_Enter:		commit_all();				break;
	case Qt::Key_F6:		hiragana();					break;
	case Qt::Key_F7:		zenkaku_katakana();			break;
	case Qt::Key_F8:		hankaku_katakana();			break;
	case Qt::Key_F9:		zenkaku_alpha_lower();		break;
	case Qt::Key_F10:		hankaku_alpha_lower();		break;
	case Qt::Key_F11:		zenkaku_alpha_upper();		break;
	case Qt::Key_F12:		hankaku_alpha_upper();		break;
	default:
		if (' ' <= ke->key() && ke->key() <= '~' && ke->text().isEmpty() == 0) {
			__imj.im_process_key(ke->text().at(0).toLatin1());
		}
		break;
	}
	update_conversion();
}

void imjqt::im_on()
{
	__imj.im_process_key(IM_KE_ON);
	update_conversion();
	MyLineEdit1->setFocus();
}
void imjqt::im_off()
{
	__imj.im_process_key(IM_KE_OFF);
	update_conversion();
}
void imjqt::convert()
{
	__imj.im_process_key(IM_KE_CONVERT);
	update_conversion();
}
void imjqt::backspace()
{
	__imj.im_process_key(IM_KE_BACKSPACE);
	update_conversion();
}
void imjqt::delete_char()
{
	__imj.im_process_key(IM_KE_DELETE);
	update_conversion();
}
void imjqt::select_prev()
{
	__imj.im_process_key(IM_KE_SELECT_PREV);
	update_conversion();
}
void imjqt::select_next()
{
	__imj.im_process_key(IM_KE_SELECT_NEXT);
	update_conversion();
}
void imjqt::separation_left()
{
	__imj.im_process_key(IM_KE_SEPARATE_LEFT);
///_FLF_
	update_conversion();
///_FLF_
}
void imjqt::separation_right()
{
	__imj.im_process_key(IM_KE_SEPARATE_RIGHT);
	update_conversion();
}
void imjqt::commit_one()
{
	char buf[IM_INPUT_LINE_LEN+1];

	__imj.im_process_key(IM_KE_COMMIT_ONE);
	textLabel2->setText(QString().sprintf("Committed [%s]",
	 __imj.im_get_committed_string(buf, IM_INPUT_LINE_LEN)));
	update_conversion();
}
void imjqt::commit_all()
{
	char buf[IM_INPUT_LINE_LEN+1];

	__imj.im_process_key(IM_KE_COMMIT_ALL);
	textLabel2->setText(QString().sprintf("Committed [%s]",
	 __imj.im_get_committed_string(buf, IM_INPUT_LINE_LEN)));
	update_conversion();
}
void imjqt::hiragana()
{
	__imj.im_process_key(IM_KE_HIRAGANA);
	update_conversion();
}
void imjqt::zenkaku_katakana()
{
	__imj.im_process_key(IM_KE_ZENKAKU_KATAKANA);
	update_conversion();
}
void imjqt::hankaku_katakana()
{
	__imj.im_process_key(IM_KE_HANKAKU_KATAKANA);
	update_conversion();
}
void imjqt::zenkaku_alpha_lower()
{
	__imj.im_process_key(IM_KE_ZENKAKU_ALPHA_LOWER);
	update_conversion();
}
void imjqt::hankaku_alpha_lower()
{
	__imj.im_process_key(IM_KE_HANKAKU_ALPHA_LOWER);
	update_conversion();
}
void imjqt::zenkaku_alpha_upper()
{
	__imj.im_process_key(IM_KE_ZENKAKU_ALPHA_UPPER);
	update_conversion();
}
void imjqt::hankaku_alpha_upper()
{
	__imj.im_process_key(IM_KE_HANKAKU_ALPHA_UPPER);
	update_conversion();
}
void imjqt::candid_selected(int candid_idx)
{
	__imj.im_c_set_cur_cand_idx(candid_idx);
	update_conversion();
}

void imjqt::update_conversion(void)
{
	char pronun[IM_INPUT_LINE_LEN+1];
	char candid[IM_INPUT_LINE_LEN+1];
	int idx;

	disconnect(listBox1, (const char *)0, (const QObject *)0, (const char *)0);

	switch(__imj.im_get_mode()) {
	case IM_MODE_OFF:
		textLabel1->setText("");
		break;
	case IM_MODE_INPUT:
		__imj.im_get_pronun(pronun, IM_INPUT_LINE_LEN);
		MyLineEdit1->setText(QString::fromUtf8(pronun));
		MyLineEdit1->setCursorPosition(utf8s_chars(pronun, __imj.im_get_cursor_offset()));
///flf_d_printf("utf8s_chars([%s], %d)==>%d\n",
/// pronun, __imj.im_get_cursor_offset(), utf8s_chars(pronun, __imj.im_get_cursor_offset()));
//		listBox1->clear();
		textLabel1->setText(tr("Inputing"));
		break;
	case IM_MODE_CONVERSION:
		__imj.im_c_get_first_bunsetsu(pronun, IM_INPUT_LINE_LEN);
		__imj.im_c_get_all_bunsetsu(candid, IM_INPUT_LINE_LEN, "");
///
flf_d_printf("first:[%s] remain:[%s]\n", pronun, candid);
		MyLineEdit1->setText(QString::fromUtf8(candid));
		MyLineEdit1->setSelection(0, utf8s_chars(pronun, IM_INPUT_LINE_LEN));
///flf_d_printf("candid_idx:%d, candids: %d\n",
/// __imj.im_c_get_cur_cand_idx(), __imj.im_c_get_num_of_candids());
		if (__imj.im_c_get_num_of_candids() > 0) {
//			if (__imj.im_c_get_cur_cand_idx() == 0) {
				listBox1->clear();
				for (idx = 0; idx < __imj.im_c_get_num_of_candids(); idx++) {
					__imj.im_c_get_one_candid(idx, candid, IM_INPUT_LINE_LEN);
					listBox1->addItem(QString().sprintf("%d: %s", idx+1, candid));
				}
//			}
///flf_d_printf("candid_idx:%d, candids: %d\n",
/// __imj.im_c_get_cur_cand_idx(), __imj.im_c_get_num_of_candids());
			listBox1->setCurrentRow(__imj.im_c_get_cur_cand_idx());
///_FLF_
			textLabel1->setText(QString().sprintf("Converting - %d/%d",
			 __imj.im_c_get_cur_cand_idx()+1, __imj.im_c_get_num_of_candids()));
///_FLF_
		} else {
			textLabel1->setText(QString().sprintf("Converting"));
		}
///_FLF_
		break;
	}

	connect(listBox1, SIGNAL(currentRowChanged(int)), this, SLOT(candid_selected(int)));
}

// End of imjqt.cpp
