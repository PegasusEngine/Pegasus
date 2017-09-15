/****************************************************************************
**
** Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team.
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtTools module of the Qt Toolkit.
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

#ifndef QINDEXREADER_P_H
#define QINDEXREADER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include "qterm_p.h"
#include "qdocument_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(index)
    class IndexReader;
CL_NS_END
CL_NS_USE(index)

QT_BEGIN_NAMESPACE

class QCLuceneIndexWriter;
class QCLuceneIndexSearcher;

class Q_CLUCENE_EXPORT QCLuceneIndexReaderPrivate : public QSharedData
{
public:
    QCLuceneIndexReaderPrivate();
    QCLuceneIndexReaderPrivate(const QCLuceneIndexReaderPrivate &other);

    ~QCLuceneIndexReaderPrivate();

    IndexReader *reader;
    bool deleteCLuceneIndexReader;

private:
    QCLuceneIndexReaderPrivate &operator=(const QCLuceneIndexReaderPrivate &other);
};

class Q_CLUCENE_EXPORT QCLuceneIndexReader
{
public:
    enum FieldOption {
        ALL = 1,
        INDEXED = 2,
        UNINDEXED = 4,
        INDEXED_WITH_TERMVECTOR = 8,
        INDEXED_NO_TERMVECTOR = 16,
        TERMVECTOR = 32,
        TERMVECTOR_WITH_POSITION = 64,
        TERMVECTOR_WITH_OFFSET = 128,
        TERMVECTOR_WITH_POSITION_OFFSET = 256
    };

    virtual ~QCLuceneIndexReader();

    static bool isLuceneFile(const QString &filename);
    static bool indexExists(const QString &directory);
    static QCLuceneIndexReader open(const QString &path);

    static void unlock(const QString &path);
    static bool isLocked(const QString &directory);

    static quint64 lastModified(const QString &directory);
    static qint64 getCurrentVersion(const QString &directory);

    void close();
    bool isCurrent();
    void undeleteAll();
    qint64 getVersion();
    void deleteDocument(qint32 docNum);
    bool hasNorms(const QString &field);
    qint32 deleteDocuments(const QCLuceneTerm &term);
    bool document(qint32 index, QCLuceneDocument &document);
    void setNorm(qint32 doc, const QString &field, qreal value);
    void setNorm(qint32 doc, const QString &field, quint8 value);

protected:
    friend class QCLuceneIndexWriter;
    friend class QCLuceneIndexSearcher;
    QSharedDataPointer<QCLuceneIndexReaderPrivate> d;

private:
    QCLuceneIndexReader();
};

QT_END_NAMESPACE

#endif  // QINDEXREADER_P_H
