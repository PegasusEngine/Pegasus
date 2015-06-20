/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetIOMessageController.h
//! \author	Kleber Garcia
//! \date	31st March 2015
//! \brief	Asset IO controller - keeps state of assets / dirty / on loading etc

#ifndef EDITOR_ASSET_IO_CONTROLLER_H
#define EDITOR_ASSET_IO_CONTROLLER_H

#include <QObject>

//forward declarations
namespace Pegasus
{
    namespace App
    {
        class IApplicationProxy;
    }

    namespace Shader
    {
        class IProgramProxy;
        class IShaderProxy;
    }

    namespace AssetLib
    {
        class IAssetProxy;
    }

    namespace Core
    {
        class ISourceCodeProxy;
    }
}

//! Asset IO controller class
class AssetIOMessageController : public QObject
{
    Q_OBJECT;
public:
    //! Asset IO controller message definition
    class Message 
    {
    public:
    
        //! Operation type
        enum MessageType
        {
            INVALID = -1,
            OPEN_ASSET,
            CLOSE_CODE,
            SAVE_CODE,
            CLOSE_PROGRAM,
            SAVE_PROGRAM,
            NEW_SHADER,
            NEW_TIMELINESCRIPT,
            NEW_PROGRAM,
            NEW_MESH,
            NEW_TEXTURE
        };
    
        //! Combo holder union of node / asset type
        union AssetNode
        {
            void                            * mPtr;
            Pegasus::Core::ISourceCodeProxy * mCode;
            Pegasus::Shader::IShaderProxy   * mShader;
            Pegasus::Shader::IProgramProxy  * mProgram;
            Pegasus::AssetLib::IAssetProxy  * mAsset;
        };
    
        //!Constructor
        Message() : mMessageType(INVALID)
        {
            mNode.mPtr = nullptr;
        }
    
        explicit Message(MessageType t) : mMessageType(t)
        {
            mNode.mPtr = nullptr;
        }
    
        //! Destructor
        ~Message()
        {
        }
    
        //Getters
        MessageType GetMessageType() const { return mMessageType; }
        QString GetString() const { return mString; }
        const AssetNode& GetAssetNode() const { return mNode; }
        AssetNode& GetAssetNode() { return mNode; }
    
        //Setters
        void SetMessageType(MessageType t) { mMessageType = t; }
        void SetString(const QString& s) { mString = s; }
        void SetAssetNode(const AssetNode& node) { mNode = node; }
    
    
    private:
        MessageType mMessageType;
        AssetNode mNode;
        QString mString;
    
    };

    //! Constructor
    explicit AssetIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~AssetIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const Message& msg);

signals:

    //!Signal triggered when a code is requested for open
    void SignalOpenCode(Pegasus::Core::ISourceCodeProxy* code);
    
    //!Signal triggered when a program is requested for open
    void SignalOpenProgram(Pegasus::Shader::IProgramProxy* program);

    //! Signal triggered when the UI needs to update the active node views
    void SignalUpdateNodeViews();


private:
    //! Called when a shader is requested for opening from the render thread
    void OnRenderRequestOpenAsset(const QString& path);

    //! Called when a shader is requested for opening from the render thread
    void OnRenderRequestCloseCode(Pegasus::Core::ISourceCodeProxy* code);

    //! Called when a program is requested for opening from the render thread
    void OnRenderRequestCloseProgram(Pegasus::Shader::IProgramProxy* program);

    //! Called when a shader is requested to be saved from the render thread
    void OnRenderRequestSaveCode(Pegasus::Core::ISourceCodeProxy* code);

    //! Called when a program is requested to be saved from the render thread
    void OnRenderRequestSaveProgram(Pegasus::Shader::IProgramProxy* program);

    //! Called when a new shader is requested from the render thread
    void OnRenderRequestNewShader(const QString& path);

    //! Called when a new timeline script is requested from the render thread
    void OnRenderRequestNewTimelineScript(const QString& path);

    //! Called when a program is requested from the render thread
    void OnRenderRequestNewProgram(const QString& path);

    Pegasus::App::IApplicationProxy* mApp;
    
};


#endif
