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
        class IShaderProxy;
        class IProgramProxy;
    }
}

//! Model class, represents the data being held in a tree form
class ProgramTreeModel : public QAbstractItemModel
{
public:
    //! Constructor
    ProgramTreeModel(QObject * parent);

    //! Destructor
    virtual ~ProgramTreeModel();

    //! Returns the number of children for a parent node
    //! \param index parent node to pass. 
    //! \return count of rows
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //! Returns the count of columns from a parent node
    //! \param parent index
    //! \return count of columns
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //! Gets the parent model index
    //! \return
    virtual QModelIndex parent(const QModelIndex &child) const ;

    //! Index encoding
    //! \return returns an index encoded as: 
    //!        row => the id of the shader or program
    //!        column => 0 for the name of shader / program, 1 for the status icon
    //!        internalId => -1 for a program shader, [0-N] for a shader with the 
    //!                         respective parent program id
    virtual QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;

    //! data representation of a qindex
    //! \brief a QModelIndex gets converted into a shader and program lookups
    //! \return the variant that represents the tree view data
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    //! data for headers
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //! gets info if has children node for a parent
    //! \return result boolean of test
    virtual bool hasChildren (const QModelIndex& parent) const;

    //! test for root empty index
    //! \return result boolean of test
    bool IsRootIndex(const QModelIndex &idx) const;

    //! determines if this is an index for a program
    //! \return result boolean of test
    bool IsProgramIndex (const QModelIndex& idx) const; 

    //! determines if this is an index for a  shader
    //! \return result boolean of test
    bool IsShaderIndex(const QModelIndex& idx) const;

    //! callback when application is done loaded
    void OnAppLoaded();

    //! callback when application is done and destroyed
    void OnAppDestroyed();

    //! translates a model index to a shader proxy. No hazard check is done to the idx.
    //! make sure that isProgramIndex is called on the questioned index
    //! \param idx the index to translate
    //! make sure that IsShaderIndex is called on the questioned index. 
    Pegasus::Shader::IShaderProxy * TranslateShaderIndex(const QModelIndex& idx);

    //! translates a model index to a program proxy. No hazard check is done to the idx.
    //! make sure that isProgramIndex is called on the questioned index
    //! \param idx the index to translate
    //! \return gets the program proxy from the specified model index
    Pegasus::Shader::IProgramProxy * TranslateProgramIndex(const QModelIndex& idx);

private:
    //! major test to determine if a tree view can be generated
    bool IsValidTree() const { return mShaderManager != nullptr; }

    //! gets the shader or program's internal name from an index
    //! \return the string data from the program or shader passed in index
    const char * GetStringDataFromIndex(const QModelIndex& index) const;

    //! miscelaneous icons and the invaluable shader manager proxy
    QIcon mWorkingIcon;
    QIcon mWarningIcon;
    Pegasus::Shader::IShaderManagerProxy * mShaderManager;
};

#endif
