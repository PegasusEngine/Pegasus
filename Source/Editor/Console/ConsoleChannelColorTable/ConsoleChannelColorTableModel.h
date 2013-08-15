/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleChannelColorTableModel.h
//! \author	Kevin Boulanger
//! \date	06th August 2013
//! \brief	Table model to show the colors associated with the log channels

#ifndef EDITOR_CONSOLECHANNELCOLORTABLEMODEL_H
#define EDITOR_CONSOLECHANNELCOLORTABLEMODEL_H

#include <QAbstractTableModel>


class ConsoleChannelColorTableModel: public QAbstractTableModel
{
	Q_OBJECT

public:

	//! Constructor
	//! \param parent Parent of the model
	ConsoleChannelColorTableModel(QObject * parent = 0);

	//! Destructor
	~ConsoleChannelColorTableModel();

    //! Refresh the model (and its associated views) after an update of the colors
    void Refresh();

	//------------------------------------------------------------------------------------

public:
    
    //! Get the data associated with a node of the table
	//! \param index Index of the table item
	//! \param role Role of the item to get
	//! \return Member name or member name of the scene node associated with the index
	QVariant data(const QModelIndex & index, int role) const;

	//! Set the color of a log channel
	//! \param index Index of the item to update
	//! \param value New value of the color
	//! \param role Role
	//! \return True if successful (valid value)
	bool setData(const QModelIndex & index, const QVariant & value,
				 int role = Qt::EditRole);

	//! Get flags to set the table items as read-only and selectable
	//! \return Qt flags for read-only and selectable table items
	Qt::ItemFlags flags(const QModelIndex & index) const;

	//! Get data associated with the table view header
	//! \param section Section of the data to get
	//! \param orientation Orientation of the header
	//! \param role Role of the header data
	//! \return Data for the tree view header
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	//! Get the number of log channels to display in the table
	//! \param parent Ignored
	//! \return Number of log channels
	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	//! Return the number of columns
	//! \param parent Ignored
	//! \return 2
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
};


#endif  // EDITOR_CONSOLECHANNELCOLORTABLEMODEL_H
