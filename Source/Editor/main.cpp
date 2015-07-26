/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Main.cpp
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Entry point of Pegasus Editor

#include "Editor.h"
#include <QtWidgets/QApplication>


//include all proxy types to declare meta types
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/SourceIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"

//! Name of the organization creating the software
#define EDITOR_ORGANIZATION_NAME				"Pegasus"

//! Domain name of the organization creating the software
#define EDITOR_ORGANIZATION_DOMAIN_NAME			"pegasusengine.org"

//! Name of the application
#define EDITOR_APPLICATION_NAME					"Pegasus Editor"

//----------------------------------------------------------------------------------------
//------------------- Public meta type declarations --------------------------------------

Q_DECLARE_METATYPE(PegasusDockWidget*);
Q_DECLARE_METATYPE(Pegasus::Core::ISourceCodeProxy*);
Q_DECLARE_METATYPE(Pegasus::Shader::IProgramProxy*);
Q_DECLARE_METATYPE(Pegasus::Timeline::ITimelineProxy*);
Q_DECLARE_METATYPE(Pegasus::AssetLib::IRuntimeAssetObjectProxy*);
Q_DECLARE_METATYPE(CodeUserData*);
Q_DECLARE_METATYPE(Pegasus::Timeline::IBlockProxy*);
Q_DECLARE_METATYPE(AssetIOMessageController::Message);
Q_DECLARE_METATYPE(AssetIOMessageController::Message::IoResponseMessage);
Q_DECLARE_METATYPE(SourceIOMessageController::Message);
Q_DECLARE_METATYPE(ProgramIOMessageController::Message);

void RegisterMetaTypes()
{
    qRegisterMetaType<PegasusDockWidget*>();
    qRegisterMetaType<Pegasus::Core::ISourceCodeProxy*>();
    qRegisterMetaType<Pegasus::Shader::IProgramProxy*>();
    qRegisterMetaType<Pegasus::Timeline::ITimelineProxy*>();
    qRegisterMetaType<Pegasus::AssetLib::IRuntimeAssetObjectProxy*>();
    qRegisterMetaType<CodeUserData*>();
    qRegisterMetaType<Pegasus::Timeline::IBlockProxy*>();
    qRegisterMetaType<AssetIOMessageController::Message>();
    qRegisterMetaType<AssetIOMessageController::Message::IoResponseMessage>();
    qRegisterMetaType<SourceIOMessageController::Message>();
    qRegisterMetaType<ProgramIOMessageController::Message>();
}

//----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    //register all meta types in pegasus
    RegisterMetaTypes();

    QApplication app(argc, argv);

    // Handle the quit message
    app.setQuitOnLastWindowClosed(true);

	// Set application-wide information that will be used for the application settings
	app.setOrganizationName(EDITOR_ORGANIZATION_NAME);
	app.setOrganizationDomain(EDITOR_ORGANIZATION_DOMAIN_NAME);
	app.setApplicationName(EDITOR_APPLICATION_NAME);

    // Show the main window
    Editor w(&app);
    w.show();

    return app.exec();
}
