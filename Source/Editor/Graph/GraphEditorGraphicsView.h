/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphEditorGraphicsView.h
//! \author	Karolyn Boulanger
//! \date	06th June 2014
//! \brief	Generic graphics view for a graph editor (texture, mesh, etc.)

#ifndef EDITOR_GRAPHEDITORGRAPHICSVIEW_H
#define EDITOR_GRAPHEDITORGRAPHICSVIEW_H

#include <QGraphicsView>

class GraphEditorGraphicsScene;


//! Generic graphics view for a graph editor (texture, mesh, etc.)
class GraphEditorGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    GraphEditorGraphicsView(QWidget *parent = 0);
    ~GraphEditorGraphicsView();

    //! Get the scene associated with the view
    inline GraphEditorGraphicsScene* GetScene() const { return mScene; }

    //------------------------------------------------------------------------------------

private:

    //! Scene associated with the view
    GraphEditorGraphicsScene* mScene;
};


#endif // EDITOR_GRAPHEDITORGRAPHICSVIEW_H
