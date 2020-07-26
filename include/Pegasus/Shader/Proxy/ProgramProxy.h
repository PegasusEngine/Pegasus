/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ProgramProxy.h
//! \author Kleber Garcia 
//! \date   16 March 2014
//! \brief  Proxy interface, used by the editor and launcher to interact with the shader mgr

#ifndef PEGASUS_PROGRAMPROXY_H
#define PEGASUS_PROGRAMPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Shader/Shared/IProgramProxy.h"

namespace Pegasus
{

namespace Graph
{
class IGraphUserData;
}

namespace Shader
{
class ProgramLinkage;

//! Program proxy. To be used by the editor
class ProgramProxy : public IProgramProxy
{
public:
    ProgramProxy(ProgramLinkage * object);
    virtual ~ProgramProxy() {}

    //! \return Gets the name of this shader program
    virtual const char * GetName() const override;

    virtual void SetSourceCode(Pegasus::Core::ISourceCodeProxy* sourceCode) override;

    virtual Pegasus::Core::ISourceCodeProxy* GetSourceCode() const override;

    virtual bool Compile() override;

    //! Sets the user data for this particular program
    //! \param userData. the user data to retrieve
    virtual void SetUserData(Pegasus::Core::IEventUserData * userData) override;

    //! Gets the user data for this particular program 
    //! \return user data reference
    virtual Pegasus::Core::IEventUserData * GetUserData() const override;

    //! \return the object that belongs to this proxy
    ProgramLinkage* GetObject() { return mObject; }

    //! \return the object that belongs to this proxy
    const ProgramLinkage* GetObject() const { return mObject; }

private:

    //! internal worker, obfuscated
    ProgramLinkage * mObject;

protected:
    virtual AssetLib::IRuntimeAssetObjectProxy* GetDecoratedObject() const;
};

}
}
#endif // Proxies 
#endif // ifdef guard
