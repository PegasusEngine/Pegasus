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

class TestApp1 : public Pegasus::Application::Application
{
public:
    //! Constructor
    //! \param config Config structure for this app.
    TestApp1(const Pegasus::Application::ApplicationConfig& config);

    //! Destructor
    virtual ~TestApp1();


    // Application interface
    virtual const char* GetAppName() const { return "TestApp1"; }
};


//! \todo Make this a macro with only the name of the app class as parameter
Pegasus::Application::Application * CreateApplication(const Pegasus::Application::ApplicationConfig& config) { return PG_NEW("TestApp1", Pegasus::Memory::PG_MEM_PERM) TestApp1(config); }
void DestroyApplication(Pegasus::Application::Application* app) { PG_DELETE app; }

#endif  // TESTAPP1_H
