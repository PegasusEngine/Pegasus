/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorGraphicsView.cpp
//! \author	Karolyn Boulanger
//! \date	06th June 2014
//! \brief	Specialization of the graph editor for textures

#include "Texture/TextureGraphEditorGraphicsView.h"
#include "Widgets/PegasusDockWidget.h"

#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"


TextureGraphEditorGraphicsView::TextureGraphEditorGraphicsView(Pegasus::Texture::ITextureNodeProxy * textureProxy,
                                                               PegasusDockWidget* messenger,
                                                               QWidget * parent)
:   GraphEditorGraphicsView(parent)
,   mTextureProxy(textureProxy)
,   mMessenger(messenger)
,   mTextureProxyHandle(INVALID_TEXTURE_NODE_PROXY_HANDLE)
{
    //! \todo Transmit the texture proxy to the parent graphics view, in a generic way

    ED_ASSERTSTR(textureProxy != nullptr, "Invalid texture proxy given to a texture editor tab");
    ED_ASSERTSTR(textureProxy->GetNodeType() == Pegasus::Texture::ITextureNodeProxy::NODETYPE_OUTPUT, "Invalid node type for a texture node proxy");
    ED_ASSERTSTR(messenger != nullptr, "Invalid messenger given to a texture editor tab");

    mObserver = new TextureGraphEditorGraphicsView::Observer(this);

    SendGraphOpenMessage();
}

//----------------------------------------------------------------------------------------

TextureGraphEditorGraphicsView::~TextureGraphEditorGraphicsView()
{
    SendGraphCloseMessage();

    delete mObserver;
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::SendGraphOpenMessage()
{
    ED_ASSERTSTR(mMessenger != nullptr, "A messenger must be set in order for the texture graph editor to work.");
    ED_ASSERT(mTextureProxyHandle == INVALID_TEXTURE_NODE_PROXY_HANDLE);

    // Request to open this graph proxy
    GraphIOMessageController::Message msg(GraphIOMessageController::Message::OPEN);
    msg.SetTextureNodeObserver(mObserver);
    msg.SetTextureNodeProxy(mTextureProxy);
    mMessenger->SendGraphIoMessage(msg);
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::SendGraphCloseMessage()
{
    ED_ASSERTSTR(mMessenger != nullptr, "A messenger must be set in order for the texture graph editor to work.");

    if (mTextureProxyHandle != INVALID_TEXTURE_NODE_PROXY_HANDLE)
    {
        // Send a message to close the observer of this handle
        GraphIOMessageController::Message msg(GraphIOMessageController::Message::CLOSE);
        msg.SetTextureNodeObserver(mObserver);
        msg.SetTextureNodeProxyHandle(mTextureProxyHandle);
        mMessenger->SendGraphIoMessage(msg);
        Clear();
    }
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::OnInitialized(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy)
{
    //! \todo Build the graph out of the message data
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::OnUpdated(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement>& updateElements)
{
    //! \todo Update the graph out of the message data
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::Clear()
{
    //! \todo Clear the graph from the view
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::Observer::OnInitialized(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy)
{
    mParent->OnInitialized(handle, textureProxy);
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::Observer::OnUpdated(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement>& updateElements)
{
    mParent->OnUpdated(handle, updateElements);
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::Observer::OnShutdown(TextureNodeProxyHandle handle)
{
    OnShutdownInternal(handle);
}

//----------------------------------------------------------------------------------------

void TextureGraphEditorGraphicsView::Observer::OnShutdownInternal(TextureNodeProxyHandle handle)
{
    mParent->Clear();
}
