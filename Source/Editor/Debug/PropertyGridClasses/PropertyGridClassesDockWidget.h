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

#include <QDockWidget>

class QTreeWidget;


//! Dock widget containing the list of classes that have a property grid
class PropertyGridClassesDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param parent Parent widget
    PropertyGridClassesDockWidget(QWidget * parent);

    //! Destructor
    virtual ~PropertyGridClassesDockWidget();

    //------------------------------------------------------------------------------------

public slots:

    //! Called when an application is successfully loaded
    void UpdateUIForAppLoaded();

    //! Called when an application is closed
    void UpdateUIForAppClosed();

    //------------------------------------------------------------------------------------
    
private:

    //! Tree widget showing the hierarchy of classes
    QTreeWidget * mTreeWidget;
};


#endif  // EDITOR_PROPERTYGRIDCLASSESDOCKWIDGET_H
