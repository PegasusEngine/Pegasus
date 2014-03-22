/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderListModel.h
//! \author Kleber Garcia
//! \date   21st March 2013
//! \brief  Pegasus Shader list  model. Represents the flat list of shaders.

#ifndef EDITOR_SHADERLISTMODEL_H
#define EDITOR_SHADERLISTMODEL_H
#include <QAbstractListModel>

namespace Pegasus {
    namespace Shader {
        class IShaderManagerProxy;
    }
}

//! Model class, represents the data being held in a list form
class ShaderListModel : public QAbstractListModel
{
public:

    //! Constructor
    ShaderListModel(QObject * parent);

    //! Destructor
    virtual ~ShaderListModel();

    //! gets the row count (how many shaders we have, ignores parent)
    //! \return shader list length
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //! data callback
    //! \return variant data callback for shaders
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    //! data for headers
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    //! callback that sets the shader manager proxy
    void OnAppLoaded();

    //! callback that clears the shader manager proxy
    void OnAppDestroyed();

private:
    QIcon mWorkingIcon;
    QIcon mWarningIcon;
    Pegasus::Shader::IShaderManagerProxy * mShaderManager;
};

#endif

