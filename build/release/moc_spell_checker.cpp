/****************************************************************************
** Meta object code from reading C++ file 'spell_checker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/spelling/spell_checker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spell_checker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SpellChecker_t {
    QByteArrayData data[11];
    char stringdata[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SpellChecker_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SpellChecker_t qt_meta_stringdata_SpellChecker = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 6),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 17),
QT_MOC_LITERAL(4, 39, 16),
QT_MOC_LITERAL(5, 56, 10),
QT_MOC_LITERAL(6, 67, 3),
QT_MOC_LITERAL(7, 71, 6),
QT_MOC_LITERAL(8, 78, 9),
QT_MOC_LITERAL(9, 88, 6),
QT_MOC_LITERAL(10, 95, 9)
    },
    "SpellChecker\0reject\0\0suggestionChanged\0"
    "QListWidgetItem*\0suggestion\0add\0ignore\0"
    "ignoreAll\0change\0changeAll\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpellChecker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a,
       3,    1,   50,    2, 0x08,
       6,    0,   53,    2, 0x08,
       7,    0,   54,    2, 0x08,
       8,    0,   55,    2, 0x08,
       9,    0,   56,    2, 0x08,
      10,    0,   57,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SpellChecker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SpellChecker *_t = static_cast<SpellChecker *>(_o);
        switch (_id) {
        case 0: _t->reject(); break;
        case 1: _t->suggestionChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: _t->add(); break;
        case 3: _t->ignore(); break;
        case 4: _t->ignoreAll(); break;
        case 5: _t->change(); break;
        case 6: _t->changeAll(); break;
        default: ;
        }
    }
}

const QMetaObject SpellChecker::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SpellChecker.data,
      qt_meta_data_SpellChecker,  qt_static_metacall, 0, 0}
};


const QMetaObject *SpellChecker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpellChecker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpellChecker.stringdata))
        return static_cast<void*>(const_cast< SpellChecker*>(this));
    return QDialog::qt_metacast(_clname);
}

int SpellChecker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
