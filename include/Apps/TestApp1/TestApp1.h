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
    // Ctor / dtor
    TestApp1();
    virtual ~TestApp1();

    // App API
    virtual void Initialize(const Pegasus::Application::ApplicationConfig& config);
    virtual void Shutdown();
    virtual void Render();
};


//! \todo Make this a macro with only the name of the app class as parameter
Pegasus::Application::Application * CreateApplication() { return new TestApp1(); }
void DestroyApplication(Pegasus::Application::Application* app) { delete app; }

#endif  // TESTAPP1_H
