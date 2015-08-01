#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/Allocator/Alloc.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Assertion.h"


using namespace Pegasus;
using namespace Pegasus::BlockScript;

#define POOL_INCREMENT 16

TypeTable::TypeTable()
{
}

TypeTable::~TypeTable()
{
    Shutdown();
}

void TypeTable::Initialize(Alloc::IAllocator* alloc)
{
    PG_ASSERT(mTypeDescPool.Size() == 0);
    mTypeDescPool.Initialize(alloc);
    mEnumNodePool.Initialize(alloc);
    mPropertyNodePool.Initialize(alloc);
}

void TypeTable::Shutdown()
{
    mTypeDescPool.Reset();
    mEnumNodePool.Reset();
    mPropertyNodePool.Reset();
}

TypeDesc* TypeTable::CreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    TypeDesc* child,
    TypeDesc::ModifierProperty modifierProperty,
    TypeDesc::AluEngine engine,
    Pegasus::BlockScript::Ast::StmtStructDef* structDef,
    EnumNode* enumNode,
    PropertyNode* propertyNode,
    ObjectPropertyAccessorCallback getPropCallback
)
{
    PG_ASSERT(modifier != TypeDesc::M_INVALID);
    int s = mTypeDescPool.Size();
    if (modifier != TypeDesc::M_ARRAY)
    {
        for (int i = 0; i < s; ++i)
        {
            TypeDesc* t = &mTypeDescPool[i];
            PG_ASSERT(t->GetModifier() != TypeDesc::M_INVALID);
            if (
                !Utils::Strcmp(name, t->GetName())
               )
            {
                if (
                        modifier == t->GetModifier() &&
                        child == t->GetChild() &&
                        modifierProperty == t->GetModifierProperty()
                   )
                {
                    return t;
                }
            }
        }
    }

    int idx = mTypeDescPool.Size();
    TypeDesc& newDesc = mTypeDescPool.PushEmpty();
    newDesc.SetName(name);
    newDesc.SetModifier(modifier);
    newDesc.SetChild(child);
    newDesc.SetModifierProperty(modifierProperty);
    newDesc.SetAluEngine(engine);
    newDesc.SetStructDef(structDef);
    newDesc.SetEnumNode(enumNode);
    newDesc.SetPropertyNode(propertyNode);
    newDesc.SetPropertyCallback(getPropCallback);

    int newSize = 0;
    bool success = newDesc.ComputeSize();
    PG_ASSERTSTR(success, "Fail computing size for type!");


    return &newDesc;
}

const TypeDesc* TypeTable::GetTypeByName(const char* name) const
{
    int s = mTypeDescPool.Size();
    for (int i = 0; i < s; ++i)
    {
        if(!Utils::Strcmp(name, mTypeDescPool[i].GetName()) && mTypeDescPool[i].GetModifier() != TypeDesc::M_ARRAY)
        {
            return &(mTypeDescPool[i]);
        }
    }
    return nullptr;
}

//!TODO: figure out how to recycle this code, and respect const correctness
TypeDesc* TypeTable::GetTypeForPatching(const char* name)
{
    int s = mTypeDescPool.Size();
    for (int i = 0; i < s; ++i)
    {
        if(!Utils::Strcmp(name, mTypeDescPool[i].GetName()) && mTypeDescPool[i].GetModifier() != TypeDesc::M_ARRAY)
        {
            return &(mTypeDescPool[i]);
        }
    }
    return nullptr;
}

bool TypeTable::FindEnumByName(const char* name, const EnumNode** outEnumNode, const TypeDesc** outEnumType) const
{
    int s = mTypeDescPool.Size();
    for (int i = 0; i < s; ++i)
    {
        const TypeDesc& typeDesc = mTypeDescPool[i];
        if(typeDesc.GetModifier() == TypeDesc::M_ENUM)
        {
            const EnumNode* node = typeDesc.GetEnumNode();
            while (node != nullptr)
            {
                if (!Utils::Strcmp(node->mIdd, name))
                {
                    *outEnumNode = node;    
                    *outEnumType = &typeDesc;
                    return true;
                }
                node = node->mNext;
            }
        }
    }
    return false;
}


EnumNode* TypeTable::NewEnumNode()
{
    return &mEnumNodePool.PushEmpty();
}

PropertyNode* TypeTable::NewPropertyNode()
{
    return &mPropertyNodePool.PushEmpty();
}

