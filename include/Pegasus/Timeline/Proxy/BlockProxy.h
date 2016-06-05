/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockProxy.h
//! \author	Karolyn Boulanger
//! \date	09th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline blocks

#ifndef PEGASUS_TIMELINE_PROXY_BLOCKPROXY_H
#define PEGASUS_TIMELINE_PROXY_BLOCKPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"

namespace Pegasus {

    namespace Core {
        class ISourceCodeProxy;
    }

    namespace Timeline {
        class Block;
        class TimelineScriptRunner;
    }
}

namespace Pegasus {
namespace Timeline {

//! Property grid object proxy decorator, to intercept events from a property grid.
//! This is so we prevent making WriteObjectProperty a virtual function.
class PropertyFlusherPropertyGridObjectDecorator : public PropertyGrid::IPropertyGridObjectProxy
{
public:
    //Constructor
    PropertyFlusherPropertyGridObjectDecorator(TimelineScriptRunner* runner, PropertyGrid::IPropertyGridObjectProxy* propertyGrid);

    //Destructor
    virtual ~PropertyFlusherPropertyGridObjectDecorator();

    //@{
    //! Functions act exactly the same for the decorated property grid object
    virtual const PropertyGrid::IPropertyGridClassInfoProxy * GetClassInfoProxy() const {return mDecorated->GetClassInfoProxy();}
    virtual unsigned int GetNumDerivedClassProperties() const { return mDecorated->GetNumDerivedClassProperties(); }
    virtual const PropertyGrid::PropertyRecord & GetDerivedClassPropertyRecord(unsigned int index) const { return mDecorated->GetDerivedClassPropertyRecord(index); }
    virtual void ReadDerivedClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) { mDecorated->ReadDerivedClassProperty(index, outputBuffer, outputBufferSize); }
    virtual void WriteDerivedClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize) { mDecorated->WriteDerivedClassProperty(index, inputBuffer, inputBufferSize); }
    virtual unsigned int GetNumClassProperties() const { return mDecorated->GetNumClassProperties(); }
    virtual const PropertyGrid::PropertyRecord & GetClassPropertyRecord(unsigned int index) const { return mDecorated->GetClassPropertyRecord(index); }
    virtual void ReadClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) { mDecorated->ReadClassProperty(index, outputBuffer, outputBufferSize); }
    virtual void WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize, bool sendMessage = true) { mDecorated->WriteClassProperty(index, inputBuffer, inputBufferSize, sendMessage); }
    virtual unsigned int GetNumObjectProperties() const { return mDecorated->GetNumObjectProperties(); }
    virtual const PropertyGrid::PropertyRecord & GetObjectPropertyRecord(unsigned int index) const { return mDecorated->GetObjectPropertyRecord(index); }
    virtual void ReadObjectProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize) { mDecorated->ReadObjectProperty(index, outputBuffer, outputBufferSize); }
    virtual void SetEventListener(PropertyGrid::IPropertyListener* listener) { mDecorated->SetEventListener(listener); }
    virtual void SetUserData(Pegasus::Core::IEventUserData* userData) { mDecorated->SetUserData(userData); }
    virtual Pegasus::Core::IEventUserData* GetUserData() const { return mDecorated->GetUserData(); }
    //@}

    //! Write the object property using the content of a buffer. Decorated to notify a block to rebuild itself based on the property modified.
    //! \param index Index of the object property (0 <= index < GetNumObjectProperties())
    //! \param inputBuffer Input buffer with content to copy to the object property (!= nullptr)
    //! \param inputBufferSize Size in bytes of the input buffer (> 0)
    virtual void WriteObjectProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize, bool sendMessage = true);

private:
    PropertyGrid::IPropertyGridObjectProxy* mDecorated;
    TimelineScriptRunner* mRunner;

};

//! Proxy object, used by the editor to interact with the timeline blocks
class BlockProxy : public IBlockProxy
{
public:

    // Constructor
    //! \param block Proxied timeline block object, cannot be nullptr
    BlockProxy(Block * block);

    //! Destructor
    virtual ~BlockProxy();

    //! Get the block associated with the proxy
    //! \return Block associated with the proxy (!= nullptr)
    inline Block * GetBlock() const { return mBlock; }


    //! Get the property grid proxy associated with the block
    //! \return Property grid proxy associated with the block
    //@{
    virtual PropertyGrid::IPropertyGridObjectProxy * GetPropertyGridProxy() override;
    virtual const PropertyGrid::IPropertyGridObjectProxy * GetPropertyGridProxy() const override;
    //@}


    //! Get the position of the block in the lane
    //! \return Position of the block, measured in ticks
    virtual Beat GetBeat() const;

    //! Get the duration of the block
    //! \return Duration of the block, measured in beats (> 0)
    virtual Duration GetDuration() const;

    //! Get the lane the block belongs to
    //! \return Lane the block belongs to, nullptr when the block is not associated with a lane yet
    virtual ILaneProxy * GetLane() const;

    //! Gets the string instance name of this block. The instance name is a unique identifier of this block.
    //! \return the name of the instance
    virtual const char * GetInstanceName() const;

    //! Returns the name of this block. If it is "Block" means its a vanilla block type. Otherwise, whichever C++
    //! Inheritance will be displayed
    //! \return the name of the internal type
    virtual const char* GetClassName() const;

    //! Set the color of the block
    //! \param red Red component (0-255)
    //! \param green Green component (0-255)
    //! \param blue Blue component (0-255)
    virtual void SetColor(unsigned char red, unsigned char green, unsigned char blue);

    //! Get the color of the block
    virtual void GetColor(unsigned char & red, unsigned char & green, unsigned char & blue) const;
    
    //! Gets the script proxy if available, null if not available
    //! \return the source code proxy, null if it does not exist.
    virtual Core::ISourceCodeProxy* GetScript() const;

    //! Returns the guid of this proxy
    virtual unsigned GetGuid() const;

    //! Sets a script proxy as a timeline element.
    //! \param code - the code to attach
    virtual void AttachScript(Core::ISourceCodeProxy* code);

    //! Clears blockscript if there is one.
    virtual void ClearScript();

    //------------------------------------------------------------------------------------
    
private:

    //! Proxied timeline block object
    Block * const mBlock;
    PropertyFlusherPropertyGridObjectDecorator mPropertyGridDecorator;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_PROXY_BLOCKPROXY_H
