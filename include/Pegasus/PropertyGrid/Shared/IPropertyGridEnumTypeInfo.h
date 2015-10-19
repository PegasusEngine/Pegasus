/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IPropertyEnumTypeInfo.h
//! \author Kleber Garcia
//! \date   October 17, 2015
//! \brief  Proxy interface for enumeration type information

#ifndef PEGASUS_PROPERTYGRID_IENUMTYPEPROXY
#define PEGASUS_PROPERTYGRID_IENUMTYPEPROXY

namespace Pegasus
{
    namespace PropertyGrid {

        class BaseEnumType;

        //! Type information of this enumeration
        class IEnumTypeInfoProxy {
        public:
            //! Constructor / Destructor
            IEnumTypeInfoProxy()  {}
            virtual ~IEnumTypeInfoProxy()  {}

            //! Returns the name of this enumeration type.
            virtual const char* GetName() const = 0;

            //! Gets a list of all the enumerations values of this enumeration.
            virtual const BaseEnumType** GetEnumerations(int& outSize) const = 0; 
        };
    }
}

#endif
