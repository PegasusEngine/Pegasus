/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	InstanceViewer.cpp
//! \author	Kleber Garcia
//! \date	19 Feb 2016
//! \brief	Pegasus asset instance viewer class.

#include "AssetLibrary/InstanceViewer.h"
#include "Pegasus/PegasusAssetTypes.h"
#include <QTreeWidget>
#include <qlist.h>
#include <QIcon>
#include <QStringList>
#include <QMenu>
#include <QAction>

enum InstanceItemType
{
    CATEGORY,
    ASSET_INSTANCE
};

class AbstractInstanceItem : public QTreeWidgetItem
{
public:
    AbstractInstanceItem() : QTreeWidgetItem() {}
    virtual ~AbstractInstanceItem() {}
    virtual InstanceItemType GetType() const = 0;
};

class AssetInstanceItem : public AbstractInstanceItem
{
public:
    AssetInstanceItem() : AbstractInstanceItem() {}
    virtual ~AssetInstanceItem() {}
    virtual InstanceItemType GetType() const { return ASSET_INSTANCE; }
};

class CategoryInstanceItem : public  AbstractInstanceItem
{
public:
    CategoryInstanceItem() : AbstractInstanceItem(), mBlockGuid(0) {}
    virtual ~CategoryInstanceItem() {}
    unsigned GetBlockGuid() const { return mBlockGuid; }
    void SetBlockGuid(unsigned blockGuid) { mBlockGuid = blockGuid; }
    virtual InstanceItemType GetType() const { return CATEGORY; }

private:
    unsigned mBlockGuid;
};

InstanceViewer::InstanceViewer(QTreeWidget* treeWidget)
: mTreeWidget(treeWidget)
{
    QStringList labels;
    labels.append("Name");
    labels.append("Type");  
    labels.append("Status");  
    mTreeWidget->setHeaderLabels(labels);
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTreeWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(PrepareContextMenu(const QPoint)));
    connect(mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));

    mAssetInstanceCtxMenu = new QMenu(treeWidget);
    mCategoryCtxMenu = new QMenu(treeWidget);
    
    //Context menu for asset instance.
    QAction* editAction = new QAction(tr("Edit Asset"), this);
    connect(editAction, SIGNAL(triggered()), this, SLOT(ExecuteOpenAssetSlot()));
    mAssetInstanceCtxMenu->addAction(editAction);

    //Context menu for category.
    QAction* highlightAction = new QAction(tr("Highlight Block"), this);
    connect(highlightAction, SIGNAL(triggered()), this, SLOT(ExecuteHighlightBlock()));
    mCategoryCtxMenu->addAction(highlightAction);

    mFilterMode = InstanceViewer::BLOCK_FILTER;

    mValidIcon = QIcon(":AssetLibrary/success.png");
    mInvalidIcon = QIcon(":AssetLibrary/error.png");
    
}
    
InstanceViewer::~InstanceViewer()
{
    delete mAssetInstanceCtxMenu;
    delete mCategoryCtxMenu;
}

void ExpandPendingItems(QTreeWidgetItem* item, const QSet<QString>& expandedItems)
{
    if (expandedItems.find(item->text(0)) != expandedItems.end())
    {
        item->setExpanded(true);
        for (int i = 0; i < item->childCount(); ++i)
        {
            ExpandPendingItems(item->child(i), expandedItems);
        }
    }
}

void InstanceViewer::SetFilterMode(InstanceViewer::FilterMode filterMode, bool clearTree)
{
    if (clearTree)
    {
        ClearTree();
    }
    
    mFilterMode = filterMode;

    mTreeWidget->insertTopLevelItems(0,mTreeWidgetItems[static_cast<unsigned>(filterMode)]);
    QSet<QString>& expandedItems = mExpandedItems[static_cast<unsigned>(filterMode)];
    for (int i = 0; i < mTreeWidget->topLevelItemCount(); ++i)
    {
        ExpandPendingItems(mTreeWidget->topLevelItem(i), expandedItems);
    }
    
    mTreeWidget->resizeColumnToContents(0);
    mTreeWidget->resizeColumnToContents(1);
}

void ExtractExpandedItems(QSet<QString>& expandedItems, QTreeWidgetItem* rootItem)
{
    for (int i = 0; i < rootItem->childCount(); ++i)
    {
        if (rootItem->child(i)->isExpanded())
        {
            expandedItems.insert(rootItem->child(i)->text(0));
            ExtractExpandedItems(expandedItems, rootItem->child(i));
        }
    }
}

void InstanceViewer::ClearTree()
{
    QSet<QString>& expandedItems = mExpandedItems[static_cast<unsigned>(mFilterMode)]; 
    expandedItems.clear();
    unsigned topLevelCount = mTreeWidget->topLevelItemCount();
    for (unsigned i = 0; i < topLevelCount; ++i) 
    {
        QTreeWidgetItem* item = mTreeWidget->topLevelItem(i); 
        if (item->isExpanded())
        {
            expandedItems.insert(item->text(0));
            ExtractExpandedItems(expandedItems, item);
        }
    }

    for (unsigned i = 0; i < topLevelCount; ++i) 
    {
        mTreeWidget->takeTopLevelItem(0);
    }
}

