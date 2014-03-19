/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramTreeModel.h
//! \author	Kleber Garcia
//! \date	16 Match 2014
//! \brief	Model representation of data. Should be a lite wrapper of the ShaderManagerProxy

#ifndef EDITOR_PROGRAMTREEVIEWMODEL_H
#define EDITOR_PROGRAMTREEVIEWMODEL_H
#include <QAbstractItemModel>

namespace Pegasus {
    namespace Shader {
        class IShaderManagerProxy;
    }
}

class ProgramTreeModel : public QAbstractItemModel
{
public:
    ProgramTreeModel(QObject * parent);
    virtual ~ProgramTreeModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const ;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool hasChildren (const QModelIndex& parent) const;
    bool IsRootIndex(const QModelIndex &idx) const;
    bool IsProgramIndex (const QModelIndex& idx) const; 
    bool IsShaderIndex(const QModelIndex& idx) const;

    void OnAppLoaded();
    void OnAppDestroyed();
private:
    bool IsValidTree() const { return mShaderManager != nullptr; }
    const char * GetStringDataFromIndex(const QModelIndex& index) const;
    QIcon mWorkingIcon;
    QIcon mWarningIcon;
    Pegasus::Shader::IShaderManagerProxy * mShaderManager;
};

#endif
