/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeFileTabBar.h
//! \author	Kleber Garcia
//! \date	July 12th, 2015
//! \brief	Widget component representing a tab bar that holds a list of q objects.
//!         bar keeps state for dirty objects

#ifndef EDITOR_NODE_FILE_BAR_H
#define EDITOR_NODE_FILE_BAR_H

#include <QWidget>
#include <QTabBar>
#include <QVector>

//fwd declarations
namespace Pegasus
{
    namespace AssetLib {
        class IRuntimeAssetObjectProxy;
    }
}

class QTabBar;

//! Advanced tab bar, for pegasus nodes
class NodeFileTabBar : public QWidget
{
    Q_OBJECT
public:

    //! Constructor
    NodeFileTabBar(QWidget* parent = 0);

    //! Destructor
    virtual ~NodeFileTabBar();

    //! Opens an object into the tab bar
    //! \param object the object to insert into the tabbed bar
    void Open(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Closes an object from the tab bar
    //! \param tab index.
    void Close(int tabId);

    //! Gets the object count
    //! returns the total count of tabbed objects contained
    int GetTabCount() const { return mTabBar->count(); }

    //! Returns the tab object contained in the tab index
    //! \param the tab index
    template <class T>
    T* GetTabObject(int tabIndex) { return static_cast<T*>(mContainerList[tabIndex].mObject); }

    //! Attempts to mark the current element in the tab editor as dirty
    void MarkCurrentAsDirty();

    //! Attempts to clear the current element's dirty flag
    void ClearCurrentDirty();

    //! Gets the current index of the tab selected
    int GetCurrentIndex() const { return mTabBar->currentIndex(); }

    //! Sets the current index of this tab.
    void SetCurrentIndex(int idx) { mTabBar->setCurrentIndex(idx); }

    //! Finds the index of the object passed, returns -1 if it doesnt exist
    int FindIndex(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object) const;

signals:
    //signal fired when a runtime object is displayed
    void DisplayRuntimeObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //signal fired when a runtime object is removed
    void RuntimeObjectRemoved(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! signal fired when a dirty element attempted to be closed is requested to be saved before being done.
    void SaveCurrentRuntimeObject();
    
    //! signal that discards the elements changes
    void DiscardCurrentObjectChanges();

    //! Called when an object has been registered as dirty
    void RegisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Called when an object has been unregistered as dirty
    void UnregisterDirtyObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

private slots:
    void ReceiveTabIdChanged(int index);

    void ReceiveTabClosedRequested(int index);


private:
    //! Sets up the ui
    void SetupUi();

    struct FileTabContainer
    {
        bool mIsDirty;
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* mObject;
        FileTabContainer() : mIsDirty(false), mObject(nullptr) {}
    };

    QVector<FileTabContainer> mContainerList;
    QTabBar* mTabBar;
    

};

#endif
