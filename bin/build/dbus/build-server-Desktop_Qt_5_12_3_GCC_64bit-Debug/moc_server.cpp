/****************************************************************************
** Meta object code from reading C++ file 'server.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/dbus/server/server.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'server.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_dbusServerClass_t {
    QByteArrayData data[10];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_dbusServerClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_dbusServerClass_t qt_meta_stringdata_dbusServerClass = {
    {
QT_MOC_LITERAL(0, 0, 15), // "dbusServerClass"
QT_MOC_LITERAL(1, 16, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 32, 21), // "QtDBus.Service.Server"
QT_MOC_LITERAL(3, 54, 11), // "aboutToQuit"
QT_MOC_LITERAL(4, 66, 0), // ""
QT_MOC_LITERAL(5, 67, 5), // "query"
QT_MOC_LITERAL(6, 73, 12), // "QDBusVariant"
QT_MOC_LITERAL(7, 86, 4), // "quit"
QT_MOC_LITERAL(8, 91, 9), // "Q_NOREPLY"
QT_MOC_LITERAL(9, 101, 5) // "value"

    },
    "dbusServerClass\0D-Bus Interface\0"
    "QtDBus.Service.Server\0aboutToQuit\0\0"
    "query\0QDBusVariant\0quit\0Q_NOREPLY\0"
    "value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_dbusServerClass[] = {

 // content:
       8,       // revision
       0,       // classname
       1,   14, // classinfo
       3,   16, // methods
       1,   36, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // classinfo: key, value
       1,    2,

 // signals: name, argc, parameters, tag, flags
       3,    0,   31,    4, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   32,    4, 0x0a /* Public */,
       7,    0,   35,    8, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    0x80000000 | 6, QMetaType::QString,    5,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::QString, 0x00095103,

       0        // eod
};

void dbusServerClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<dbusServerClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->aboutToQuit(); break;
        case 1: { QDBusVariant _r = _t->query((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->quit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (dbusServerClass::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&dbusServerClass::aboutToQuit)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<dbusServerClass *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->value(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<dbusServerClass *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setValue(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject dbusServerClass::staticMetaObject = { {
    &QDBusAbstractAdaptor::staticMetaObject,
    qt_meta_stringdata_dbusServerClass.data,
    qt_meta_data_dbusServerClass,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *dbusServerClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *dbusServerClass::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_dbusServerClass.stringdata0))
        return static_cast<void*>(this);
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int dbusServerClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void dbusServerClass::aboutToQuit()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
