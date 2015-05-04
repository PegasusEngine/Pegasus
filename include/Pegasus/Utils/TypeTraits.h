/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeTraits.h
//! \author Kevin Boulanger
//! \date   18th February 2015
//! \brief  Primitives to analyze types and their properties

#ifndef PEGASUS_UTILS_TYPETRAITS_H
#define PEGASUS_UTILS_TYPETRAITS_H

#include "Pegasus/Math/Types.h"

namespace Pegasus {
namespace Utils {


//! Type selector, the returned type is TypeSelect<flag, Type1, Type2>::Type.
//! If flag == true, Type = TTrue, otherwise Type = TFalse.
template <bool flag, typename TTrue, typename TFalse>
struct TypeSelect
{
    typedef TTrue Type;
};

template <typename TTrue, typename TFalse>
struct TypeSelect<false, TTrue, TFalse>
{
    typedef TFalse Type;
};

//----------------------------------------------------------------------------------------

//! Null type, used to say for example that the pointee type
//! of a non-pointer type is undefined
class NullType { };

//----------------------------------------------------------------------------------------

//! Class giving traits for the type T.
//! Usage: TypeTraits<T>::IsPointer for example to test if T is a pointer type,
//!        TypeTraits<T>::PointeeType for example to get the pointee type
//!            if T is a pointer type
template <typename T>
class TypeTraits
{
private:

    //! Structure to test if an unqualified type is void
    template <typename U> struct IsVoidStruct
        {   static bool const Result = false;   };
    template <> struct IsVoidStruct<void>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is an unsigned integer
    template <typename U> struct IsUnsignedIntStruct
        {   static bool const Result = false;   };
    template <> struct IsUnsignedIntStruct<Math::PUInt8>
        {   static bool const Result = true;    };
    template <> struct IsUnsignedIntStruct<Math::PUInt16>
        {   static bool const Result = true;    };
    template <> struct IsUnsignedIntStruct<Math::PUInt32>
        {   static bool const Result = true;    };
    template <> struct IsUnsignedIntStruct<Math::PUInt64>
        {   static bool const Result = true;    };
    template <> struct IsUnsignedIntStruct<unsigned int>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is a signed integer
    template <typename U> struct IsSignedIntStruct
        {   static bool const Result = false;   };
    template <> struct IsSignedIntStruct<Math::PInt8>
        {   static bool const Result = true;    };
    template <> struct IsSignedIntStruct<Math::PInt16>
        {   static bool const Result = true;    };
    template <> struct IsSignedIntStruct<Math::PInt32>
        {   static bool const Result = true;    };
    template <> struct IsSignedIntStruct<Math::PInt64>
        {   static bool const Result = true;    };
    template <> struct IsSignedIntStruct<int>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is a float
    template <typename U> struct IsFloatStruct
        {   static bool const Result = false;   };
    template <> struct IsFloatStruct<Math::PFloat32>
        {   static bool const Result = true;    };
    template <> struct IsFloatStruct<Math::PFloat64>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is a boolean
    template <typename U> struct IsBooleanStruct
        {   static bool const Result = false;   };
    template <> struct IsBooleanStruct<bool>
        {   static bool const Result = true;    };
    
    //! Structure to test if an unqualified type is a character
    template <typename U> struct IsCharStruct
        {   static bool const Result = false;   };
    template <> struct IsCharStruct<char>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is null
    template <typename U> struct IsNullStruct
        {   static bool const Result = false;   };
    template <> struct IsNullStruct<NullType>
        {   static bool const Result = true;    };

    //! Structure to test if an unqualified type is a pointer
    template <typename U> struct IsPointerStruct
        {
            static bool const Result = false;
            typedef NullType PointeeType;
        };
    template <typename U> struct IsPointerStruct<U *>
        {
            static bool const Result = true;
            typedef U PointeeType;
        };

