/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.cpp
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#include "Viewport/ViewportWidget.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/Window/Shared/IWindowProxy.h"
#include "MessageControllers/PropertyGridIOMessageController.h"

#include <QResizeEvent>

    //! Observer of this widget, used to communicate with the IO controller.
class VwObserver : public PropertyGridObserver
{
public:
    explicit VwObserver(ViewportWidget * parent) : mParent(parent) {}
    virtual ~VwObserver() {}

    virtual void OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);

    virtual void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els);

    virtual void OnShutdown(PropertyGridHandle handle);

    void OnShutdownInternal(PropertyGridHandle handle);

private:
    ViewportWidget * mParent;
};

ViewportWidget::ViewportWidget(QWidget * parent, Pegasus::App::ComponentTypeFlags componentFlags)
:   QWidget(parent), mWindowProxy(nullptr), mComponentFlags(componentFlags)
{
    // Set the size policy of the widget
    QSizePolicy newSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    newSizePolicy.setHorizontalStretch(0);
    newSizePolicy.setVerticalStretch(0);
    newSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(newSizePolicy);
    setMinimumSize(QSize(128, 128));

    //! \todo Remove that temporary background filling with black when we get an actual viewport
    //! \todo Or replace the black background with a dark gray one to indicate that an app is not running
    //! \todo Remove the background refresh completely (see N3D) when an application is open
    setAutoFillBackground(true);
    QPalette palette(palette());
    palette.setBrush(QPalette::Window, Qt::black);
    setPalette(palette);

    //! Get the native window ID
    //! \todo Do not use unsigned int, use a Pegasus type instead
    WId windowHandle = winId();
    mWindowHandle = *reinterpret_cast<unsigned int *>(&windowHandle);
    mObserver = new VwObserver(this);
}

//----------------------------------------------------------------------------------------

ViewportWidget::~ViewportWidget()
{
    delete mObserver;
}

//----------------------------------------------------------------------------------------

void ViewportWidget::resizeEvent(QResizeEvent * event)
{
    if (mWindowProxy != nullptr)
    {
        WindowIOMCMessage msg;
        msg.SetMessageType(WindowIOMCMessage::WINDOW_RESIZED);
        msg.SetViewportWidget(this);
        msg.SetWidth(event->size().width());
        msg.SetHeight(event->size().height());
        emit(OnSendWindowIoMessage(msg));
    }
}

//----------------------------------------------------------------------------------------

void ViewportWidget::DrawPegasusWindow()
{
    WindowIOMCMessage msg;
    msg.SetMessageType(WindowIOMCMessage::DRAW_WINDOW);
    msg.SetViewportWidget(this);
    emit(OnSendWindowIoMessage(msg));
}

//----------------------------------------------------------------------------------------

void ViewportWidget::OnAppLoaded()
{
    WindowIOMCMessage msg;
    msg.SetMessageType(WindowIOMCMessage::INITIALIZE_WINDOW);
    msg.SetViewportWidget(this);
    msg.SetComponentFlags(mComponentFlags);
    msg.SetWidth(width());
    msg.SetHeight(height());
    emit(OnSendWindowIoMessage(msg));
}

//----------------------------------------------------------------------------------------
void ViewportWidget::AttachWindowProxy(Pegasus::Wnd::IWindowProxy* proxy)
{
    mWindowProxy = proxy;
    //fill in property grid information
    for (int t = Pegasus::App::COMPONENT_BEGIN; t < Pegasus::App::COMPONENT_COUNT; ++t)
    {
        if ((1 << t) & mComponentFlags)
        {
            Pegasus::PropertyGrid::IPropertyGridObjectProxy* state = proxy->GetComponentState(static_cast<Pegasus::App::ComponentType>(t));
            if (state != nullptr)
            {
                PropertyGridIOMCMessage msg;
                QString encodedTitle = QString::number(t);
                msg.SetMessageType(PropertyGridIOMCMessage::OPEN);
                msg.SetPropertyGridObserver(mObserver);
                msg.SetTitle(encodedTitle);
                msg.SetPropertyGrid(state);
                OnSendPropertyGridIoMessage(msg);
            }
        }
    }
}

