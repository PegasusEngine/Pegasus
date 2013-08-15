/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleChannelColorTableView.cpp
//! \author	Kevin Boulanger
//! \date	06th August 2013
//! \brief	Table view to show the colors associated with the log channels

#include "Console/ConsoleChannelColorTable/ConsoleChannelColorTableView.h"
#include "Console/ConsoleChannelColorTable/ConsoleChannelColorTableModel.h"

#include <QHeaderView>
#include <QColorDialog>


ConsoleChannelColorTableView::ConsoleChannelColorTableView(QWidget * parent)
:   QTableView(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setWindowTitle(tr("Log Channel Color Table"));
	setAlternatingRowColors(true);
	setSelectionMode(SingleSelection);

	verticalHeader()->hide();
	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader()->setSectionsMovable(false);
    verticalHeader()->setSectionsClickable(false);

    horizontalHeader()->show();
	horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	horizontalHeader()->setSectionsMovable(false);
    horizontalHeader()->setSectionsClickable(false);

    // Create the model.
    // It should be done outside of this constructor in theory, to follow
    // the MVC convention. However, only one model will ever be used
    // with this view, and it makes the calling code messier.
	setModel(new ConsoleChannelColorTableModel);

    // Resize the rows of the model so they are all of equal height
    // and smaller than the default size
    const int numRows = model()->rowCount();
    for (int row = 0; row < numRows; row++)
    {
    	verticalHeader()->resizeSection(row, 18);
    }

    // Connect the clicked() signal to open the color picker if desired
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(ColorClicked(const QModelIndex &)));
}

//----------------------------------------------------------------------------------------

ConsoleChannelColorTableView::~ConsoleChannelColorTableView()
{
}

//----------------------------------------------------------------------------------------

void ConsoleChannelColorTableView::Refresh()
{
    (static_cast<ConsoleChannelColorTableModel *>(model()))->Refresh();
}

//----------------------------------------------------------------------------------------

void ConsoleChannelColorTableView::ColorClicked(const QModelIndex & index)
{
    if (index.isValid() && (index.column() == 1))
    {
        // Get the current color associated with the log channel
        const Settings * const settings = Editor::GetSettings();
        const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
        QColor currentColor = settings->GetConsoleTextColorForLogChannel(logChannel);

        // Open the color picker
        QColor newColor = QColorDialog::getColor(currentColor, this);
	    if (newColor.isValid())
	    {
            // Set the model data (updates the view at the same time)
	        model()->setData(index, newColor);
	    }
    }
}
