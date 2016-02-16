/****************************************************************************
** Meta object code from reading C++ file 'MarkdownHighlighter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/MarkdownHighlighter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MarkdownHighlighter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MarkdownHighlighter_t {
    QByteArrayData data[9];
    char stringdata[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MarkdownHighlighter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MarkdownHighlighter_t qt_meta_stringdata_MarkdownHighlighter = {
    {
QT_MOC_LITERAL(0, 0, 19),
QT_MOC_LITERAL(1, 20, 12),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 8),
QT_MOC_LITERAL(4, 43, 5),
QT_MOC_LITERAL(5, 49, 7),
QT_MOC_LITERAL(6, 57, 14),
QT_MOC_LITERAL(7, 72, 24),
QT_MOC_LITERAL(8, 97, 26)
    },
    "MarkdownHighlighter\0headingFound\0\0"
    "position\0level\0heading\0headingRemoved\0"
    "highlightBlockAtPosition\0"
    "onHighlightBlockAtPosition\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MarkdownHighlighter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x06,
       6,    1,   41,    2, 0x06,
       7,    1,   44,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       8,    1,   47,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void MarkdownHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MarkdownHighlighter *_t = static_cast<MarkdownHighlighter *>(_o);
        switch (_id) {
        case 0: _t->headingFound((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 1: _t->headingRemoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->highlightBlockAtPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onHighlightBlockAtPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MarkdownHighlighter::*_t)(int , int , const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MarkdownHighlighter::headingFound)) {
                *result = 0;
            }
        }
        {
            typedef void (MarkdownHighlighter::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MarkdownHighlighter::headingRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (MarkdownHighlighter::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MarkdownHighlighter::highlightBlockAtPosition)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject MarkdownHighlighter::staticMetaObject = {
    { &QSyntaxHighlighter::staticMetaObject, qt_meta_stringdata_MarkdownHighlighter.data,
      qt_meta_data_MarkdownHighlighter,  qt_static_metacall, 0, 0}
};


const QMetaObject *MarkdownHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MarkdownHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MarkdownHighlighter.stringdata))
        return static_cast<void*>(const_cast< MarkdownHighlighter*>(this));
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int MarkdownHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void MarkdownHighlighter::headingFound(int _t1, int _t2, const QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MarkdownHighlighter::headingRemoved(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MarkdownHighlighter::highlightBlockAtPosition(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
