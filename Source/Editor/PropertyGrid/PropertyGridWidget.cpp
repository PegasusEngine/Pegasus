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

#include <QVBoxLayout>
#include <QPair>
#include <QStack>


PropertyGridWidget::PropertyGridWidget(QWidget * parent)
:   QWidget(parent)
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
    //! \todo Add missing enum manager and editor factory

    connect(mBrowser, SIGNAL(currentItemChanged(QtBrowserItem *)),
            this, SLOT(currentItemChanged(QtBrowserItem *)));

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(mBrowser);
    layout->setMargin(0);
    layout->setSpacing(0);
}

//----------------------------------------------------------------------------------------

PropertyGridWidget::~PropertyGridWidget()
{
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::currentItemChanged(QtBrowserItem * browserItem)
{
    //! \todo Implement the interface with the application
    /****/
}

//----------------------------------------------------------------------------------------

void PropertyGridWidget::SetCurrentProxy(Pegasus::PropertyGrid::IPropertyGridObjectProxy * proxy)
{
    mBrowser->clear();

    if (proxy != nullptr)
    {
        const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * classInfoProxy = proxy->GetClassInfoProxy();

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
    }
}
