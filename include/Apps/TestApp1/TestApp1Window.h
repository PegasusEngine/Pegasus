/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1Window.h
//! \author David Worsham
//! \date   16th October 2013
//! \brief  Specialized window for TestApp1.

#ifndef TESTAPP1WINDOW_H
#define TESTAPP1WINDOW_H

#include "Pegasus/Pegasus.h"
#include "Pegasus/Render/Render.h"

//! This is a specialized window for the TestApp1 application main window
//! Place specialized rendering code here.
class TestApp1Window : public Pegasus::Wnd::Window
{
public:
    // Ctor / dtor
    TestApp1Window(const Pegasus::Wnd::WindowConfig& config);
    ~TestApp1Window();

    // Factory API
    static Pegasus::Wnd::Window* Create(const Pegasus::Wnd::WindowConfig& config, Pegasus::Alloc::IAllocator* alloc);

    // App-specific API
    virtual void Initialize();
    virtual void Shutdown();
    virtual void Render();

private:
    Pegasus::Alloc::IAllocator * mAllocator;
    Pegasus::Render::RasterizerState mRasterState;
};

#endif  // TESTAPP1WINDOW_H
