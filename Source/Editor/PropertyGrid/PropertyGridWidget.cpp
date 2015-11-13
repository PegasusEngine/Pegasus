/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridWidget.cpp
//! \author	Karolyn Boulanger
//! \date	August 4th, 2015
//! \brief	Widget show the tree of a property grid

#include "PropertyGrid/PropertyGridWidget.h"

#include "PropertyGrid/qtpropertybrowser/qttreepropertybrowser.h"
#include "PropertyGrid/qtpropertybrowser/qtpropertymanager.h"
#include "PropertyGrid/qtpropertybrowser/qteditorfactory.h"

#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"
#include "Widgets/PegasusDockWidget.h"

#include <QVBoxLayout>
#include <QPair>
#include <QStack>


PropertyGridWidget::PropertyGridWidget(QWidget * parent)
:   QWidget(parent), mMessenger(nullptr), mProxyHandle(INVALID_PGRID_HANDLE)
{
    mBrowser = new QtTreePropertyBrowser(parent);
    mBrowser->setRootIsDecorated(false);
    mBrowser->setPropertiesWithoutValueMarked(true);
    mBrowser->setResizeMode(QtTreePropertyBrowser::Interactive);

    mBrowser->setFactoryForManager(&mBoolManager, &mBoolEditorFactory);
    mBrowser->setFactoryForManager(&mUIntManager, &mUIntEditorFactory);
    mBrowser->setFactoryForManager(&mIntManager, &mIntEditorFactory);
    mBrowser->setFactoryForManager(&mFloatManager, &mFloatEditorFactory);
    mBrowser->setFactoryForManager(mVec2Manager.getSubDoublePropertyManager(), &mFloatEditorFactory);
    mBrowser->setFactoryForManager(mVec3Manager.getSubDoublePropertyManager(), &mFloatEditorFactory);
    mBrowser->setFactoryForManager(mVec4Manager.getSubDoublePropertyManager(), &mFloatEditorFactory);
    mBrowser->setFactoryForManager(&mColor8RGBManager, &mColor8RGBEditorFactory);
    mBrowser->setFactoryForManager(&mColor8RGBAManager, &mColor8RGBAEditorFactory);
    mBrowser->setFactoryForManager(&mString64Manager, &mString64EditorFactory);
    
    // connect change of values events
    connect(&mBoolManager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(boolPropertyChanged(QtProperty* )));
    connect(&mUIntManager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(uintPropertyChanged(QtProperty* )));
    connect(&mFloatManager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(floatPropertyChanged(QtProperty* )));
    connect(&mVec2Manager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(vec2PropertyChanged(QtProperty* )));
    connect(&mVec3Manager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(vec3PropertyChanged(QtProperty* )));
    connect(&mVec4Manager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(vec4PropertyChanged(QtProperty* )));
    connect(&mColor8RGBManager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(rgbPropertyChanged(QtProperty* )));
    connect(&mColor8RGBAManager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(rgbaPropertyChanged(QtProperty* )));
    connect(&mString64Manager, SIGNAL(propertyChanged(QtProperty* )),
            this, SLOT(s64PropertyChanged(QtProperty* )));

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(mBrowser);
    layout->setMargin(0);
    layout->setSpacing(0);

    mObserver = new PropertyGridWidget::Observer(this);
}

//----------------------------------------------------------------------------------------

