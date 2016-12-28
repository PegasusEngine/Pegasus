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
#include "Pegasus/PropertyGrid/Shared/IPropertyGridEnumTypeInfo.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "MessageControllers/PropertyGridIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"

#include <QVBoxLayout>
#include <QPair>
#include <QStack>
#include <QLabel>
class PgwObserver : public PropertyGridObserver
{
public:
    explicit PgwObserver(PropertyGridWidget * parent) : mParent(parent) {}
    virtual ~PgwObserver() {}

    virtual void OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);

    virtual void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els);

    virtual void OnShutdown(PropertyGridHandle handle);

    void OnShutdownInternal(PropertyGridHandle handle);

private:
    PropertyGridWidget * mParent;
};

PropertyGridWidget::PropertyGridWidget(QWidget * parent)
:   QWidget(parent)
,   mApplicationProxy(nullptr)
,   mMessenger(nullptr)
,   mProxyHandle(INVALID_PGRID_HANDLE)
,   mIsInitializing(false)
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
    mBrowser->setFactoryForManager(&mEnumManager, &mEnumEditorFactory);
    
    // connect change of values events
    connect(&mBoolManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(boolPropertyChanged(QtProperty *)));
    connect(&mIntManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(intPropertyChanged(QtProperty *)));
    connect(&mUIntManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(uintPropertyChanged(QtProperty *)));
    connect(&mFloatManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(floatPropertyChanged(QtProperty *)));
    connect(&mVec2Manager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(vec2PropertyChanged(QtProperty *)));
    connect(&mVec3Manager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(vec3PropertyChanged(QtProperty *)));
    connect(&mVec4Manager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(vec4PropertyChanged(QtProperty *)));
    connect(&mColor8RGBManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(rgbPropertyChanged(QtProperty *)));
    connect(&mColor8RGBAManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(rgbaPropertyChanged(QtProperty *)));
    connect(&mString64Manager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(s64PropertyChanged(QtProperty *)));
    connect(&mEnumManager, SIGNAL(propertyChanged(QtProperty *)),
            this, SLOT(enumPropertyChanged(QtProperty *)));
    mTitle = new QLabel(this);
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(mTitle);
    layout->addWidget(mBrowser);
    layout->setMargin(0);
    layout->setSpacing(0);

    mObserver = new PgwObserver(this);
}

//----------------------------------------------------------------------------------------

