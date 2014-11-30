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
: mGuid(-1), mFunDec(nullptr), mSignatureLength(0), mCallback(nullptr), mInputArgumentByteSize(0)
{
    mName[0] = '\0'; 
    mSignature[0] = '\0';
}

FunDesc::~FunDesc()
{
}

void FunDesc::ConstructImpl(StmtFunDec* funDec)
{
    //construct function declaration by first using the name
    mName[0] = '\0';
    Utils::Strcat(mName, funDec->GetName());

    mSignature[0] = '\0';
    mSignatureLength = 0;
    Utils::Strcat(&mSignature[0], mName);

    int sz = Utils::Strlen(mName);

    ArgList* head = funDec->GetArgList();

    mInputArgumentByteSize = 0;
    
    while (head != nullptr)
    {
        ArgDec* argDec = head->GetArgDec();
        if (argDec != nullptr)
        {
            mSignature[sz++] = '_';
            PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
            long long tid = reinterpret_cast<long long>(argDec->GetType());
            Utils::Memcpy(&mSignature[sz], &tid, sizeof(long long));
            sz += sizeof(long long);
            PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
            mInputArgumentByteSize += argDec->GetType()->GetByteSize();
        }

        head = head->GetTail();
    }

    mSignature[sz++] = '\0';
    PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
    mSignatureLength = sz;
    mFunDec = funDec;
}

void FunDesc::ConstructDec(FunCall* funDec)
{
    //construct function declaration by first using the name
    mName[0] = '\0';
    Utils::Strcat(mName, funDec->GetName());

    mSignature[0] = '\0';
    mSignatureLength = 0;
    Utils::Strcat(&mSignature[0], mName);

    int sz = Utils::Strlen(mName);
    
    ExpList* head = funDec->GetArgs();
    
    while (head != nullptr)
    {
        Exp* argDec = head->GetExp();
        if (argDec != nullptr)\
        {
            mSignature[sz++] = '_';
            PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
            long long tid = reinterpret_cast<long long>(argDec->GetTypeDesc());
            Utils::Memcpy(&mSignature[sz], &tid, sizeof(long long));
            sz += sizeof(long long);
            PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
        }

        head = head->GetTail();
    }

    mSignature[sz++] = '\0';
    PG_ASSERT(sz < MAX_SIGNATURE_LENGTH);
    mSignatureLength = sz;
    mFunDec = nullptr;
}
bool FunDesc::Equals(const FunDesc* other) const
{
    if (mSignatureLength != other->mSignatureLength)
    {
        return false;
    }
    else
    {
        for (int byte = 0; byte < mSignatureLength; ++byte)
        {
            if (mSignature[byte] != other->mSignature[byte])
            {
                return false;
            }
        }
    }

    return true;
}
