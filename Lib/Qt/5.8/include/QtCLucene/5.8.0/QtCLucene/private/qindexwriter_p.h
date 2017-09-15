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

#ifndef QINDEXWRITER_P_H
#define QINDEXWRITER_P_H

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

#include "qanalyzer_p.h"
#include "qdocument_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(index)
    class IndexWriter;
CL_NS_END
CL_NS_USE(index)

QT_BEGIN_NAMESPACE

class QCLuceneIndexReader;

class Q_CLUCENE_EXPORT QCLuceneIndexWriterPrivate : public QSharedData
{
public:
    QCLuceneIndexWriterPrivate();
    QCLuceneIndexWriterPrivate(const QCLuceneIndexWriterPrivate &other);

    ~QCLuceneIndexWriterPrivate();

    IndexWriter *writer;
    bool deleteCLuceneIndexWriter;

private:
    QCLuceneIndexWriterPrivate &operator=(const QCLuceneIndexWriterPrivate &other);
};

class Q_CLUCENE_EXPORT QCLuceneIndexWriter
{
public:
    enum {
        DEFAULT_MERGE_FACTOR = 10,
        COMMIT_LOCK_TIMEOUT = 10000,
        DEFAULT_MAX_BUFFERED_DOCS = 10,
        DEFAULT_MAX_FIELD_LENGTH = 10000,
        DEFAULT_TERM_INDEX_INTERVAL = 128,
        DEFAULT_MAX_MERGE_DOCS = 0x7FFFFFFFL
    };

    QCLuceneIndexWriter(const QString &path, QCLuceneAnalyzer &analyzer,
    bool create, bool closeDir = true);
    virtual ~QCLuceneIndexWriter();

    void close();
    void optimize();
    qint32 docCount();
    QCLuceneAnalyzer getAnalyzer();

    void addIndexes(const QList<QCLuceneIndexReader*> &readers);
    void addDocument(QCLuceneDocument &doc, QCLuceneAnalyzer &analyzer);

    qint32 getMaxFieldLength() const;
    void setMaxFieldLength(qint32 value);

    qint32 getMaxBufferedDocs() const;
    void setMaxBufferedDocs(qint32 value);

    qint64 getWriteLockTimeout() const;
    void setWriteLockTimeout(qint64 writeLockTimeout);

    qint64 getCommitLockTimeout() const;
    void setCommitLockTimeout(qint64 commitLockTimeout);

    qint32 getMergeFactor() const;
    void setMergeFactor(qint32 value);

    qint32 getTermIndexInterval() const;
    void setTermIndexInterval(qint32 interval);

    qint32 getMinMergeDocs() const;
    void setMinMergeDocs(qint32 value);

    qint32 getMaxMergeDocs() const;
    void setMaxMergeDocs(qint32 value);

    bool getUseCompoundFile() const;
    void setUseCompoundFile(bool value);

protected:
    QSharedDataPointer<QCLuceneIndexWriterPrivate> d;

private:
    QCLuceneAnalyzer analyzer;
};

QT_END_NAMESPACE

#endif  // QINDEXWRITER_P_H
