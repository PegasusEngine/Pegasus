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
}

void TypeTable::Shutdown()
{
    mTypeDescPool.Reset();
    mEnumNodePool.Reset();
}

TypeDesc* TypeTable::CreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    const TypeDesc* child,
    int modifierProperty,
    TypeDesc::AluEngine engine,
    Pegasus::BlockScript::Ast::StmtStructDef* structDef,
    TypeDesc::EnumNode* enumNode
)
{
    PG_ASSERT(modifier != TypeDesc::M_INVALID);
    int s = mTypeDescPool.Size();
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


    int idx = mTypeDescPool.Size();
    TypeDesc& newDesc = mTypeDescPool.PushEmpty();
    newDesc.SetName(name);
    newDesc.SetModifier(modifier);
    newDesc.SetChild(child);
    newDesc.SetModifierProperty(modifierProperty);
    newDesc.SetAluEngine(engine);
    newDesc.SetStructDef(structDef);
    newDesc.SetEnumNode(enumNode);

    int newSize = 0;
    bool success = ComputeSize(&newDesc, newSize);
    PG_ASSERTSTR(success, "Fail computing size for type!");
    newDesc.SetByteSize(newSize);


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
bool TypeTable::FindEnumByName(const char* name, const TypeDesc::EnumNode** outEnumNode, const TypeDesc** outEnumType) const
{
    int s = mTypeDescPool.Size();
    for (int i = 0; i < s; ++i)
    {
        const TypeDesc& typeDesc = mTypeDescPool[i];
        if(typeDesc.GetModifier() == TypeDesc::M_ENUM)
        {
            const TypeDesc::EnumNode* node = typeDesc.GetEnumNode();
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

bool TypeTable::ComputeSize(const TypeDesc* t, int& outSize) const
{
    if (t != nullptr)
    {
        switch (t->GetModifier())
        {
        case TypeDesc::M_SCALAR:
        case TypeDesc::M_ENUM:
            outSize = 4; //4 bytes for scalars, enums and imms
            return true;
        case TypeDesc::M_VECTOR:
            outSize = 4 * t->GetModifierProperty();
            return true;
        case TypeDesc::M_REFERECE:
            outSize = 4; //4 bytes for references
            return true;
        case TypeDesc::M_ARRAY:
            {
                outSize = t->GetModifierProperty() * t->GetChild()->GetByteSize(); //4 bytes for reference.
            return true;
            }
        case TypeDesc::M_STRUCT:
            {
                const Ast::StmtStructDef* structDef = t->GetStructDef();
                PG_ASSERT(structDef != nullptr);
                Ast::ArgList* argList = structDef->GetArgList();
                int totalSize = 0;
                while (argList != nullptr)
                {
                    int computedSize = 0;
                    const TypeDesc* typeDesc = argList->GetArgDec()->GetType();

                    totalSize += typeDesc->GetByteSize();
                    argList = argList->GetTail();
                }
                outSize = totalSize;
            }
            return true;
        default:
            PG_FAILSTR("Unhandled modifier while computing file size :(");
            return false;
        }
    }
    return false;
}

TypeDesc::EnumNode* TypeTable::NewEnumNode()
{
    return &mEnumNodePool.PushEmpty();
}



