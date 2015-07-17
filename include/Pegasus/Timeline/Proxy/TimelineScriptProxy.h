/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScriptProxy.h
//! \author	Kleber Garcia
//! \date	5th November 2014
//! \brief	Proxy class for interactions with editor

#ifndef PEGASUS_SCRIPT_PROXY_H
#define PEGASUS_SCRIPT_PROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Core/Shared/ISourceCodeProxy.h"

namespace Pegasus
{
    namespace Io
    {
        class IOManager;
    }
}

namespace Pegasus
{
namespace Timeline
{

class TimelineSource;

class TimelineScriptProxy : public Core::ISourceCodeProxy
{
public:
    TimelineScriptProxy(TimelineSource* helper);
    ~TimelineScriptProxy();

    //! Gets the name of the source code file
    //! \return the name given to the source code (the filename) as null terminated string
    virtual const char * GetName() const;

    //! Gets the source of the source code file
    //! \param outSrc output param to be filled with a string pointer containing the src
    //! \param outSize output param to an int, to be filled with the size of outSize
    virtual void GetSource(const char ** outSrc, int& outSize) const;

    //! Sets the source and marks source code file as dirty
    //! \param source string. Doesn't need to be null terminated
    //! \param source size to copy 
    virtual void SetSource(const char * source, int sourceSize);

    //! Sets the user data for this particular source code file
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Graph::IGraphUserData * userData);

    //! Gets the user data for this particular source code file
    //! \return user data reference
    virtual Pegasus::Graph::IGraphUserData * GetUserData() const;

    //! set the compilation policy to be only on file save. Meaning, files won't compile until saving the file occurs.
    //! \return compilation policy
    virtual Pegasus::Core::ISourceCodeProxy::CompilationPolicy GetCompilationPolicy() const { return Pegasus::Core::ISourceCodeProxy::POLICY_FORCE_ON_SAVE; }

    //! returns a 64 bit guid for a particular source code file.
    //! \return the guid must correspond to a single source code filestage instance
    virtual long long GetGuid() const;

    //! Forces compilation on this object
    virtual void Compile();

    //! \returns the internal object
    TimelineSource* GetObject() { return mTimelineSource; }

    //! \returns the internal const object
    const TimelineSource* GetObject() const { return mTimelineSource; }

private:
    TimelineSource* mTimelineSource;

protected:
    virtual AssetLib::IRuntimeAssetObjectProxy* GetDecoratedObject() const;
};

} //namespace Timeline
}//namespace Pegasus

#endif // PEGASUS_ENABLE_PROXIES
#endif //PEGASUS_SCRIPT_PROXY_H
