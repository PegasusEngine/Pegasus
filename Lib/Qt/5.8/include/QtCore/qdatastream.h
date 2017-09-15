/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDATASTREAM_H
#define QDATASTREAM_H

#include <QtCore/qscopedpointer.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qpair.h>

#ifdef Status
#error qdatastream.h must be included before any header file that defines Status
#endif

QT_BEGIN_NAMESPACE


class QByteArray;
class QIODevice;

template <typename T> class QList;
template <typename T> class QLinkedList;
template <typename T> class QVector;
template <typename T> class QSet;
template <class Key, class T> class QHash;
template <class Key, class T> class QMap;

#if !defined(QT_NO_DATASTREAM) || defined(QT_BOOTSTRAPPED)
class QDataStreamPrivate;
namespace QtPrivate {
class StreamStateSaver;
}
class Q_CORE_EXPORT QDataStream
{
public:
    enum Version {
        Qt_1_0 = 1,
        Qt_2_0 = 2,
        Qt_2_1 = 3,
        Qt_3_0 = 4,
        Qt_3_1 = 5,
        Qt_3_3 = 6,
        Qt_4_0 = 7,
        Qt_4_1 = Qt_4_0,
        Qt_4_2 = 8,
        Qt_4_3 = 9,
        Qt_4_4 = 10,
        Qt_4_5 = 11,
        Qt_4_6 = 12,
        Qt_4_7 = Qt_4_6,
        Qt_4_8 = Qt_4_7,
        Qt_4_9 = Qt_4_8,
        Qt_5_0 = 13,
        Qt_5_1 = 14,
        Qt_5_2 = 15,
        Qt_5_3 = Qt_5_2,
        Qt_5_4 = 16,
        Qt_5_5 = Qt_5_4,
        Qt_5_6 = 17,
        Qt_5_7 = Qt_5_6,
        Qt_5_8 = Qt_5_7,
#if QT_VERSION >= 0x050900
#error Add the datastream version for this Qt version and update Qt_DefaultCompiledVersion
#endif
        Qt_DefaultCompiledVersion = Qt_5_8
    };

    enum ByteOrder {
        BigEndian = QSysInfo::BigEndian,
        LittleEndian = QSysInfo::LittleEndian
    };

    enum Status {
        Ok,
        ReadPastEnd,
        ReadCorruptData,
        WriteFailed
    };

    enum FloatingPointPrecision {
        SinglePrecision,
        DoublePrecision
    };

    QDataStream();
    explicit QDataStream(QIODevice *);
    QDataStream(QByteArray *, QIODevice::OpenMode flags);
    QDataStream(const QByteArray &);
    ~QDataStream();

    QIODevice *device() const;
    void setDevice(QIODevice *);
    void unsetDevice();

    bool atEnd() const;

    Status status() const;
    void setStatus(Status status);
    void resetStatus();

    FloatingPointPrecision floatingPointPrecision() const;
    void setFloatingPointPrecision(FloatingPointPrecision precision);

    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder);

    int version() const;
    void setVersion(int);

    QDataStream &operator>>(qint8 &i);
    QDataStream &operator>>(quint8 &i);
    QDataStream &operator>>(qint16 &i);
    QDataStream &operator>>(quint16 &i);
    QDataStream &operator>>(qint32 &i);
    QDataStream &operator>>(quint32 &i);
    QDataStream &operator>>(qint64 &i);
    QDataStream &operator>>(quint64 &i);

    QDataStream &operator>>(bool &i);
    QDataStream &operator>>(float &f);
    QDataStream &operator>>(double &f);
    QDataStream &operator>>(char *&str);

    QDataStream &operator<<(qint8 i);
    QDataStream &operator<<(quint8 i);
    QDataStream &operator<<(qint16 i);
    QDataStream &operator<<(quint16 i);
    QDataStream &operator<<(qint32 i);
    QDataStream &operator<<(quint32 i);
    QDataStream &operator<<(qint64 i);
    QDataStream &operator<<(quint64 i);
    QDataStream &operator<<(bool i);
    QDataStream &operator<<(float f);
    QDataStream &operator<<(double f);
    QDataStream &operator<<(const char *str);

    QDataStream &readBytes(char *&, uint &len);
    int readRawData(char *, int len);

    QDataStream &writeBytes(const char *, uint len);
    int writeRawData(const char *, int len);

    int skipRawData(int len);

    void startTransaction();
    bool commitTransaction();
    void rollbackTransaction();
    void abortTransaction();

