/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BasicSky.h
//! \author Kleber Garcia
//! \date   January 14th 2018
//! \brief  A basic procedural sky.

#ifndef PEGASUS_BASIC_SKY
#define PEGASUS_BASIC_SKY

#include "Pegasus/RenderSystems/Config.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

#include "Pegasus/Render/Render.h"
#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/Math/Vector.h"

namespace Pegasus
{
namespace RenderSystems
{

class BasicSky : public Application::GenericResource
{
    BEGIN_DECLARE_PROPERTIES(BasicSky, GenericResource)
        DECLARE_PROPERTY(int, Visible, 1)
        DECLARE_PROPERTY(float, Distance, 2500)
        DECLARE_PROPERTY(int, CubeMapEnable, true)
        DECLARE_PROPERTY(int, CubeMapResolution, 256)
        DECLARE_PROPERTY(int, CubeMapAlwaysDirty, 0)
    END_DECLARE_PROPERTIES()

public:
    static void GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions);

    BasicSky(Alloc::IAllocator* allocator);
    virtual ~BasicSky() {};
    virtual void Update();
    void Draw();
    Render::CubeMapRef GetSkyCubeMap() { return mSkyCubeMap; }

private:
    void DrawUpdate();
    Render::CubeMapRef mSkyCubeMap;    
    Render::RenderTargetRef mSkyCubeTargets[Render::CUBE_FACE_COUNT];    
    bool mEnableStencil;
};

}
}

#endif
#endif
