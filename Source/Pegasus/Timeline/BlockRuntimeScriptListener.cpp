/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockRuntimeScriptListener.cpp
//! \author	Kleber Garcia
//! \date	29th November 2015
//! \brief	class containing callbacks that act on a block.
//!         these callbacks are triggered from a script running.

#include "Pegasus/Timeline/BlockRuntimeScriptListener.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Utils/String.h"

namespace Pegasus {
namespace Timeline {


void BlockRuntimeScriptListener::OnRuntimeBegin(Pegasus::BlockScript::BsVmState& state)
{
    /*nop*/
}

struct VarTranslation
{
    int typeSize;
    const char* typeName;
    const char* varName;
    Pegasus::PropertyGrid::PropertyType typeEnum;

    VarTranslation() 
      :  typeEnum(Pegasus::PropertyGrid::PROPERTYTYPE_INVALID)
      ,  typeSize(0)
      ,  typeName(nullptr)
    {
    }
};

static bool TranslateType(const BlockScript::Ast::Idd* blockscriptVar, VarTranslation& out)
{
    Pegasus::BlockScript::TypeDesc::AluEngine typeAlu = blockscriptVar->GetTypeDesc()->GetAluEngine();
    Pegasus::PropertyGrid::PropertyType targetType = Pegasus::PropertyGrid::PROPERTYTYPE_INVALID;
    out.varName = blockscriptVar->GetName();
    switch (typeAlu)
    {
    case Pegasus::BlockScript::TypeDesc::E_INT:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
        out.typeSize = sizeof(int);
        out.typeName = PropertyGrid::PropertyDefinition<int>::GetTypeName();
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
        out.typeSize = sizeof(float);
        out.typeName = PropertyGrid::PropertyDefinition<float>::GetTypeName();
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT2:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC2;
        out.typeSize = sizeof(Math::Vec2);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec2>::GetTypeName();
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT3:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC3;
        out.typeSize = sizeof(Math::Vec3);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec3>::GetTypeName();
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT4:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC4;
        out.typeSize = sizeof(Math::Vec4);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec4>::GetTypeName();
        break;
    default:
        PG_LOG('ERR_', "Unsupported pegasus type from blockscript extern. On variable: %s ", out.varName);
        return false;
    }

    return true;
}

void BlockRuntimeScriptListener::SetScriptVariable(
    const VarTranslation& varDef,
    unsigned int pgIndex,
    Pegasus::BlockScript::BsVmState& state,
    int bsIndex 
)
{
    float scratch[16];
    PropertyGrid::PropertyAccessor a = mPropGrid->GetObjectPropertyAccessor(pgIndex);
    PG_ASSERT(varDef.typeSize <= sizeof(scratch));
    //initialize the runtime propertly
    a.Read(scratch, varDef.typeSize);
    mScript->WriteGlobalValue(&state, bsIndex, scratch, varDef.typeSize);
}

void BlockRuntimeScriptListener::OnStackInitalized(Pegasus::BlockScript::BsVmState& state)
{
    if (!IsReady())
    {
        PG_LOG('ERR_', "Trying to initialize stack without the script being ready.");
        return;
    }

    //The stack has been initialized. Now lets patch everything that has a property grid.
    Pegasus::BlockScript::Assembly a = mScript->GetAsm();
    Pegasus::BlockScript::Container<Pegasus::BlockScript::GlobalMapEntry>& bsGlobals = *a.mGlobalsMap;
    if (bsGlobals.Size() == 0 && mPropGrid->GetNumObjectProperties() == 0)
    {
        return;
    }

    Utils::Vector<bool> foundInGrid;
    for (unsigned int i = 0; i < mPropGrid->GetNumObjectProperties(); ++i)
    {
        foundInGrid.PushEmpty() = false;
    }

    for (int i = 0; i < bsGlobals.Size(); ++i)
    {
        Pegasus::BlockScript::GlobalMapEntry& bsGlobalEntry = bsGlobals[i];
        VarTranslation translation;
        if (!TranslateType(bsGlobalEntry.mVar, translation))
        {
            continue;
        }

        //find the target property grid
        bool foundObject = false;
        for (unsigned int obPropIndex = 0; !foundObject && obPropIndex < mPropGrid->GetNumObjectProperties(); ++obPropIndex)
        {
            const PropertyGrid::PropertyRecord& r = mPropGrid->GetObjectPropertyRecord(obPropIndex);
            if (!Utils::Strcmp(translation.varName, r.name))
            {
                foundObject = true;
                if (translation.typeEnum == r.type)
                {
                    foundInGrid[obPropIndex] = true; 
                    mGridToGlobalMap[obPropIndex] = i;
                    SetScriptVariable(translation, obPropIndex, state, i);
                }
                else
                { 
                    PG_ASSERTSTR(!foundInGrid[obPropIndex], "Cannot attempt to delete an object twice!");
                    mPropGrid->RemoveObjectProperty(obPropIndex);
                    foundInGrid.Delete(obPropIndex);

                    mPropGrid->AddObjectProperty(
                        translation.typeEnum, 
                        translation.typeSize, 
                        translation.varName, 
                        translation.typeName, 
                        &bsGlobalEntry.mDefaultVal->GetVariant());

                    foundInGrid.PushEmpty() = true;
                    foundInGrid[mPropGrid->GetNumObjectProperties() - 1] = true;
                    mGridToGlobalMap.Delete(obPropIndex);
                    mGridToGlobalMap.PushEmpty() = i;
                }
            }
        }

        //We never found this property, lets create it from scratch
        if (!foundObject)
        {
            mPropGrid->AddObjectProperty(
                translation.typeEnum, 
                translation.typeSize,
                translation.varName, 
                translation.typeName, 
                &bsGlobalEntry.mDefaultVal->GetVariant());

            foundInGrid.PushEmpty() = true;
            mGridToGlobalMap.PushEmpty() = i;
        }
    }

    //delete now all the properties that are unused
    for (unsigned int i = foundInGrid.GetSize(); i >= 1; --i)
    {
        if (!foundInGrid[i - 1])
        {
            mPropGrid->RemoveObjectProperty(i - 1);
            mGridToGlobalMap.Delete(i - 1);
        }
    }   
}

void BlockRuntimeScriptListener::OnRuntimeExit(Pegasus::BlockScript::BsVmState& state)
{
    /*nop*/
}

void BlockRuntimeScriptListener::OnCrash(Pegasus::BlockScript::BsVmState& state, const Pegasus::BlockScript::CrashInfo& crashInfo)
{
    /*TODO: handle this*/
}

BlockRuntimeScriptListener::UpdateType BlockRuntimeScriptListener::FlushProperty(Pegasus::BlockScript::BsVmState& state, unsigned int index)
{
    if (IsReady())
    {
        Pegasus::BlockScript::Assembly a = mScript->GetAsm();
        Pegasus::BlockScript::Container<Pegasus::BlockScript::GlobalMapEntry>& bsGlobals = *a.mGlobalsMap;
        PG_ASSERT(index >= 0 && index < mPropGrid->GetNumObjectProperties());

        int globalIndex = mGridToGlobalMap[index];
        if (globalIndex >= 0 && globalIndex < bsGlobals.Size())
        {
            VarTranslation translation;
            if (TranslateType(bsGlobals[globalIndex].mVar, translation))
            {
                SetScriptVariable(translation, index, state, globalIndex);
                return bsGlobals[globalIndex].mVar->GetMetaData().isUsedInGlobalScope 
                       ? BlockRuntimeScriptListener::RERUN_GLOBALS
                       : BlockRuntimeScriptListener::RERUN_RENDER;
            }
        }
        else
        {
            PG_FAILSTR("Invalid index indirection. Check variables reset from blockscript to propgrid.");
        }
    }
    else
    {
        PG_LOG('ERR_', "Trying to set a property without the script being ready.");
    }
    return BlockRuntimeScriptListener::NOTHING;
}

} //namespace Timeline
} //namespace Pegasus
