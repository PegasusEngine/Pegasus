/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   SourceIOMessageController.h
//! \author Kleber Garcia
//! \date   June 6th 2015
//! \brief  Source IO Controller, organized class that executes commands towards the render thread
//!         these commands intend to modify shaders.

#ifndef EDITOR_SOURCEIOMESSAGE_CTRL_H
#define EDITOR_SOURCEIOMESSAGE_CTRL_H

#include <QObject>

//fwd declarations
namespace Pegasus
{
namespace App
{
    class IApplicationProxy;    
}

namespace Core
{
    class ISourceCodeProxy;
}
}

class SourceIOMessageController : public QObject
{
    Q_OBJECT;
public:
    class Message
    {
    public:
        enum MessageType
        {
            INVALID = -1,
            SET_SOURCE_AND_COMPILE_SOURCE,
            COMPILE_SOURCE,
            SET_SOURCE
        };

        //! Constructor
        Message() : mType(INVALID), mSrc(nullptr) {}

        //! Sets the message type
        void SetMessageType(MessageType type) { mType = type; } 

        //! Sets the source of the shader in the message
        void SetSourceText(const QString& str) { mSourceText = str; }

        //! Gets the message type
        MessageType GetMessageType() const { return mType; } 

        //! Gets the source of the shader in the message
        const QString GetSourceText() const { return mSourceText; }

        //! Sets the source node pointer
        void SetSource(Pegasus::Core::ISourceCodeProxy* src) { mSrc = src; }
    
        //! Gets the source pointer
        Pegasus::Core::ISourceCodeProxy* GetSource() const { return mSrc; }

    private:
        MessageType mType;
        QString     mSourceText;
        Pegasus::Core::ISourceCodeProxy* mSrc;
    };

    //! constructor
    explicit SourceIOMessageController(Pegasus::App::IApplicationProxy* app);
    
    //! destructor
    ~SourceIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const Message& msg);

signals:

    //! Signal triggered when a redraw of the viewport is requested
    void SignalRedrawViewports();

    //! Signal triggered when Compilation request has ended
    void SignalCompilationRequestEnded();

private:

    //! called when a source code is compiled
    void OnRenderRequestCompileSource(Pegasus::Core::ISourceCodeProxy* src);

    //! called when a source code is getting its text set
    void OnRenderRequestSetSource(Pegasus::Core::ISourceCodeProxy* src, const QString& srcTxt);

    Pegasus::App::IApplicationProxy* mApp;
};

#endif
