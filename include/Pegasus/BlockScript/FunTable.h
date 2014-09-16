/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunTable.h
//! \author Kleber Garcia
//! \date   September 14th
//! \brief  Function table - stores and retrieves functions, supports function overload

#ifndef PEGASUS_FUN_TABLE_H
#define PEGASUS_FUN_TABLE_H

#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/FunDesc.h"

namespace Pegasus
{
namespace BlockScript
{

namespace Ast
{
    class FunCall;
    class StmtFunDec;
}

class FunTable
{
public:

    //! constructor
    FunTable(); 

    //! destructor
    ~FunTable();

    //! initializes memory management for function table
    void Initialize(Alloc::IAllocator* alloc);

    //! resets the allocations (no free memory though)
    void Reset();

    //! Finds or inserts a function declaration if function is not found.
    //! a FunDesc* struct is guaranteed to be returned
    //! \param funCall the function call to find a function description for
    //! \return the id of this function call
    int Find(Ast::FunCall* funCall);

    //! \param the guid to get this desc from
    //! \return the description structure we get
    const FunDesc* GetDesc(int guid);

    //! Inserts a new function declaration. Function declaration must be only inserted once.
    //! if there is an existing function declared, false is returned, otherwise always true.
    //! \return id if function insertion was successful, -1 if there is an existing declaration.
    int Insert(Ast::StmtFunDec* funDec);

private:
    Container<FunDesc> mContainer;

};

}
}


#endif
