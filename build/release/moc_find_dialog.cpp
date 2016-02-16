/****************************************************************************
** Meta object code from reading C++ file 'find_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/find_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'find_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FindDialog_t {
    QByteArrayData data[15];
    char stringdata[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_FindDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_FindDialog_t qt_meta_stringdata_FindDialog = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 17),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 9),
QT_MOC_LITERAL(4, 40, 18),
QT_MOC_LITERAL(5, 59, 8),
QT_MOC_LITERAL(6, 68, 12),
QT_MOC_LITERAL(7, 81, 6),
QT_MOC_LITERAL(8, 88, 12),
QT_MOC_LITERAL(9, 101, 15),
QT_MOC_LITERAL(10, 117, 4),
QT_MOC_LITERAL(11, 122, 11),
QT_MOC_LITERAL(12, 134, 4),
QT_MOC_LITERAL(13, 139, 7),
QT_MOC_LITERAL(14, 147, 10)
    },
    "FindDialog\0findNextAvailable\0\0available\0"
    "replaceAllComplete\0findNext\0findPrevious\0"
    "reject\0showFindMode\0showReplaceMode\0"
    "find\0findChanged\0text\0replace\0replaceAll\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FindDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06,
       4,    0,   72,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   73,    2, 0x0a,
       6,    0,   74,    2, 0x0a,
       7,    0,   75,    2, 0x0a,
       8,    0,   76,    2, 0x0a,
       9,    0,   77,    2, 0x0a,
      10,    0,   78,    2, 0x08,
      11,    1,   79,    2, 0x08,
      13,    0,   82,    2, 0x08,
      14,    0,   83,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FindDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FindDialog *_t = static_cast<FindDialog *>(_o);
        switch (_id) {
        case 0: _t->findNextAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->replaceAllComplete(); break;
        case 2: _t->findNext(); break;
        case 3: _t->findPrevious(); break;
        case 4: _t->reject(); break;
        case 5: _t->showFindMode(); break;
        case 6: _t->showReplaceMode(); break;
        case 7: _t->find(); break;
        case 8: _t->findChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->replace(); break;
        case 10: _t->replaceAll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FindDialog::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FindDialog::findNextAvailable)) {
                *result = 0;
            }
        }
        {
            typedef void (FindDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FindDialog::replaceAllComplete)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject FindDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FindDialog.data,
      qt_meta_data_FindDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *FindDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FindDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FindDialog.stringdata))
        return static_cast<void*>(const_cast< FindDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int FindDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void FindDialog::findNextAvailable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FindDialog::replaceAllComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
