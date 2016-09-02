#ifndef PEGASUS_3D_RESOURCE
#define PEGASUS_3D_RESOURCE

/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Terrain3d.h
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  3d Terrain class.

#include "Pegasus/Application/GenericResource.h"
#include "Pegasus/PropertyGrid/Property.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
namespace RenderSystems
{

class Terrain3d : public Application::GenericResource
{
    BEGIN_DECLARE_PROPERTIES(Terrain3d,GenericResource)
        DECLARE_PROPERTY(bool, Visible, true)
        DECLARE_PROPERTY(Math::Vec3, Translation, Math::Vec3(0.0,0.0,0.0))
        DECLARE_PROPERTY(Math::Vec3, Rotation, Math::Vec3(0.0,0.0,0.0))
        DECLARE_PROPERTY(Math::Vec3, Scale, Math::Vec3(1.0,1.0,1.0))
    END_DECLARE_PROPERTIES()

public:
    Terrain3d(Alloc::IAllocator* allocator);
    virtual ~Terrain3d() {}
    virtual void Update();
    void Draw();
    static void GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions);
};

}
}

#endif
