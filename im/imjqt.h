/****************************************************************************
	Qt base IM UI imjqt.h
 ****************************************************************************/

#ifndef IMJQT_H
#define IMJQT_H

#include "stdincs.h"
#include "qincludes.h"
#include "imj.h"

#include "ui_imjqt.h"

extern _imj_ __imj;

class MyLineEdit;

class imjqt : public QDialog, public Ui::imjqt
{
	Q_OBJECT

public:
	imjqt(QWidget* parent = 0);
	~imjqt();

public slots:
///	void timerEvent(QTimerEvent *te);
	void receiveKeyPressEvent(QKeyEvent *ke);
	void im_on();
	void im_off();
	void convert();
	void backspace();
	void delete_char();
	void select_prev();
	void select_next();
	void separation_left();
	void separation_right();
	void commit_one();
	void commit_all();
	void hiragana();
	void zenkaku_katakana();
	void hankaku_katakana();
	void zenkaku_alpha_lower();
	void hankaku_alpha_lower();
	void zenkaku_alpha_upper();
	void hankaku_alpha_upper();
	void candid_selected(int candid_idx);

protected:
//protected slots:

private:
	MyLineEdit *MyLineEdit1;
	void update_conversion(void);
//	int timer_id;
};

#include "mylineedit.h"

extern imjqt *__imjqt;

#endif // IMJQT_H

// End of imjqt.h
