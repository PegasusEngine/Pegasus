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
#include "MessageControllers/AssetIOMessageController.h"

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

class SourceIOMessageController : public QObject, public IAssetTranslator
{
    Q_OBJECT;
public:
    //! constructor
    explicit SourceIOMessageController(Pegasus::App::IApplicationProxy* app);
    
    //! destructor
    virtual ~SourceIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const SourceIOMCMessage& msg);

    //! \param object input asset to open
    //! \return a qvariant that contains the representation of this asset for the ui to read.
    virtual QVariant TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! \param object asked for validity.
    //! \return a boolean expressing if the state of this object is valid, or not.
    //!         for example, a shader code that had a compilation error must return false. This will help the asset instance
    //!         viewer display feedback on such object.
    virtual bool IsRuntimeObjectValid(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! \return null terminated list that contains all the necessary asset data so assets can be translated.
    virtual const Pegasus::PegasusAssetTypeDesc** GetTypeList() const;

signals:

    //! Signal triggered when a redraw of the viewport is requested
    void SignalRedrawViewports();

    //! Signal triggered when Compilation request has ended
    void SignalCompilationRequestEnded();

private:

    //! called when a source code is compiled
    void OnRenderRequestCompileSource(AssetInstanceHandle handle);

    //! called when a source code is getting its text set
    void OnRenderRequestSetSource(AssetInstanceHandle handle, const QString& srcTxt);

    Pegasus::App::IApplicationProxy* mApp;
};

#endif
