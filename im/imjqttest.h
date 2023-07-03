#ifndef IMJQTTEST_H
#define IMJQTTEST_H

#include "stdincs.h"
#include "qincludes.h"

#include "imjqt.h"
#include "ui_imjqttest.h"

class imjqttest : public QMainWindow, public Ui::imjqttest
{
	Q_OBJECT

public:
	imjqttest(QWidget *parent = 0);
	~imjqttest();

protected:

protected slots:
	void slot01();
	void slot02();
	void slot03();
	void slot04();
	void slot05();
	void slot06();
	void slot07();
	void slot09();
//	void closeEvent(QCloseEvent *ce);
	void showMinimized(void);

private:

};

extern QApplication *app;
extern imjqttest *appWin;

#endif // IMJQTTEST_H
