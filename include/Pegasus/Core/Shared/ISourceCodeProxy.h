/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ISourceCodeProxy.cpp
//! \author Kleber Garcia
//! \date   November 1st
//! \brief  Pegasus source code entry point for editor. This interface will define any
//!         operation done to a source code file.

#ifndef PEGASUS_SOURCE_CODE_PROXY_H
#define PEGASUS_SOURCE_CODE_PROXY_H

#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

// forward declarations
namespace Pegasus {
    namespace Graph {
        class IGraphUserData;
    }
}

namespace Pegasus
{
namespace Core
{

class IBasicSourceProxy : public AssetLib::IRuntimeAssetObjectProxyDecorator
{
public:
    //! Sets the user data for this particular source code file
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Graph::IGraphUserData * userData) = 0;

    //! Gets the user data for this particular source code file
    //! \return user data reference
    virtual Pegasus::Graph::IGraphUserData * GetUserData() const = 0;
};

class ISourceCodeProxy : public IBasicSourceProxy
{
public:
    //! definition of how the script is compiled 
    enum CompilationPolicy
    {
        POLICY_USER_DEFINED, //! user decides if compilation occurs per keystroke or only at save
        POLICY_FORCE_ON_SAVE //! compilation only occurs at save
    };
   
    //! Constructor
    ISourceCodeProxy(){}

    //! Destructor
    virtual ~ISourceCodeProxy(){}

    //! Gets the name of the source code file
    //! \return the name given to the source code (the filename) as null terminated string
    virtual const char * GetName() const = 0;

    //! Gets the source of the source code file
    //! \param outSrc output param to be filled with a string pointer containing the src
    //! \param outSize output param to an int, to be filled with the size of outSize
    virtual void GetSource(const char ** outSrc, int& outSize) const = 0;

    //! Sets the source and marks source code file as dirty
    //! \param source string. Doesn't need to be null terminated
    //! \param source size to copy 
    virtual void SetSource(const char * source, int sourceSize) = 0;

    //! Returns the compilation policy that this script has.
    //! \return the policy for the editor to use
    virtual CompilationPolicy GetCompilationPolicy() const = 0;

    //! Internally compiles this source code.
    virtual void Compile() = 0;

    //! returns a 64 bit guid for a particular source code file.
    //! \return the guid must correspond to a single source code filestage instance
    virtual long long GetGuid() const = 0;
};


} //namespace Core
} //namespace Pegasus

#endif // PEGASUS_SOURCE_CODE_PROXY
