/****************************************************************************************/
/*                                                                                      */
/*                                  Pegasus Application                                 */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.h
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1

#ifndef TESTAPP1_H
#define TESTAPP1_H

#include "Pegasus/Pegasus.h"

class TestApp1 : public Pegasus::App::Application
{
public:
    //! Constructor
    //! \param config Config structure for this app.
    TestApp1(const Pegasus::App::ApplicationConfig& config);

    //! Destructor
    virtual ~TestApp1();


    //! Application interface
    virtual const char* GetAppName() const { return "TestApp1"; }

    //! Register the custom timeline blocks
    virtual void RegisterTimelineBlocks();

    //! Custom initialization, done before the timeline triggers the loading of blocks and their assets
    virtual void InitializeApp();

    //! Custom shutdown
    virtual void ShutdownApp();
};


//! \todo Make this a macro with only the name of the app class as parameter
Pegasus::App::Application * CreateApplication(const Pegasus::App::ApplicationConfig& config);
void DestroyApplication(Pegasus::App::Application* app);

#endif  // TESTAPP1_H
