/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorGraphicsView.cpp
//! \author	Kevin Boulanger
//! \date	06th June 2014
//! \brief	Specialization of the graph editor for textures

#include "Texture/TextureGraphEditorGraphicsView.h"
#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"


TextureGraphEditorGraphicsView::TextureGraphEditorGraphicsView(Pegasus::Texture::ITextureNodeProxy * textureProxy,
                                                               QWidget * parent)
:   GraphEditorGraphicsView(parent),
    mTextureProxy(textureProxy)
{
    //! \todo Transmit the texture proxy to the parent graphics view, in a generic way

    ED_ASSERTSTR(textureProxy != nullptr, "Invalid texture proxy given to a texture editor tab");
    ED_ASSERTSTR(textureProxy->GetNodeType() == Pegasus::Texture::ITextureNodeProxy::NODETYPE_OUTPUT, "Invalid node type for a texture node proxy");
}

//----------------------------------------------------------------------------------------

TextureGraphEditorGraphicsView::~TextureGraphEditorGraphicsView()
{
}
