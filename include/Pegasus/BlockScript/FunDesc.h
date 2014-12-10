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

namespace Pegasus
{
namespace BlockScript
{

namespace Ast
{
    class FunCall;
    class StmtFunDec;
    class ArgList;
    class ExpList;
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

    //! initializes with the current function declaration
    //! \param function declaration to use
    void Initialize(Ast::StmtFunDec* funDec); 

    //! Returns true if this function is compatible with this call. False otherwise
    //! \param the implementation of this function
    bool IsCompatible(const Ast::FunCall* funCall) const;

    //! Returns true if this function is compatible with this call. False otherwise
    //! \param the implementation of this function
    bool IsCompatible(const Ast::StmtFunDec* funDec) const;

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

    //! returns true if these type arg lists are equal, false otherwise
    bool AreSignaturesEqual(const char* name, Ast::ArgList* argList) const;

    //! returns true if these type arg lists are equal, false otherwise
    bool AreSignaturesEqual(const char* name, Ast::ExpList* argList) const;

    //! returns wether this function declaration is a method or not
    bool IsMethod() const { return mIsMethod; }

    //! Sets if this function is a method or not
    void SetIsMethod(bool isMethod) { mIsMethod = isMethod; }

private:
    int  mInputArgumentByteSize;
    Ast::StmtFunDec* mFunDec;
    int mGuid;
    bool mIsMethod;

    FunCallback mCallback;
};

}
}

#endif
