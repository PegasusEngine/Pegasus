/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassesDockWidget.cpp
//! \author	Karolyn Boulanger
//! \date	26th May 2015
//! \brief	Dock widget containing the list of classes that have a property grid

#include "Debug/PropertyGridClasses/PropertyGridClassesDockWidget.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/PropertyGrid/Shared/IPropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridEnumTypeInfo.h"

#include "Pegasus/Application/Shared/IApplicationProxy.h"

#include <QTreeWidget>
#include <QMap>
#include <QList>


PropertyGridClassesDockWidget::PropertyGridClassesDockWidget(QWidget * parent, Editor* editor)
:   PegasusDockWidget(parent, editor)
{
}

void PropertyGridClassesDockWidget::SetupUi()
{
    // Create the tree widget showing the hierarchy of classes
    mTreeWidget = new QTreeWidget(this);
    mTreeWidget->setColumnCount(4);
    mTreeWidget->setAlternatingRowColors(true);
    QStringList labels;
    labels.append("Name");
    labels.append("Type");
    labels.append("Size");
    labels.append("Default Value");
    mTreeWidget->setHeaderLabels(labels);

    // Set the tree view as the main widget
    setWidget(mTreeWidget);
}

//----------------------------------------------------------------------------------------

PropertyGridClassesDockWidget::~PropertyGridClassesDockWidget()
{
}

//----------------------------------------------------------------------------------------

void PropertyGridClassesDockWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* app)
{
    ED_LOG("Generating the list of classes using a property grid");

    Pegasus::PropertyGrid::IPropertyGridManagerProxy * managerProxy =
        app->GetPropertyGridManagerProxy();

    QMap<const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy *, QTreeWidgetItem *> classMap;
    QList<QTreeWidgetItem *> items;

    // For each registered class
    const unsigned int numClasses = managerProxy->GetNumRegisteredClasses();
    for (unsigned int c = 0; c < numClasses; ++c)
    {
        const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * classInfoProxy = managerProxy->GetClassInfo(c);
        const char * className = classInfoProxy->GetClassName();

        const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * parentClassInfoProxy = classInfoProxy->GetParentClassInfo();


        // Create the item for the current class
        QTreeWidgetItem * classItem = new QTreeWidgetItem(QStringList(QString(className)));
        classItem->setIcon(0, QIcon(":/PropertyGridClasses/Class16.png"));
        items.append(classItem);
        classMap.insert(classInfoProxy, classItem);

        // For each class property
        const unsigned int numClassProperties = classInfoProxy->GetNumClassProperties();
        for (unsigned int cp = 0; cp < numClassProperties; ++cp)
        {
            const Pegasus::PropertyGrid::PropertyRecord & record = classInfoProxy->GetClassProperty(cp);
            
            // Create the content of the property item
            QStringList propertyList;
            propertyList.append(record.name);
            propertyList.append(record.typeName);

            propertyList.append(QString("%1").arg(record.size));
            switch (record.type)
            {
                case Pegasus::PropertyGrid::PROPERTYTYPE_BOOL:
                    {
                        const bool value = *reinterpret_cast<bool *>(record.defaultValuePtr);
                        propertyList.append(value ? "True" : "False");
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_INT:
                    {
                        const int value = *reinterpret_cast<int *>(record.defaultValuePtr);
                        propertyList.append(QString("%1").arg(value));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_UINT:
                    {
                        const unsigned int value = *reinterpret_cast<unsigned int *>(record.defaultValuePtr);
                        propertyList.append(QString("%1").arg(value));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT:
                    {
                        const float value = *reinterpret_cast<float *>(record.defaultValuePtr);
                        propertyList.append(QString("%1").arg(value));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC2:
                    {
                        const float * value = reinterpret_cast<float *>(record.defaultValuePtr);
                        propertyList.append(QString("(%1, %2)").arg(value[0]).arg(value[1]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC3:
                    {
                        const float * value = reinterpret_cast<float *>(record.defaultValuePtr);
                        propertyList.append(QString("(%1, %2, %3)").arg(value[0]).arg(value[1]).arg(value[2]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC4:
                    {
                        const float * value = reinterpret_cast<float *>(record.defaultValuePtr);
                        propertyList.append(QString("(%1, %2, %3, %4)").arg(value[0]).arg(value[1]).arg(value[2]).arg(value[3]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB:
                    {
                        const unsigned char * value = reinterpret_cast<unsigned char *>(record.defaultValuePtr);
                        propertyList.append(QString("(%1, %2, %3)").arg(value[0]).arg(value[1]).arg(value[2]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA:
                    {
                        const unsigned char * value = reinterpret_cast<unsigned char *>(record.defaultValuePtr);
                        propertyList.append(QString("(%1, %2, %3, %4)").arg(value[0]).arg(value[1]).arg(value[2]).arg(value[3]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_STRING64:
                    {
                        const char * value = reinterpret_cast<char *>(record.defaultValuePtr);
                        propertyList.append(QString("\"%1\"").arg(value));
                    }
                    break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_CUSTOM_ENUM:
                    {
                        const Pegasus::PropertyGrid::BaseEnumType* value = reinterpret_cast< const Pegasus::PropertyGrid::BaseEnumType *>(record.defaultValuePtr);
                        const Pegasus::PropertyGrid::IEnumTypeInfoProxy* enumInfo = managerProxy->GetEnumInfo(record.typeName);
                        if (enumInfo == nullptr)
                        {
                            propertyList.append(QString("???"));
                        }
                        else
                        {
                            int sz = 0;
                            const Pegasus::PropertyGrid::BaseEnumType** outList = enumInfo->GetEnumerations(sz);
                            const Pegasus::PropertyGrid::BaseEnumType* targetEnum = nullptr;
                            for (int i = 0; i < sz; ++i) if (outList[i]->GetValue() == value->GetValue()) {  targetEnum = outList[i]; break; }
                            propertyList.append(QString("\"%1\"").arg(targetEnum ==  nullptr ? "????" : targetEnum->GetName()));
                        }
                    }
                    break;

                default:
                    ED_FAILSTR("Invalid type for a property (%u), it must be < %u", record.type, Pegasus::PropertyGrid::NUM_PROPERTY_TYPES);
                    propertyList.append("unknown");
                    break;
            }

            // Create the item for the property
            QTreeWidgetItem * propertyItem = new QTreeWidgetItem(classItem, propertyList);
            propertyItem->setIcon(0, QIcon(":/PropertyGridClasses/Property16.png"));
            items.append(propertyItem);
        }
    }
    
    for (unsigned int c = 0; c < numClasses; ++c)
    {
        QTreeWidgetItem * item = classMap[managerProxy->GetClassInfo(c)];
               
        if (managerProxy->GetClassInfo(c)->GetParentClassInfo() != nullptr)
        {
            QTreeWidgetItem * parentClassItem = classMap[managerProxy->GetClassInfo(c)->GetParentClassInfo()];
            parentClassItem->addChild(item);
        }

    }
    mTreeWidget->insertTopLevelItems(0, items);
    mTreeWidget->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------------------

void PropertyGridClassesDockWidget::OnUIForAppClosed()
{
    mTreeWidget->clear();
}
