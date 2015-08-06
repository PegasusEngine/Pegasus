/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	AddOperator.h
//! \author	Karolyn Boulanger
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
    DECLARE_TEXTURE_OPERATOR_NODE(AddOperator)

    BEGIN_DECLARE_PROPERTIES()
        DECLARE_PROPERTY(bool, Clamp)
    END_DECLARE_PROPERTIES()

public:

    //! Specifies the minimum number of input nodes accepted by the current node
    //! \return 1
    virtual unsigned int GetMinNumInputNodes() const { return 1; }

    //! Specifies the maximum number of input nodes accepted by the current node
    //! \return MAX_NUM_INPUTS
    virtual unsigned int GetMaxNumInputNodes() const { return MAX_NUM_INPUTS; }


    //! Update the operator internal state by pulling external parameters.
    //! This function sets the dirty flag of the node data if the internal state has changed
    //! or if an input node is dirty, and returns the dirty flag to the parent caller.
    //! That will trigger a chain of refreshed data when calling GetUpdatedData().
    //! \return True if the node data is dirty or if any input node is.
    //virtual bool Update();

    //------------------------------------------------------------------------------------
    
protected:

    //! Generate the content of the data associated with the texture operator
    virtual void GenerateData();
};


}   // namespace Texture
}   // namespace Pegasus

#endif  // PEGASUS_TEXTURE_OPERATOR_ADDOPERATOR_H