private:
    Q_DISABLE_COPY(QDataStream)

    QScopedPointer<QDataStreamPrivate> d;

    QIODevice *dev;
    bool owndev;
    bool noswap;
    ByteOrder byteorder;
    int ver;
    Status q_status;

    int readBlock(char *data, int len);
    friend class QtPrivate::StreamStateSaver;
};

namespace QtPrivate {

class StreamStateSaver
{
public:
    inline StreamStateSaver(QDataStream *s) : stream(s), oldStatus(s->status())
    {
        if (!stream->dev || !stream->dev->isTransactionStarted())
            stream->resetStatus();
    }
    inline ~StreamStateSaver()
    {
        if (oldStatus != QDataStream::Ok) {
            stream->resetStatus();
            stream->setStatus(oldStatus);
        }
    }

private:
    QDataStream *stream;
    QDataStream::Status oldStatus;
};

} // QtPrivate namespace

/*****************************************************************************
  QDataStream inline functions
 *****************************************************************************/

inline QIODevice *QDataStream::device() const
{ return dev; }

inline QDataStream::ByteOrder QDataStream::byteOrder() const
{ return byteorder; }

inline int QDataStream::version() const
{ return ver; }

inline void QDataStream::setVersion(int v)
{ ver = v; }

inline QDataStream &QDataStream::operator>>(quint8 &i)
{ return *this >> reinterpret_cast<qint8&>(i); }

inline QDataStream &QDataStream::operator>>(quint16 &i)
{ return *this >> reinterpret_cast<qint16&>(i); }

inline QDataStream &QDataStream::operator>>(quint32 &i)
{ return *this >> reinterpret_cast<qint32&>(i); }

inline QDataStream &QDataStream::operator>>(quint64 &i)
{ return *this >> reinterpret_cast<qint64&>(i); }

inline QDataStream &QDataStream::operator<<(quint8 i)
{ return *this << qint8(i); }

inline QDataStream &QDataStream::operator<<(quint16 i)
{ return *this << qint16(i); }

inline QDataStream &QDataStream::operator<<(quint32 i)
{ return *this << qint32(i); }

inline QDataStream &QDataStream::operator<<(quint64 i)
{ return *this << qint64(i); }

template <typename T>
QDataStream& operator>>(QDataStream& s, QList<T>& l)
{
    QtPrivate::StreamStateSaver stateSaver(&s);

    l.clear();
    quint32 c;
    s >> c;
    l.reserve(c);
    for(quint32 i = 0; i < c; ++i)
    {
        T t;
        s >> t;
        if (s.status() != QDataStream::Ok) {
            l.clear();
            break;
        }
        l.append(t);
    }

    return s;
}

template <typename T>
QDataStream& operator<<(QDataStream& s, const QList<T>& l)
{
    s << quint32(l.size());
    for (int i = 0; i < l.size(); ++i)
        s << l.at(i);
    return s;
}

template <typename T>
QDataStream& operator>>(QDataStream& s, QLinkedList<T>& l)
{
    QtPrivate::StreamStateSaver stateSaver(&s);

    l.clear();
    quint32 c;
    s >> c;
    for(quint32 i = 0; i < c; ++i)
    {
        T t;
        s >> t;
        if (s.status() != QDataStream::Ok) {
            l.clear();
            break;
        }
        l.append(t);
    }

    return s;
}

template <typename T>
QDataStream& operator<<(QDataStream& s, const QLinkedList<T>& l)
{
    s << quint32(l.size());
    typename QLinkedList<T>::ConstIterator it = l.constBegin();
    for(; it != l.constEnd(); ++it)
        s << *it;
    return s;
}

