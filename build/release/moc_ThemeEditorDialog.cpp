/****************************************************************************
** Meta object code from reading C++ file 'ThemeEditorDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ThemeEditorDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThemeEditorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ThemeEditorDialog_t {
    QByteArrayData data[8];
    char stringdata[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ThemeEditorDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ThemeEditorDialog_t qt_meta_stringdata_ThemeEditorDialog = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 12),
QT_MOC_LITERAL(2, 31, 0),
QT_MOC_LITERAL(3, 32, 5),
QT_MOC_LITERAL(4, 38, 5),
QT_MOC_LITERAL(5, 44, 6),
QT_MOC_LITERAL(6, 51, 6),
QT_MOC_LITERAL(7, 58, 5)
    },
    "ThemeEditorDialog\0themeUpdated\0\0Theme\0"
    "theme\0accept\0reject\0apply\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ThemeEditorDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x08,
       6,    0,   38,    2, 0x08,
       7,    0,   39,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ThemeEditorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ThemeEditorDialog *_t = static_cast<ThemeEditorDialog *>(_o);
        switch (_id) {
        case 0: _t->themeUpdated((*reinterpret_cast< const Theme(*)>(_a[1]))); break;
        case 1: _t->accept(); break;
        case 2: _t->reject(); break;
        case 3: _t->apply(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ThemeEditorDialog::*_t)(const Theme & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ThemeEditorDialog::themeUpdated)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ThemeEditorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ThemeEditorDialog.data,
      qt_meta_data_ThemeEditorDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *ThemeEditorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThemeEditorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ThemeEditorDialog.stringdata))
        return static_cast<void*>(const_cast< ThemeEditorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ThemeEditorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void ThemeEditorDialog::themeUpdated(const Theme & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
