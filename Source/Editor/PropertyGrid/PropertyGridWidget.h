/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridWidget.h
//! \author	Karolyn Boulanger
//! \date	August 4th, 2015
//! \brief	Widget show the tree of a property grid

#ifndef EDITOR_PROPERTYGRIDWIDGET_H
#define EDITOR_PROPERTYGRIDWIDGET_H

#include "PropertyGrid/PropertyGridPropertyManagers.h"
#include "PropertyGrid/PropertyGridEditorFactories.h"
#include <QWidget>

namespace Pegasus {
    namespace PropertyGrid {
        class IPropertyGridObjectProxy;
    }
}

class QtTreePropertyBrowser;


//! \todo See why we cannot use protected
class PropertyGridWidget : public QWidget
{
    Q_OBJECT

public:

    PropertyGridWidget(QWidget * parent);
    ~PropertyGridWidget();

    //! Set the property grid object proxy associated with the widget.
    //! the content of the property browser will update accordingly.
    //! \param proxy Proxy providing the property grid interface,
    //!              nullptr to remove the displayed property grid
    void SetCurrentProxy(Pegasus::PropertyGrid::IPropertyGridObjectProxy * proxy);

private slots:

    //! Called when an item of the property browser has its value changed
    void currentItemChanged(QtBrowserItem * browserItem);

private:

    // Widget showing a set of properties
    QtTreePropertyBrowser * mBrowser;

    // Group manager, to create sections per class in the tree property browser
    QtGroupPropertyManager m_groupManager;

    // List of property managers, one per property type
    PropertyGridBoolPropertyManager mBoolManager;
    PropertyGridUIntPropertyManager mUIntManager;
    PropertyGridIntPropertyManager mIntManager;
    PropertyGridFloatPropertyManager mFloatManager;
    PropertyGridVec2PropertyManager mVec2Manager;
    PropertyGridVec3PropertyManager mVec3Manager;
    PropertyGridVec4PropertyManager mVec4Manager;
    PropertyGridColor8RGBPropertyManager mColor8RGBManager;
    PropertyGridColor8RGBAPropertyManager mColor8RGBAManager;
    PropertyGridString64PropertyManager mString64Manager;
    
    // List of editor factories, one per property editor type
    PropertyGridBoolEditorFactory mBoolEditorFactory;
    PropertyGridUIntEditorFactory mUIntEditorFactory;
    PropertyGridIntEditorFactory mIntEditorFactory;
    PropertyGridFloatEditorFactory mFloatEditorFactory;
    PropertyGridColor8RGBEditorFactory mColor8RGBEditorFactory;
    PropertyGridColor8RGBAEditorFactory mColor8RGBAEditorFactory;
    PropertyGridString64EditorFactory mString64EditorFactory;

    //! \todo Add missing managers and editors
};


#endif // EDITOR_PROPERTYGRIDWIDGET_H
