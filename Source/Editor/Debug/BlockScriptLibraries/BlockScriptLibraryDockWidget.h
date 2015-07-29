/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassesDockWidget.h
//! \author	Kleber Garcia
//! \date	July 26th, 2015
//! \brief	Dock widget showing the runtime contents of the registered pegasus libraries

#ifndef EDITOR_BSLIBDOCKWIDGET_H
#define EDITOR_BSLIBDOCKWIDGET_H

#include "Widgets/PegasusDockWidget.h"
#include <QIcon>

//fwd declarations
class QTreeWidget;
class QTreeWidgetItem;

namespace Pegasus
{
    namespace App
    {
        class IBsTypeInfo;
        class IBsFunInfo;
    }
}

class BlockScriptLibraryDockWidget : public PegasusDockWidget
{
    Q_OBJECT
public:

    //!Constructor
    BlockScriptLibraryDockWidget(QWidget* parent, Editor* editor);

    //! Destructor
    virtual ~BlockScriptLibraryDockWidget();

    //! Callback fired when the UI needs to be set.
    void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const  { return "BlockScriptLibraryWidget"; }

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "BlockScript Libraries"; }
    
protected:
    
    //! Callback called when an app has been loaded
    virtual void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application);

    //! Callback called when an app has been closed
    virtual void OnUIForAppClosed();


private:

    QTreeWidgetItem* CreateTypeItem(const Pegasus::App::IBsTypeInfo* typeInfo);

    QTreeWidgetItem* CreateFunItem(const Pegasus::App::IBsFunInfo* funInfo);
     
    QTreeWidget* mView;
    QIcon mFolderIcon;
    QIcon mTypeIcon;
    QIcon mFunctionIcon;
    QIcon mLibIcon;
    QIcon mNameIcon;
    QIcon mReturnIcon;
    QIcon mEnumIcon;
};


#endif //EDITOR_BSLIBDOCKWIDGET_H
