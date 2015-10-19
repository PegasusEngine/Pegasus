/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PropertyEnumTypeInfo.h
//! \author Kleber Garcia
//! \date   October 17, 2015
//! \brief  Proxy implementation for enumeration type information

#ifndef PEGASUS_PROPERTYGRID_ENUMTYPEPROXY
#define PEGASUS_PROPERTYGRID_ENUMTYPEPROXY

#include "Pegasus/PropertyGrid/Shared/IPropertyGridEnumTypeInfo.h"

namespace Pegasus
{
    namespace PropertyGrid {

        class BaseEnumType;
        class EnumTypeInfo;

        //! Type information of this enumeration
        class EnumTypeInfoProxy : public IEnumTypeInfoProxy {
        public:
            //! Constructor / Destructor
            EnumTypeInfoProxy(EnumTypeInfo* obj) : mObject(obj)  {}

            virtual ~EnumTypeInfoProxy()  {}

            //! Returns the name of this enumeration type.
            virtual const char* GetName() const;

            //! Gets a list of all the enumerations values of this enumeration.
            virtual const BaseEnumType** GetEnumerations(int& outSize) const;

        private:
            EnumTypeInfo* mObject;
        };
    }
}

#endif
