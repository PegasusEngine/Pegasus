/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	WaveFieldOperator.h
//! \author	Kleber Garcia
//! \date	May 21th 2017
//! \brief	WaveFieldOperator

#ifndef PEGASUS_WAVE_FIELD_OPERATOR_H
#define PEGASUS_WAVE_FIELD_OPERATOR_H

#include "Pegasus/Mesh/MeshOperator.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Matrix.h"

namespace Pegasus
{

namespace Mesh
{

//! Mesh cube generator. Generates a cube
class WaveFieldOperator : public MeshOperator
{
    DECLARE_MESH_OPERATOR_NODE(WaveFieldOperator)

    //! Property declarations
    BEGIN_DECLARE_PROPERTIES(WaveFieldOperator, MeshOperator)
        DECLARE_PROPERTY(Math::Vec4, WFFreqFieldQuaternion, Math::Vec4(0.0f, 1.0f, 0.0f, 0.0))
        DECLARE_PROPERTY(Math::Vec3, WFFreqAmpOffset0, Math::Vec3(1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, WFFreqAmpOffset1, Math::Vec3(1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, WFFreqAmpOffset2, Math::Vec3(1.0f, 0.0f, 0.0f))
        DECLARE_PROPERTY(Math::Vec3, WFFreqAmpOffset3, Math::Vec3(1.0f, 0.0f, 0.0f))
    END_DECLARE_PROPERTIES()

public:

    enum {
        NumOfWaves = 4
    };
    
    //! constructor
    WaveFieldOperator(Pegasus::Alloc::IAllocator* nodeAllocator, 
                  Pegasus::Alloc::IAllocator* nodeDataAllocator);

    virtual ~WaveFieldOperator();

    virtual unsigned int GetMinNumInputNodes() const override { return 1; }

    virtual unsigned int GetMaxNumInputNodes() const override { return 1; }

protected:

    //! Generate the content of the data associated with the texture generator
    virtual void GenerateData();

};
}

}

#endif//PEGASUS_CUBE_GENERATOR_H
