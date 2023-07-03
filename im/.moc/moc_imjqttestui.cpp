/****************************************************************************
** imjqttestui meta object code from reading C++ file 'imjqttestui.h'
**
** Created: Sun Dec 27 23:22:38 2009
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/imjqttestui.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *imjqttestui::className() const
{
    return "imjqttestui";
}

QMetaObject *imjqttestui::metaObj = 0;
static QMetaObjectCleanUp cleanUp_imjqttestui( "imjqttestui", &imjqttestui::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString imjqttestui::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "imjqttestui", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString imjqttestui::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "imjqttestui", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* imjqttestui::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "languageChange()", &slot_0, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"imjqttestui", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_imjqttestui.setMetaObject( metaObj );
    return metaObj;
}

void* imjqttestui::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "imjqttestui" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool imjqttestui::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool imjqttestui::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool imjqttestui::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool imjqttestui::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
