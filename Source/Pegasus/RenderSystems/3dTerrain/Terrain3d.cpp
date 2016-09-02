/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Terrain3d.cpp
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  3d Terrain class.

#include "Pegasus/RenderSystems/3dTerrain/Terrain3d.h"
#include "Pegasus/Application/RenderCollection.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/BlockScript/BsVm.h"

using namespace Pegasus;
using namespace Pegasus::Application;

namespace Pegasus
{
namespace RenderSystems
{

BEGIN_IMPLEMENT_PROPERTIES(Terrain3d)
    IMPLEMENT_PROPERTY(Terrain3d, Visible)
    IMPLEMENT_PROPERTY(Terrain3d, Translation)
    IMPLEMENT_PROPERTY(Terrain3d, Rotation)
    IMPLEMENT_PROPERTY(Terrain3d, Scale)
END_IMPLEMENT_PROPERTIES(Terrain3d)

Terrain3d::Terrain3d(Alloc::IAllocator* allocator)
    : Application::GenericResource(allocator)
{
    //INIT properties
    BEGIN_INIT_PROPERTIES(Terrain3d)
        INIT_PROPERTY(Visible)
        INIT_PROPERTY(Translation)
        INIT_PROPERTY(Rotation)
        INIT_PROPERTY(Scale)
    END_INIT_PROPERTIES()
}

void Terrain3d::Update()
{
}

static void CreateTerrain3d_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());

    int height = stream.NextArgument<int>();    
    int maxBlockCount = stream.NextArgument<int>();    
    Terrain3d* newTerrain = PG_NEW(Memory::GetCoreAllocator(), -1, "3dTerrain", Pegasus::Alloc::PG_MEM_TEMP) Terrain3d(Memory::GetCoreAllocator());

    RenderCollection::CollectionHandle handle = RenderCollection::AddResource<GenericResource>(collection, static_cast<GenericResource*>(newTerrain));
    stream.SubmitReturn<RenderCollection::CollectionHandle>(handle);

}

static void DrawTerrain3d_Callback(BlockScript::FunCallbackContext& context)
{
    BlockScript::FunParamStream stream(context);
    RenderCollection* collection = static_cast<RenderCollection*>(context.GetVmState()->GetUserContext());
    RenderCollection::CollectionHandle thisHandle = stream.NextArgument<RenderCollection::CollectionHandle>();    

    if (thisHandle != RenderCollection::INVALID_HANDLE)
    {
        
        Terrain3d* theTerrain = static_cast<Terrain3d*>(RenderCollection::GetResource<GenericResource>(collection, thisHandle));
        theTerrain->Draw();
    }
    else
    {
        PG_LOG('ERR_', "Error: attempting to draw an invalid terrain.");
    }
}

void Terrain3d::GetBlockScriptApi(Utils::Vector<BlockScript::FunctionDeclarationDesc>& methods, Utils::Vector<BlockScript::FunctionDeclarationDesc>& functions)
{
    BlockScript::FunctionDeclarationDesc desc;
    desc.functionName = "CreateTerrain3d";
    desc.returnType = "Terrain3d";
    desc.argumentTypes[0] = "int";
    desc.argumentNames[0] = "heigth";
    desc.argumentTypes[1] = "int";
    desc.argumentNames[1] = "maxBlockCount";
    desc.argumentTypes[2] = nullptr;
    desc.argumentNames[2] = nullptr;
    desc.callback = CreateTerrain3d_Callback;
    functions.PushEmpty() = desc; 

    desc.functionName = "Draw";
    desc.returnType = "int";
    desc.argumentTypes[0] = "Terrain3d";
    desc.argumentNames[0] = "this";
    desc.argumentTypes[1] = nullptr;
    desc.argumentNames[1] = nullptr;
    desc.callback = DrawTerrain3d_Callback;
    methods.PushEmpty() = desc;

}

void Terrain3d::Draw()
{
}

}
}
