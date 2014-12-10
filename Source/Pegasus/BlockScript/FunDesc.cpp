/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunDesc.cpp
//! \author Kleber Garcia
//! \date   September 14th
//! \brief  Function description structure, stored in a function table

#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

FunDesc::FunDesc()
: mGuid(-1), mFunDec(nullptr), mCallback(nullptr), mInputArgumentByteSize(0), mIsMethod(false)
{
}

FunDesc::~FunDesc()
{
}

void FunDesc::Initialize(StmtFunDec* funDec)
{
    mFunDec = funDec;

    ArgList* argList = funDec->GetArgList();
    while (argList != nullptr)
    {
        if (argList->GetArgDec() != nullptr)
        {
            mInputArgumentByteSize += argList->GetArgDec()->GetType()->GetByteSize();
        }
        argList = argList->GetTail();
    }
}

bool FunDesc::Equals(const FunDesc* other) const
{
    return AreSignaturesEqual(other->mFunDec->GetName(), other->mFunDec->GetArgList());
}

bool FunDesc::AreSignaturesEqual(const char* name, Ast::ArgList* argList) const
{
    if (Utils::Strcmp(name, mFunDec->GetName()))
    {
        return false;
    }

    ArgList* argList1 = argList;
    ArgList* argList2 = mFunDec->GetArgList();

    while (argList1 != nullptr && argList2 != nullptr)
    {
        ArgDec* argDec1 = argList1->GetArgDec();
        ArgDec* argDec2 = argList2->GetArgDec();
        if (argDec1 != nullptr && argDec2 != nullptr)
        {
            if (!argDec1->GetType()->Equals(argDec2->GetType()))
            {
                return false;
            }
        }
        else
        {
            return argDec1 == argDec2;
        }
        argList1 = argList1->GetTail();        
        argList2 = argList2->GetTail();        
    }

    return ((argList1 == nullptr || argList1->GetArgDec() == nullptr) && (argList2 == nullptr || argList2->GetArgDec() == nullptr));
}

bool FunDesc::AreSignaturesEqual(const char* name, Ast::ExpList* argList) const
{
    if (Utils::Strcmp(name, mFunDec->GetName()))
    {
        return false;
    }

    ExpList* argList1 = argList;
    ArgList* argList2 = mFunDec->GetArgList();

    while (argList1 != nullptr && argList2 != nullptr)
    {
        Exp*    argDec1 = argList1->GetExp();
        ArgDec* argDec2 = argList2->GetArgDec();
        if (argDec1 != nullptr && argDec2 != nullptr)
        {
            if (!argDec1->GetTypeDesc()->Equals(argDec2->GetType()))
            {
                return false;
            }
        }
        else
        {
            return argDec1 == nullptr && argDec2 == nullptr;
        }
        argList1 = argList1->GetTail();        
        argList2 = argList2->GetTail();        
    }

    return ((argList1 == nullptr || argList1->GetExp() == nullptr) && (argList2 == nullptr || argList2->GetArgDec() == nullptr));
}

bool FunDesc::IsCompatible(const FunCall* funCall) const
{
    return mIsMethod == funCall->IsMethod() && AreSignaturesEqual(funCall->GetName(), funCall->GetArgs());
}

bool FunDesc::IsCompatible(const StmtFunDec* funDec) const
{
    return mIsMethod == false && AreSignaturesEqual(funDec->GetName(), funDec->GetArgList());
}
