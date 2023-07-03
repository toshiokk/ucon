/****************************************************************************
	mylineedit.h
 ****************************************************************************/

#if !defined(MYLINEEDIT_H)
#define	MYLINEEDIT_H

#include <qlineedit.h>
#include "imjqt.h"

class MyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	MyLineEdit(QWidget* parent = 0) : QLineEdit(parent) {}
	~MyLineEdit() {}

public Q_SLOTS:
	void keyPressEvent(QKeyEvent *ke)
	{
		emit keyPressEventSent(ke);
	}

Q_SIGNALS:
	void keyPressEventSent(QKeyEvent *ke);

};

#endif /* MYLINEEDIT_H */

// End of mylineedit.h
