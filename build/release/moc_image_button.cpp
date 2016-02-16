/****************************************************************************
** Meta object code from reading C++ file 'image_button.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/image_button.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'image_button.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ImageButton_t {
    QByteArrayData data[8];
    char stringdata[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ImageButton_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ImageButton_t qt_meta_stringdata_ImageButton = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 7),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 4),
QT_MOC_LITERAL(4, 26, 8),
QT_MOC_LITERAL(5, 35, 5),
QT_MOC_LITERAL(6, 41, 10),
QT_MOC_LITERAL(7, 52, 9)
    },
    "ImageButton\0changed\0\0path\0setImage\0"
    "image\0unsetImage\0onClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageButton[] = {

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
       4,    2,   37,    2, 0x0a,
       6,    0,   42,    2, 0x0a,
       7,    0,   43,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ImageButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ImageButton *_t = static_cast<ImageButton *>(_o);
        switch (_id) {
        case 0: _t->changed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setImage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->unsetImage(); break;
        case 3: _t->onClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ImageButton::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageButton::changed)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ImageButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_ImageButton.data,
      qt_meta_data_ImageButton,  qt_static_metacall, 0, 0}
};


const QMetaObject *ImageButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ImageButton.stringdata))
        return static_cast<void*>(const_cast< ImageButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int ImageButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
void ImageButton::changed(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
