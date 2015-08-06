/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeTraitsDebug.h
//! \author Karolyn Boulanger
//! \date   26th February 2015
//! \brief  Logging function to display the traits of a type

#ifndef PEGASUS_UTILS_TYPETRAITSDEBUG_H
#define PEGASUS_UTILS_TYPETRAITSDEBUG_H

#if PEGASUS_DEV

#include "Pegasus/Utils/TypeTraits.h"
#include "Pegasus/Utils/String.h"

namespace Pegasus {
namespace Utils {


//! Function outputting the traits of a type T
//! \param outputString Output string containing the type description.
//!                     Must be an allocated buffer large enough to contain the entire description.
//! \param outputSubTypes True to output information about the subtypes,
//!                       pointee type for example with a pointer type
//! \param subTypeLevel Current level of type (1 for the pointee, 2 for the pointee
//!                     of a pointer, etc.). Only for recursive use.
//! \warning Does not work for T == void because of the presence of the sizeof() operator
template <typename T>
void OutputTypeTraits(char * outputBuffer, bool outputSubTypes, unsigned int subTypeLevel = 0)
{
    // Buffer used to indent the text in the output buffer
    char indentBuffer[256];

    // Size of an indentation in characters
    static const unsigned int IndentSize = 4;

    // Prepare the indentation buffer in case it becomes useful
    unsigned int i;
    for (i = 0; i < IndentSize * subTypeLevel; ++i)
    {
        indentBuffer[i] = ' ';
    }
    indentBuffer[i] = '\0';

    // Make sure the output buffer starts with an empty string
    if (subTypeLevel == 0)
    {
        outputBuffer[0] = '\0';
    }

	// Void type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Void: ");
    Strcat(outputBuffer, TypeTraits<T>::IsVoid ? "yes\n" : "no\n");

    // Unsigned integer type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Unsigned integer: ");
    if (TypeTraits<T>::IsUnsignedInteger)
    {
        Strcat(outputBuffer, "yes (");
        Strcat(outputBuffer, sizeof(T));
		Strcat(outputBuffer, " byte");
        Strcat(outputBuffer, (sizeof(T) > 1 ? "s)\n" : ")\n"));
    }
    else
    {
        Strcat(outputBuffer, "no\n");
    }

	// Signed integer type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Signed integer: ");
    if (TypeTraits<T>::IsSignedInteger)
    {
        Strcat(outputBuffer, "yes (");
        Strcat(outputBuffer, sizeof(T));
		Strcat(outputBuffer, " byte");
        Strcat(outputBuffer, (sizeof(T) > 1 ? "s)\n" : ")\n"));
    }
    else
    {
        Strcat(outputBuffer, "no\n");
    }
    
	// Float type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Float: ");
    if (TypeTraits<T>::IsFloat)
    {
        Strcat(outputBuffer, "yes (");
        Strcat(outputBuffer, sizeof(T));
		Strcat(outputBuffer, " byte");
        Strcat(outputBuffer, (sizeof(T) > 1 ? "s)\n" : ")\n"));
    }
    else
    {
        Strcat(outputBuffer, "no\n");
    }

	// Number type (integer or float)
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Number: ");
    Strcat(outputBuffer, TypeTraits<T>::IsNumber ? "yes\n" : "no\n");

	// Boolean type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Boolean: ");
    if (TypeTraits<T>::IsBoolean)
    {
        Strcat(outputBuffer, "yes (");
        Strcat(outputBuffer, sizeof(T));
		Strcat(outputBuffer, " byte");
        Strcat(outputBuffer, (sizeof(T) > 1 ? "s)\n" : ")\n"));
    }
    else
    {
        Strcat(outputBuffer, "no\n");
    }

	// Character type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Character: ");
    if (TypeTraits<T>::IsChar)
    {
        Strcat(outputBuffer, "yes (");
        Strcat(outputBuffer, sizeof(T));
		Strcat(outputBuffer, " byte");
        Strcat(outputBuffer, (sizeof(T) > 1 ? "s)\n" : ")\n"));
    }
    else
    {
        Strcat(outputBuffer, "no\n");
    }

    // Null type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Null: ");
    Strcat(outputBuffer, TypeTraits<T>::IsNull ? "yes\n" : "no\n");

	// Pointer type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Pointer: ");
    Strcat(outputBuffer, TypeTraits<T>::IsPointer ? "yes\n" : "no\n");

    // Pointee description
	if (outputSubTypes && TypeTraits<T>::IsPointer)
	{
        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "Pointee:\n");
        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "{\n");

		OutputTypeTraits<TypeTraits<T>::PointeeType>(outputBuffer, true, subTypeLevel + 1);

        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "}\n");
	}

	// Pointer type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Reference: ");
    Strcat(outputBuffer, TypeTraits<T>::IsReference ? "yes\n" : "no\n");

    // Reference description
	if (outputSubTypes && TypeTraits<T>::IsReference)
	{
        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "Referred:\n");
        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "{\n");

		OutputTypeTraits<TypeTraits<T>::ReferredType>(outputBuffer, true, subTypeLevel + 1);

        Strcat(outputBuffer, indentBuffer);
        Strcat(outputBuffer, "}\n");
	}

	// Pointer to member type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Pointer to member: ");
    Strcat(outputBuffer, TypeTraits<T>::IsPointerToMember ? "yes\n" : "no\n");

    // Base type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Base type: ");
    Strcat(outputBuffer, TypeTraits<T>::IsBaseType ? "yes\n" : "no\n");

    // Complex type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Complex type: ");
    Strcat(outputBuffer, TypeTraits<T>::IsComplexType ? "yes\n" : "no\n");

    // POD type
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "POD type: ");
    Strcat(outputBuffer, TypeTraits<T>::IsPOD ? "yes\n" : "no\n");

	// Const qualifier
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Const qualifier: ");
    Strcat(outputBuffer, TypeTraits<T>::IsConst ? "yes\n" : "no\n");

	// Volatile qualifier
	Strcat(outputBuffer, indentBuffer);
	Strcat(outputBuffer, "Volatile qualifier: ");
    Strcat(outputBuffer, TypeTraits<T>::IsVolatile ? "yes\n" : "no\n");
}


}   // namespace Utils
}   // namespace Pegasus


#endif  // PEGASUS_DEV

#endif  // PEGASUS_UTILS_TYPETRAITSDEBUG_H
