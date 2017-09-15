/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#ifndef QHELPSEARCHINDEXWRITERCLUCENE_H
#define QHELPSEARCHINDEXWRITERCLUCENE_H

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

#include "qhelpenginecore.h"
#include "private/qanalyzer_p.h"

#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QWaitCondition>

QT_BEGIN_NAMESPACE

class QCLuceneIndexWriter;

namespace fulltextsearch {
namespace clucene {

class QHelpSearchIndexWriter : public QThread
{
    Q_OBJECT

public:
    QHelpSearchIndexWriter();
    ~QHelpSearchIndexWriter();

    void cancelIndexing();
    void updateIndex(const QString &collectionFile,
        const QString &indexFilesFolder, bool reindex);
    void optimizeIndex();

signals:
    void indexingStarted();
    void indexingFinished();

private:
    void run();

    bool addDocuments(const QList<QUrl> docFiles, const QHelpEngineCore &engine,
        const QStringList &attributes, const QString &namespaceName,
        QCLuceneIndexWriter *writer, QCLuceneAnalyzer &analyzer);
    void removeDocuments(const QString &indexPath, const QString &namespaceName);

    bool writeIndexMap(QHelpEngineCore& engine,
        const QMap<QString, QDateTime>& indexMap);

    QList<QUrl> indexableFiles(QHelpEngineCore *helpEngine,
        const QString &namespaceName, const QStringList &attributes) const;

    void closeIndexWriter(QCLuceneIndexWriter *writer);

private:
    QMutex mutex;
    QWaitCondition waitCondition;

    bool m_cancel;
    bool m_reindex;
    QString m_collectionFile;
    QString m_indexFilesFolder;
};

}   // namespace clucene
}   // namespace fulltextsearch


QT_END_NAMESPACE

#endif  // QHELPSEARCHINDEXWRITERCLUCENE_H
