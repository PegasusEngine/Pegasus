/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   3dTerrainSystem.h
//! \author Kleber Garcia
//! \date   June 23rd, 2016
//! \brief  3dTerrain system definition file

#ifndef PEGASUS_RENDER_SYSTEM_3DTERRAIN_H
#define PEGASUS_RENDER_SYSTEM_3DTERRAIN_H
#include "Pegasus\RenderSystems\Config.h"
#if RENDER_SYSTEM_CONFIG_ENABLE_3DTERRAIN

#include "Pegasus/RenderSystems/System/RenderSystem.h"

namespace Pegasus 
{
namespace RenderSystems
{

//! Deferred renderer system implementation. Adds a blockscript library.
class Terrain3dSystem : public RenderSystem
{
public:
    //! Constructor
    explicit Terrain3dSystem (Alloc::IAllocator* allocator) : RenderSystem(allocator) {}

    //! destructor
    virtual ~Terrain3dSystem () {}

    virtual bool CanCreateBlockScriptApi() const { return true; }

    virtual const char* GetSystemName() const { return "3dTerrainSystem"; } 

    virtual void OnRegisterBlockscriptApi(BlockScript::BlockLib* blocklib, Core::IApplicationContext* appContext);


};
}
}
#endif
#endif
