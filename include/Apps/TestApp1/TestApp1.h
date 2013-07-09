/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                  */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.h
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1

#ifndef TESTAPP1_H
#define TESTAPP1_H

#include "Pegasus/Pegasus.h"


class TestApp1 : public Pegasus::Application
{
public:

    TestApp1();
    ~TestApp1();

    virtual void Initialize(const Pegasus::ApplicationConfig& config);
    virtual void Shutdown();
    virtual void Render();
};


//! \todo Make this a macro with only the name of the app class as parameter
#if PEGASUS_INCLUDE_LAUNCHER

Pegasus::Application * CreatePegasusApp() { return new TestApp1(); }

#else

#ifdef __cplusplus
extern "C" {
#endif
PEGASUSAPP_SHARED Pegasus::IApplication * CreatePegasusApp() { return new TestApp1(); }
#ifdef __cplusplus
}
#endif

#endif  // PEGASUS_INCLUDE_LAUNCHER


#endif  // TESTAPP1_H
