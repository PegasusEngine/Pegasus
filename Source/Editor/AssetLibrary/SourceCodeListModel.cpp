/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceCodeListModel.cpp
//! \author Kleber Garcia
//! \date   21st March 2013
//! \brief  Pegasus Source Code list  model

#include "Log.h"
#include "Assertion.h"
#include "AssetLibrary/SourceCodeListModel.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeManagerProxy.h"


//----------------------------------------------------------------------------------------

SourceCodeListModel::SourceCodeListModel(QObject * parent)
: 
    QAbstractListModel(parent),
    mSourceCodeManager(nullptr)
    
{
    mWorkingIcon.addFile(QStringLiteral(":/AssetLibrary/success.png"), QSize(), QIcon::Normal, QIcon::On);   
    mWarningIcon.addFile(QStringLiteral(":/AssetLibrary/error.png"), QSize(), QIcon::Normal, QIcon::On);   
}

//----------------------------------------------------------------------------------------

SourceCodeListModel::~SourceCodeListModel()
{
}

//----------------------------------------------------------------------------------------

int SourceCodeListModel::rowCount (const QModelIndex& parent) const
{
    if (mSourceCodeManager != nullptr)
    {
        return mSourceCodeManager->GetSourceCount();
    }
    return 0;
}

//----------------------------------------------------------------------------------------

QVariant SourceCodeListModel::data(const QModelIndex &index, int role) const
{
    int id = index.row();
    if (id >= 0 && id < mSourceCodeManager->GetSourceCount())
    {
        Pegasus::Core::ISourceCodeProxy * proxy = mSourceCodeManager->GetSource(id);
        switch (role)
        {
        case Qt::DisplayRole:
            return proxy->GetName();
            break;
        case Qt::DecorationRole:
            {
                CodeUserData * userData = static_cast<CodeUserData*>(proxy->GetUserData());

                if (userData!=nullptr)
                {
                    ED_ASSERT(!userData->IsProgram());
                    return userData->IsValid() ? mWorkingIcon : mWarningIcon;
                }
                else
                {
                    return mWorkingIcon;
                }
                
            }
            break;
        }
    }
    return QVariant();
}

//----------------------------------------------------------------------------------------

Pegasus::Core::ISourceCodeProxy * SourceCodeListModel::Translate(const QModelIndex& index)
{
    if (mSourceCodeManager != nullptr)
    {
        return mSourceCodeManager->GetSource(index.row());
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------

QVariant SourceCodeListModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

//----------------------------------------------------------------------------------------

void SourceCodeListModel::OnAppLoaded(Pegasus::Core::ISourceCodeManagerProxy* dataProvider)
{
    ED_ASSERT(dataProvider != nullptr);
    mSourceCodeManager = dataProvider; 
}

//----------------------------------------------------------------------------------------

void SourceCodeListModel::OnAppDestroyed()
{
    //set everything off
    mSourceCodeManager = nullptr;
}

