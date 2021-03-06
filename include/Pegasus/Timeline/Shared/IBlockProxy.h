/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	IBlockProxy.h
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline blocks

#ifndef PEGASUS_TIMELINE_SHARED_IBLOCKPROXY_H
#define PEGASUS_TIMELINE_SHARED_IBLOCKPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/TimelineDefs.h"

namespace Pegasus {

    namespace Core {
        class ISourceCodeProxy;
    }
    namespace PropertyGrid {
        class IPropertyGridObjectProxy;
    }
    namespace Timeline {
        class ILaneProxy;
    }
    namespace AssetLib {
        class IAssetProxy;
        class ICategoryProxy;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy interface, used by the editor to interact with the timeline blocks
class IBlockProxy
{
public:

    //! Destructor
    virtual ~IBlockProxy() {};

    //! Get the property grid proxy associated with the block
    //! \return Property grid proxy associated with the block
    //@{
    virtual PropertyGrid::IPropertyGridObjectProxy * GetPropertyGridProxy() = 0;
    virtual const PropertyGrid::IPropertyGridObjectProxy * GetPropertyGridProxy() const = 0;
    //@}

    //! initializes all render resources.
    virtual void Initialize() = 0;

    //! shutsdown all renderr resources.
    virtual void Shutdown() = 0;

    //! Get the position of the block in the lane
    //! \return Position of the block, measured in ticks
    virtual Beat GetBeat() const = 0;

    //! Get the duration of the block
    //! \return Duration of the block, measured in beats (> 0)
    virtual Duration GetDuration() const = 0;

    //! Get the lane the block belongs to
    //! \return Lane the block belongs to, nullptr when the block is not associated with a lane yet
    virtual ILaneProxy * GetLane() const = 0;

    //! Gets the string instance name of this block. The instance name is a unique identifier of this block.
    //! \return the name of the instance
    virtual const char * GetInstanceName() const = 0;

    //! Returns the name of this block. If it is "Block" means its a vanilla block type. Otherwise, whichever C++
    //! Inheritance will be displayed
    //! \return the name of the internal type
    virtual const char* GetClassName() const = 0;

    //! Set the color of the block
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    virtual void SetColor(unsigned char red, unsigned char green, unsigned char blue) = 0;

    //! Get the color of the block
    virtual void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const = 0;

    //! Gets the script proxy if available, null if not available
    //! \return the source code proxy, null if it does not exist.
    virtual Core::ISourceCodeProxy* GetScript() const = 0;

    //! Sets a script proxy as a timeline element.
    //! \param code - the code to attach
    virtual void AttachScript(Core::ISourceCodeProxy* code) = 0;

    //! Clears blockscript if there is one.
    virtual void ClearScript() = 0;

    //! Returns the guid of this proxy
    virtual unsigned GetGuid() const = 0;

    //! Overrides a guid for a block. Only to be used for proper behaviour of doing / undoing state.
    //! \param newGuid the guid to use for this object
    //! \warning there is no check for duplicate of guids. So make sure the right one is used.
    virtual void OverrideGuid(unsigned int newGuid) = 0;

    //! dumps the contents of a block to a single asset.
    //! Note- this is only supported for the use case of Undo/Redo, which serializes the entire state of an object as json
    //! \param assetProxy target asset to dump state into
    virtual void DumpToAsset(Pegasus::AssetLib::IAssetProxy* assetProxy) = 0;

    //! loads the contents of a block from a single asset.
    //! Note- this is only supported for the use case of Undo/Redo, which serializes the entire state of an object as json
    //! \param assetProxy target asset to load state from
    virtual void LoadFromAsset(const Pegasus::AssetLib::IAssetProxy* assetProxy) = 0;

    //! Gets the asset category of this block
    //! \return asset category pointer
    virtual Pegasus::AssetLib::ICategoryProxy* GetAssetCategory() = 0; 
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_SHARED_IBLOCKPROXY_H
