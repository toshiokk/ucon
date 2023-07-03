/****************************************************************************
** imjqtui meta object code from reading C++ file 'imjqtui.h'
**
** Created: Sun Jan 31 18:30:34 2010
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/imjqtui.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *imjqtui::className() const
{
    return "imjqtui";
}

QMetaObject *imjqtui::metaObj = 0;
static QMetaObjectCleanUp cleanUp_imjqtui( "imjqtui", &imjqtui::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString imjqtui::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "imjqtui", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString imjqtui::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "imjqtui", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* imjqtui::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "languageChange()", &slot_0, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"imjqtui", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_imjqtui.setMetaObject( metaObj );
    return metaObj;
}

void* imjqtui::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "imjqtui" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool imjqtui::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool imjqtui::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool imjqtui::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool imjqtui::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
