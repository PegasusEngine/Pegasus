/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	AddOperator.h
//! \author	Kevin Boulanger
//! \date	01st November 2013
//! \brief	Texture operator that adds an arbitrary number of textures together

#ifndef PEGASUS_TEXTURE_OPERATOR_ADDOPERATOR_H
#define PEGASUS_TEXTURE_OPERATOR_ADDOPERATOR_H

#include "Pegasus/Texture/TextureOperator.h"

namespace Pegasus {
namespace Texture {


//! Texture operator that adds an arbitrary number of textures together
class AddOperator : public TextureOperator
{
public:

    //! Default constructor
    AddOperator();

    //! \todo Add functions to specify the minimum and maximum number of inputs


    //! Update the operator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data are dirty or if any input node is.
    //virtual bool Update() = 0;

    //------------------------------------------------------------------------------------
    
protected:

    //! Destructor
    virtual ~AddOperator();


    //! Generate the content of the data associated with the texture operator
    virtual void GenerateData();

    //------------------------------------------------------------------------------------

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(AddOperator)
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_OPERATOR_ADDOPERATOR_H
