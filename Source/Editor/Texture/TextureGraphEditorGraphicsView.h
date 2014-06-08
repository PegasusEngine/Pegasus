/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorGraphicsView.h
//! \author	Kevin Boulanger
//! \date	06th June 2014
//! \brief	Specialization of the graph editor for textures

#ifndef EDITOR_TEXTUREGRAPHEDITORGRAPHICSVIEW_H
#define EDITOR_TEXTUREGRAPHEDITORGRAPHICSVIEW_H

#include "Graph/GraphEditorGraphicsView.h"


class TextureGraphEditorGraphicsView : public GraphEditorGraphicsView
{
    Q_OBJECT

public:

    TextureGraphEditorGraphicsView(QWidget *parent = 0);
    ~TextureGraphEditorGraphicsView();

    //------------------------------------------------------------------------------------
    
private:
    
};


#endif // EDITOR_TEXTUREGRAPHEDITORGRAPHICSVIEW_H
