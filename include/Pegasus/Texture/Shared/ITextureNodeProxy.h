/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITextureNodeProxy.h
//! \author	Karolyn Boulanger
//! \date	08th June 2014
//! \brief	Proxy interface, used by the editor to interact with a texture node

#ifndef PEGASUS_TEXTURE_SHARED_ITEXTURENODEPROXY_H
#define PEGASUS_TEXTURE_SHARED_ITEXTURENODEPROXY_H

#if PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Texture {

class ITextureConfigurationProxy;


//! Proxy interface, used by the editor to interact with a texture node
class ITextureNodeProxy
{
public:

    //! Definition of the different types of nodes
    enum NodeType
    {
        NODETYPE_GENERATOR = 0,
        NODETYPE_OPERATOR,
        NODETYPE_OUTPUT,
        NUM_NODETYPES,
        NODETYPE_UNKNOWN = NUM_NODETYPES
    };


    //! Destructor
    virtual ~ITextureNodeProxy() {};


    //! Get the type of node
    //! \return Type of the proxied node (ITextureNodeProxy::NODETYPE_xxx constant)
    virtual NodeType GetNodeType() const = 0;


    //! Get the name of the texture node
    //! \return Name of the texture node
    virtual const char * GetName() const = 0;

    //! Get the configuration of the texture node
    //! \return Configuration proxy of the texture node
    virtual const ITextureConfigurationProxy * GetConfiguration() const = 0;


    //! Get the number of input nodes connected to the current node
    //! \return Number of input nodes (from 0 to Node::MAX_NUM_INPUTS - 1)
    virtual unsigned int GetNumInputs() const = 0;

    //! Get one of the input nodes
    //! \param index Index of the input node (< GetNumInputs())
    //! \return Node proxy, nullptr in case of error
    virtual ITextureNodeProxy * GetInputNode(unsigned int index) = 0;
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TEXTURE_SHARED_ITEXTURENODEPROXY_H
