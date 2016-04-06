/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramIOMessageController.h
//! \author	Kleber Garcia
//! \date	June 16th 2015
//! \brief	Program IO controller 

#ifndef PROGRAM_IO_MESSAGE_CONTROLLER
#define PROGRAM_IO_MESSAGE_CONTROLLER

#include <QObject>
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "MessageControllers/AssetIOMessageController.h"

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
}

//! Program IO controller Message class
class ProgramIOMessageController : public QObject, public IAssetTranslator
{
    Q_OBJECT;
public:
    //! Program IO controller message definition
    class Message 
    {
    public:
    
        //! Operation type
        enum MessageType
        {
            INVALID = -1,
            REMOVE_SHADER,
            MODIFY_SHADER
        };
    
        //!Constructor
        Message() : mMessageType(INVALID), mShaderType(Pegasus::Shader::SHADER_STAGE_INVALID)
        {
        }
    
        //! Destructor
        ~Message()
        {
        }
    
        //Getters
        MessageType GetMessageType() const { return mMessageType; }
        const QString& GetShaderPath() const { return mShaderPath; }
        AssetInstanceHandle GetProgram() const { return mProgram; }
        Pegasus::Shader::ShaderType GetShaderType() const { return mShaderType; }
        
    
        //Setters
        void SetMessageType(MessageType t) { mMessageType = t; }
        void SetShaderPath(const QString& shader) { mShaderPath = shader; }
        void SetProgram(AssetInstanceHandle handle) { mProgram = handle; }
        void SetShaderType(Pegasus::Shader::ShaderType type) { mShaderType = type; }
    
    private:
        MessageType mMessageType;
        QString mShaderPath;
        AssetInstanceHandle mProgram;
        Pegasus::Shader::ShaderType mShaderType;
    };

    //! Constructor
    explicit ProgramIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~ProgramIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const Message& msg);

    //! \param object input asset to open
    //! \return a qvariant that contains the representation of this asset for the ui to read.
    virtual QVariant TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! \return null terminated list that contains all the necessary asset data so assets can be translated.
    virtual const Pegasus::PegasusAssetTypeDesc** GetTypeList() const;

signals:

    //! Signal triggered when the UI needs to update the active node views
    void SignalUpdateProgramView();

    //! Signal triggered when a redraw of the viewport is requested
    void SignalRedrawViewports();

private:

    void OnRenderThreadRemoveShader(AssetInstanceHandle handle, Pegasus::Shader::ShaderType shaderType);
    
    void OnRenderThreadModifyShader(AssetInstanceHandle handle, const QString& path);

    //! Called when a shader is requested for opening from the render thread
    Pegasus::App::IApplicationProxy* mApp;
    
};


#endif
