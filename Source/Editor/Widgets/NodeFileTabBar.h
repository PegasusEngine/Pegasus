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
#include "MessageControllers/AssetIOMessageController.h"


class QTabBar;
class PegasusDockWidget;

//! Advanced tab bar, for pegasus nodes
class NodeFileTabBar : public QWidget
{
    Q_OBJECT
public:

    //! Constructor
    NodeFileTabBar(PegasusDockWidget* parent = 0);

    //! Destructor
    virtual ~NodeFileTabBar();

    //! Opens an object into the tab bar
    //! \param object the object to insert into the tabbed bar
    //! \param extra data attached to this pegasus node object
    void Open(AssetInstanceHandle handle, const QString& displayName, QObject* extraData = nullptr);

    //! Closes an object from the tab bar
    //! \param tab index.
    void Close(int tabId);

    //! Gets the object count
    //! returns the total count of tabbed objects contained
    int GetTabCount() const { return mTabBar->count(); }

    //! Returns the tab object contained in the tab index
    //! \param the tab index
    //! \return the handle of this object
    AssetInstanceHandle GetTabObject(int tabIndex) { return mContainerList[tabIndex].mObject; }

    //! Sets this object extra data
    void SetExtraData(int index, QObject* obj);

    //! Gets this objects extra data
    QObject* GetExtraData(int index) const;

    //! Attempts to mark the current element in the tab editor as dirty
    void MarkCurrentAsDirty();

    //! Attempts to clear the current element's dirty flag
    void ClearCurrentDirty();

    //! Gets the current index of the tab selected
    int GetCurrentIndex() const { return mTabBar->currentIndex(); }

    //! Sets the current index of this tab.
    void SetCurrentIndex(int idx) { mTabBar->setCurrentIndex(idx); }

    //! Finds the index of the object passed, returns -1 if it doesnt exist
    int FindIndex(AssetInstanceHandle object) const;

    static void SetEnableSaveCloseSafety(bool enable);

signals:
    //signal fired when a runtime object is displayed
    void DisplayRuntimeObject(AssetInstanceHandle object);

    //signal fired when a runtime object is removed
    void RuntimeObjectRemoved(AssetInstanceHandle object, QObject* extraData);

    //! signal fired when a dirty element attempted to be closed is requested to be saved before being done.
    void SaveRuntimeObject(int tabId);
    
    //! signal that discards the elements changes
    void DiscardObjectChanges(AssetInstanceHandle object);

    //! Called when an object has been registered as dirty
    void RegisterDirtyObject(AssetInstanceHandle object);

    //! Called when an object has been unregistered as dirty
    void UnregisterDirtyObject(AssetInstanceHandle object);

private slots:
    void ReceiveTabIdChanged(int index);

    void ReceiveTabClosedRequested(int index);


private:
    //! Sets up the ui
    void SetupUi();

    struct FileTabContainer
    {
        bool mIsDirty;
        AssetInstanceHandle mObject;
        QString mDisplayName;
        QObject* mExtraData;
        FileTabContainer() : mIsDirty(false), mExtraData(nullptr) {}
    };

    QVector<FileTabContainer> mContainerList;
    QTabBar* mTabBar;
    
    PegasusDockWidget* mParent;

};

#endif
