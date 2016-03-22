/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorGraphicsView.h
//! \author	Karolyn Boulanger
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

class PegasusDockWidget;


class TextureGraphEditorGraphicsView : public GraphEditorGraphicsView
{
    Q_OBJECT

public:

    //! Constructor
    //! \param textureProxy Texture proxy associated with the graphics view (!= nullptr)
    //! \param messenger Messenger used to send messages to the application
    //! \param parent Parent widget of the graphics view
    TextureGraphEditorGraphicsView(Pegasus::Texture::ITextureNodeProxy * textureProxy,
                                   PegasusDockWidget* messenger,
                                   QWidget * parent = 0);

    //! Destructor
    ~TextureGraphEditorGraphicsView();


    //! Get the texture proxy associated with the graphics view
    //! \return Texture proxy associated with the graphics view (!= nullptr)
    inline Pegasus::Texture::ITextureNodeProxy * GetTextureProxy() const { return mTextureProxy; }

    //------------------------------------------------------------------------------------
    
private:

    //! Send an open message to the graph IO controller
    void SendGraphOpenMessage();

    //! Send a close message to the graph IO controller if there is a handle defined
    void SendGraphCloseMessage();

    //! Called when a message is received and we want to build the graph out of the data
    //! \param handle Unique handle of the texture proxy
    //! \param textureProxy Proxy of the output node of the texture graph
    void OnInitialized(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy);

    //! Called when a message is received and we want to update elements of the graph out of the data
    //! \param handle Unique handle of the texture proxy
    //! \param updateElemenst List of updates to apply
    void OnUpdated(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement>& updateElements);

    //! Clear the graph from the view
    void Clear();

    //------------------------------------------------------------------------------------

private:

    //! Texture proxy associated with the graphics view (!= nullptr)
    Pegasus::Texture::ITextureNodeProxy * const mTextureProxy;

    //! Handle returned in OnInitialized() from observer, which represents the current texture
    TextureNodeProxyHandle mTextureProxyHandle;

    //! Messenger used to send messages to the application
    PegasusDockWidget* mMessenger;

    //! Class for an observer of this view, used to communicate with the IO controller
    class Observer : public TextureNodeObserver
    {
    public:
        explicit Observer(TextureGraphEditorGraphicsView * parent) : mParent(parent) {}
        virtual ~Observer() {}

        virtual void OnInitialized(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy);
        virtual void OnUpdated(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement>& updateElements);
        virtual void OnShutdown(TextureNodeProxyHandle handle);

        void OnShutdownInternal(TextureNodeProxyHandle handle);

    private:
        TextureGraphEditorGraphicsView* mParent;
    };
    
    //! Observer of this view, used to communicate with the IO controller
    Observer* mObserver;
};


#endif // EDITOR_TEXTUREGRAPHEDITORGRAPHICSVIEW_H
