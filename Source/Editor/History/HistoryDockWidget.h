/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	HistoryDockWidget.h
//! \author	Kevin Boulanger
//! \date	20th January 2014
//! \brief	Dock widget containing the history view (for undo/redo)

#ifndef EDITOR_HISTORYDOCKWIDGET_H
#define EDITOR_HISTORYDOCKWIDGET_H

#include <QDockWidget>

class QUndoStack;
class QUndoView;


//! Dock widget containing the history view (for undo/redo)
class HistoryDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    //! Constructor
    //! \param undoStack Undo stack attached to the undo view
    //! \param parent Parent widget
    HistoryDockWidget(QWidget * parent);

    //! Destructor
    virtual ~HistoryDockWidget();

    //! Sets the undo view
    void SetUndoStack(QUndoStack* stack);

public slots:

    //! Triggers an undo action
    void TriggerUndo();

    //! Triggers a redo action
    void TriggerRedo();

private:

    //! The undo view
    QUndoView* mUndoView;

};


#endif  // EDITOR_HISTORYDOCKWIDGET_H
