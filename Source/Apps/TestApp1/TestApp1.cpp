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


namespace SomeStrangeNamespace {
BEGIN_IMPLEMENT_ENUM(TestEnum)
    IMPLEMENT_ENUM(TestEnum,OPTION1)
    IMPLEMENT_ENUM(TestEnum,OPTION2)
    IMPLEMENT_ENUM(TestEnum,OPTION3)
    IMPLEMENT_ENUM(TestEnum,OPTION4)
END_IMPLEMENT_ENUM()
}

//! \todo TEMPORARY, property grid testing

BEGIN_IMPLEMENT_PROPERTIES(TestClass2)
    IMPLEMENT_PROPERTY(TestClass2, ParentBool)
    IMPLEMENT_PROPERTY(TestClass2, ParentInt)
    IMPLEMENT_PROPERTY(TestClass2, ParentUInt)
    IMPLEMENT_PROPERTY(TestClass2, ParentFloat)
    IMPLEMENT_PROPERTY(TestClass2, ParentVec2)
    IMPLEMENT_PROPERTY(TestClass2, ParentVec3)
    IMPLEMENT_PROPERTY(TestClass2, ParentVec4)
    IMPLEMENT_PROPERTY(TestClass2, ParentRGB)
    IMPLEMENT_PROPERTY(TestClass2, ParentRGBA)
    IMPLEMENT_PROPERTY(TestClass2, ParentStr64)
END_IMPLEMENT_PROPERTIES(TestClass2)


TestClass2::TestClass2()
:   Pegasus::PropertyGrid::PropertyGridObject()
{
    BEGIN_INIT_PROPERTIES(TestClass2)
        INIT_PROPERTY(ParentBool)
        INIT_PROPERTY(ParentInt)
        INIT_PROPERTY(ParentUInt)
        INIT_PROPERTY(ParentFloat)
        INIT_PROPERTY(ParentVec2)
        INIT_PROPERTY(ParentVec3)
        INIT_PROPERTY(ParentVec4)
        INIT_PROPERTY(ParentRGB)
        INIT_PROPERTY(ParentRGBA)
        INIT_PROPERTY(ParentStr64)
    END_INIT_PROPERTIES()
}

TestClass2::~TestClass2()
{
}

BEGIN_IMPLEMENT_PROPERTIES(TestClass3)
    IMPLEMENT_PROPERTY(TestClass3, DerivedBool)
    IMPLEMENT_PROPERTY(TestClass3, DerivedInt)
    IMPLEMENT_PROPERTY(TestClass3, DerivedUInt)
    IMPLEMENT_PROPERTY(TestClass3, DerivedFloat)
    IMPLEMENT_PROPERTY(TestClass3, DerivedVec2)
    IMPLEMENT_PROPERTY(TestClass3, DerivedVec3)
    IMPLEMENT_PROPERTY(TestClass3, DerivedVec4)
    IMPLEMENT_PROPERTY(TestClass3, DerivedRGB)
    IMPLEMENT_PROPERTY(TestClass3, DerivedRGBA)
    IMPLEMENT_PROPERTY(TestClass3, DerivedStr64)
END_IMPLEMENT_PROPERTIES(TestClass3)

TestClass3::TestClass3()
:   TestClass2()
{
    BEGIN_INIT_PROPERTIES(TestClass3)
        INIT_PROPERTY(DerivedBool)
        INIT_PROPERTY(DerivedInt)
        INIT_PROPERTY(DerivedUInt)
        INIT_PROPERTY(DerivedFloat)
        INIT_PROPERTY(DerivedVec2)
        INIT_PROPERTY(DerivedVec3)
        INIT_PROPERTY(DerivedVec4)
        INIT_PROPERTY(DerivedRGB)
        INIT_PROPERTY(DerivedRGBA)
        INIT_PROPERTY(DerivedStr64)
    END_INIT_PROPERTIES()
}

TestClass3::~TestClass3()
{
}

//----------------------------------------------------------------------------------------

TestApp1::TestApp1(const Pegasus::App::ApplicationConfig& config)
:   Pegasus::App::Application(config)
{
    SomeStrangeNamespace::TestEnum t1 = SomeStrangeNamespace::OPTION1;
    SomeStrangeNamespace::TestEnum t2 = SomeStrangeNamespace::OPTION4;
    PG_ASSERT(t1 < t2);
       

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
            const Pegasus::PropertyGrid::PropertyRecord & record = classInfo.GetClassProperty(cp);
            PG_LOG('PROP', "  Name: %s", record.name);
            PG_LOG('PROP', "    Type: %u", record.type);
            PG_LOG('PROP', "    Size: %u", record.size);    
            PG_LOG('PROP', "    Type name: %s", record.typeName);
        }
        PG_LOG('PROP', "Properties:");
        const unsigned int numProperties = classInfo.GetNumProperties();
        for (unsigned int p = 0; p < numProperties; ++p)
        {
            const Pegasus::PropertyGrid::PropertyRecord & record = classInfo.GetProperty(p);
            PG_LOG('PROP', "  Name: %s", record.name);
            PG_LOG('PROP', "    Type: %u", record.type);
            PG_LOG('PROP', "    Size: %u", record.size);    
            PG_LOG('PROP', "    Type name: %s", record.typeName);
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
}

//----------------------------------------------------------------------------------------
    
void TestApp1::InitializeApp()
{
    // initialize timeline blocks. required to load the timeline.
    REGISTER_TIMELINE_BLOCK(FractalCubeBlock);
    REGISTER_TIMELINE_BLOCK(FractalCube2Block);
    REGISTER_TIMELINE_BLOCK(TextureTestBlock);
    REGISTER_TIMELINE_BLOCK(GeometryTestBlock);

    // load the timeline. For now gets unloaded at destruction of timeline manager.
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
