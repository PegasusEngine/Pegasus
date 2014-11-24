/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IBlockScriptCompilerListener.h
//! \author	Kleber Garcia
//! \date	November 5th
//! \brief	Callback for compiler errors

#ifndef PEGASUS_BLOCKSCRIPT_COMPILER_ERRORS_H
#define PEGASUS_BLOCKSCRIPT_COMPILER_ERRORS_H

namespace Pegasus
{
namespace BlockScript
{

//! Callback for compilation errors
class IBlockScriptCompilerListener
{
public:

    //! Triggered when compilation starts
    virtual void OnCompilationBegin() = 0;

    //! Triggered when there is a compilation error
    //! \param line the actual
    //! \param errorMessage the actual error message
    virtual void OnCompilationError(int line, const char* errorMessage, const char* token) = 0;

    //! Called at the end of a compilation
    //! \param success true if it was successful, false otherwise
    virtual void OnCompilationEnd(bool success) = 0;
};

}
}

#endif
