/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeDesc.cpp
//! \author Kleber Garcia
//! \date   2nd September 2014
//! \brief  BlockScript type descriptor class. A type descriptor is a linked list of type
//!         modifiers (implementation)


#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Assertion.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

TypeDesc::TypeDesc()
:
mModifier(M_INVALID),
mAluEngine(E_NONE),
mChild(nullptr),
mModifierProperty(0),
mStructDef(nullptr)
{
    mName[0] = '\0';
}

TypeDesc::~TypeDesc()
{
}

void TypeDesc::SetName(const char * typeName)
{
#if PEGASUS_ENABLE_ASSERT
    int strlen = Utils::Strlen(typeName) + 1;
    PG_ASSERTSTR(strlen < TypeDesc::sMaxTypeName, "Maximum type name description registered");
#endif
    mName[0] = '\0';
    Utils::Strcat(mName, typeName);
}