void ViewportWidget::OnInitialized(const PropertyGridHandle& handle, const QString& title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy)
{
    Pegasus::App::ComponentType componentType = static_cast<Pegasus::App::ComponentType>(title.toInt());
    ED_ASSERT(mComponentPropertyHandles[componentType] == INVALID_PGRID_HANDLE);
    ED_ASSERT(componentType >= Pegasus::App::COMPONENT_BEGIN && componentType < Pegasus::App::COMPONENT_COUNT );
    mComponentPropertyHandles[componentType] = handle;
    const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy* classInfo = objectProxy->GetClassInfoProxy();
    QMap<QString, int> lookupMap;
    for (unsigned int i = 0; i < classInfo->GetNumClassProperties(); ++i)
    {
        const Pegasus::PropertyGrid::PropertyRecord& r = classInfo->GetClassPropertyRecord(i);
        lookupMap.insert(tr(r.name), (int)i);
    }
    mComponentPropertyLookups[componentType] = lookupMap;
    
    emit(OnWindowProxyReady());
}

void ViewportWidget::InitUpdateElement(Pegasus::App::ComponentType component, const char* name, PropertyGridIOMCUpdateElement& outUpdateEl)
{
    if (mComponentPropertyHandles[component] != INVALID_PGRID_HANDLE)
    {
        QMap<QString,int>::iterator it = mComponentPropertyLookups[component].find(tr(name));
        if (it != mComponentPropertyLookups[component].end())
        {
            int index = it.value();
            outUpdateEl.mIndex = index;
            outUpdateEl.mCategory = Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS;
        }
        else
        {
            ED_FAILSTR("Could not find property: %s on component %d", name, component);
        }
    }
}


void ViewportWidget::SendUpdateEl(Pegasus::App::ComponentType component, PropertyGridIOMCUpdateElement& el)
{
    if (mComponentPropertyHandles[component] != INVALID_PGRID_HANDLE)
    {
        PropertyGridIOMCMessage msg;
        msg.SetMessageType(PropertyGridIOMCMessage::UPDATE);
        msg.SetPropertyGridHandle(mComponentPropertyHandles[component]);
        msg.SetPropertyGridObserver(mObserver);
        msg.GetUpdateBatch().push_back(el);
        OnSendPropertyGridIoMessage(msg);
        DrawPegasusWindow(); //redraw
    }
}

void ViewportWidget::SetBoolProperty(Pegasus::App::ComponentType component, const char* name, bool value)
{
    PropertyGridIOMCUpdateElement el;
    InitUpdateElement(component, name, el);
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_BOOL;
    el.mData.b = value;
    SendUpdateEl(component, el); 
}

void ViewportWidget::SetFloatProperty(Pegasus::App::ComponentType component, const char* name, float value)
{
    PropertyGridIOMCUpdateElement el;
    InitUpdateElement(component, name, el);
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
    el.mData.f = value;
    SendUpdateEl(component, el); 
}

void ViewportWidget::SetIntProperty(Pegasus::App::ComponentType component, const char* name, int value)
{
    PropertyGridIOMCUpdateElement el;
    InitUpdateElement(component, name, el);
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
    el.mData.i = value;
    SendUpdateEl(component, el); 
}

void ViewportWidget::OnUpdated(const PropertyGridHandle& handle, const QVector<PropertyGridIOMCUpdateElement>& els)
{    
}

void ViewportWidget::OnShutdown(const PropertyGridHandle& handle)
{

}

//----------------------------------------------------------------------------------------

void VwObserver::OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy)
{
    mParent->OnInitialized(handle, title, objectProxy);
}

//----------------------------------------------------------------------------------------

void VwObserver::OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els)
{
    mParent->OnUpdated(handle, els);
}

//----------------------------------------------------------------------------------------

void VwObserver::OnShutdown(PropertyGridHandle handle)
{
    mParent->OnShutdown(handle);
}

//----------------------------------------------------------------------------------------

void ViewportWidget::OnAppUnloaded()
{
    for (int i = (int)Pegasus::App::COMPONENT_BEGIN; i < (int)Pegasus::App::COMPONENT_COUNT; ++i)
    {
        mComponentPropertyHandles[i] = INVALID_PGRID_HANDLE;
        mComponentPropertyLookups[i].clear();
    }
    mWindowProxy = nullptr; //let the application interface clean the window.
}
