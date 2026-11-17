/****************************************************************************
** Meta object code from reading C++ file 'GrabWorker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../GrabWorker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GrabWorker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GrabWorker_t {
    QByteArrayData data[27];
    char stringdata0[279];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GrabWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GrabWorker_t qt_meta_stringdata_GrabWorker = {
    {
QT_MOC_LITERAL(0, 0, 10), // "GrabWorker"
QT_MOC_LITERAL(1, 11, 6), // "opened"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 10), // "cameraInfo"
QT_MOC_LITERAL(4, 30, 6), // "closed"
QT_MOC_LITERAL(5, 37, 15), // "grabbingChanged"
QT_MOC_LITERAL(6, 53, 8), // "grabbing"
QT_MOC_LITERAL(7, 62, 10), // "frameReady"
QT_MOC_LITERAL(8, 73, 5), // "frame"
QT_MOC_LITERAL(9, 79, 10), // "fpsChanged"
QT_MOC_LITERAL(10, 90, 3), // "fps"
QT_MOC_LITERAL(11, 94, 15), // "exposureApplied"
QT_MOC_LITERAL(12, 110, 2), // "us"
QT_MOC_LITERAL(13, 113, 11), // "gainApplied"
QT_MOC_LITERAL(14, 125, 2), // "db"
QT_MOC_LITERAL(15, 128, 13), // "errorOccurred"
QT_MOC_LITERAL(16, 142, 7), // "message"
QT_MOC_LITERAL(17, 150, 10), // "openCamera"
QT_MOC_LITERAL(18, 161, 11), // "closeCamera"
QT_MOC_LITERAL(19, 173, 13), // "startGrabbing"
QT_MOC_LITERAL(20, 187, 12), // "stopGrabbing"
QT_MOC_LITERAL(21, 200, 13), // "setExposureUs"
QT_MOC_LITERAL(22, 214, 9), // "setGainDb"
QT_MOC_LITERAL(23, 224, 14), // "setTriggerMode"
QT_MOC_LITERAL(24, 239, 11), // "softTrigger"
QT_MOC_LITERAL(25, 251, 22), // "executeSoftwareTrigger"
QT_MOC_LITERAL(26, 274, 4) // "pump"

    },
    "GrabWorker\0opened\0\0cameraInfo\0closed\0"
    "grabbingChanged\0grabbing\0frameReady\0"
    "frame\0fpsChanged\0fps\0exposureApplied\0"
    "us\0gainApplied\0db\0errorOccurred\0message\0"
    "openCamera\0closeCamera\0startGrabbing\0"
    "stopGrabbing\0setExposureUs\0setGainDb\0"
    "setTriggerMode\0softTrigger\0"
    "executeSoftwareTrigger\0pump"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GrabWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   99,    2, 0x06 /* Public */,
       4,    0,  102,    2, 0x06 /* Public */,
       5,    1,  103,    2, 0x06 /* Public */,
       7,    1,  106,    2, 0x06 /* Public */,
       9,    1,  109,    2, 0x06 /* Public */,
      11,    1,  112,    2, 0x06 /* Public */,
      13,    1,  115,    2, 0x06 /* Public */,
      15,    1,  118,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,  121,    2, 0x0a /* Public */,
      18,    0,  122,    2, 0x0a /* Public */,
      19,    0,  123,    2, 0x0a /* Public */,
      20,    0,  124,    2, 0x0a /* Public */,
      21,    1,  125,    2, 0x0a /* Public */,
      22,    1,  128,    2, 0x0a /* Public */,
      23,    1,  131,    2, 0x0a /* Public */,
      25,    0,  134,    2, 0x0a /* Public */,
      26,    0,  135,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QImage,    8,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::QString,   16,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GrabWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GrabWorker *_t = static_cast<GrabWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->opened((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->closed(); break;
        case 2: _t->grabbingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->frameReady((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 4: _t->fpsChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->exposureApplied((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->gainApplied((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->openCamera(); break;
        case 9: _t->closeCamera(); break;
        case 10: _t->startGrabbing(); break;
        case 11: _t->stopGrabbing(); break;
        case 12: _t->setExposureUs((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 13: _t->setGainDb((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->setTriggerMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 15: _t->executeSoftwareTrigger(); break;
        case 16: _t->pump(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GrabWorker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::opened)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::closed)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::grabbingChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(const QImage & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::frameReady)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::fpsChanged)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::exposureApplied)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::gainApplied)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (GrabWorker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GrabWorker::errorOccurred)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject GrabWorker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GrabWorker.data,
      qt_meta_data_GrabWorker,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GrabWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GrabWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GrabWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GrabWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void GrabWorker::opened(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GrabWorker::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GrabWorker::grabbingChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GrabWorker::frameReady(const QImage & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GrabWorker::fpsChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GrabWorker::exposureApplied(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void GrabWorker::gainApplied(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void GrabWorker::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
