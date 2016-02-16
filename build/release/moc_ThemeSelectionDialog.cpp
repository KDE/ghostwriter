/****************************************************************************
** Meta object code from reading C++ file 'ThemeSelectionDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ThemeSelectionDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThemeSelectionDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ThemeSelectionDialog_t {
    QByteArrayData data[10];
    char stringdata[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ThemeSelectionDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ThemeSelectionDialog_t qt_meta_stringdata_ThemeSelectionDialog = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 10),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 5),
QT_MOC_LITERAL(4, 39, 5),
QT_MOC_LITERAL(5, 45, 15),
QT_MOC_LITERAL(6, 61, 10),
QT_MOC_LITERAL(7, 72, 13),
QT_MOC_LITERAL(8, 86, 11),
QT_MOC_LITERAL(9, 98, 14)
    },
    "ThemeSelectionDialog\0applyTheme\0\0Theme\0"
    "theme\0onThemeSelected\0onNewTheme\0"
    "onDeleteTheme\0onEditTheme\0onThemeUpdated\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThemeSelectionDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   47,    2, 0x08,
       6,    0,   48,    2, 0x08,
       7,    0,   49,    2, 0x08,
       8,    0,   50,    2, 0x08,
       9,    1,   51,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void ThemeSelectionDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ThemeSelectionDialog *_t = static_cast<ThemeSelectionDialog *>(_o);
        switch (_id) {
        case 0: _t->applyTheme((*reinterpret_cast< const Theme(*)>(_a[1]))); break;
        case 1: _t->onThemeSelected(); break;
        case 2: _t->onNewTheme(); break;
        case 3: _t->onDeleteTheme(); break;
        case 4: _t->onEditTheme(); break;
        case 5: _t->onThemeUpdated((*reinterpret_cast< const Theme(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ThemeSelectionDialog::*_t)(const Theme & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ThemeSelectionDialog::applyTheme)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ThemeSelectionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ThemeSelectionDialog.data,
      qt_meta_data_ThemeSelectionDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *ThemeSelectionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThemeSelectionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ThemeSelectionDialog.stringdata))
        return static_cast<void*>(const_cast< ThemeSelectionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ThemeSelectionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ThemeSelectionDialog::applyTheme(const Theme & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
