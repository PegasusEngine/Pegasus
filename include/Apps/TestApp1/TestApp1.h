/****************************************************************************************/
/*                                                                                      */
/*                                  Pegasus Application                                 */
/*                                                                                      */
/****************************************************************************************/

//! \file   TestApp1.h
//! \author Karolyn Boulanger
//! \date   07th July 2013
//! \brief  Test application 1

#ifndef TESTAPP1_H
#define TESTAPP1_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Application/Application.h"
#include "Pegasus/Timeline/Block.h"


//! \todo TEMPORARY, property grid testing
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/PropertyGrid/PropertyGridEnumType.h"
using namespace Pegasus::Math;

namespace SomeStrangeNamespace {
BEGIN_DECLARE_ENUM(TestEnum)
    DECLARE_ENUM(TestEnum,OPTION1)
    DECLARE_ENUM(TestEnum,OPTION2)
    DECLARE_ENUM(TestEnum,OPTION3)
    DECLARE_ENUM(TestEnum,OPTION4)
END_DECLARE_ENUM()

}

REGISTER_ENUM_METATYPE(SomeStrangeNamespace::TestEnum)



class TestClass2 : public Pegasus::PropertyGrid::PropertyGridObject
{

    BEGIN_DECLARE_PROPERTIES_BASE(TestClass2)
        DECLARE_PROPERTY2(bool, ParentBool, true)
        DECLARE_PROPERTY2(int, ParentInt, -10)
        DECLARE_PROPERTY2(unsigned int, ParentUInt, 23)
        DECLARE_PROPERTY2(float, ParentFloat, 0.5f)
        DECLARE_PROPERTY2(Vec2, ParentVec2, Vec2(1.0f, 2.0f))
        DECLARE_PROPERTY2(Vec3, ParentVec3, Vec3(3.0f, 4.0f, 5.0f))
        DECLARE_PROPERTY2(Vec4, ParentVec4, Vec4(6.0f, 7.0f, 8.0f, 9.0f))
        DECLARE_PROPERTY2(Color8RGB, ParentRGB, Color8RGB(10, 11, 12))
        DECLARE_PROPERTY2(Color8RGBA, ParentRGBA, Color8RGBA(13, 14, 15, 16))
        DECLARE_PROPERTY2(Pegasus::PropertyGrid::String64, ParentStr64, "ParentStr64")
    END_DECLARE_PROPERTIES2()
    
public:
    TestClass2();
    virtual ~TestClass2();
};

class TestClass3 : public TestClass2
{
    BEGIN_DECLARE_PROPERTIES2(TestClass3, TestClass2)
        DECLARE_PROPERTY2(bool, DerivedBool, true)
        DECLARE_PROPERTY2(int, DerivedInt, -20)
        DECLARE_PROPERTY2(unsigned int, DerivedUInt, 45)
        DECLARE_PROPERTY2(float, DerivedFloat, -0.5f)
        DECLARE_PROPERTY2(Vec2, DerivedVec2, Vec2(101.0f, 102.0f))
        DECLARE_PROPERTY2(Vec3, DerivedVec3, Vec3(103.0f, 104.0f, 105.0f))
        DECLARE_PROPERTY2(Vec4, DerivedVec4, Vec4(106.0f, 107.0f, 108.0f, 109.0f))
        DECLARE_PROPERTY2(Color8RGB, DerivedRGB, Color8RGB(110, 111, 112))
        DECLARE_PROPERTY2(Color8RGBA, DerivedRGBA, Color8RGBA(113, 114, 115, 116))
        DECLARE_PROPERTY2(Pegasus::PropertyGrid::String64, DerivedStr64, "DerivedStr64")
    END_DECLARE_PROPERTIES2()
    
public:
    TestClass3();
    virtual ~TestClass3();
};

//----------------------------------------------------------------------------------------

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

    //! Custom initialization, done before the timeline triggers the loading of blocks and their assets
    virtual void InitializeApp();

    //! Custom shutdown
    virtual void ShutdownApp();

    //------------------------------------------------------------------------------------
    
private:
    
    //! \todo Temporary
    //! Texture test block, used to retrieve the list of textures for the editor
    Pegasus::Timeline::Block * mTextureTestBlock;
};


//! \todo Make this a macro with only the name of the app class as parameter
Pegasus::App::Application * CreateApplication(const Pegasus::App::ApplicationConfig& config);
void DestroyApplication(Pegasus::App::Application* app);

#endif  // TESTAPP1_H
