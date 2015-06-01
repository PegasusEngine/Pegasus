/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	HistoryDockWidget.cpp
//! \author	Kevin Boulanger
//! \date	20th January 2014
//! \brief	Dock widget containing the history view (for undo/redo)

#include "History/HistoryDockWidget.h"

#include <QUndoView>
#include <QUndoStack>


HistoryDockWidget::HistoryDockWidget(QUndoStack * undoStack, QWidget * parent)
:   QDockWidget(parent)
{
    ED_ASSERTSTR(undoStack != nullptr, "Invalid undo stack given to the history dock widget");

    // Set the dock widget parameters
    setWindowTitle(tr("History"));
    setObjectName("HistoryDockWidget");
    setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create the history view
    mUndoView = new QUndoView(undoStack, this);
    mUndoView->setEmptyLabel(tr("<empty>"));
    mUndoView->setCleanIcon(QIcon(":/Toolbar/File/SaveScene24.png"));

    // Set the history view as the main widget
    setWidget(mUndoView);
}

//----------------------------------------------------------------------------------------

HistoryDockWidget::~HistoryDockWidget()
{
}
