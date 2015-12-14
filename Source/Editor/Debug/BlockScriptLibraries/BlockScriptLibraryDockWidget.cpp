/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassesDockWidget.cpp
//! \author	Kleber Garcia
//! \date	July 26th, 2015
//! \brief	Dock widget showing the runtime contents of the registered pegasus libraries

#include "Debug/BlockScriptLibraries/BlockScriptLibraryDockWidget.h"
#include "Pegasus/Application/Shared/IAppBsReflectionInfo.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include <QTreeWidget>

using namespace Pegasus;
using namespace Pegasus::App;

BlockScriptLibraryDockWidget::BlockScriptLibraryDockWidget(QWidget* parent, Editor* editor)
: PegasusDockWidget(parent, editor),
  mLibIcon(tr(":/MiscViews/LibraryIcon.png")), 
  mFolderIcon(tr(":/MiscViews/slickfoldericon.png")), 
  mTypeIcon(tr(":/MiscViews/TypeIcon.png")), 
  mFunctionIcon(tr(":/MiscViews/FunctionIcon.png")),
  mNameIcon(tr(":/MiscViews/NameIcon.png")), 
  mReturnIcon(tr(":/MiscViews/ReturnIcon.png")),
  mEnumIcon(tr(":/MiscViews/EnumIcon.png"))
{
}

BlockScriptLibraryDockWidget::~BlockScriptLibraryDockWidget()
{
}

void BlockScriptLibraryDockWidget::SetupUi()
{
    mView = new QTreeWidget(this);
    mView->setColumnCount(2);
    QStringList labels;
    labels.append("Element");
    labels.append("Desc");
    mView->setHeaderLabels(labels);
    mView->setSortingEnabled(true);
    setWidget(mView);
}

void BlockScriptLibraryDockWidget::OnUIForAppLoaded(Pegasus::App::IApplicationProxy* application)
{
    QList<QTreeWidgetItem*> libItems;
    IAppBsReflectionInfo* refInfo = application->GetBsReflectionInfo();
    IAppBlockscriptLibInfo*const* libInfos = refInfo->GetLibs();


    for (int i = 0; i < refInfo->GetLibraryCounts(); ++i)
    {
        const IAppBlockscriptLibInfo* info = libInfos[i];
        IBsTypeInfo*const* typeInfos = info->GetTypes();
        QTreeWidgetItem* libInfoItem = new QTreeWidgetItem(QStringList(QString(info->GetName())));
        libInfoItem->setIcon(0, mLibIcon);

        QTreeWidgetItem* typeTitle = new QTreeWidgetItem(QStringList(QString("Types")));
        QList<QTreeWidgetItem*> typeInfosTreeItems;
        QList<QTreeWidgetItem*> titles;
        for (int j = 0; j < info->GetTypeCounts(); ++j)
        {
            typeInfosTreeItems.append(CreateTypeItem(typeInfos[j]));
        }

        typeTitle->addChildren(typeInfosTreeItems); 
        typeTitle->setIcon(0,mFolderIcon);
        titles.append(typeTitle);

        QTreeWidgetItem* funTitle = new QTreeWidgetItem(QStringList(QString("Functions")));
        funTitle->setIcon(0,mFolderIcon);
        QList<QTreeWidgetItem*> funInfoTreeItems;

        IBsFunInfo*const* funInfos = info->GetFuns();
        for (int j = 0; j < info->GetFunctionCounts(); ++j)
        {
            funInfoTreeItems.append(CreateFunItem(funInfos[j]));
        }
        funTitle->addChildren(funInfoTreeItems);
        titles.append(funTitle);

        libInfoItem->addChildren(titles);

        libItems.append(libInfoItem);
    }


    mView->addTopLevelItems(libItems);
    mView->resizeColumnToContents(0);
    mView->resizeColumnToContents(1);
}

QTreeWidgetItem* BlockScriptLibraryDockWidget::CreateTypeItem(const Pegasus::App::IBsTypeInfo* typeInfo)
{
    QTreeWidgetItem* typeWidgetItem = new QTreeWidgetItem(QStringList(QString(typeInfo->GetName())));
    typeWidgetItem->setIcon(0,mTypeIcon);
    QString modifierName = QString(typeInfo->GetModifierName());
    QTreeWidgetItem* modNameItem = new QTreeWidgetItem(QStringList(QString("Modifier:")+modifierName));
    typeWidgetItem->addChild(modNameItem);

    if (modifierName == "Struct")
    {
        QList<QTreeWidgetItem*> childTypes;        
        for (int i = 0; i < typeInfo->GetChildrenCount(); ++i)
        {
            QTreeWidgetItem* child = CreateTypeItem(typeInfo->GetChildren()[i].mType);
            child->setIcon(1, mNameIcon);
            child->setText(1, typeInfo->GetChildren()[i].mName);
            childTypes.append(child);
        }
        typeWidgetItem->addChildren(childTypes);
    }
    else if (modifierName == "Enum")
    {
        QList<QTreeWidgetItem*> childTypes;        
        for (int i = 0; i < typeInfo->GetEnumsCount(); ++i)
        {
            QTreeWidgetItem* child = new QTreeWidgetItem(QStringList(QString(typeInfo->GetEnums()[i])));
            child->setIcon(0,mEnumIcon);
            childTypes.append(child);
        }
        typeWidgetItem->addChildren(childTypes);
    }
    else if (modifierName == "ObjectRef")
    {
        QList<QTreeWidgetItem*> childTypes;        
        for (int i = 0; i < typeInfo->GetPropertiesCount(); ++i)
        {
            QTreeWidgetItem* child = CreateTypeItem(typeInfo->GetProperties()[i].mType);
            child->setIcon(1, mNameIcon);
            child->setText(1, typeInfo->GetProperties()[i].mName);
            childTypes.append(child);
        }
        typeWidgetItem->addChildren(childTypes);
    }
    return typeWidgetItem;
}

QTreeWidgetItem* BlockScriptLibraryDockWidget::CreateFunItem(const Pegasus::App::IBsFunInfo* funInfo)
{
    QTreeWidgetItem* funInfoWidget = new QTreeWidgetItem(QStringList(QString(funInfo->GetName())));
    funInfoWidget->setIcon(0,mFunctionIcon);
    QList<QTreeWidgetItem*> elements;


    const Pegasus::App::NameTypePair* bsArgs = funInfo->GetArgs();

    for (int i = 0; i < funInfo->GetArgCount(); ++i)
    {
        QTreeWidgetItem* ti = CreateTypeItem(bsArgs[i].mType);
        ti->setText(1, QString(bsArgs[i].mName));
        ti->setIcon(1, mNameIcon);
        elements.append(ti);
    }

    QTreeWidgetItem* retEl = CreateTypeItem(funInfo->GetReturnType());
    retEl->setText(1, QString("return"));
    retEl->setIcon(1, mReturnIcon);
    
    elements.append(retEl);
    
    funInfoWidget->addChildren(elements);
    
    return funInfoWidget;
}

void BlockScriptLibraryDockWidget::OnUIForAppClosed()
{
    mView->clear();
}
