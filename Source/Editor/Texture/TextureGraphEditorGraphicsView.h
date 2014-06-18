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

namespace Pegasus {
    namespace Texture {
        class ITextureNodeProxy;
    }
}


class TextureGraphEditorGraphicsView : public GraphEditorGraphicsView
{
    Q_OBJECT

public:

    //! Constructor
    //! \param textureProxy Texture proxy associated with the graphics view (!= nullptr)
    //! \param parent Parent widget of the graphics view
    TextureGraphEditorGraphicsView(Pegasus::Texture::ITextureNodeProxy * textureProxy,
                                   QWidget * parent = 0);

    //! Destructor
    ~TextureGraphEditorGraphicsView();


    //! Get the texture proxy associated with the graphics view
    //! \return Texture proxy associated with the graphics view (!= nullptr)
    inline Pegasus::Texture::ITextureNodeProxy * GetTextureProxy() const { return mTextureProxy; }

    //------------------------------------------------------------------------------------
    
private:

    //! Texture proxy associated with the graphics view (!= nullptr)
    Pegasus::Texture::ITextureNodeProxy * const mTextureProxy;
};


#endif // EDITOR_TEXTUREGRAPHEDITORGRAPHICSVIEW_H
