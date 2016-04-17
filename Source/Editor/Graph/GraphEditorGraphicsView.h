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

class GraphNodeOutputGraphicsItem;
class GraphNodeInputGraphicsItem;


//! Generic graphics view for a graph editor (texture, mesh, etc.)
class GraphEditorGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    GraphEditorGraphicsView(QWidget *parent = 0);
    ~GraphEditorGraphicsView();


    //! Create a new node item in the view
    //! \param title Name that appears in the header of the node
    //! \param inputList List of inputs for the new node
    void CreateNode(const QString& title, const QList<QString>& inputList);

    //! Create a new connection item between a source node output and a destination node input
    //! \param srcOutput Output item of the source node
    //! \param dstInput One of the input items of the destination node
    void CreateConnection(const GraphNodeOutputGraphicsItem* srcOutput,
                          const GraphNodeInputGraphicsItem* dstInput);
};


#endif // EDITOR_GRAPHEDITORGRAPHICSVIEW_H