    //! Structure to test if an unqualified type is a reference
    template <typename U> struct IsReferenceStruct
        {
            static bool const Result = false;
            typedef U ReferredType;
        };
    template <typename U> struct IsReferenceStruct<U &>
        {
            static bool const Result = true;
            typedef U ReferredType;
        };

    //! Structure to test if an unqualified type is a pointer to member
    template <typename U> struct IsPointerToMemberStruct
        {   static bool const Result = false;   };
    template <typename U, typename V> struct IsPointerToMemberStruct<U V::*>
        {   static bool const Result = true;    };

    //! Structure to test if a type is constant
    template <typename U> struct IsConstStruct
        {
            static bool const Result = false;
            typedef U NonConstType;
        };
    template <typename U> struct IsConstStruct<const U>
        {
            static bool const Result = true;
            typedef U NonConstType;
        };

    //! Structure to test if a type is volatile
    template <typename U> struct IsVolatileStruct
        {
            static bool const Result = false;
            typedef U NonVolatileType;
        };
    template <typename U> struct IsVolatileStruct<volatile U>
        {
            static bool const Result = true;
            typedef U NonVolatileType;
        };

    //------------------------------------------------------------------------------------

public:

    //! Type with the const and volatile qualifiers removed
    typedef typename IsVolatileStruct<typename IsConstStruct<T>::NonConstType>
                                                    ::NonVolatileType UnqualifiedType;

    //! True if T is void
    static bool const IsVoid = IsVoidStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is an unsigned integer
    static bool const IsUnsignedInteger = IsUnsignedIntStruct<
                                            TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a signed integer
    static bool const IsSignedInteger = IsSignedIntStruct<
                                            TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is an integer (unsigned or signed)
    static bool const IsInteger = IsUnsignedInteger || IsSignedInteger;

    //! True if T is a float
    static bool const IsFloat = IsFloatStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a number (integer or float)
    static bool const IsNumber = IsInteger || IsFloat;

    //! True if T is a boolean
    static bool const IsBoolean = IsBooleanStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a character
    static bool const IsChar = IsCharStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a null type (NullType)
    static bool const IsNull = IsNullStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a pointer type
    static bool const IsPointer = IsPointerStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! Type of the pointee, NullType if T is not a pointer type
    typedef typename IsPointerStruct<typename TypeTraits<T>::UnqualifiedType>
                                                            ::PointeeType PointeeType;

    //! True if T is a reference type
    static bool const IsReference = IsReferenceStruct<TypeTraits<T>::UnqualifiedType>::Result;

    //! Type of the referred type
    typedef typename IsReferenceStruct<typename TypeTraits<T>::UnqualifiedType>
                                                            ::ReferredType ReferredType;

    //! True if T is a pointer to member type
    static bool const IsPointerToMember = IsPointerToMemberStruct<
                                            TypeTraits<T>::UnqualifiedType>::Result;

    //! True if T is a base type (that can be passed by value with a function)
    static bool const IsBaseType = IsNumber || IsBoolean || IsChar || IsPointer || IsReference || IsPointerToMember;

    //! True if T is a complex type (class, structure)
    static bool const IsComplexType = !IsBaseType;

    //! True if T is Plain Old Data (base type or structure of PODs)
#if PEGASUS_COMPILER_MSVC
    static bool const IsPOD = IsVoid || IsBaseType || (__is_pod(T) && __has_trivial_constructor(T));
#else
    // Compilation fail until we have a proper implementation
    static bool const IsPOD = UN-DEFINED
#endif

    //! True if T is a const type
    static bool const IsConst = IsConstStruct<T>::Result;

    //! Type with the const qualifier removed
    typedef typename IsConstStruct<T>::NonConstType NonConstType;

    //! True if T is a volatile type
    static bool const IsVolatile = IsVolatileStruct<T>::Result;

    //! Type with the volatile qualifier removed
    typedef typename IsVolatileStruct<T>::NonVolatileType NonVolatileType;
};


}   // namespace Utils
}   // namespace Pegasus

#endif  // PEGASUS_UTILS_TYPETRAITS_H
