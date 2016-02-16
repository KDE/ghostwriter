/****************************************************************************
** Meta object code from reading C++ file 'StyleSheetManagerDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/StyleSheetManagerDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StyleSheetManagerDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_StyleSheetManagerDialog_t {
    QByteArrayData data[6];
    char stringdata[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_StyleSheetManagerDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_StyleSheetManagerDialog_t qt_meta_stringdata_StyleSheetManagerDialog = {
    {
QT_MOC_LITERAL(0, 0, 23),
QT_MOC_LITERAL(1, 24, 6),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 6),
QT_MOC_LITERAL(4, 39, 16),
QT_MOC_LITERAL(5, 56, 24)
    },
    "StyleSheetManagerDialog\0accept\0\0reject\0"
    "addNewStyleSheet\0removeSelectedStyleSheet\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StyleSheetManagerDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08,
       3,    0,   35,    2, 0x08,
       4,    0,   36,    2, 0x08,
       5,    0,   37,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void StyleSheetManagerDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StyleSheetManagerDialog *_t = static_cast<StyleSheetManagerDialog *>(_o);
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->reject(); break;
        case 2: _t->addNewStyleSheet(); break;
        case 3: _t->removeSelectedStyleSheet(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject StyleSheetManagerDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_StyleSheetManagerDialog.data,
      qt_meta_data_StyleSheetManagerDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *StyleSheetManagerDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StyleSheetManagerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_StyleSheetManagerDialog.stringdata))
        return static_cast<void*>(const_cast< StyleSheetManagerDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int StyleSheetManagerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
