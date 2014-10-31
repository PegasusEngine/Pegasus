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
    TypeDesc* floatT = CreateType(
        TypeDesc::M_SCALAR,
        "float"
    );

    floatT->SetAluEngine(TypeDesc::E_FLOAT);

    TypeDesc* intT   = CreateType(
        TypeDesc::M_SCALAR,
        "int"
    );
    intT->SetAluEngine(TypeDesc::E_INT);

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
        TypeDesc* t1 = CreateType(
            TypeDesc::M_VECTOR,
            fStr,
            floatT,
            i
        );
        t1->SetAluEngine(static_cast<TypeDesc::AluEngine>(TypeDesc::E_FLOAT + i - 1));

    }

    CreateType(TypeDesc::M_REFERECE, "string");
}

void TypeTable::Shutdown()
{
    mTypeDescPool.Reset();
}

TypeDesc* TypeTable::CreateType(
    TypeDesc::Modifier modifier,
    const char * name,
    const TypeDesc* child,
    int modifierProperty,
    TypeDesc::AluEngine engine,
    Pegasus::BlockScript::Ast::StmtStructDef* structDef
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
    newDesc.SetGuid(idx);
    newDesc.SetModifier(modifier);
    newDesc.SetChild(child);
    newDesc.SetModifierProperty(modifierProperty);
    newDesc.SetAluEngine(engine);
    newDesc.SetStructDef(structDef);

    int newSize = 0;
    bool success = ComputeSize(&newDesc, newSize);
    PG_ASSERTSTR(success, "Fail computing size for type!");
    newDesc.SetByteSize(newSize);

    return &newDesc;
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
        if(!Utils::Strcmp(name, mTypeDescPool[i].GetName()) && mTypeDescPool[i].GetModifier() != TypeDesc::M_ARRAY)
        {
            PG_ASSERT(mTypeDescPool[i].GetGuid() == i);
            return i;
        }
    }
    return -1;
}

bool TypeTable::ComputeSize(const TypeDesc* t, int& outSize) const
{
    if (t != nullptr)
    {
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
            return false;
        }
    }
    return false;
}



