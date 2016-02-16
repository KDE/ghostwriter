/****************************************************************************
** Meta object code from reading C++ file 'HtmlPreview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/HtmlPreview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HtmlPreview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_HtmlPreview_t {
    QByteArrayData data[21];
    char stringdata[269];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_HtmlPreview_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_HtmlPreview_t qt_meta_stringdata_HtmlPreview = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 16),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 11),
QT_MOC_LITERAL(4, 42, 17),
QT_MOC_LITERAL(5, 60, 13),
QT_MOC_LITERAL(6, 74, 17),
QT_MOC_LITERAL(7, 92, 21),
QT_MOC_LITERAL(8, 114, 11),
QT_MOC_LITERAL(9, 126, 18),
QT_MOC_LITERAL(10, 145, 5),
QT_MOC_LITERAL(11, 151, 16),
QT_MOC_LITERAL(12, 168, 12),
QT_MOC_LITERAL(13, 181, 18),
QT_MOC_LITERAL(14, 200, 9),
QT_MOC_LITERAL(15, 210, 7),
QT_MOC_LITERAL(16, 218, 8),
QT_MOC_LITERAL(17, 227, 8),
QT_MOC_LITERAL(18, 236, 13),
QT_MOC_LITERAL(19, 250, 3),
QT_MOC_LITERAL(20, 254, 13)
    },
    "HtmlPreview\0operationStarted\0\0description\0"
    "operationFinished\0updatePreview\0"
    "navigateToHeading\0headingSequenceNumber\0"
    "onHtmlReady\0onPreviewerChanged\0index\0"
    "changeStyleSheet\0printPreview\0"
    "printHtmlToPrinter\0QPrinter*\0printer\0"
    "onExport\0copyHtml\0onLinkClicked\0url\0"
    "updateBaseDir\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HtmlPreview[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06,
       4,    0,   82,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   83,    2, 0x0a,
       6,    1,   84,    2, 0x0a,
       8,    0,   87,    2, 0x08,
       9,    1,   88,    2, 0x08,
      11,    1,   91,    2, 0x08,
      12,    0,   94,    2, 0x08,
      13,    1,   95,    2, 0x08,
      16,    0,   98,    2, 0x08,
      17,    0,   99,    2, 0x08,
      18,    1,  100,    2, 0x08,
      20,    0,  103,    2, 0x08,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,   19,
    QMetaType::Void,

       0        // eod
};

void HtmlPreview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HtmlPreview *_t = static_cast<HtmlPreview *>(_o);
        switch (_id) {
        case 0: _t->operationStarted((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->operationFinished(); break;
        case 2: _t->updatePreview(); break;
        case 3: _t->navigateToHeading((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onHtmlReady(); break;
        case 5: _t->onPreviewerChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->changeStyleSheet((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->printPreview(); break;
        case 8: _t->printHtmlToPrinter((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        case 9: _t->onExport(); break;
        case 10: _t->copyHtml(); break;
        case 11: _t->onLinkClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 12: _t->updateBaseDir(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HtmlPreview::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HtmlPreview::operationStarted)) {
                *result = 0;
            }
        }
        {
            typedef void (HtmlPreview::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HtmlPreview::operationFinished)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject HtmlPreview::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_HtmlPreview.data,
      qt_meta_data_HtmlPreview,  qt_static_metacall, 0, 0}
};


const QMetaObject *HtmlPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HtmlPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HtmlPreview.stringdata))
        return static_cast<void*>(const_cast< HtmlPreview*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int HtmlPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void HtmlPreview::operationStarted(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HtmlPreview::operationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
