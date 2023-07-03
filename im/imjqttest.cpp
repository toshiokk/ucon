// Qt based IM UI "imjqt" test program

#include "stdincs.h"
#include "qincludes.h"

#include "imjqttest.h"

QApplication *app;
imjqttest *appWin;

int main(int argc, char **argv)
{
//	sig_register_handler();

//_TFLF_
//	app = new QApplication(argc, argv, QApplication::GuiServer);
	app = new QApplication(argc, argv);
///stderr_printf("qwsServer:%p\n", qwsServer);

	appWin = new imjqttest();

///	appWin->setCaption(appWin->caption() + (qwsServer ? "(QWSServer)" : "(QWSClient)"));
///	app->setMainWidget(appWin);
	appWin->show();

	app->exec();
//_TFLF_

	delete appWin;
//_TFLF_
//	delete app;		// This cause signal 11. DNU(DoNotUse)
//_TFLF_

//	sig_unregister_handler();

//_TFLF_
	exit(0);
}

//------------------------------------------------------

imjqttest::imjqttest(QWidget *parent) : QMainWindow(parent)
{
//_TFLF_
	setupUi(this);
	textEdit1->setFocus();
	connect(pushButton01, SIGNAL(clicked()), this, SLOT(slot01()));
	connect(pushButton02, SIGNAL(clicked()), this, SLOT(slot02()));
	connect(pushButton03, SIGNAL(clicked()), this, SLOT(slot03()));
	connect(pushButton04, SIGNAL(clicked()), this, SLOT(slot04()));
	connect(pushButton05, SIGNAL(clicked()), this, SLOT(slot05()));
	connect(pushButton06, SIGNAL(clicked()), this, SLOT(slot06()));
	connect(pushButton07, SIGNAL(clicked()), this, SLOT(slot07()));
	connect(pushButton09, SIGNAL(clicked()), this, SLOT(slot09()));
	slot01();
//_TFLF_
}
imjqttest::~imjqttest()
{
//_TFLF_
}

void imjqttest::showMinimized(void)
{
//_TFLF_
	resize(QSize(100, 10));
//	imjqttestui::showMinimized();
}

//void imjqttest::closeEvent(QCloseEvent *ce)
//{
//_TFLF_
//	if (QMessageBox::question(this, tr("Shutdown application"),
//	 tr("Are you OK to quit application ?"),
//	 QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
//		ce->ignore();		// Exit canceled
//	else
//		ce->accept();		// Exit accepted
//}

void imjqttest::slot01()
{
//_TFLF_
	imjqt().exec();
}
void imjqttest::slot02()
{
//_TFLF_
}
void imjqttest::slot03()
{
//_TFLF_
}
void imjqttest::slot04()
{
//_TFLF_
}
void imjqttest::slot05()
{
//_TFLF_
	QInputDialog::getText(this, "caption", "label");
}
void imjqttest::slot06()
{
//_TFLF_
}
void imjqttest::slot07()
{
//_TFLF_
	showMinimized();
}
void imjqttest::slot09()
{
//_TFLF_
	close();
}
