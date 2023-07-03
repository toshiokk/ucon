TEMPLATE = app
TARGET = imjqttest

CONFIG += qt warn_on release

FORMS += imjqttest.ui
FORMS += imjqt.ui

HEADERS += imjqttest.h
HEADERS += imjqt.h
HEADERS += imj.h

HEADERS += mylineedit.h
HEADERS += myminmax.h
HEADERS += mystring.h
HEADERS += mydebug.h
HEADERS += mytypes.h
HEADERS += util.h
HEADERS += utilincs.h

SOURCES += imjqttest.cpp
SOURCES += imjqt.cpp
SOURCES += imj.cpp

SOURCES += myminmax.c
SOURCES += mystring.c
SOURCES += mydebug.c
SOURCES += util.c

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
