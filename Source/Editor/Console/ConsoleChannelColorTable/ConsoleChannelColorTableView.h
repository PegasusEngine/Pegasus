/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleChannelColorTableView.h
//! \author	Karolyn Boulanger
//! \date	06th August 2013
//! \brief	Table view to show the colors associated with the log channels

#ifndef EDITOR_CONSOLECHANNELCOLORTABLEVIEW_H
#define EDITOR_CONSOLECHANNELCOLORTABLEVIEW_H

#include <QTableView>


//! Table view to show the colors associated with the log channels
class ConsoleChannelColorTableView : public QTableView
{
	Q_OBJECT

public:

	//! Constructor
	//! \param parent Parent of the widget
	ConsoleChannelColorTableView(QWidget * parent = 0);

	//! Destructor
	virtual ~ConsoleChannelColorTableView();

    //! Refresh the associated model and the entire view after an update of the colors
    void Refresh();

    //------------------------------------------------------------------------------------

private slots:

    //! Called when one of the table view cells has been clicked on
    //! \param index Index of the table view cell that has been clicked on
    void ColorClicked(const QModelIndex & index);
};


#endif  // EDITOR_CONSOLECHANNELCOLORTABLEVIEW_H
