#ifndef PEGASUS_GENERIC_RESOURCE
#define PEGASUS_GENERIC_RESOURCE

/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   GenericResource.h
//! \author Kleber Garcia
//! \date   August 26th 2016
//! \brief  A generic resource is an extendable type, that will automatically:
//!         - setup a blockscript type to represent a custom resource (usually from a render system)
//!         - setup property editor for such types on blockscript

#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Core/RefCounted.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{

namespace BlockScript
{
    class BlockLib;
    struct FunctionDeclarationDesc;
}

namespace PropertyGrid
{
    class PropertyGridClassInfo;
}

namespace Alloc
{
    class IAllocator;
}

namespace Application
{

    
class RenderCollectionFactory;


class GenericResource : public Core::RefCounted,  public PropertyGrid::PropertyGridObject
{
    BEGIN_DECLARE_PROPERTIES_BASE(GenericResource)
    END_DECLARE_PROPERTIES()

public:
    GenericResource(Alloc::IAllocator*);
    virtual ~GenericResource(){}

    //! Generic update function of this resource.
    virtual void Update() = 0;

    //! \param typeName type name to register
    //! \param lib the blockscript library to register this resource type into
    //! \param extraMethods any extra methods that this class will have
    //! \param extraMethodsSize the size of the extra methods
    static void RegisterGenericResourceType(
        const PropertyGrid::PropertyGridClassInfo* propertyGridClass,
        BlockScript::BlockLib* lib,
        BlockScript::FunctionDeclarationDesc* extraMethods = nullptr,
        unsigned int extraMethodsSize = 0);

};

}
}

#endif