PropertyGridWidget::~PropertyGridWidget()
{
    delete mObserver;
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::intPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);
    
    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
    el.mData.i = mIntManager.value(property);
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::boolPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_BOOL;
    el.mData.b = mBoolManager.value(property);
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::uintPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_UINT;
    el.mData.u = mUIntManager.value(property);
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::floatPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
    el.mData.f = mFloatManager.value(property);
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec2PropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC2;
    Vec2Property v = mVec2Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec3PropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC3;
    Vec3Property v = mVec3Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mData.v[2] = v.value[2];
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::vec4PropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC4;
    Vec4Property v = mVec4Manager.value(property);
    el.mData.v[0] = v.value[0];
    el.mData.v[1] = v.value[1];
    el.mData.v[2] = v.value[2];
    el.mData.v[3] = v.value[3];
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::rgbPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB;
    QColor c = mColor8RGBManager.value(property);
    el.mData.rgba8[0] = (unsigned char)c.red();
    el.mData.rgba8[1] = (unsigned char)c.green();
    el.mData.rgba8[2] = (unsigned char)c.blue();
    el.mData.rgba8[3] = (unsigned char)1;
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::rgbaPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA;
    QColor c = mColor8RGBAManager.value(property);
    el.mData.rgba8[0] = (unsigned char)c.red();
    el.mData.rgba8[1] = (unsigned char)c.green();
    el.mData.rgba8[2] = (unsigned char)c.blue();
    el.mData.rgba8[3] = (unsigned char)c.alpha();
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::s64PropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;
    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_STRING64;
    QString s = mString64Manager.value(property);
    int i = 0;
    for (; i < s.size() && i < 63 ; ++i) { el.mData.s64[i] = s.at(i).toLatin1(); }
    el.mData.s64[i] = '\0';
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);

}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::enumPropertyChanged(QtProperty * property)
{
    if (!IsReady()) return;

    unsigned int propertyIndex = 0;
    const Pegasus::PropertyGrid::PropertyRecord * record = FindPropertyRecord(property, propertyIndex);

    PropertyGridIOMCUpdateElement el;
    el.mCategory = record->category;
    el.mType = Pegasus::PropertyGrid::PROPERTYTYPE_CUSTOM_ENUM;
    int e = mEnumManager.value(property);
    el.mData.i = e;
    el.mIndex = propertyIndex;

    UpdateProxy(el);
    OnPropertyUpdated(property);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::UpdateProxy(const PropertyGridIOMCUpdateElement & el)
{
    PropertyGridIOMCMessage msg;
    msg.SetMessageType(PropertyGridIOMCMessage::UPDATE);
    msg.SetPropertyGridHandle(mProxyHandle);
    msg.GetUpdateBatch().push_back(el);
    mMessenger->SendPropertyGridIoMessage(msg);
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::SendCloseMessage()
{
    ED_ASSERTSTR(mMessenger != nullptr, "A messenger must be set in order for the property grid widget to work.");

    if (mProxyHandle != INVALID_PGRID_HANDLE)
    {
        // Send a message to close the observer of this handle
        PropertyGridIOMCMessage msg;
        msg.SetMessageType(PropertyGridIOMCMessage::CLOSE);
        msg.SetPropertyGridObserver(mObserver);
        msg.SetPropertyGridHandle(mProxyHandle);
        mMessenger->SendPropertyGridIoMessage(msg);
        Clear();
    }
}

//----------------------------------------------------------------------------------------

const Pegasus::PropertyGrid::PropertyRecord * PropertyGridWidget::FindPropertyRecord(const QtProperty * property, unsigned int & outIndex) const
{
    for (unsigned int c = 0; c < Pegasus::PropertyGrid::NUM_PROPERTY_CATEGORIES; ++c)
    {
        for (int i = 0; i < mProperties[c].size(); ++i)
        { 
            if (mProperties[c][i].mProperty == property)
            {
                outIndex = i;
                return mProperties[c][i].mRecord;
            }
        }
    }

    outIndex = 0;
    return nullptr;
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::SetCurrentProxy(AssetInstanceHandle assetHandle)
{
    ClearProperties();

    PropertyGridIOMCMessage msg;
    msg.SetMessageType(PropertyGridIOMCMessage::OPEN_FROM_ASSET_HANDLE);
    msg.SetPropertyGridObserver(mObserver);
    msg.SetAssetHandle(assetHandle);
    mMessenger->SendPropertyGridIoMessage(msg);
}

void PropertyGridWidget::SetCurrentTimelineBlock(AssetInstanceHandle timelineHandle, unsigned int blockGuid, const QString& title)
{
    ClearProperties();

    PropertyGridIOMCMessage msg;
    msg.SetMessageType(PropertyGridIOMCMessage::OPEN_BLOCK_FROM_TIMELINE);
    msg.SetPropertyGridObserver(mObserver);
    msg.SetAssetHandle(timelineHandle);
    msg.SetBlockGuid(blockGuid);
    msg.SetTitle(title);
    mMessenger->SendPropertyGridIoMessage(msg);
}


//----------------------------------------------------------------------------------------

void PropertyGridWidget::ClearProperties()
{
    if (mProxyHandle != INVALID_PGRID_HANDLE)
    {
        SendCloseMessage();
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy * objectProxy)
{
    //! \todo THIS ENTIRE FUNCTION IS THREAD-UNSAFE.
    //!       The function executes in the UI thread, so it should not fetch from the proxy.
    //!       Use UpdateElement or a new InitElement to describe the properties

    if (mApplicationProxy == nullptr)
    {
        return;
    }
    mIsInitializing = true;
    mTitle->setText(title);
    if (mProxyHandle == handle)
    {
        return;
    }

    SendCloseMessage();

    Pegasus::PropertyGrid::IPropertyGridManagerProxy * propertyGridManagerProxy = mApplicationProxy->GetPropertyGridManagerProxy();

    const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * classInfoProxy = objectProxy->GetClassInfoProxy();

    // Stack the number of class properties for each part of the class hierarchy.
    // When popping the elements of the stack, we know what is the global property index
    // of each class property.
    typedef QPair<const char *, unsigned int> SectionNameNumPropertiesPair;
    QStack<SectionNameNumPropertiesPair> sectionStack;
    sectionStack.push_back(SectionNameNumPropertiesPair("Object Properties",
                                                        objectProxy->GetNumObjectProperties()));
    sectionStack.push_back(SectionNameNumPropertiesPair(classInfoProxy->GetClassName(),
                                                        classInfoProxy->GetNumDerivedClassProperties()));
    const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * parentInfoProxy = classInfoProxy->GetParentClassInfo();
    while (parentInfoProxy != nullptr)
    {
        sectionStack.push_back(SectionNameNumPropertiesPair(parentInfoProxy->GetClassName(),
                                                            parentInfoProxy->GetNumDerivedClassProperties()));
        parentInfoProxy = parentInfoProxy->GetParentClassInfo();
    }

    // For each class of the hierarchy (and object properties), starting from the root class
    static const QRegExp sString64RegExp("^[a-z0-9]{0,64}$", Qt::CaseInsensitive);
    unsigned int firstSectionPropertyIndex = 0;
    while (!sectionStack.isEmpty())
    {
        SectionNameNumPropertiesPair sectionPair = sectionStack.pop();

        // Create a group property for the class name (or "Object Properties"),
        // corresponding to a collapsible section of the property grid widget
        QtProperty * groupProperty = m_groupManager.addProperty(sectionPair.first);

        // For each class/object property, create a property in the tree property browser
        const unsigned int numSectionProperties = sectionPair.second;
        for (unsigned int propertyIndex = 0; propertyIndex < numSectionProperties; ++propertyIndex)
        {
            // Retrieve the description of the property
            const Pegasus::PropertyGrid::PropertyRecord & record = firstSectionPropertyIndex >= classInfoProxy->GetNumClassProperties() ?
                objectProxy->GetObjectPropertyRecord(propertyIndex) : 
                objectProxy->GetClassPropertyRecord(firstSectionPropertyIndex + propertyIndex);

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

                case Pegasus::PropertyGrid::PROPERTYTYPE_CUSTOM_ENUM:
                    {
                        // Gather the information about the enumerant
                        const Pegasus::PropertyGrid::IEnumTypeInfoProxy * enumTypeInfoProxy = propertyGridManagerProxy->GetEnumInfo(record.typeName);
                        ED_ASSERTSTR(enumTypeInfoProxy != nullptr, "Invalid enum type info for \"%s\"", record.typeName);
                        unsigned int enumOptionCount = 0;
                        const Pegasus::PropertyGrid::BaseEnumType ** enumerations = enumTypeInfoProxy->GetEnumerations(enumOptionCount);
                        ED_ASSERTSTR(enumerations != nullptr, "Invalid enum type info for \"%s\"", record.typeName);
                        
                        // Build the list of enumerants for the current type
                        QStringList enumOptionList;
                        for (unsigned int enumTypeIndex = 0; enumTypeIndex < enumOptionCount; ++enumTypeIndex)
                        {
                            const Pegasus::PropertyGrid::BaseEnumType * enumRecord = enumerations[enumTypeIndex];
                            ED_ASSERTSTR(enumRecord != nullptr, "Invalid enum type info for \"%s\"", record.typeName);
                            enumOptionList += enumRecord->GetName();                            
                        }

                        // Add the property and its set of values
                        property = mEnumManager.addProperty(record.name);
                        mEnumManager.setEnumNames(property, enumOptionList);
                    }
                    break;

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
            PropertyRecordPair pp = { &record, property };
            ED_ASSERT(record.category < Pegasus::PropertyGrid::NUM_PROPERTY_CATEGORIES);
            mProperties[record.category].push_back(pp);
        }

        // Add the group and its sub-properties to the tree property browser
        mBrowser->addProperty(groupProperty);

        firstSectionPropertyIndex += numSectionProperties;
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

//----------------------------------------------------------------------------------------

void PropertyGridWidget::Clear()
{
    // No op
    if (mProxyHandle == INVALID_PGRID_HANDLE)
    {
        return;
    }

    mTitle->setText(tr(""));

    for (unsigned int c = 0; c < Pegasus::PropertyGrid::NUM_PROPERTY_CATEGORIES; ++c)
    {
        mProperties[c].clear();
    }

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
    mEnumManager.clear();
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement> & els)
{
    if (mProxyHandle == handle)
    {
        foreach (const PropertyGridIOMCUpdateElement & el, els)
        {
            if (el.mIndex >= 0 && el.mIndex < mProperties[el.mCategory].size())
            {
                PropertyGridWidget::PropertyRecordPair& pp = mProperties[el.mCategory][el.mIndex];
                if (el.mType == pp.mRecord->type)
                {
                    switch (pp.mRecord->type)
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

                        case Pegasus::PropertyGrid::PROPERTYTYPE_CUSTOM_ENUM:
                            mEnumManager.setValue(pp.mProperty, el.mData.i);
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
        mIsInitializing = false;
    }
}

//----------------------------------------------------------------------------------------

void PgwObserver::OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy)
{
    mParent->OnInitialized(handle, title, objectProxy);
}

//----------------------------------------------------------------------------------------

void PgwObserver::OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els)
{
    mParent->OnUpdated(handle, els);
}

//----------------------------------------------------------------------------------------

void PgwObserver::OnShutdown(PropertyGridHandle handle)
{
    OnShutdownInternal(handle);
}

//----------------------------------------------------------------------------------------

void PgwObserver::OnShutdownInternal(PropertyGridHandle handle)
{
    mParent->Clear();
}
