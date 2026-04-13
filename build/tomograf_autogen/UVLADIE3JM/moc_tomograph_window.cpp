/****************************************************************************
** Meta object code from reading C++ file 'tomograph_window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/tomograph_window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tomograph_window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TomographWindow_t {
    QByteArrayData data[11];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TomographWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TomographWindow_t qt_meta_stringdata_TomographWindow = {
    {
QT_MOC_LITERAL(0, 0, 15), // "TomographWindow"
QT_MOC_LITERAL(1, 16, 11), // "onLoadImage"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 19), // "onParametersChanged"
QT_MOC_LITERAL(4, 49, 9), // "onCompute"
QT_MOC_LITERAL(5, 59, 15), // "onSliderChanged"
QT_MOC_LITERAL(6, 75, 5), // "value"
QT_MOC_LITERAL(7, 81, 15), // "onIterativeMode"
QT_MOC_LITERAL(8, 97, 5), // "state"
QT_MOC_LITERAL(9, 103, 11), // "onSaveDICOM"
QT_MOC_LITERAL(10, 115, 11) // "onLoadDICOM"

    },
    "TomographWindow\0onLoadImage\0\0"
    "onParametersChanged\0onCompute\0"
    "onSliderChanged\0value\0onIterativeMode\0"
    "state\0onSaveDICOM\0onLoadDICOM"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TomographWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    1,   52,    2, 0x08 /* Private */,
       7,    1,   55,    2, 0x08 /* Private */,
       9,    0,   58,    2, 0x08 /* Private */,
      10,    0,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TomographWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TomographWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onLoadImage(); break;
        case 1: _t->onParametersChanged(); break;
        case 2: _t->onCompute(); break;
        case 3: _t->onSliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onIterativeMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onSaveDICOM(); break;
        case 6: _t->onLoadDICOM(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TomographWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_TomographWindow.data,
    qt_meta_data_TomographWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TomographWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TomographWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TomographWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int TomographWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
