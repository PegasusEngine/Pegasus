/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramTreeModel.cpp
//! \author	Kleber Garcia
//! \date	13 Match 2014
//! \brief	Model representation of data. Should be a lite wrapper of the ShaderManagerProxy

#include "Editor.h"
#include "Log.h"
#include "Assertion.h"
#include "ShaderLibrary/ProgramTreeModel.h"
#include "ShaderLibrary/ShaderManagerEventListener.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"

ProgramTreeModel::ProgramTreeModel(
    QObject * parent
)
    : QAbstractItemModel(parent),
      mShaderManager(nullptr)

{
    mWorkingIcon.addFile(QStringLiteral(":/ShaderLibrary/success.png"), QSize(), QIcon::Normal, QIcon::On);   
    mWarningIcon.addFile(QStringLiteral(":/ShaderLibrary/error.png"), QSize(), QIcon::Normal, QIcon::On);   
}

//----------------------------------------------------------------------------------------

ProgramTreeModel::~ProgramTreeModel()
{
}

//----------------------------------------------------------------------------------------

bool ProgramTreeModel::hasChildren(const QModelIndex& parent) const
{
    if (IsValidTree())
    {
        if (IsRootIndex(parent))
        {
            return mShaderManager->GetProgramCount() > 0;
        }
        else if (IsProgramIndex(parent))//node is a program
        {
            const Pegasus::Shader::IProgramProxy * programProxy = mShaderManager->GetProgram(parent.row());
            if (programProxy != nullptr)
            {
                return programProxy->GetShaderCount() > 0;
            }
        }
    }
    return false;
}

//----------------------------------------------------------------------------------------

int ProgramTreeModel::rowCount (const QModelIndex &parent) const
{
    if (IsValidTree())
    {
        if (IsRootIndex(parent))
        {
            return mShaderManager->GetProgramCount();
        }
        else if (IsProgramIndex(parent))//node is a program
        {
            const Pegasus::Shader::IProgramProxy * programProxy = mShaderManager->GetProgram(parent.row()); 
            if (programProxy != nullptr)
            {
                return programProxy->GetShaderCount();
            }
        }
    }
    return 0;
}

//----------------------------------------------------------------------------------------

int ProgramTreeModel::columnCount (const QModelIndex &parent) const
{
    if (IsValidTree())
    {
        return 2;
    }
    return 0;
}

//----------------------------------------------------------------------------------------

QModelIndex ProgramTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (IsRootIndex(parent)) //if parent is root, then this must be a program
    {
        return createIndex(row, column, (int)-1); //invalid root index
    }
    else if (IsProgramIndex(parent)) // if parent is program, this must be a shader
    {
        return createIndex(row, column, parent.row());
    }
    return QModelIndex(); //otherwise return an invalid model index
}

//----------------------------------------------------------------------------------------

QModelIndex ProgramTreeModel::parent (const QModelIndex &child) const
{
    if (IsRootIndex(child)) // the root node has no parent
    {
        return QModelIndex();
    }
    else if (IsProgramIndex(child))
    {
        return createIndex(-1, -1, (quintptr)0);
    }
    else if (IsShaderIndex(child))
    {
        return createIndex(child.internalId(), child.row(), -1);//recreate the parent, which is a program
    }
    return QModelIndex();
}

//----------------------------------------------------------------------------------------

QVariant ProgramTreeModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0)
    {
        switch(role)
        {
        case Qt::DisplayRole:
            {
                return GetStringDataFromIndex(index);
            }
        }
    }
    else if (index.column() == 1)
    {
        switch(role)
        {
        case Qt::DecorationRole:
            {
               return GetIconFromIndex(index);
            }
        }
    }
    return QVariant();
}

//----------------------------------------------------------------------------------------

QVariant ProgramTreeModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

//----------------------------------------------------------------------------------------

bool ProgramTreeModel::IsRootIndex(const QModelIndex &idx) const
{
    return idx.row() == -1 && idx.column() == -1;
}

//----------------------------------------------------------------------------------------