void InstanceViewer::OnInitialized(const AssetViewTree& tree)
{
    mTree = tree;
    mAssetViews.clear();
    ClearTree();

    for (unsigned itFilterMode = 0; itFilterMode< static_cast<unsigned>(InstanceViewer::FILTER_COUNT); ++itFilterMode)
    {
        InstanceViewer::FilterMode filterMode = static_cast<InstanceViewer::FilterMode>(itFilterMode);
        QList<QTreeWidgetItem*>& categoryItems = mTreeWidgetItems[itFilterMode];        
        for (int i = 0; i < categoryItems.size(); ++i)
        {
            QTreeWidgetItem* item = categoryItems[i];
            delete(item);
        }

        categoryItems.clear();        
        
        //render this tree!
        QVector<AssetCategory>* targetCategories = nullptr;
        targetCategories = filterMode == InstanceViewer::BLOCK_FILTER ? &mTree.categories : &mTree.typeCategories;
        for (AssetCategory c : *targetCategories)
        {
            if (c.children.empty()) continue;
            CategoryInstanceItem* categoryItem = new CategoryInstanceItem();
            categoryItem->SetBlockGuid(c.blockGuid);
            categoryItem->setText(0, c.displayName);

            for (int assetIdx : c.children)
            {
                const AssetInformation& info = mTree.allAssets[assetIdx];
                PopulateAsset(info, categoryItem);
            }

            categoryItems.append(categoryItem);
        }
        
    }

    SetFilterMode(mFilterMode, false/*tree already cleared.*/);
}

void InstanceViewer::OnSetValidity(const QString& path, bool validity)
{
    QMap<QString, QSet<QTreeWidgetItem*>>::iterator it = mAssetViews.find(path);
    QIcon emptyIcon;
    if (it != mAssetViews.end())
    {
        for (QTreeWidgetItem* widgetItem : (*it))
        {
            if (validity)
            {
                widgetItem->setIcon(2, emptyIcon);
            }
            else
            {
                widgetItem->setIcon(2, mInvalidIcon);
            }
        }
    }
}


QTreeWidgetItem* InstanceViewer::CreateAssetWidgetItem(const AssetInformation& info)
{
    QTreeWidgetItem* assetInfoItem = new AssetInstanceItem();
    QString iconPath = info.typeInfo->mIconPath;
    QIcon ic(iconPath);
    assetInfoItem->setIcon(0, ic);
    assetInfoItem->setText(0, info.displayName);
    assetInfoItem->setText(1, QString(info.typeInfo->mTypeName));
    //its actually prettier to just have an invalid icon instead of a valid / invalid state.
    if (!info.isValid)
    {
        assetInfoItem->setIcon(2, mInvalidIcon);
    }

    QMap<QString, QSet<QTreeWidgetItem*>>::iterator it = mAssetViews.find(info.displayName);
    if (it == mAssetViews.end())
    {
        QSet<QTreeWidgetItem*> widgetItemSet;
        widgetItemSet.insert(assetInfoItem);
        mAssetViews.insert(info.displayName, widgetItemSet);
    }
    else
    {
        (*it).insert(assetInfoItem);
    }
    return assetInfoItem;
}

void InstanceViewer::PopulateAsset(const AssetInformation& info, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* infoItem = CreateAssetWidgetItem(info);
    parent->addChild(infoItem);

    for (int childIdx : info.children)
    {        
        const AssetInformation& childInfo = mTree.allAssets[childIdx];
        PopulateAsset(childInfo, infoItem);
    }
}

void InstanceViewer::PrepareContextMenu(const QPoint& point)
{
    QTreeWidgetItem* item = mTreeWidget->itemAt(point);
    if (item != nullptr)
    {
        AbstractInstanceItem* instanceItem = static_cast<AbstractInstanceItem*>(item);
        switch(instanceItem->GetType())
        {
        case CATEGORY:
            {
                if (mFilterMode == InstanceViewer::BLOCK_FILTER)
                {
                    CategoryInstanceItem* catInstanceItem = static_cast<CategoryInstanceItem*>(instanceItem);
                    mHighlightBlockCmdParams.targetBlockGuid = catInstanceItem->GetBlockGuid();
                    mCategoryCtxMenu->exec(mTreeWidget->mapToGlobal(point));
                }
                break;
            }
        case ASSET_INSTANCE:
            {
                mOpenAssetCmdParams.targetAsset = instanceItem->text(0);
                mAssetInstanceCtxMenu->exec(mTreeWidget->mapToGlobal(point));
                break;
            }
        default:
            return;
        }
    }
}

void InstanceViewer::ExecuteOpenAssetSlot()
{
    emit(RequestOpenAsset(mOpenAssetCmdParams.targetAsset));
}

void InstanceViewer::ExecuteHighlightBlock()
{
    emit(RequestHighlightBlock(mHighlightBlockCmdParams.targetBlockGuid));
}

void InstanceViewer::OnItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    AbstractInstanceItem* abstractItem = static_cast<AbstractInstanceItem*>(item);
    if (abstractItem->GetType() == ASSET_INSTANCE)
    {
        emit(RequestOpenAsset(abstractItem->text(0)));
    }
}

void InstanceViewer::OnShutdown()
{
    mTreeWidget->clear();
    mTree = AssetViewTree();
    for (unsigned itFilterMode = 0; itFilterMode< static_cast<unsigned>(InstanceViewer::FILTER_COUNT); ++itFilterMode)
    {        
        mTreeWidgetItems[itFilterMode].clear();    
    }
}
