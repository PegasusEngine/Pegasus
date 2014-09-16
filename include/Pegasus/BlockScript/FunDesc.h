/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunDesc.h
//! \author Kleber Garcia
//! \date   September 14th
//! \brief  Function description structure, stored in a function table

#ifndef PEGASUS_FUN_DESC
#define PEGASUS_FUN_DESC

#include "Pegasus/BlockScript/IddStrPool.h"

#define MAX_SIGNATURE_LENGTH 256

namespace Pegasus
{
namespace BlockScript
{

namespace Ast
{
    class FunCall;
    class StmtFunDec;
}

class FunDesc
{
public:
    
    //! Constructor
    FunDesc();

    //! Destructor
    ~FunDesc();

    //! Sets a guid for this description
    //! \param guid - the guid
    void  SetGuid(int guid) { mGuid = guid; }

    //! Gets a guid for this description
    //! \return the guid
    int   GetGuid() const { return mGuid; }

    //! Gets the name of this function
    //! \return the name of this function
    const char* GetName() const { return mName; }

    //! Constructs the implementation of a function
    //! \param the implementation of this function
    void ConstructImpl(Ast::StmtFunDec* funDec);

    //! Constructs the implementation of a function
    //! \param the implementation of this function
    void ConstructDec(Ast::FunCall* funDec);

    //! Compares this function description with another function description
    //! \return true if equal otherwise false
    bool Equals(const FunDesc* other);

    //! returns the declaration of a function
    //! \return the declaration of a function
    const Ast::StmtFunDec* GetDec() const { return mFunDec; }


private:
    char mName[IddStrPool::sCharsPerString];
    char mSignature[MAX_SIGNATURE_LENGTH];
    int  mSignatureLength;
    Ast::StmtFunDec* mFunDec;
    int mGuid;
};

}
}

#endif