PropertyGridWidget::~PropertyGridWidget()
{
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::intPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
    el.mData.i = mIntManager.value(property);
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::boolPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_BOOL;
    el.mData.b = mBoolManager.value(property);
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::uintPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_UINT;
    el.mData.u = mUIntManager.value(property);
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::floatPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
    el.mData.f = mFloatManager.value(property);
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec2PropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC2;
    Vec2Property v = mVec2Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec3PropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC3;
    Vec3Property v = mVec3Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mData.v[2] = v.value[2];
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec4PropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC4;
    Vec4Property v = mVec4Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mData.v[2] = v.value[2];
    el.mData.v[3] = v.value[3];
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::rgbPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB;
    QColor c = mColor8RGBManager.value(property);
    el.mData.rgba8[0] = (unsigned char)c.red();
    el.mData.rgba8[1] = (unsigned char)c.green();
    el.mData.rgba8[2] = (unsigned char)c.blue();
    el.mData.rgba8[3] = (unsigned char)1;
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::rgbaPropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA;
    QColor c = mColor8RGBAManager.value(property);
    el.mData.rgba8[0] = (unsigned char)c.red();
    el.mData.rgba8[1] = (unsigned char)c.green();
    el.mData.rgba8[2] = (unsigned char)c.blue();
    el.mData.rgba8[3] = (unsigned char)c.alpha();
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::s64PropertyChanged(QtProperty* property)
{
    if (!IsReady()) return;

    PropertyGridIOMessageController::UpdateElement el;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_STRING64;
    QString s = mString64Manager.value(property);
    int i = 0;
    for (; i < s.size() && i < 63 ; ++i) { el.mData.s64[i] = s.at(i).toLatin1(); }
    el.mData.s64[i] = '\0';
    el.mIndex = FindPropertyIndex(property);

    UpdateProxy(el);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::UpdateProxy(const PropertyGridIOMessageController::UpdateElement& el)
{
    PropertyGridIOMessageController::Message msg;
    msg.SetMessageType(PropertyGridIOMessageController::Message::UPDATE);
    msg.SetPropertyGridHandle(mProxyHandle);
    msg.GetUpdateBatch().push_back(el);
    mMessenger->SendPropertyGridIoMessage(msg);
}

//----------------------------------------------------------------------------------------

int PropertyGridWidget::FindPropertyIndex(const QtProperty* property) const
{
    for (int i = 0; i < mProperties.size(); ++i)
    { 
        if (mProperties[i].mProperty == property)
        {
            return i;
        }
    }

    return -1;
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::SetCurrentProxy(Pegasus::PropertyGrid::IPropertyGridObjectProxy * proxy)
{
    ED_ASSERTSTR(mMessenger != nullptr, "A messenger must be set in order for this widget to work.");

    // request to open this property proxy
    PropertyGridIOMessageController::Message msg;
    msg.SetMessageType(PropertyGridIOMessageController::Message::OPEN);
    msg.SetPropertyGridObserver(mObserver);
    msg.SetPropertyGrid(proxy);
    mMessenger->SendPropertyGridIoMessage(msg);
}

void PropertyGridWidget::OnInitialized(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy* classInfoProxy)
{
    if (mProxyHandle == handle)
    {
        return;
    }

    if (mProxyHandle != INVALID_PGRID_HANDLE)
    {
        //send a message to close the observer of this handle.
        PropertyGridIOMessageController::Message msg;
        msg.SetMessageType(PropertyGridIOMessageController::Message::CLOSE);
        msg.SetPropertyGridObserver(mObserver);
        msg.SetPropertyGridHandle(mProxyHandle);
        mMessenger->SendPropertyGridIoMessage(msg);
        Clear();
    }

    mProxyHandle = INVALID_PGRID_HANDLE;

    // Stack the number of class properties for each part of the class hierarchy.
    // When popping the elements of the stack, we know what is the global property index
    // of each class property
    typedef QPair<const char *, unsigned int> ClassNameNumPropertiesPair;
    QStack<ClassNameNumPropertiesPair> classStack;
    classStack.push_back(ClassNameNumPropertiesPair(classInfoProxy->GetClassName(),
                                                    classInfoProxy->GetNumClassProperties()));
    const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * parentInfoProxy = classInfoProxy->GetParentClassInfo();
    while (parentInfoProxy != nullptr)
    {
        classStack.push_back(ClassNameNumPropertiesPair(parentInfoProxy->GetClassName(),
                                                        parentInfoProxy->GetNumClassProperties()));
        parentInfoProxy = parentInfoProxy->GetParentClassInfo();
    }

    // For each class of the hierarchy, starting from the root class
    static const QRegExp sString64RegExp("^[a-z0-9]{0,64}$", Qt::CaseInsensitive);
    unsigned int firstClassPropertyIndex = 0;
    while (!classStack.isEmpty())
    {
        ClassNameNumPropertiesPair classPair = classStack.pop();

        // Create a group property for the class name,
        // corresponding to a collapsible section of the property grid widget
        QtProperty * groupProperty = m_groupManager.addProperty(classPair.first);

        // For each class property, create a property in the tree property browser
        const unsigned int numClassProperties = classPair.second;
        for (unsigned int classPropertyIndex = 0; classPropertyIndex < numClassProperties; ++classPropertyIndex)
        {
            // Retrieve the description of the property
            const Pegasus::PropertyGrid::PropertyRecord & record = classInfoProxy->GetProperty(firstClassPropertyIndex + classPropertyIndex);

            // Create the property object, with the class determined from the property type
            QtProperty * property = nullptr;
            switch (record.type)
            {
                case Pegasus::PropertyGrid::PROPERTYTYPE_BOOL:
                    property = mBoolManager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_UINT:
                    property = mUIntManager.addProperty(record.name);
                    mUIntManager.setMinimum(property, 0);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_INT:
                    property = mIntManager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT:
                    property = mFloatManager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC2:
                    property = mVec2Manager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC3:
                    property = mVec3Manager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC4:
                    property = mVec4Manager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB:
                    property = mColor8RGBManager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA:
                    property = mColor8RGBAManager.addProperty(record.name);
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_STRING64:
                    property = mString64Manager.addProperty(record.name);
                    mString64Manager.setRegExp(property, sString64RegExp); 
                    break;

                //! \todo Add support for enums

                default:
                    ED_ASSERTSTR("Unknown property \"%s\" in a property grid. Ignored.", record.name);
                    break;
            }

            //! \todo Add tooltips
            //property->setToolTip("Task Priority");

            // Add the new property to the group
            if (property != nullptr)
            {
                groupProperty->addSubProperty(property);
            }
            PropertyTypePair pp = { record.type, property };
            mProperties.push_back(pp);
        }

        // Add the group and its sub-properties to the tree property browser
        mBrowser->addProperty(groupProperty);

        firstClassPropertyIndex += numClassProperties;
    }

    // Expand the section of each class, and collapse all the items (particularly vectors)
    QList<QtBrowserItem *> classNameItems = mBrowser->topLevelItems();
    foreach (QtBrowserItem * classNameItem, classNameItems)
    {
        mBrowser->setExpanded(classNameItem, true);
        QList<QtBrowserItem *> propertyItems = classNameItem->children();
        foreach (QtBrowserItem * propertyItem, propertyItems)
        {
            mBrowser->setExpanded(propertyItem, false);
        }
    }

    
    mProxyHandle = handle;

}

void PropertyGridWidget::Clear()
{
    // No op
    if (mProxyHandle == INVALID_PGRID_HANDLE)
    {
        return;
    }

    mProperties.clear();

    mProxyHandle = INVALID_PGRID_HANDLE;
    // clear local copy of property values
    mBrowser->clear();
    mBoolManager.clear();
    mIntManager.clear();
    mFloatManager.clear();
    mVec2Manager.clear();
    mVec3Manager.clear();
    mVec4Manager.clear();
    mColor8RGBManager.clear();
    mColor8RGBAManager.clear();
    mString64Manager.clear();
}

void PropertyGridWidget::OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& els)
{
    mProxyHandle = handle;
    foreach (const PropertyGridIOMessageController::UpdateElement& el, els)
    {
        if (el.mIndex >= 0 && el.mIndex < mProperties.size())
        {
            PropertyGridWidget::PropertyTypePair& pp = mProperties[el.mIndex];
            if (el.mType == pp.mType)
            {
                switch (pp.mType)
                {
                    case Pegasus::PropertyGrid::PROPERTYTYPE_BOOL:
                        {
                            mBoolManager.setValue(pp.mProperty, el.mData.b);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_UINT:
                        {
                            mUIntManager.setValue(pp.mProperty, el.mData.u);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_INT:
                        {
                            mIntManager.setValue(pp.mProperty, el.mData.i);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT:
                        {
                            mFloatManager.setValue(pp.mProperty, el.mData.f);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_VEC2:
                        {
                            mVec2Manager.setValue(pp.mProperty, Vec2Property(el.mData.v));
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_VEC3:
                        {
                            mVec3Manager.setValue(pp.mProperty, Vec3Property(el.mData.v));
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_VEC4:
                        {
                            mVec4Manager.setValue(pp.mProperty, Vec4Property(el.mData.v));
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB:
                        {
                            QColor c(el.mData.rgba8[0], el.mData.rgba8[1], el.mData.rgba8[2], 255 ); 
                            mColor8RGBManager.setValue(pp.mProperty, c);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA:
                        {
                            QColor c(el.mData.rgba8[0], el.mData.rgba8[1], el.mData.rgba8[2], el.mData.rgba8[3] ); 
                            mColor8RGBAManager.setValue(pp.mProperty, c);
                            break;
                        }

                    case Pegasus::PropertyGrid::PROPERTYTYPE_STRING64:
                        mString64Manager.setValue(pp.mProperty, el.mData.s64);
                        break;

                    default:
                        ED_FAILSTR("Unknown property type to set value");
                        break;
                }
            }
            else
            {
                ED_FAILSTR("Unmatched type of property! check the schema!");
            }
        }
        else
        {
            ED_FAILSTR("Unmatched index of property! check the schema!");
        }
    } 
}



void PropertyGridWidget::Observer::OnInitialized(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy* classInfoProxy)
{
    mParent->OnInitialized(handle, classInfoProxy);
}

void PropertyGridWidget::Observer::OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& els)
{
    mParent->OnUpdated(handle, els);
}

void PropertyGridWidget::Observer::OnShutdown(PropertyGridHandle handle)
{
}

void PropertyGridWidget::Observer::OnShutdownInternal(PropertyGridHandle handle)
{
    mParent->Clear();

}
