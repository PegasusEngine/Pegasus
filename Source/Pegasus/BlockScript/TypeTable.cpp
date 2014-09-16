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
    
    //type description pool is ready, lets register some intrinsic types
    InternalRegisterIntrinsicTypes();
}

void TypeTable::InternalRegisterIntrinsicTypes()
{
    //Register ints and scalars
    int floatId = CreateType(
        TypeDesc::M_SCALAR,
        "float"
    );

    int intId   = CreateType(
        TypeDesc::M_SCALAR,
        "int"
    );

    char fStr[25];
    fStr[0] = '\0';
    char iStr[25];
    iStr[0] = '\0';
    Utils::Strcat(fStr, "float");
    Utils::Strcat(iStr, "int");
    int fStrLen = Utils::Strlen(fStr);
    int iStrLen = Utils::Strlen(iStr);
    PG_ASSERT(Ast::gMaxAluDimensions < 10); //only 1 digit numbers
    for (int i = 2; i <= Ast::gMaxAluDimensions; ++i)
    {
        fStr[fStrLen] = i + '0';
        iStr[iStrLen] = i + '0';
        fStr[fStrLen+1] = '\0';
        iStr[iStrLen+1] = '\0';
        CreateType(
            TypeDesc::M_VECTOR,
            fStr,
            floatId,
            i
        );

        CreateType(
            TypeDesc::M_VECTOR,
            iStr,
            intId,
            i
        );
    }
}

void TypeTable::Shutdown()
{
    mTypeDescPool.Reset();
}

int TypeTable::CreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    int child,
    int modifierProperty
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
                return i;
            }
            else
            {
                PG_ASSERT("WTF!!");
                return -1;
            }
        }
    }


    int idx = mTypeDescPool.Size();
    TypeDesc& newDesc = mTypeDescPool.PushEmpty();
    newDesc.SetName(name);
    newDesc.SetGuid(idx);
    newDesc.SetModifier(modifier);
    newDesc.SetChild(child);
    newDesc.SetModifierProperty(modifierProperty);
    return idx;
}

const TypeDesc* TypeTable::GetTypeDesc(int guid) const
{
    PG_ASSERT(guid >= 0 && guid < mTypeDescPool.Size());
    return &mTypeDescPool[guid];
}

int TypeTable::GetTypeByName(const char* name) const
{
    int s = mTypeDescPool.Size();
    for (int i = 0; i < s; ++i)
    {
        if(!Utils::Strcmp(name, mTypeDescPool[i].GetName()))
        {
            PG_ASSERT(mTypeDescPool[i].GetGuid() == i);
            return i;
        }
    }
    return -1;
}

bool TypeTable::ComputeSize(int guid, int& outSize) const
{
    if (guid >= 0 && guid < mTypeDescPool.Size())
    {
        const TypeDesc* t = GetTypeDesc(guid);
        switch (t->GetModifier())
        {
        case TypeDesc::M_SCALAR:
            outSize = 4; //4 bytes for scalars
            return true;
        case TypeDesc::M_VECTOR:
            outSize = 4 * t->GetModifierProperty();
            return true;
        case TypeDesc::M_REFERECE:
            outSize = 4; //4 bytes for references
            return true;
        case TypeDesc::M_ARRAY:
            {
                int tmpSz = 0;
                bool found = ComputeSize(t->GetChild(), tmpSz);
                outSize = tmpSz * t->GetModifierProperty();
                return found;
            }
        case TypeDesc::M_TREE:
            outSize = t->GetModifierProperty();
            return true;
        default:
            return false;
        }
    }
    return false;
}



