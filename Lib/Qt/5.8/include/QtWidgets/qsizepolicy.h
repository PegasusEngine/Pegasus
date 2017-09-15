/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
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

#ifndef QSIZEPOLICY_H
#define QSIZEPOLICY_H

#include <QtWidgets/qtwidgetsglobal.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE


class QVariant;
class QSizePolicy;

Q_DECL_CONST_FUNCTION inline uint qHash(QSizePolicy key, uint seed = 0) Q_DECL_NOTHROW;

class Q_WIDGETS_EXPORT QSizePolicy
{
    Q_GADGET
    Q_FLAGS(ControlTypes)

public:
    enum PolicyFlag {
        GrowFlag = 1,
        ExpandFlag = 2,
        ShrinkFlag = 4,
        IgnoreFlag = 8
    };

    enum Policy {
        Fixed = 0,
        Minimum = GrowFlag,
        Maximum = ShrinkFlag,
        Preferred = GrowFlag | ShrinkFlag,
        MinimumExpanding = GrowFlag | ExpandFlag,
        Expanding = GrowFlag | ShrinkFlag | ExpandFlag,
        Ignored = ShrinkFlag | GrowFlag | IgnoreFlag
    };
    Q_ENUM(Policy)

    enum ControlType {
        DefaultType      = 0x00000001,
        ButtonBox        = 0x00000002,
        CheckBox         = 0x00000004,
        ComboBox         = 0x00000008,
        Frame            = 0x00000010,
        GroupBox         = 0x00000020,
        Label            = 0x00000040,
        Line             = 0x00000080,
        LineEdit         = 0x00000100,
        PushButton       = 0x00000200,
        RadioButton      = 0x00000400,
        Slider           = 0x00000800,
        SpinBox          = 0x00001000,
        TabWidget        = 0x00002000,
        ToolButton       = 0x00004000
    };
    Q_DECLARE_FLAGS(ControlTypes, ControlType)

    QSizePolicy() : data(0) { }

    QSizePolicy(Policy horizontal, Policy vertical, ControlType type = DefaultType)
        : data(0) {
        bits.horPolicy = horizontal;
        bits.verPolicy = vertical;
        setControlType(type);
    }
    Policy horizontalPolicy() const { return static_cast<Policy>(bits.horPolicy); }
    Policy verticalPolicy() const { return static_cast<Policy>(bits.verPolicy); }
    ControlType controlType() const;

    void setHorizontalPolicy(Policy d) { bits.horPolicy = d; }
    void setVerticalPolicy(Policy d) { bits.verPolicy = d; }
    void setControlType(ControlType type);

    Qt::Orientations expandingDirections() const {
        return ( (verticalPolicy()   & ExpandFlag) ? Qt::Vertical   : Qt::Orientations() )
             | ( (horizontalPolicy() & ExpandFlag) ? Qt::Horizontal : Qt::Orientations() ) ;
    }

    void setHeightForWidth(bool b) { bits.hfw = b;  }
    bool hasHeightForWidth() const { return bits.hfw; }
    void setWidthForHeight(bool b) { bits.wfh = b;  }
    bool hasWidthForHeight() const { return bits.wfh; }

    bool operator==(const QSizePolicy& s) const { return data == s.data; }
    bool operator!=(const QSizePolicy& s) const { return data != s.data; }

    friend Q_DECL_CONST_FUNCTION uint qHash(QSizePolicy key, uint seed) Q_DECL_NOTHROW { return qHash(key.data, seed); }

    operator QVariant() const;

    int horizontalStretch() const { return static_cast<int>(bits.horStretch); }
    int verticalStretch() const { return static_cast<int>(bits.verStretch); }
    void setHorizontalStretch(int stretchFactor) { bits.horStretch = static_cast<quint32>(qBound(0, stretchFactor, 255)); }
    void setVerticalStretch(int stretchFactor) { bits.verStretch = static_cast<quint32>(qBound(0, stretchFactor, 255)); }

    bool retainSizeWhenHidden() const { return bits.retainSizeWhenHidden; }
    void setRetainSizeWhenHidden(bool retainSize) { bits.retainSizeWhenHidden = retainSize; }

    void transpose();


private:
#ifndef QT_NO_DATASTREAM
    friend Q_WIDGETS_EXPORT QDataStream &operator<<(QDataStream &, const QSizePolicy &);
    friend Q_WIDGETS_EXPORT QDataStream &operator>>(QDataStream &, QSizePolicy &);
#endif
    QSizePolicy(int i) : data(i) { }

    struct Bits {
        quint32 horStretch : 8;
        quint32 verStretch : 8;
        quint32 horPolicy : 4;
        quint32 verPolicy : 4;
        quint32 ctype : 5;
        quint32 hfw : 1;
        quint32 wfh : 1;
        quint32 retainSizeWhenHidden : 1;
    };
    union {
        Bits bits;
        quint32 data;
    };
};
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
// Can't add in Qt 5, as QList<QSizePolicy> would be BiC:
Q_DECLARE_TYPEINFO(QSizePolicy, Q_PRIMITIVE_TYPE);
#endif

Q_DECLARE_OPERATORS_FOR_FLAGS(QSizePolicy::ControlTypes)

#ifndef QT_NO_DATASTREAM
Q_WIDGETS_EXPORT QDataStream &operator<<(QDataStream &, const QSizePolicy &);
Q_WIDGETS_EXPORT QDataStream &operator>>(QDataStream &, QSizePolicy &);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_WIDGETS_EXPORT QDebug operator<<(QDebug dbg, const QSizePolicy &);
#endif

inline void QSizePolicy::transpose() {
    Policy hData = horizontalPolicy();
    Policy vData = verticalPolicy();
    int hStretch = horizontalStretch();
    int vStretch = verticalStretch();
    setHorizontalPolicy(vData);
    setVerticalPolicy(hData);
    setHorizontalStretch(vStretch);
    setVerticalStretch(hStretch);
}

QT_END_NAMESPACE

#endif // QSIZEPOLICY_H
