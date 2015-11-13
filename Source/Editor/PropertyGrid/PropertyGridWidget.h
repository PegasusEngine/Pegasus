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
#include "MessageControllers/PropertyGridIOMessageController.h"
#include <QWidget>
#include <QVector>

namespace Pegasus {
    namespace PropertyGrid {
        class IPropertyGridObjectProxy;
    }
}

class QtTreePropertyBrowser;
class PegasusDockWidget;


//! Property grid widget UI element
class PropertyGridWidget : public QWidget
{
    Q_OBJECT

public:

    //! Constructor
    PropertyGridWidget(QWidget* parent);

    //! Destructor
    ~PropertyGridWidget();

    //! Sets the messenger responsible to send messages to the render thread.
    //! \param messenger - the messenger to cache
    void SetMessenger(PegasusDockWidget* messenger) { mMessenger = messenger; }

    //! Set the property grid object proxy associated with the widget.
    //! the content of the property browser will update accordingly.
    //! \param proxy Proxy providing the property grid interface,
    //!              nullptr to remove the displayed property grid
    void SetCurrentProxy(Pegasus::PropertyGrid::IPropertyGridObjectProxy * proxy);

    //! Call when you want this property to lose references, only when the app closes.
    void Clear();

private slots:

    //! Called when an item of the property browser has its value changed
    void boolPropertyChanged(QtProperty* property);
    void uintPropertyChanged(QtProperty* property);
    void intPropertyChanged(QtProperty* property);
    void floatPropertyChanged(QtProperty* property);
    void vec2PropertyChanged(QtProperty* property);
    void vec3PropertyChanged(QtProperty* property);
    void vec4PropertyChanged(QtProperty* property);
    void rgbPropertyChanged(QtProperty* property);
    void rgbaPropertyChanged(QtProperty* property);
    void s64PropertyChanged(QtProperty* property);
    

private:

    void OnInitialized(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy* classInfoProxy);
    void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& els);

    int FindPropertyIndex(const QtProperty* property) const;

    bool IsReady() const { return mProxyHandle != INVALID_PGRID_HANDLE; }

    void UpdateProxy(const PropertyGridIOMessageController::UpdateElement& el);

    // Widget showing a set of properties
    QtTreePropertyBrowser * mBrowser;

    // Group manager, to create sections per class in the tree property browser
    QtGroupPropertyManager m_groupManager;

    // dock widget that sends messages.
    PegasusDockWidget* mMessenger;

    //! Handle retuned in OnInitialized from observer, which represents the current propertyGrid
    PropertyGridHandle mProxyHandle;

    //! Pair containing a property / type pair
    struct PropertyTypePair
    {
        Pegasus::PropertyGrid::PropertyType mType;
        QtProperty* mProperty;
    };

    //! List of properties, follows same order as schema (index are consistent)
    QVector<PropertyTypePair> mProperties;

    //! Observer of this widget, used to communicate with the IO controller.
    class Observer : public PropertyGridObserver
    {
    public:
        explicit Observer(PropertyGridWidget* parent) : mParent(parent) {}
        virtual ~Observer() {}

        virtual void OnInitialized(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy* classInfo);

        virtual void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& els);

        virtual void OnShutdown(PropertyGridHandle handle);

        void OnShutdownInternal(PropertyGridHandle handle);

    private:
        PropertyGridWidget* mParent;
    }* mObserver;

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
    

};


#endif // EDITOR_PROPERTYGRIDWIDGET_H
