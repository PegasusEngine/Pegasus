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
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"

//! Name of the organization creating the software
#define EDITOR_ORGANIZATION_NAME				"Pegasus"

//! Domain name of the organization creating the software
#define EDITOR_ORGANIZATION_DOMAIN_NAME			"pegasusengine.org"

//! Name of the application
#define EDITOR_APPLICATION_NAME					"Pegasus Editor"

//----------------------------------------------------------------------------------------
//------------------- Public meta type declarations --------------------------------------

Q_DECLARE_METATYPE(Pegasus::Core::ISourceCodeProxy*);
Q_DECLARE_METATYPE(CodeUserData*);
Q_DECLARE_METATYPE(Pegasus::Timeline::IBlockProxy*);

void RegisterMetaTypes()
{
    qRegisterMetaType<Pegasus::Core::ISourceCodeProxy*>();
    qRegisterMetaType<CodeUserData*>();
    qRegisterMetaType<Pegasus::Timeline::IBlockProxy*>();
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
