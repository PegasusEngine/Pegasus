#include "Widgets/NodeFileTabBar.h"
#include "Widgets/PegasusDockWidget.h"

#include <Editor.h>
#include <QVBoxLayout>
#include <QMessageBox>

NodeFileTabBar::NodeFileTabBar(PegasusDockWidget* parent)
    : QWidget(parent), mParent(parent)
{
    SetupUi();
}

NodeFileTabBar::~NodeFileTabBar()
{
}

void NodeFileTabBar::Open(AssetInstanceHandle object, const QString& displayName, QObject* extraData)
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
    newEl.mDisplayName = displayName;
    newEl.mExtraData = extraData;
    mContainerList.push_back(newEl);

    mTabBar->addTab(displayName);
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
    if (mParent != nullptr)
    {
        mParent->PerformFocus();
    }
    if (id >= 0)
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[id];
        emit(DisplayRuntimeObject(c.mObject));
    }
    else
    {
        emit(DisplayRuntimeObject(AssetInstanceHandle()));
    }
}


int NodeFileTabBar::FindIndex(AssetInstanceHandle object) const
{
    for (int i = 0; i < mContainerList.count(); ++i)
    {
        AssetInstanceHandle other = mContainerList[i].mObject;
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
        QMessageBox::StandardButton reply = Editor::AskSaveQuestion(this);
        if (reply == QMessageBox::Cancel)
        {
            return;
        }
        else if (reply == QMessageBox::Yes) 
        {
            emit(SaveRuntimeObject(tabId));
        }
        else
        {
            emit(UnregisterDirtyObject(c.mObject));
            emit(DiscardObjectChanges(c.mObject));
            mContainerList.remove(tabId);
            mTabBar->removeTab(tabId);
            emit(RuntimeObjectRemoved(c.mObject, c.mExtraData));
        }
    }
    else
    {
        emit(UnregisterDirtyObject(c.mObject));
        mContainerList.remove(tabId);
        mTabBar->removeTab(tabId);
        emit(RuntimeObjectRemoved(c.mObject, c.mExtraData));
    }
}

void NodeFileTabBar::MarkCurrentAsDirty()
{
    if (!mContainerList.isEmpty())
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
        if (!c.mIsDirty)
        {
            c.mIsDirty = true;
            QString currName = c.mDisplayName;
            currName += "*";
            mTabBar->setTabText(mTabBar->currentIndex(), currName);
            emit(RegisterDirtyObject(c.mObject));
        }
    }
}

void NodeFileTabBar::ClearCurrentDirty()
{
    if (!mContainerList.isEmpty())
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
        if (c.mIsDirty)
        {
            c.mIsDirty = false;
            const QString& currName = c.mDisplayName;
            mTabBar->setTabText(mTabBar->currentIndex(), currName);
            emit(UnregisterDirtyObject(c.mObject));
        }
    }
}

void NodeFileTabBar::SetExtraData(int index, QObject* object)
{
    if (!mContainerList.isEmpty())
    {
        NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
        c.mExtraData = object;
    }
}

QObject* NodeFileTabBar::GetExtraData(int index) const
{
    if (!mContainerList.isEmpty())
    {
        const NodeFileTabBar::FileTabContainer& c = mContainerList[mTabBar->currentIndex()];
        return c.mExtraData;
    }

    return nullptr;
}
