/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	CombineTransformOperator.h
//! \author	Kleber Garcia
//! \date	May 17th 2017
//! \brief	CombineTransformOperator

#ifndef PEGASUS_COMBINE_TRANSFORM_OPERATOR_H
#define PEGASUS_COMBINE_TRANSFORM_OPERATOR_H

#include "Pegasus/Mesh/MeshOperator.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"

namespace Pegasus
{

namespace Mesh
{

//! Mesh cube generator. Generates a cube
class CombineTransformOperator : public MeshOperator
{
    DECLARE_MESH_OPERATOR_NODE(CombineTransformOperator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(CombineTransformOperator, MeshOperator)
        DECLARE_PROPERTY(Math::Vec3, Translation0, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, Translation1, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, Translation2, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, Translation3, Math::Vec3(0.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, Scale0, Math::Vec3(1.0f, 1.0f, 1.0f))
        DECLARE_PROPERTY(Math::Vec3, Scale1, Math::Vec3(1.0f, 1.0f, 1.0f))
        DECLARE_PROPERTY(Math::Vec3, Scale2, Math::Vec3(1.0f, 1.0f, 1.0f))
        DECLARE_PROPERTY(Math::Vec3, Scale3, Math::Vec3(1.0f, 1.0f, 1.0f))
        DECLARE_PROPERTY(Math::Vec4, QuaternionRot0, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec4, QuaternionRot1, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec4, QuaternionRot2, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec4, QuaternionRot3, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0f))
    END_DECLARE_PROPERTIES()

public:
    
    //! Maximum number of inputs
    enum { MaxCombineTransformInputs = 4 };

    //! constructor
    CombineTransformOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~CombineTransformOperator();

    virtual unsigned int GetMinNumInputNodes() const override { return 2; }

    virtual unsigned int GetMaxNumInputNodes() const override { return MaxCombineTransformInputs; }

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

private:
    void GenerateMatrices(Math::Mat44* matrices) const;

};
}

}

#endif//PEGASUS_CUBE_GENERATOR_H
