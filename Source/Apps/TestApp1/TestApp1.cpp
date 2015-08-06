/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Launcher                                   */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.cpp
//! \author Karolyn Boulanger
//! \date   07th July 2013
//! \brief  Test application 1
//! \todo Have the app wizard include appexport and set up this file correctly

#include "TestApp1.h"
#include "TestApp1Window.h"
#include "Pegasus/AppExport.h" // Needs including, for exported functions.  Only include this once!

#include "TimelineBlocks/FractalCubeBlock.h"
#include "TimelineBlocks/FractalCube2Block.h"
#include "TimelineBlocks/TextureTestBlock.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "TimelineBlocks/GeometryTestBlock.h"

#include "Pegasus/TimelineBlock/Camera/CameraLineBlock.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"

#include "Pegasus/Sound/Sound.h"

#include "Pegasus/PropertyGrid/Shared/IPropertyGridManagerProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridClassInfoProxy.h"


//! \todo TEMPORARY, property grid testing

BEGIN_IMPLEMENT_PROPERTIES2(TestClass2)
    IMPLEMENT_PROPERTY2(TestClass2, ParentBool)
    IMPLEMENT_PROPERTY2(TestClass2, ParentInt)
    IMPLEMENT_PROPERTY2(TestClass2, ParentUInt)
    IMPLEMENT_PROPERTY2(TestClass2, ParentFloat)
    IMPLEMENT_PROPERTY2(TestClass2, ParentVec2)
    IMPLEMENT_PROPERTY2(TestClass2, ParentVec3)
    IMPLEMENT_PROPERTY2(TestClass2, ParentVec4)
    IMPLEMENT_PROPERTY2(TestClass2, ParentRGB)
    IMPLEMENT_PROPERTY2(TestClass2, ParentRGBA)
    IMPLEMENT_PROPERTY2(TestClass2, ParentStr64)
END_IMPLEMENT_PROPERTIES2(TestClass2)

TestClass2::TestClass2()
:   Pegasus::PropertyGrid::PropertyGridObject()
{
    BEGIN_INIT_PROPERTIES(TestClass2)
        INIT_PROPERTY2(ParentBool)
        INIT_PROPERTY2(ParentInt)
        INIT_PROPERTY2(ParentUInt)
        INIT_PROPERTY2(ParentFloat)
        INIT_PROPERTY2(ParentVec2)
        INIT_PROPERTY2(ParentVec3)
        INIT_PROPERTY2(ParentVec4)
        INIT_PROPERTY2(ParentRGB)
        INIT_PROPERTY2(ParentRGBA)
        INIT_PROPERTY2(ParentStr64)
    END_INIT_PROPERTIES()
}

TestClass2::~TestClass2()
{
}

BEGIN_IMPLEMENT_PROPERTIES2(TestClass3)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedBool)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedInt)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedUInt)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedFloat)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedVec2)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedVec3)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedVec4)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedRGB)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedRGBA)
    IMPLEMENT_PROPERTY2(TestClass3, DerivedStr64)
END_IMPLEMENT_PROPERTIES2(TestClass3)

TestClass3::TestClass3()
:   TestClass2()
{
    BEGIN_INIT_PROPERTIES(TestClass3)
        INIT_PROPERTY2(DerivedBool)
        INIT_PROPERTY2(DerivedInt)
        INIT_PROPERTY2(DerivedUInt)
        INIT_PROPERTY2(DerivedFloat)
        INIT_PROPERTY2(DerivedVec2)
        INIT_PROPERTY2(DerivedVec3)
        INIT_PROPERTY2(DerivedVec4)
        INIT_PROPERTY2(DerivedRGB)
        INIT_PROPERTY2(DerivedRGBA)
        INIT_PROPERTY2(DerivedStr64)
    END_INIT_PROPERTIES()
}

TestClass3::~TestClass3()
{
}

//----------------------------------------------------------------------------------------

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

    //! \todo TEMPORARY, property grid testing

    Pegasus::PropertyGrid::PropertyGridManager & mgr = Pegasus::PropertyGrid::PropertyGridManager::GetInstance();
    const unsigned int numClasses = mgr.GetNumRegisteredClasses();
    for (unsigned int c = 0 ; c < numClasses; ++c)
    {
        const Pegasus::PropertyGrid::PropertyGridClassInfo & classInfo = mgr.GetClassInfo(c);
        PG_LOG('PROP', "Class: %s", classInfo.GetClassName());
        PG_LOG('PROP', "Parent class: %s", classInfo.GetParentClassName());
        PG_LOG('PROP', "Class properties:");
        const unsigned int numClassProperties = classInfo.GetNumClassProperties();
        for (unsigned int cp = 0; cp < numClassProperties; ++cp)
        {
            const Pegasus::PropertyGrid::PropertyGridClassInfo::PropertyRecord & record = classInfo.GetClassProperty(cp);
            PG_LOG('PROP', "  Name: %s", record.name);
            PG_LOG('PROP', "    Type: %u", record.type);
            PG_LOG('PROP', "    Size: %u", record.size);    
        }
        PG_LOG('PROP', "Properties:");
        const unsigned int numProperties = classInfo.GetNumProperties();
        for (unsigned int p = 0; p < numProperties; ++p)
        {
            const Pegasus::PropertyGrid::PropertyGridClassInfo::PropertyRecord & record = classInfo.GetProperty(p);
            PG_LOG('PROP', "  Name: %s", record.name);
            PG_LOG('PROP', "    Type: %u", record.type);
            PG_LOG('PROP', "    Size: %u", record.size);    
        }
    }

    //! \todo TEMPORARY class info testing
    TestClass2 class2;
    const Pegasus::PropertyGrid::PropertyGridClassInfo * classInfo2 = class2.GetClassInfo();
    TestClass3 class3;
    const Pegasus::PropertyGrid::PropertyGridClassInfo * classInfo3 = class3.GetClassInfo();
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
    REGISTER_TIMELINE_BLOCK(FractalCubeBlock);
    REGISTER_TIMELINE_BLOCK(FractalCube2Block);
    REGISTER_TIMELINE_BLOCK(TextureTestBlock);
    REGISTER_TIMELINE_BLOCK(GeometryTestBlock);
}

//----------------------------------------------------------------------------------------
    
void TestApp1::InitializeApp()
{
    Pegasus::Timeline::TimelineManager * const timelineManager = GetTimelineManager();
    timelineManager->LoadTimeline("Timeline/mainTimeline.pas");
    Pegasus::Sound::LoadMusic("Imported\\Music\\Test.mp3");
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
