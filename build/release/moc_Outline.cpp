/****************************************************************************
** Meta object code from reading C++ file 'Outline.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Outline.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Outline.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Outline_t {
    QByteArrayData data[17];
    char stringdata[270];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_Outline_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_Outline_t qt_meta_stringdata_Outline = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 25),
QT_MOC_LITERAL(2, 34, 0),
QT_MOC_LITERAL(3, 35, 8),
QT_MOC_LITERAL(4, 44, 22),
QT_MOC_LITERAL(5, 67, 21),
QT_MOC_LITERAL(6, 89, 30),
QT_MOC_LITERAL(7, 120, 13),
QT_MOC_LITERAL(8, 134, 12),
QT_MOC_LITERAL(9, 147, 10),
QT_MOC_LITERAL(10, 158, 24),
QT_MOC_LITERAL(11, 183, 5),
QT_MOC_LITERAL(12, 189, 7),
QT_MOC_LITERAL(13, 197, 24),
QT_MOC_LITERAL(14, 222, 24),
QT_MOC_LITERAL(15, 247, 16),
QT_MOC_LITERAL(16, 264, 4)
    },
    "Outline\0documentPositionNavigated\0\0"
    "position\0headingNumberNavigated\0"
    "headingSequenceNumber\0"
    "updateCurrentNavigationHeading\0"
    "onTextChanged\0charsRemoved\0charsAdded\0"
    "insertHeadingIntoOutline\0level\0heading\0"
    "removeHeadingFromOutline\0"
    "onOutlineHeadingSelected\0QListWidgetItem*\0"
    "item\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Outline[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06,
       4,    1,   52,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       6,    1,   55,    2, 0x0a,
       7,    3,   58,    2, 0x0a,
      10,    3,   65,    2, 0x0a,
      13,    1,   72,    2, 0x0a,
      14,    1,   75,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,    3,   11,   12,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 15,   16,

       0        // eod
};

void Outline::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Outline *_t = static_cast<Outline *>(_o);
        switch (_id) {
        case 0: _t->documentPositionNavigated((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 1: _t->headingNumberNavigated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->updateCurrentNavigationHeading((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onTextChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->insertHeadingIntoOutline((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 5: _t->removeHeadingFromOutline((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->onOutlineHeadingSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Outline::*_t)(const int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Outline::documentPositionNavigated)) {
                *result = 0;
            }
        }
        {
            typedef void (Outline::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Outline::headingNumberNavigated)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Outline::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_Outline.data,
      qt_meta_data_Outline,  qt_static_metacall, 0, 0}
};


const QMetaObject *Outline::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Outline::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Outline.stringdata))
        return static_cast<void*>(const_cast< Outline*>(this));
    return QListWidget::qt_metacast(_clname);
}

int Outline::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Outline::documentPositionNavigated(const int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Outline::headingNumberNavigated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
