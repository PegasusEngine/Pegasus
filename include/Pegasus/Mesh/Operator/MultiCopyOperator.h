/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MultiCopyOperator.h
//! \author	Kleber Garcia
//! \date	May 21th 2017
//! \brief	MultiCopyOperator

#ifndef PEGASUS_MULTI_COPY_OPERATOR_H
#define PEGASUS_MULTI_COPY_OPERATOR_H

#include "Pegasus/Mesh/MeshOperator.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"

namespace Pegasus
{

namespace Mesh
{

//! Mesh cube generator. Generates a cube
class MultiCopyOperator : public MeshOperator
{
    DECLARE_MESH_OPERATOR_NODE(MultiCopyOperator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(MultiCopyOperator, MeshOperator)
        DECLARE_PROPERTY(int, MultiCopyFactor, 1)
        DECLARE_PROPERTY(Math::Vec3, TranslationOffset, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, ScaleOffset, Math::Vec3(1.0f, 1.0f, 1.0f))
        DECLARE_PROPERTY(Math::Vec4, QuaternionRotOffset, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0f))
    END_DECLARE_PROPERTIES()

public:
    
    //! constructor
    MultiCopyOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~MultiCopyOperator();

    virtual unsigned int GetMinNumInputNodes() const override { return 1; }

    virtual unsigned int GetMaxNumInputNodes() const override { return 1; }

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

};
}

}

#endif//PEGASUS_CUBE_GENERATOR_H
