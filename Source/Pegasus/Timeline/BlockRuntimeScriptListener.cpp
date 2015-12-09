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
#include "Pegasus/Utils/Memset.h"

namespace Pegasus {
namespace Timeline {


void BlockRuntimeScriptListener::OnRuntimeBegin(Pegasus::BlockScript::BsVmState& state)
{
    /*nop*/
}

struct VarTranslation
{
    unsigned int typeSize;
    const char* typeName;
    const char* varName;
    const void* varDefault;
    Pegasus::PropertyGrid::PropertyType typeEnum;
    Pegasus::PropertyGrid::EditorDesc editorDesc;

    //default pieces of memory:
    Math::Color8RGBA colorDefault;
    bool boolDefault;

    VarTranslation() 
      :  typeEnum(Pegasus::PropertyGrid::PROPERTYTYPE_INVALID)
      ,  typeSize(0)
      ,  typeName(nullptr)
      ,  varDefault(nullptr)
      ,  colorDefault(0,0,0,0)
    {
    }
};

static BlockRuntimeScriptListener::ExternEditorType ReadEditorDesc(const BlockScript::Ast::Idd* var, PropertyGrid::EditorDesc& out)
{
    BlockRuntimeScriptListener::ExternEditorType returnEditorType = BlockRuntimeScriptListener::EDITOR_TYPE_DEFAULT;

    //does this guy have annotations?
    BlockScript::Ast::Annotations* annotations = var->GetAnnotations();
    if (annotations)
    {
        BlockScript::Ast::ExpList* current = annotations->GetExpList();
        while (current != nullptr && current->GetExp() != nullptr)
        {
            if (current->GetExp()->GetExpType() == BlockScript::Ast::Binop::sType &&
                static_cast<BlockScript::Ast::Binop*>(current->GetExp())->GetLhs()->GetExpType() == BlockScript::Ast::Idd::sType
               )
            {
                BlockScript::Ast::Idd* param = static_cast<BlockScript::Ast::Idd*>(static_cast<BlockScript::Ast::Binop*>(current->GetExp())->GetLhs());
                BlockScript::Ast::Exp* valueExp = static_cast<BlockScript::Ast::Binop*>(current->GetExp())->GetRhs();
                if (valueExp->GetExpType() == BlockScript::Ast::Imm::sType)
                {
                    BlockScript::Ast::Imm* value = static_cast<BlockScript::Ast::Imm*>(valueExp);
                    if (!Utils::Strcmp("EditorType", param->GetName())
                        && value->GetTypeDesc()->GetModifier() == BlockScript::TypeDesc::M_ENUM)
                    {
                        returnEditorType = static_cast<BlockRuntimeScriptListener::ExternEditorType>(value->GetVariant().i[0]);
                        PG_ASSERT(out.editorType >= PropertyGrid::EDITOR_DEFAULT && out.editorType < PropertyGrid::EDITOR_COUNT);
                    }
                    else if (!Utils::Strcmp("Min", param->GetName()) && sizeof(out.params.slidebar.min)>=value->GetTypeDesc()->GetByteSize())
                    {
                        Utils::Memcpy(&out.params.slidebar.min, &value->GetVariant(), value->GetTypeDesc()->GetByteSize());
                    }
                    else if (!Utils::Strcmp("Max", param->GetName()) && sizeof(out.params.slidebar.min)>=value->GetTypeDesc()->GetByteSize())
                    {   
                        Utils::Memcpy(&out.params.slidebar.max, &value->GetVariant(), value->GetTypeDesc()->GetByteSize());
                    }
                }
            }
            current = current->GetTail();
        }
    }

    return returnEditorType;
}

void SetDefaultEditor(PropertyGrid::EditorDesc& desc)
{
    desc.editorType = Pegasus::PropertyGrid::EDITOR_DEFAULT;
    Utils::Memset8(&desc.params, 0, sizeof(desc.params));
}

static bool TranslateType(
    const BlockScript::Ast::Idd* blockscriptVar,
    const BlockScript::Ast::Imm* defaultVal,
    VarTranslation& out,
    bool processAnnotations)
{
    Pegasus::BlockScript::TypeDesc::AluEngine typeAlu = blockscriptVar->GetTypeDesc()->GetAluEngine();
    Pegasus::PropertyGrid::PropertyType targetType = Pegasus::PropertyGrid::PROPERTYTYPE_INVALID;
    out.varName = blockscriptVar->GetName();
    out.varDefault = &defaultVal->GetVariant();
    SetDefaultEditor(out.editorDesc);
    bool isFloat = false;
    bool isVector4 = false;
    bool isVector3 = false;
    bool isInt = false;

    switch (typeAlu)
    {
    case Pegasus::BlockScript::TypeDesc::E_INT:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
        out.typeSize = sizeof(int);
        out.typeName = PropertyGrid::PropertyDefinition<int>::GetTypeName();
        isInt = true;
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
        out.typeSize = sizeof(float);
        out.typeName = PropertyGrid::PropertyDefinition<float>::GetTypeName();
        isFloat = true;
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT2:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC2;
        out.typeSize = sizeof(Math::Vec2);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec2>::GetTypeName();
        isFloat = true;
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT3:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC3;
        out.typeSize = sizeof(Math::Vec3);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec3>::GetTypeName();
        isFloat = true;
        isVector3 = true;
        break;
    case Pegasus::BlockScript::TypeDesc::E_FLOAT4:
        out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_VEC4;
        out.typeSize = sizeof(Math::Vec4);
        out.typeName = PropertyGrid::PropertyDefinition<Math::Vec4>::GetTypeName();
        isFloat = true;
        isVector4 = true;
        break;
    default:
        PG_LOG('ERR_', "Unsupported pegasus type from blockscript extern. On variable: %s ", out.varName);
        return false;
    }

    if (processAnnotations)
    {
        BlockRuntimeScriptListener::ExternEditorType externEditorType = ReadEditorDesc(blockscriptVar, out.editorDesc);
        if (externEditorType == BlockRuntimeScriptListener::EDITOR_TYPE_COLOR)
        {
            if (isVector4)
            {
                out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGBA;
                out.typeSize = sizeof(Math::Color8RGBA);
                out.typeName = PropertyGrid::PropertyDefinition<Math::Color8RGBA>::GetTypeName();
                for (int i = 0; i < 4; ++i) out.colorDefault.v[i] = (Math::PUInt8)(255.0f * defaultVal->GetVariant().f[i] + 0.5f);
                out.varDefault = &out.colorDefault;
            }
            else if (isVector3)
            {
                out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_COLOR8RGB;
                out.typeSize = sizeof(Math::Color8RGB);
                out.typeName = PropertyGrid::PropertyDefinition<Math::Color8RGB>::GetTypeName();
                for (int i = 0; i < 3; ++i) out.colorDefault.v[i] = (Math::PUInt8)(255.0f * defaultVal->GetVariant().f[i] + 0.5f);
                out.varDefault = &out.colorDefault;
            }
            else
            {
                SetDefaultEditor(out.editorDesc);
            }
        }
        else if (externEditorType == BlockRuntimeScriptListener::EDITOR_TYPE_CHECKBOX)
        {
            if (isInt)
            {
                out.typeEnum = Pegasus::PropertyGrid::PROPERTYTYPE_BOOL;
                out.typeSize = sizeof(bool);
                out.typeName = PropertyGrid::PropertyDefinition<bool>::GetTypeName();
                out.boolDefault = defaultVal->GetVariant().i[0] != 0;
                out.varDefault = &out.boolDefault;
            }
            else
            {
                SetDefaultEditor(out.editorDesc);
            }
        }
        else if (externEditorType == BlockRuntimeScriptListener::EDITOR_TYPE_SLIDER)
        {
            if (!isFloat)
            {
                SetDefaultEditor(out.editorDesc);
            }
        }
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
    PropertyGrid::PropertyAccessor a = mPropGrid->GetObjectPropertyAccessor(pgIndex);
    const PropertyGrid::PropertyRecord& r = mPropGrid->GetObjectPropertyRecord(pgIndex);
    union 
    {
        float v[16];
        int i;
    } scratch;
    unsigned int varDefSize = varDef.typeSize;
    if (r.type == PropertyGrid::PROPERTYTYPE_COLOR8RGB)
    {
        Math::Color8RGB c = a.Get<Math::Color8RGB>();
        varDefSize = 3*sizeof(float);
        for (int i = 0; i < 3; ++i) scratch.v[i] = ((float)c.v[i]) / 255.0f;
    }
    else if (r.type == PropertyGrid::PROPERTYTYPE_COLOR8RGBA)
    {
        Math::Color8RGBA c = a.Get<Math::Color8RGBA>();
        varDefSize = 4*sizeof(float);
        for (int i = 0; i < 4; ++i) scratch.v[i] = ((float)c.v[i]) / 255.0f;
    }
    else if (r.type == PropertyGrid::PROPERTYTYPE_BOOL)
    {
        bool b = a.Get<bool>();
        varDefSize = sizeof(int);
        scratch.i = b == true ? 1 : 0;
    }
    else
    {
        a.Read(&scratch, varDef.typeSize);
    }

    PG_ASSERT(varDefSize <= sizeof(scratch));
    mScript->WriteGlobalValue(&state, bsIndex, &scratch, varDefSize);

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
        if (!TranslateType(bsGlobalEntry.mVar, bsGlobalEntry.mDefaultVal, translation, true))
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
                if (translation.typeEnum == r.type && translation.editorDesc == r.editorDesc)
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
                        translation.varDefault,
                        &translation.editorDesc);

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
                translation.varDefault,
                &translation.editorDesc);

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
            if (TranslateType(bsGlobals[globalIndex].mVar, bsGlobals[globalIndex].mDefaultVal, translation, false))
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
