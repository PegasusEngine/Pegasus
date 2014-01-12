/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                   */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.cpp
//! \author Kevin Boulanger
//! \date   07th July 2013
//! \brief  Test application 1
//! \todo Have the app wizard include appexport and set up this file correctly

#include "TestApp1.h"
#include "TestApp1Window.h"
#include "Pegasus/AppExport.h" // Needs including, for exported functions.  Only include this once!

#include "TimelineBlocks/KleberTriangleBlock.h"
#include "TimelineBlocks/PsyBeadsBlock.h"
#include "TimelineBlocks/FractalCubeBlock.h"
#include "TimelineBlocks/FractalCube2Block.h"

#include "Pegasus/Timeline/Lane.h"


// Variables
static const char* MAIN_WND_TYPE = "TestApp1Window";
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
static const char* SECONDARY_WND_TYPE = "TestApp1SecondaryWindow";
#endif

//----------------------------------------------------------------------------------------

TestApp1::TestApp1(const Pegasus::App::ApplicationConfig& config)
:   Pegasus::App::Application(config)
{
    Pegasus::App::WindowRegistration reg;

    // Register the main window
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_MAIN;
    reg.mDescription = "TestApp1 Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(MAIN_WND_TYPE, reg);

#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    // Register the secondary window
    //! \todo That is weird to have to register that window here.
    //!       Also, where should the texture and mesh viewers be created?
    //! \todo Is it normal to have to use a different type tag?
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_SECONDARY;
    reg.mDescription = "TestApp1 Secondary Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(SECONDARY_WND_TYPE, reg);
#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{
    // Unregister the main window
    GetWindowRegistry()->UnregisterWindowClass(MAIN_WND_TYPE);
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    GetWindowRegistry()->UnregisterWindowClass(SECONDARY_WND_TYPE);
#endif

}

//----------------------------------------------------------------------------------------

void TestApp1::InitializeApp()
{
    Pegasus::Timeline::Timeline * const timeline = GetTimeline();
    Pegasus::Alloc::IAllocator * const timelineAllocator = timeline->GetAllocator();

    // Create the content of the timeline
    timeline->CreateLane();
    timeline->CreateLane();
    timeline->CreateLane();
    timeline->CreateLane();

    KleberTriangleBlock * kleberTriangleBlock = PG_NEW(timelineAllocator, -1, "KleberTriangleBlock", Pegasus::Alloc::PG_MEM_PERM) KleberTriangleBlock(timelineAllocator, this);
    kleberTriangleBlock->SetPosition(6.0f);
    kleberTriangleBlock->SetLength(4.0f);
#if PEGASUS_ENABLE_PROXIES
    kleberTriangleBlock->SetColor(255, 255, 128);
#endif
    timeline->GetLane(2)->InsertBlock(kleberTriangleBlock);

    PsyBeadsBlock * psyBeadBlock = PG_NEW(timelineAllocator, -1, "PsyBeadsBlock", Pegasus::Alloc::PG_MEM_PERM) PsyBeadsBlock(timelineAllocator, this);
    psyBeadBlock->SetPosition(0.0f);
    psyBeadBlock->SetLength(8.0f);
#if PEGASUS_ENABLE_PROXIES
    psyBeadBlock->SetColor(255, 128, 128);
#endif
    timeline->GetLane(0)->InsertBlock(psyBeadBlock);

    PsyBeadsBlock * psyBeadBlock2 = PG_NEW(timelineAllocator, -1, "PsyBeadsBlock 2", Pegasus::Alloc::PG_MEM_PERM) PsyBeadsBlock(timelineAllocator, this);
    psyBeadBlock2->SetPosition(19.0f);
    psyBeadBlock2->SetLength(10.0f);
#if PEGASUS_ENABLE_PROXIES
    psyBeadBlock2->SetColor(255, 128, 192);
#endif
    timeline->GetLane(0)->InsertBlock(psyBeadBlock2);

    FractalCubeBlock * fractalCubeBlock = PG_NEW(timelineAllocator, -1, "FractalCubeBlock", Pegasus::Alloc::PG_MEM_PERM) FractalCubeBlock(timelineAllocator, this);
    fractalCubeBlock->SetPosition(8.0f);
    fractalCubeBlock->SetLength(5.0f);
#if PEGASUS_ENABLE_PROXIES
    fractalCubeBlock->SetColor(128, 255, 128);
#endif
    timeline->GetLane(1)->InsertBlock(fractalCubeBlock);

    FractalCube2Block * fractalCube2Block = PG_NEW(timelineAllocator, -1, "FractalCube2Block", Pegasus::Alloc::PG_MEM_PERM) FractalCube2Block(timelineAllocator, this);
    fractalCube2Block->SetPosition(13.0f);
    fractalCube2Block->SetLength(12.0f);
#if PEGASUS_ENABLE_PROXIES
    fractalCube2Block->SetColor(128, 255, 192);
#endif
    timeline->GetLane(1)->InsertBlock(fractalCube2Block);
}

//----------------------------------------------------------------------------------------

void TestApp1::ShutdownApp()
{
}

//----------------------------------------------------------------------------------------

Pegasus::App::Application * CreateApplication(const Pegasus::App::ApplicationConfig& config)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    return PG_NEW(globalAlloc, -1, "TestApp1", Pegasus::Alloc::PG_MEM_PERM) TestApp1(config);
}

//----------------------------------------------------------------------------------------

void DestroyApplication(Pegasus::App::Application* app)
{
    Pegasus::Alloc::IAllocator* globalAlloc = Pegasus::Memory::GetGlobalAllocator();

    PG_DELETE(globalAlloc, app);
}
