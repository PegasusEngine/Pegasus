#include "Widgets/NodeFileTabBar.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

#include <QVBoxLayout>
#include <QMessageBox>

NodeFileTabBar::NodeFileTabBar(QWidget* parent)
    : QWidget(parent)
{
    SetupUi();
}

NodeFileTabBar::~NodeFileTabBar()
{
}

void NodeFileTabBar::Open(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    for (int i = 0; i < mContainerList.count(); ++i)
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[i];
        if (object == c.mObject)
        {
            mTabBar->setCurrentIndex(i);
            return;
        }
    }

    NodeFileTabBar::FileTabContainer newEl;
    newEl.mObject = object;
    mContainerList.push_back(newEl);

    mTabBar->addTab(object->GetDisplayName());
    mTabBar->setCurrentIndex(mTabBar->count() - 1);

}

void NodeFileTabBar::Close(int tabId)
{
    ReceiveTabClosedRequested(tabId);
}

void NodeFileTabBar::SetupUi()
{
    QVBoxLayout* vboxLayout = new QVBoxLayout(); 
    setLayout(vboxLayout);
    mTabBar = new QTabBar(this);
    mTabBar->setTabsClosable(true);
    vboxLayout->addWidget(mTabBar);

    connect(mTabBar, SIGNAL(currentChanged(int)),
            this, SLOT(ReceiveTabIdChanged(int)));

    connect(mTabBar, SIGNAL(tabCloseRequested(int)),
            this, SLOT(ReceiveTabClosedRequested(int)));
}

void NodeFileTabBar::ReceiveTabIdChanged(int id)
{
    if (id >= 0)
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[id];
        emit(DisplayRuntimeObject(c.mObject));
    }
    else
    {
        emit(DisplayRuntimeObject(nullptr));
    }
}


int NodeFileTabBar::FindIndex(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object) const
{
    for (int i = 0; i < mContainerList.count(); ++i)
    {
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* other = mContainerList[i].mObject;
        if ( other == object) 
        {
            return i;
        }
    }
    return -1;
}

void NodeFileTabBar::ReceiveTabClosedRequested(int tabId)
{
    ED_ASSERT(tabId >= 0 && tabId < mTabBar->count());
    NodeFileTabBar::FileTabContainer c = mContainerList[tabId];
    if (c.mIsDirty)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
                                       this, "Unsaved changes in file.",
                                      "The current asset has unsaved changes. Would you like to save before closing?",
                                       QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if (reply == QMessageBox::Cancel)
        {
            return;
        }
        else if (reply == QMessageBox::Yes) 
        {
            emit(SaveCurrentRuntimeObject());
        }
        else
        {
            emit(UnregisterDirtyObject(c.mObject));
            emit(DiscardCurrentObjectChanges());
            mContainerList.remove(tabId);
            mTabBar->removeTab(tabId);
            emit(RuntimeObjectRemoved(c.mObject));
        }
    }
    else
    {
        emit(UnregisterDirtyObject(c.mObject));
        mContainerList.remove(tabId);
        mTabBar->removeTab(tabId);
        emit(RuntimeObjectRemoved(c.mObject));
    }
}

void NodeFileTabBar::MarkCurrentAsDirty()
{
    NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
    if (!c.mIsDirty)
    {
        c.mIsDirty = true;
        QString currName = c.mObject->GetDisplayName();
        currName += "*";
        mTabBar->setTabText(mTabBar->currentIndex(), currName);
        emit(RegisterDirtyObject(c.mObject));
    }
}

void NodeFileTabBar::ClearCurrentDirty()
{
    NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
    if (c.mIsDirty)
    {
        c.mIsDirty = false;
        QString currName = c.mObject->GetDisplayName();
        mTabBar->setTabText(mTabBar->currentIndex(), currName);
        emit(UnregisterDirtyObject(c.mObject));
    }
}
