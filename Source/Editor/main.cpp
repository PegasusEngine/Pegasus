/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	Main.cpp
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Entry point of Pegasus Editor

#include "Editor.h"
#include <QtWidgets/QApplication>
#include <QVector>


//include all proxy types to declare meta types
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"
#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/SourceIOMessageController.h"
#include "MessageControllers/WindowIOMessageController.h"
#include "MessageControllers/PropertyGridIOMessageController.h"
#include "MessageControllers/GraphIOMessageController.h"
#include "MessageControllers/TimelineIOMessageController.h"
#include "Widgets/PegasusDockWidget.h"

//! Name of the organization creating the software
#define EDITOR_ORGANIZATION_NAME				"Pegasus"

//! Domain name of the organization creating the software
#define EDITOR_ORGANIZATION_DOMAIN_NAME			"pegasusengine.org"

//! Name of the application
#define EDITOR_APPLICATION_NAME					"Pegasus Editor"


//------------------- Public meta type declarations --------------------------------------

Q_DECLARE_METATYPE(PegasusDockWidget*);

Q_DECLARE_METATYPE(Pegasus::Core::ISourceCodeProxy*);
Q_DECLARE_METATYPE(Pegasus::Shader::IProgramProxy*);
Q_DECLARE_METATYPE(Pegasus::Timeline::ITimelineProxy*);
Q_DECLARE_METATYPE(AssetInstanceHandle);
Q_DECLARE_METATYPE(Pegasus::AssetLib::IRuntimeAssetObjectProxy*);
Q_DECLARE_METATYPE(Pegasus::PegasusAssetTypeDesc*);

Q_DECLARE_METATYPE(CodeUserData*);

Q_DECLARE_METATYPE(Pegasus::Timeline::IBlockProxy*);

Q_DECLARE_METATYPE(AssetIOMessageController::Message);
Q_DECLARE_METATYPE(AssetIOMessageController::Message::IoResponseMessage);

Q_DECLARE_METATYPE(SourceIOMessageController::Message);

Q_DECLARE_METATYPE(ProgramIOMessageController::Message);

Q_DECLARE_METATYPE(WindowIOMessageController::Message);

Q_DECLARE_METATYPE(PropertyGridIOMessageController::Message);
Q_DECLARE_METATYPE(PropertyGridIOMessageController::UpdateElement);
Q_DECLARE_METATYPE(TimelineIOMessageController::Message);
Q_DECLARE_METATYPE(TimelineIOMessageController::BlockOpResponse);
Q_DECLARE_METATYPE(TimelineIOMessageObserver*);
Q_DECLARE_METATYPE(QVector<PropertyGridIOMessageController::UpdateElement>);
Q_DECLARE_METATYPE(PropertyGridHandle);
Q_DECLARE_METATYPE(Pegasus::PropertyGrid::IPropertyGridClassInfoProxy*);
Q_DECLARE_METATYPE(AssetInformation);
Q_DECLARE_METATYPE(AssetCategory);
Q_DECLARE_METATYPE(AssetViewTree);

Q_DECLARE_METATYPE(GraphIOMessageController::Message);
Q_DECLARE_METATYPE(GraphIOMessageController::UpdateElement);
Q_DECLARE_METATYPE(QVector<GraphIOMessageController::UpdateElement>);

//----------------------------------------------------------------------------------------

void RegisterMetaTypes()
{
    qRegisterMetaType<PegasusDockWidget*>();

    qRegisterMetaType<Pegasus::Core::ISourceCodeProxy*>();
    qRegisterMetaType<Pegasus::Shader::IProgramProxy*>();
    qRegisterMetaType<Pegasus::Timeline::ITimelineProxy*>();
    qRegisterMetaType<AssetInstanceHandle>();
    qRegisterMetaType<Pegasus::AssetLib::IRuntimeAssetObjectProxy*>();
    qRegisterMetaType<Pegasus::PegasusAssetTypeDesc*>();

    qRegisterMetaType<CodeUserData*>();

    qRegisterMetaType<Pegasus::Timeline::IBlockProxy*>();

    qRegisterMetaType<AssetIOMessageController::Message>();
    qRegisterMetaType<AssetIOMessageController::Message::IoResponseMessage>();

    qRegisterMetaType<SourceIOMessageController::Message>();

    qRegisterMetaType<ProgramIOMessageController::Message>();

    qRegisterMetaType<WindowIOMessageController::Message>();

    qRegisterMetaType<PropertyGridIOMessageController::Message>();
    qRegisterMetaType<PropertyGridIOMessageController::UpdateElement>();
    qRegisterMetaType<TimelineIOMessageController::Message>();
    qRegisterMetaType<TimelineIOMessageController::BlockOpResponse>();
    qRegisterMetaType<TimelineIOMessageObserver*>();
    qRegisterMetaType<QVector<PropertyGridIOMessageController::UpdateElement> >();
    qRegisterMetaType<PropertyGridHandle>();
    qRegisterMetaType<Pegasus::PropertyGrid::IPropertyGridClassInfoProxy*>();

    qRegisterMetaType<GraphIOMessageController::Message>();
    qRegisterMetaType<GraphIOMessageController::UpdateElement>();
    qRegisterMetaType<QVector<GraphIOMessageController::UpdateElement> >();
    qRegisterMetaType<AssetInformation>();
    qRegisterMetaType<AssetCategory>();
    qRegisterMetaType<AssetViewTree>();
}

//----------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    // Register all meta types in Pegasus
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
