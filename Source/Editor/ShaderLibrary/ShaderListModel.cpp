/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderListModel.cpp
//! \author Kleber Garcia
//! \date   21st March 2013
//! \brief  Pegasus Shader list  model

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "ShaderLibrary/ShaderListModel.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"

//----------------------------------------------------------------------------------------

ShaderListModel::ShaderListModel(QObject * parent)
: 
    QAbstractListModel(parent),
    mShaderManager(nullptr)
    
{
    mWorkingIcon.addFile(QStringLiteral(":/ShaderLibrary/success.png"), QSize(), QIcon::Normal, QIcon::On);   
    mWarningIcon.addFile(QStringLiteral(":/ShaderLibrary/error.png"), QSize(), QIcon::Normal, QIcon::On);   
}

//----------------------------------------------------------------------------------------

ShaderListModel::~ShaderListModel()
{
}

//----------------------------------------------------------------------------------------

int ShaderListModel::rowCount (const QModelIndex& parent) const
{
    if (mShaderManager != nullptr)
    {
        return mShaderManager->GetShaderCount();
    }
    return 0;
}

//----------------------------------------------------------------------------------------

QVariant ShaderListModel::data(const QModelIndex &index, int role) const
{
    int id = index.row();
    if (id >= 0 && id < mShaderManager->GetShaderCount())
    {
        Pegasus::Shader::IShaderProxy * proxy = mShaderManager->GetShader(id);
        switch (role)
        {
        case Qt::DisplayRole:
            return proxy->GetName();
            break;
        case Qt::DecorationRole:
            return mWorkingIcon;
            break;
        }
    }
    return QVariant();
}

//----------------------------------------------------------------------------------------

Pegasus::Shader::IShaderProxy * ShaderListModel::Translate(const QModelIndex& index)
{
    if (mShaderManager != nullptr)
    {
        return mShaderManager->GetShader(index.row());
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------

QVariant ShaderListModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

//----------------------------------------------------------------------------------------

void ShaderListModel::OnAppLoaded()
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            mShaderManager = appProxy->GetShaderManager();
            ED_ASSERTSTR(mShaderManager != nullptr, "Failed retrieving shader manager");            
        }
    }
}

//----------------------------------------------------------------------------------------

void ShaderListModel::OnAppDestroyed()
{
    //set everything off
    mShaderManager = nullptr;
}

