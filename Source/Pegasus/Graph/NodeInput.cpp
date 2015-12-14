/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeInput.cpp
//! \author	Karolyn Boulanger
//! \date	02nd December 2015
//! \brief	Node input class, used to describe one input of a node

#include "Pegasus/Graph/NodeInput.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace Graph {


NodeInput::NodeInput(Node* node)
:   mNode(node)
,   mRequired(false)
{
    PG_ASSERTSTR(node != nullptr, "A Node object needs to own this input");
    mName[0] = '\0';
}

//----------------------------------------------------------------------------------------

void NodeInput::SetParameters(const char* name, bool required)
{
    PG_ASSERTSTR(name != nullptr, "A name must be defined for a node input");
    const unsigned int nameLength = Utils::Strlen(name);
    PG_ASSERTSTR(nameLength <= MAX_NAME_LENGTH, "The name length (%u) for an input name is too long, maximum length is %u characters", nameLength, MAX_NAME_LENGTH);

    //! \todo Create and use a Strcpy() function
    Utils::Memcpy(mName, name, nameLength);
    mName[nameLength] = '\0';

    mRequired = required;
}


}   // namespace Graph
}   // namespace Pegasus
