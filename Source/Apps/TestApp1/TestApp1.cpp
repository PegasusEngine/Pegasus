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

#include "TimelineBlocks/PsyBeadsBlock.h"
#include "TimelineBlocks/FractalCubeBlock.h"
#include "TimelineBlocks/FractalCube2Block.h"
#include "TimelineBlocks/TextureTestBlock.h"
#include "TimelineBlocks/GeometryTestBlock.h"

#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"

#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Sound/Sound.h"


// Variables
static const char* MAIN_WND_TYPE = "TestApp1Window";
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
static const char* SECONDARY_WND_TYPE = "TestApp1SecondaryWindow";
static const char* TEXTURE_EDITOR_PREVIEW_WND_TYPE = "TestApp1TextureEditorPreviewWindow";
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
    // Register the editor windows
    //! \todo That is weird to have to register those windows here.
    //! \todo Is it normal to have to use a different type tag?
    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_SECONDARY;
    reg.mDescription = "TestApp1 Secondary Viewport";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(SECONDARY_WND_TYPE, reg);

    reg.mTypeTag = Pegasus::App::WINDOW_TYPE_TEXTUREEDITORPREVIEW;
    reg.mDescription = "TestApp1 Texture Editor Preview";
    reg.mCreateFunc = TestApp1Window::Create;
    GetWindowRegistry()->RegisterWindowClass(TEXTURE_EDITOR_PREVIEW_WND_TYPE, reg);
#endif  // PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
}

//----------------------------------------------------------------------------------------

TestApp1::~TestApp1()
{
    // Unregister the main window
    GetWindowRegistry()->UnregisterWindowClass(MAIN_WND_TYPE);
#if PEGASUS_ENABLE_EDITOR_WINDOW_TYPES
    GetWindowRegistry()->UnregisterWindowClass(SECONDARY_WND_TYPE);
    GetWindowRegistry()->UnregisterWindowClass(TEXTURE_EDITOR_PREVIEW_WND_TYPE);
#endif

}

//----------------------------------------------------------------------------------------

void TestApp1::RegisterTimelineBlocks()
{
    REGISTER_TIMELINE_BLOCK(PsyBeadsBlock);
    REGISTER_TIMELINE_BLOCK(FractalCubeBlock);
    REGISTER_TIMELINE_BLOCK(FractalCube2Block);
    REGISTER_TIMELINE_BLOCK(TextureTestBlock);
    REGISTER_TIMELINE_BLOCK(GeometryTestBlock);
}

//----------------------------------------------------------------------------------------
    
void TestApp1::InitializeApp()
{
    Pegasus::Timeline::Timeline * const timeline = GetTimeline();

    // Create the content of the timeline
    const unsigned int TICKS_PER_BEAT = 128;
    timeline->SetNumTicksPerBeat(TICKS_PER_BEAT);
    timeline->SetBeatsPerMinute(138.0f);
    timeline->CreateLane();
    timeline->CreateLane();
    timeline->CreateLane();
    timeline->CreateLane();
    timeline->CreateLane();
#if PEGASUS_ENABLE_PROXIES
    timeline->GetLane(0)->SetName("Camera");
    timeline->GetLane(1)->SetName("Beads");
    timeline->GetLane(2)->SetName("Fractal");
    timeline->GetLane(3)->SetName("Blob");
#endif

    Pegasus::Timeline::Block * cameraLineBlock = timeline->CreateBlock("CameraLineBlock");
#if PEGASUS_ENABLE_PROXIES
    cameraLineBlock->SetColor(255, 255, 0);
#endif
    timeline->GetLane(0)->InsertBlock(cameraLineBlock, 0 * TICKS_PER_BEAT, 8 * TICKS_PER_BEAT);

    Pegasus::Timeline::Block * psyBeadBlock = timeline->CreateBlock("PsyBeadsBlock");
#if PEGASUS_ENABLE_PROXIES
    psyBeadBlock->SetColor(255, 128, 128);
#endif
    timeline->GetLane(1)->InsertBlock(psyBeadBlock, 0 * TICKS_PER_BEAT, 8 * TICKS_PER_BEAT);

    Pegasus::Timeline::Block * geometryTestBlock = timeline->CreateBlock("GeometryTestBlock");
#if PEGASUS_ENABLE_PROXIES
    geometryTestBlock->SetColor(19, 55, 28);
#endif
    timeline->GetLane(1)->InsertBlock(geometryTestBlock, 8 * TICKS_PER_BEAT, 8 * TICKS_PER_BEAT);

    Pegasus::Timeline::Block * psyBeadBlock2 = timeline->CreateBlock("PsyBeadsBlock");
#if PEGASUS_ENABLE_PROXIES
    psyBeadBlock2->SetColor(255, 128, 192);
#endif
    timeline->GetLane(1)->InsertBlock(psyBeadBlock2, 16 * TICKS_PER_BEAT, 16 * TICKS_PER_BEAT);

    Pegasus::Timeline::Block * fractalCube2Block = timeline->CreateBlock("FractalCube2Block");
#if PEGASUS_ENABLE_PROXIES
    fractalCube2Block->SetColor(128, 255, 192);
#endif
    timeline->GetLane(2)->InsertBlock(fractalCube2Block, 16 * TICKS_PER_BEAT, 8 * TICKS_PER_BEAT);

    mTextureTestBlock = timeline->CreateBlock("TextureTestBlock");
#if PEGASUS_ENABLE_PROXIES
    mTextureTestBlock->SetColor(192, 255, 128);
#endif
    timeline->GetLane(3)->InsertBlock(mTextureTestBlock, 24 * TICKS_PER_BEAT, 18 * TICKS_PER_BEAT);

    Pegasus::Timeline::Block * fractalCubeBlock = timeline->CreateBlock("FractalCubeBlock");
#if PEGASUS_ENABLE_PROXIES
    fractalCubeBlock->SetColor(128, 255, 128);
#endif
    timeline->GetLane(2)->InsertBlock(fractalCubeBlock, 42 * TICKS_PER_BEAT, 6 * TICKS_PER_BEAT);

    // Set the music file to play
    Pegasus::Sound::LoadMusic("..\\..\\..\\..\\Data\\TestApp1\\Imported\\Music\\Test.mp3");
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
