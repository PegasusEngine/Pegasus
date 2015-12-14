/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	NodeInput.h
//! \author	Karolyn Boulanger
//! \date	02nd December 2015
//! \brief	Node input class, used to describe one input of a node

#ifndef PEGASUS_GRAPH_NODEINPUT_H
#define PEGASUS_GRAPH_NODEINPUT_H

namespace Pegasus {
namespace Graph {

class Node;


//! Node input class, used to describe one input of a node
class NodeInput
{
    friend class Node;

public:

    //! Default constructor
    //! \param node Node owning this input (!= nullptr)
    NodeInput(Node* node);

    //! Get the node owning this input
    //! \return Node owning this input (!= nullptr)
    inline Node* GetNode() const { return mNode; }

    //! Maximum length of the input name
    enum { MAX_NAME_LENGTH = 15 };

    //! Get the name of the input
    //! \return Name of the input (non-empty string if SetParameters() has been called properly)
    inline const char* GetName() const { return mName; }

    //! Test if the input is required to be connected
    //! \return True if the input is required to be connected
    inline bool IsRequired() const { return mRequired; }


#if PEGASUS_ENABLE_PROXIES

    //! \todo Add proxy support

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    //! Set the parameters of the input
    //! \warning To be called only in the constructor of the Node derived classes,
    //!          through the Node::AddInput() function
    //! \param name Name of the input, non-empty string with maximum length of MAX_NAME_LENGTH characters
    //! \param required True if the input requires to a connection to be valid
    void SetParameters(const char* name, bool required);


    // Node input cannot be copied
    PG_DISABLE_COPY(NodeInput)

    //! Node owning this input
    Node* mNode;

    //! Name of the input
    char mName[MAX_NAME_LENGTH + 1];

    //! True if the input is required to be connected
    bool mRequired;
};


}   // namespace Graph
}   // namespace Pegasus

#endif  // PEGASUS_GRAPH_NODEINPUT_H
