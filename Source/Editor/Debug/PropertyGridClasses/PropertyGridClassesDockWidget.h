/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassesDockWidget.h
//! \author	Kevin Boulanger
//! \date	26th May 2015
//! \brief	Dock widget containing the list of classes that have a property grid

#ifndef EDITOR_PROPERTYGRIDCLASSESDOCKWIDGET_H
#define EDITOR_PROPERTYGRIDCLASSESDOCKWIDGET_H

#include "Widgets/PegasusDockWidget.h"

class QTreeWidget;


//! Dock widget containing the list of classes that have a property grid
class PropertyGridClassesDockWidget : public PegasusDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent widget
    PropertyGridClassesDockWidget(QWidget * parent, Editor* editor);

    //! Destructor
    virtual ~PropertyGridClassesDockWidget();

    //! Callback fired when the UI needs to be set.
    void SetupUi();

    //! Returns the name this widget
    virtual const char* GetName() const  { return "PropertyGridClassesDockWidget"; }

    //! Returns the title of this widget
    virtual const char* GetTitle() const { return "Property Grid Classes"; }

    //------------------------------------------------------------------------------------

public slots:

    //! Called when an application is successfully loaded
    void OnUIForAppLoaded(Pegasus::App::IApplicationProxy* app);

    //! Called when an application is closed
    void OnUIForAppClosed();

    //------------------------------------------------------------------------------------
    
private:

    //! Tree widget showing the hierarchy of classes
    QTreeWidget * mTreeWidget;
};


#endif  // EDITOR_PROPERTYGRIDCLASSESDOCKWIDGET_H
