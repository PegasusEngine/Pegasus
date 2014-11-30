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
#include "Pegasus/BlockScript/BsIntrinsics.h"
#include "Pegasus/BlockScript/FunCallback.h"

#define MAX_SIGNATURE_LENGTH 1024

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

    //! Gets the signature
    //! \return byte signature
    const char* GetSignature() const { return mSignature; }

    //! Gets the signature length
    //! \return byte signature length
    int GetSignatureLength() const { return mSignatureLength; }

    //! Constructs the implementation of a function
    //! \param the implementation of this function
    void ConstructImpl(Ast::StmtFunDec* funDec);

    //! Constructs the implementation of a function
    //! \param the implementation of this function
    void ConstructDec(Ast::FunCall* funDec);

    //! Compares this function description with another function description
    //! \return true if equal otherwise false
    bool Equals(const FunDesc* other) const ; 

    //! returns the declaration of a function
    //! \return the declaration of a function
    const Ast::StmtFunDec* GetDec() const { return mFunDec; }

    //! returns true if this function is a callback, false otherwise
    bool IsCallback() const { return mCallback != nullptr; }

    //! sets the callback
    void SetCallback(FunCallback callback) { mCallback = callback; }

    //! returns the callback for intrinsic functions
    FunCallback GetCallback() const { return mCallback; } 

    //! returns the total size of the concatenated input arguments.
    int GetInputArgumentsByteSize() const { return mInputArgumentByteSize; }


private:
    char mName[IddStrPool::sCharsPerString];
    char mSignature[MAX_SIGNATURE_LENGTH];
    int  mSignatureLength;
    int  mInputArgumentByteSize;
    Ast::StmtFunDec* mFunDec;
    int mGuid;

    FunCallback mCallback;
};

}
}

#endif