template<typename T>
QDataStream& operator>>(QDataStream& s, QVector<T>& v)
{
    QtPrivate::StreamStateSaver stateSaver(&s);

    v.clear();
    quint32 c;
    s >> c;
    v.resize(c);
    for(quint32 i = 0; i < c; ++i) {
        T t;
        s >> t;
        if (s.status() != QDataStream::Ok) {
            v.clear();
            break;
        }
        v[i] = t;
    }

    return s;
}

template<typename T>
QDataStream& operator<<(QDataStream& s, const QVector<T>& v)
{
    s << quint32(v.size());
    for (typename QVector<T>::const_iterator it = v.begin(); it != v.end(); ++it)
        s << *it;
    return s;
}

template <typename T>
QDataStream &operator>>(QDataStream &in, QSet<T> &set)
{
    QtPrivate::StreamStateSaver stateSaver(&in);

    set.clear();
    quint32 c;
    in >> c;
    for (quint32 i = 0; i < c; ++i) {
        T t;
        in >> t;
        if (in.status() != QDataStream::Ok) {
            set.clear();
            break;
        }
        set << t;
    }

    return in;
}

template <typename T>
QDataStream& operator<<(QDataStream &out, const QSet<T> &set)
{
    out << quint32(set.size());
    typename QSet<T>::const_iterator i = set.constBegin();
    while (i != set.constEnd()) {
        out << *i;
        ++i;
    }
    return out;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE QDataStream &operator>>(QDataStream &in, QHash<Key, T> &hash)
{
    QtPrivate::StreamStateSaver stateSaver(&in);

    hash.clear();
    quint32 n;
    in >> n;

    for (quint32 i = 0; i < n; ++i) {
        if (in.status() != QDataStream::Ok)
            break;

        Key k;
        T t;
        in >> k >> t;
        hash.insertMulti(k, t);
    }

    if (in.status() != QDataStream::Ok)
        hash.clear();
    return in;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE QDataStream &operator<<(QDataStream &out, const QHash<Key, T>& hash)
{
    out << quint32(hash.size());
    typename QHash<Key, T>::ConstIterator it = hash.end();
    typename QHash<Key, T>::ConstIterator begin = hash.begin();
    while (it != begin) {
        --it;
        out << it.key() << it.value();
    }
    return out;
}
#ifdef Q_QDOC
template <class Key, class T>
Q_OUTOFLINE_TEMPLATE QDataStream &operator>>(QDataStream &in, QMap<Key, T> &map)
#else
template <class aKey, class aT>
Q_OUTOFLINE_TEMPLATE QDataStream &operator>>(QDataStream &in, QMap<aKey, aT> &map)
#endif
{
    QtPrivate::StreamStateSaver stateSaver(&in);

    map.clear();
    quint32 n;
    in >> n;

    map.detach();
    for (quint32 i = 0; i < n; ++i) {
        if (in.status() != QDataStream::Ok)
            break;

        aKey key;
        aT value;
        in >> key >> value;
        map.insertMulti(key, value);
    }
    if (in.status() != QDataStream::Ok)
        map.clear();
    return in;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE QDataStream &operator<<(QDataStream &out, const QMap<Key, T> &map)
{
    out << quint32(map.size());
    typename QMap<Key, T>::ConstIterator it = map.end();
    typename QMap<Key, T>::ConstIterator begin = map.begin();
    while (it != begin) {
        --it;
        out << it.key() << it.value();
    }
    return out;
}

#ifndef QT_NO_DATASTREAM
template <class T1, class T2>
inline QDataStream& operator>>(QDataStream& s, QPair<T1, T2>& p)
{
    s >> p.first >> p.second;
    return s;
}

template <class T1, class T2>
inline QDataStream& operator<<(QDataStream& s, const QPair<T1, T2>& p)
{
    s << p.first << p.second;
    return s;
}
#endif

#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE

#endif // QDATASTREAM_H
