/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	InstanceViewer.h
//! \author	Kleber Garcia
//! \date	19 Feb 2016
//! \brief	Pegasus asset instance viewer class.

#ifndef PEGASUS_EDITOR_INSTANCE_VIEWER_H
#define PEGASUS_EDITOR_INSTANCE_VIEWER_H

#include <QObject>
#include <QPoint>
#include <QSet>
#include <QMap>
#include <QIcon>
#include "MessageControllers/AssetIOMessageController.h"

class QTreeWidget;
class QTreeWidgetItem;
class QMenu;

class InstanceViewer : public AssetTreeObserver
{
    Q_OBJECT

public:
    //!Constructor
    explicit InstanceViewer(QTreeWidget* treeWidget);
    
    //! Destructor
    virtual ~InstanceViewer();

    //! Called when an asset is notified regarding its validity
    virtual void OnSetValidity(const QString& assetPath, bool validity); 

    //! Called when there is a new tree, display this in this tree widget.
    virtual void OnInitialized(const AssetViewTree& tree); 

    //! Called when the tree of assets in the project has been closed.
    virtual void OnShutdown();

    enum FilterMode
    {
        BLOCK_FILTER,
        TYPE_FILTER,
        FILTER_COUNT
    };

    void SetFilterMode(FilterMode mode, bool clearTree = true);

    void ClearTree();

signals:
    void RequestOpenAsset(QString assetPath);

    void RequestHighlightBlock(unsigned blockId);

private slots:
    void ExecuteOpenAssetSlot();

    void ExecuteHighlightBlock();

    void PrepareContextMenu(const QPoint& point);

    void OnItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    QTreeWidgetItem* InstanceViewer::CreateAssetWidgetItem(const AssetInformation& info);

    void PopulateAsset(const AssetInformation& info, QTreeWidgetItem* parent);

    //! Callback that sets up the ui of this element.
    void SetupUi();

    QTreeWidget* mTreeWidget;
    QMenu*       mAssetInstanceCtxMenu;
    QMenu*       mCategoryCtxMenu;
    AssetViewTree mTree;

    struct {
        QString targetAsset;
    } mOpenAssetCmdParams;

    struct {
        int targetBlockGuid;
    } mHighlightBlockCmdParams;

    QList<QTreeWidgetItem*> mTreeWidgetItems[FILTER_COUNT];
    QSet<QString> mExpandedItems[FILTER_COUNT];
    QMap<QString, QSet<QTreeWidgetItem*>> mAssetViews;
    FilterMode mFilterMode;

    QIcon mValidIcon;
    QIcon mInvalidIcon;
};

#endif
