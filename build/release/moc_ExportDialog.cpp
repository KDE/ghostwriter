/****************************************************************************
** Meta object code from reading C++ file 'ExportDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ExportDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExportDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ExportDialog_t {
    QByteArrayData data[11];
    char stringdata[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ExportDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ExportDialog_t qt_meta_stringdata_ExportDialog = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 13),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 11),
QT_MOC_LITERAL(4, 40, 14),
QT_MOC_LITERAL(5, 55, 6),
QT_MOC_LITERAL(6, 62, 6),
QT_MOC_LITERAL(7, 69, 17),
QT_MOC_LITERAL(8, 87, 5),
QT_MOC_LITERAL(9, 93, 16),
QT_MOC_LITERAL(10, 110, 6)
    },
    "ExportDialog\0exportStarted\0\0description\0"
    "exportComplete\0accept\0reject\0"
    "onExporterChanged\0index\0onFilterSelected\0"
    "filter\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ExportDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06,
       4,    0,   47,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   48,    2, 0x08,
       6,    0,   49,    2, 0x08,
       7,    1,   50,    2, 0x08,
       9,    1,   53,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void ExportDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ExportDialog *_t = static_cast<ExportDialog *>(_o);
        switch (_id) {
        case 0: _t->exportStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->exportComplete(); break;
        case 2: _t->accept(); break;
        case 3: _t->reject(); break;
        case 4: _t->onExporterChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onFilterSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ExportDialog::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExportDialog::exportStarted)) {
                *result = 0;
            }
        }
        {
            typedef void (ExportDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ExportDialog::exportComplete)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ExportDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ExportDialog.data,
      qt_meta_data_ExportDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *ExportDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ExportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExportDialog.stringdata))
        return static_cast<void*>(const_cast< ExportDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ExportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ExportDialog::exportStarted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ExportDialog::exportComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
