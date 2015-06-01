/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassesDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	26th May 2015
//! \brief	Dock widget containing the list of classes that have a property grid

#include "Debug/PropertyGridClasses/PropertyGridClassesDockWidget.h"
#include "Application/ApplicationManager.h"

#include "Pegasus/PropertyGrid/Shared/IPropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"

#include <QTreeWidget>
#include <QMap>
#include <QList>


PropertyGridClassesDockWidget::PropertyGridClassesDockWidget(QWidget * parent)
:   QDockWidget(parent)
{
    // Set the dock widget parameters
    setWindowTitle(tr("Property Grid Classes"));
    setObjectName("PropertyGridClassesDockWidget");
    setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

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

void PropertyGridClassesDockWidget::UpdateUIForAppLoaded()
{
    ED_LOG("Generating the list of classes using a property grid");

    Pegasus::PropertyGrid::IPropertyGridManagerProxy * managerProxy =
        Editor::GetInstance().GetApplicationManager().GetApplication()->GetPropertyGridManagerProxy();

    QMap<const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy *, QTreeWidgetItem *> classMap;
    QList<QTreeWidgetItem *> items;

    // For each registered class
    const unsigned int numClasses = managerProxy->GetNumRegisteredClasses();
    for (unsigned int c = 0; c < numClasses; ++c)
    {
        const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * classInfoProxy = managerProxy->GetClassInfo(c);
        const char * className = classInfoProxy->GetClassName();

        // Find the parent item if defined
        QTreeWidgetItem * parentClassItem = nullptr;
        const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy * parentClassInfoProxy = classInfoProxy->GetParentClassInfo();
        if (parentClassInfoProxy != nullptr)
        {
            parentClassItem = classMap[parentClassInfoProxy];
            ED_ASSERTSTR(parentClassItem != nullptr, "Trying to display a property grid class (\"%s\") with a parent but the parent was not declared yet", className);
        }

        // Create the item for the current class
        QTreeWidgetItem * classItem = new QTreeWidgetItem(parentClassItem, QStringList(QString(className)));
        classItem->setIcon(0, QIcon(":/PropertyGridClasses/Class16.png"));
        items.append(classItem);
        classMap.insert(classInfoProxy, classItem);

        // For each class property
        const unsigned int numClassProperties = classInfoProxy->GetNumClassProperties();
        for (unsigned int cp = 0; cp < numClassProperties; ++cp)
        {
            const Pegasus::PropertyGrid::IPropertyGridClassInfoProxy::PropertyRecord record = classInfoProxy->GetClassProperty(cp);
            
            // Create the content of the property item
            QStringList propertyList;
            propertyList.append(record.name);
            switch (record.type)
            {
                case Pegasus::PropertyGrid::PROPERTYTYPE_BOOL:
                    propertyList.append("bool");        break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_INT:
                    propertyList.append("int");         break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_UINT:
                    propertyList.append("uint");        break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT:
                    propertyList.append("float");       break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC2:
                    propertyList.append("vec2");        break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC3:
                    propertyList.append("vec3");        break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_VEC4:
                    propertyList.append("vec4");        break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB:
                    propertyList.append("color8RGB");   break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA:
                    propertyList.append("color8RGBA");  break;
                case Pegasus::PropertyGrid::PROPERTYTYPE_STRING64:
                    propertyList.append("string64");    break;
                default:
                    ED_FAILSTR("Invalid type for a property (%u), it must be < %u", record.type, Pegasus::PropertyGrid::NUM_PROPERTY_TYPES);
                    propertyList.append("unknown");     break;
            }
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
                        propertyList.append(QString("(%1, %2, %3)").arg(value[0]).arg(value[1]).arg(value[2]).arg(value[3]));
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYTYPE_STRING64:
                    {
                        const char * value = reinterpret_cast<char *>(record.defaultValuePtr);
                        propertyList.append(QString("\"%1\"").arg(value));
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

    mTreeWidget->insertTopLevelItems(0, items);
    mTreeWidget->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------------------

void PropertyGridClassesDockWidget::UpdateUIForAppClosed()
{
    mTreeWidget->clear();
}