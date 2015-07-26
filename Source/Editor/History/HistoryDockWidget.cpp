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


HistoryDockWidget::HistoryDockWidget(QWidget * parent)
:   QDockWidget(parent)
{
     // Set the dock widget parameters
    setWindowTitle(tr("History"));
    setObjectName("HistoryDockWidget");
    setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create the history view
    mUndoView = new QUndoView(this);
    mUndoView->setEmptyLabel(tr("<empty>"));
    mUndoView->setCleanIcon(QIcon(":/Toolbar/File/SaveScene24.png"));

    // Set the history view as the main widget
    setWidget(mUndoView);
}

//----------------------------------------------------------------------------------------

HistoryDockWidget::~HistoryDockWidget()
{
}

void HistoryDockWidget::SetUndoStack(QUndoStack* undoStack)
{
    mUndoView->setStack(undoStack);
    mUndoView->doItemsLayout();
}

void HistoryDockWidget::TriggerUndo()
{
    if (mUndoView->stack() != nullptr)
    {
        mUndoView->stack()->undo();
    }
}

void HistoryDockWidget::TriggerRedo()
{
    if (mUndoView->stack() != nullptr)
    {
        mUndoView->stack()->redo();
    }
}
