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
    //! Constructor
    explicit ProgramIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~ProgramIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const ProgramIOMCMessage& msg);

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
    void OnRenderThreadModifyShader(AssetInstanceHandle handle, const QString& path);
    void OnRenderThreadRemoveShader(AssetInstanceHandle handle);

    //! Called when a shader is requested for opening from the render thread
    Pegasus::App::IApplicationProxy* mApp;
    
};


#endif
