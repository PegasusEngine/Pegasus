/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceCodeListModel.h
//! \author Kleber Garcia
//! \date   21st March 2013
//! \brief  Pegasus Source Code list  model. Represents the flat list of source code.

#ifndef EDITOR_CODELISTMODEL_H
#define EDITOR_CODELISTMODEL_H
#include <QAbstractListModel>

namespace Pegasus {
    namespace Core {
        class ISourceCodeManagerProxy;
        class ISourceCodeProxy;
    }
}

//! Model class, represents the data being held in a list form
class SourceCodeListModel : public QAbstractListModel
{
public:

    //! Constructor
    SourceCodeListModel(QObject * parent);

    //! Destructor
    virtual ~SourceCodeListModel();

    //! gets the row count (how many source code we have, ignores parent)
    //! \return source code list length
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //! data callback
    //! \return variant data callback for source code
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    //! data for headers
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    //! callback that sets the source code manager proxy
    void OnAppLoaded(Pegasus::Core::ISourceCodeManagerProxy* dataProvider);

    //! callback that clears the source code manager proxy
    void OnAppDestroyed();

    //! Translates an index to a source code proxy
    Pegasus::Core::ISourceCodeProxy * Translate(const QModelIndex& index);

private:
    QIcon mWorkingIcon;
    QIcon mWarningIcon;
    Pegasus::Core::ISourceCodeManagerProxy * mSourceCodeManager;
};

#endif

