/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunTable.cpp
//! \author Kleber Garcia
//! \date   September 14th
//! \brief  Function table - stores and retrieves functions, supports function overload

#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Utils/Memcpy.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

FunTable::FunTable()
{
}

FunTable::~FunTable()
{
}

void FunTable::Initialize(Alloc::IAllocator* alloc)
{
    mContainer.Initialize(alloc);
}

void FunTable::Reset()
{
    mContainer.Reset();
}

FunDesc* FunTable::Find(Ast::FunCall* funCall)
{
    int sz = mContainer.Size();

    for (int i = 0; i < sz; ++i)
    {
        FunDesc& candidate = mContainer[i];
        if (candidate.IsCompatible(funCall))
        {
            PG_ASSERT(candidate.GetGuid() == i);
            return &candidate;
        }
    }

    return nullptr;
}

FunDesc* FunTable::Insert(StmtFunDec* funDec)
{
    int sz = mContainer.Size();
    FunDesc* foundDeclaration = nullptr;
    for (int i = 0; i < sz; ++i)
    {
        FunDesc& candidate = mContainer[i];
        if (
            candidate.IsCompatible(funDec) 
        )
        {
            if (candidate.GetDec()->GetStmtList() != nullptr)
            {
                // function body already exists
                return nullptr;
            }
            else
            {
                PG_ASSERT(candidate.GetGuid() == i);
                foundDeclaration = &candidate;
            }
        }
    }

    // no declaration found. Its ok! lets just register its implementation then :)
    if (foundDeclaration == nullptr)
    {
        foundDeclaration = &(mContainer.PushEmpty());
        foundDeclaration->SetGuid(sz);
    }

    foundDeclaration->Initialize(funDec);
	return foundDeclaration;
}

