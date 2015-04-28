/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeDesc.cpp
//! \author Kleber Garcia
//! \date   2nd September 2014
//! \brief  BlockScript type descriptor class. A type descriptor is a linked list of type
//!         modifiers (implementation)


#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

TypeDesc::TypeDesc()
:
mModifier(M_INVALID),
mAluEngine(E_NONE),
mChild(nullptr),
mModifierProperty(0),
mStructDef(nullptr),
mEnumNode(nullptr),
mPropertyNode(nullptr),
mPropertyCallback(nullptr)
{
    mName[0] = '\0';
}

TypeDesc::~TypeDesc()
{
}

void TypeDesc::SetName(const char * typeName)
{
#if PEGASUS_ENABLE_ASSERT
    int strlen = Utils::Strlen(typeName) + 1;
    PG_ASSERTSTR(strlen < TypeDesc::sMaxTypeName, "Maximum type name description registered");
#endif
    mName[0] = '\0';
    Utils::Strcat(mName, typeName);
}

bool TypeDesc::Equals(const TypeDesc* other) const
{
    return  other->mModifier == TypeDesc::M_STAR || mModifier == TypeDesc::M_STAR ||  //star means any type, so accept it
            (
                !Utils::Strcmp(mName, other->mName) &&
                CmpStructProperty(other) &&
                CmpEnumProperty(other) &&
                mModifier == other->mModifier &&
                mAluEngine == other->mAluEngine &&
                ((mChild == nullptr && other->mChild == nullptr) || (mChild != nullptr && other->mChild != nullptr && mChild->Equals(other->mChild))) &&
                mModifierProperty == other->mModifierProperty &&
                mByteSize == other->mByteSize
            );
}

bool TypeDesc::CmpEnumProperty(const TypeDesc* other) const
{
    if (mEnumNode == nullptr || other->mEnumNode == nullptr)
    {
        return mEnumNode == other->mEnumNode;
    }
    else
    {
        EnumNode* node1 = mEnumNode;
        EnumNode* node2 = other->mEnumNode;
    
        while (node1 != nullptr && node2 != nullptr)
        {
            if (Utils::Strcmp(node1->mIdd, node2->mIdd))
            {
                return false;
            }
            
            node1 = node1->mNext;
            node2 = node2->mNext;
        } 
        
        return node1 == node2;
    }

}

bool TypeDesc::CmpStructProperty(const TypeDesc* other) const
{
    if (mStructDef == nullptr || other->mStructDef == nullptr)
    {
        return mStructDef == other->mStructDef;
    }
    else
    {
        if (Utils::Strcmp(mStructDef->GetName(), other->mStructDef->GetName()))
        {
            return false;
        }

        ArgList* argList = mStructDef->GetArgList();
        ArgList* otherArgList = other->mStructDef->GetArgList();

        
        while (argList != nullptr && otherArgList != nullptr)
        {
            ArgDec* arg1 = argList->GetArgDec();
            ArgDec* arg2 = otherArgList->GetArgDec();
            if (arg1 != nullptr && arg2 != nullptr)
            {
                if (!arg1->GetType()->Equals(arg2->GetType()))
                {
                    return false;
                }
            }
            else if (arg1 != arg2)
            {
                return false;
            }
            argList = argList->GetTail();
            otherArgList = otherArgList->GetTail();
        }
        return (argList == nullptr || argList->GetArgDec() == nullptr) && (otherArgList == nullptr || otherArgList->GetArgDec() == nullptr);
    }

    
}

bool TypeDesc::ComputeSize()
{
    switch (GetModifier())
    {
    case TypeDesc::M_STAR:
    case TypeDesc::M_SCALAR:
    case TypeDesc::M_ENUM:
    case TypeDesc::M_REFERECE:
        mByteSize = 4; //4 bytes for scalars, enums, object refs and imms
        return true;
    case TypeDesc::M_VECTOR:
        mByteSize = 4 * GetModifierProperty();
        return true;
    case TypeDesc::M_ARRAY:
        {
            if (GetChild() != nullptr) GetChild()->ComputeSize();
            mByteSize = GetModifierProperty() * GetChild()->GetByteSize(); //4 bytes for reference.
            return true;
        }
    case TypeDesc::M_STRUCT:
        {
            const Ast::StmtStructDef* structDef = GetStructDef();
            PG_ASSERT(structDef != nullptr);
            Ast::ArgList* argList = structDef->GetArgList();
            int totalSize = 0;
            while (argList != nullptr)
            {
                if (argList->GetArgDec() != nullptr)
                {
                    const TypeDesc* typeDesc = argList->GetArgDec()->GetType();

                    totalSize += typeDesc->GetByteSize();
                }
                argList = argList->GetTail();                    
            }
            mByteSize = totalSize;
        }
        return true;
    default:
        PG_FAILSTR("Unhandled modifier while computing file size :(");
        return false;
    }
    return false;
}