bool ProgramTreeModel::IsProgramIndex(const QModelIndex& idx) const
{
    return idx.internalId() == -1;
}

//----------------------------------------------------------------------------------------

bool ProgramTreeModel::IsShaderIndex(const QModelIndex& idx) const
{
    return idx.internalId() >= 0;
}

//----------------------------------------------------------------------------------------

const char * ProgramTreeModel::GetStringDataFromIndex(const QModelIndex& index) const
{
    if (IsProgramIndex(index)) // is program node
    {
        const Pegasus::Shader::IProgramProxy * proxy = mShaderManager->GetProgram(index.row()); 
        if (proxy != nullptr)
        {
            return proxy->GetName();
        }
    }
    else
    {
        Pegasus::Shader::IProgramProxy * proxy = mShaderManager->GetProgram(index.internalId()); 
        if (proxy != nullptr)
        {
            Pegasus::Shader::IShaderProxy * shaderProxy = proxy->GetShader(index.row());
            if (shaderProxy != nullptr)
            {
                return shaderProxy->GetName();
            }
        }
    }
    return "0xdeadbeef";
}
//----------------------------------------------------------------------------------------

QIcon ProgramTreeModel::GetIconFromIndex(const QModelIndex& index) const
{
    if (IsProgramIndex(index)) // is program node
    {
        const Pegasus::Shader::IProgramProxy * proxy = mShaderManager->GetProgram(index.row()); 
        if (proxy != nullptr)
        {
            ProgramUserData * userData = static_cast<ProgramUserData*>(proxy->GetUserData());
            if (userData != nullptr)
            {
                return userData->IsValid() ? mWorkingIcon : mWarningIcon;
            }
            else
            {
                return mWorkingIcon;
            }
        }
    }
    else
    {
        Pegasus::Shader::IProgramProxy * proxy = mShaderManager->GetProgram(index.internalId()); 
        if (proxy != nullptr)
        {
            Pegasus::Shader::IShaderProxy * shaderProxy = proxy->GetShader(index.row());
            if (shaderProxy != nullptr)
            {
                ShaderUserData * userData =  static_cast<ShaderUserData*>(shaderProxy->GetUserData());
                if (userData != nullptr)
                {
                    return userData->IsValid() ? mWorkingIcon : mWarningIcon;
                }

            }
        }
    }

    //default
    return mWorkingIcon;
}
//----------------------------------------------------------------------------------------

void ProgramTreeModel::OnAppLoaded() 
{
    Application * app = Editor::GetInstance().GetApplicationManager().GetApplication(); 
    ED_ASSERTSTR(app != nullptr, "App cannot be nulL!");
    if (app != nullptr)
    {
        Pegasus::App::IApplicationProxy * appProxy = app->GetApplicationProxy();
        ED_ASSERTSTR(appProxy != nullptr, "App proxy cannot be null!");
        if (appProxy != nullptr)
        {
            mShaderManager = appProxy->GetShaderManagerProxy();
            ED_ASSERTSTR(mShaderManager != nullptr, "Failed retrieving shader manager");            
        }
    }
}

//----------------------------------------------------------------------------------------

Pegasus::Shader::IShaderProxy * ProgramTreeModel::TranslateShaderIndex(const QModelIndex& idx)
{
    if (mShaderManager != nullptr)
    {
        Pegasus::Shader::IProgramProxy * proxy = mShaderManager->GetProgram(idx.internalId()); 
        if (proxy != nullptr)
        {
            return proxy->GetShader(idx.row());
        }
    }
    return nullptr;
}

//----------------------------------------------------------------------------------------
Pegasus::Shader::IProgramProxy * ProgramTreeModel::TranslateProgramIndex(const QModelIndex& idx)
{
    if (mShaderManager != nullptr)
    {
        return mShaderManager->GetProgram(idx.row()); 
    }
    return nullptr;
}


//----------------------------------------------------------------------------------------

void ProgramTreeModel::OnAppDestroyed()
{
    //set everything off
    mShaderManager = nullptr;
}
