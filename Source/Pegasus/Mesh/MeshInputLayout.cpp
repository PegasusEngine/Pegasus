/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshInputLayout.cpp
//! \author	Kleber Garcia
//! \date	11th May 2014
//! \brief	Mesh configuration used by all mesh nodes, which must be compatible
//!         between nodes to link them

#include "Pegasus/Mesh/MeshInputLayout.h"
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace Mesh {


const char MeshInputLayout::SemanticNames[MeshInputLayout::SEMANTIC_COUNT] =
{
    'p', //! position string in shader
    'n', //! normal string in shader
    'g', //! tangent string in shader
    'b', //! bitangent string in shader
    't', //! uv string in shader
    'c', //! color string in shader
    'i', //! blend index in shader
    'w', //! blend weight in shader
    'u'  //! user defined in shader
};

const int MeshInputLayout::AttrTypeSizes[MeshInputLayout::ATTRTYPE_COUNT] =
{
    4, //FLOAT
    4, //INT
    4, //UINT
    2, //SHORT
    2, //USHORT
    1 //BOOL
};


MeshInputLayout::MeshInputLayout()
    :
mAttributeCount(0)
{
}

//----------------------------------------------------------------------------------------

MeshInputLayout::MeshInputLayout(const MeshInputLayout & other)
{
    Pegasus::Utils::Memcpy(this, &other, sizeof(MeshInputLayout));
}

//----------------------------------------------------------------------------------------

MeshInputLayout & MeshInputLayout::operator=(const MeshInputLayout & other)
{
    Pegasus::Utils::Memcpy(this, &other, sizeof(MeshInputLayout));
    return *this;
}

//----------------------------------------------------------------------------------------

void MeshInputLayout::GenerateEditorLayout(MeshInputLayout::LayoutUsageBitMask mask)
{
    mAttributeCount = 0; // delete all previous attributes, if any
    int offset = 0;

    if (mask & MeshInputLayout::USE_POSITION)
    {
        AttrDesc attrPos = { 
            MeshInputLayout::POSITION,
            MeshInputLayout::FLOAT, // 32 bit precision
            offset, // byte offset
            0, // semantic index
            4, // vec4
            0, // use the first stream
            true, // snorm
        };
        RegisterAttribute(attrPos);
        offset += 4 * MeshInputLayout::AttrTypeSizes[attrPos.mType];
    }

    
    if (mask & MeshInputLayout::USE_NORMAL)
    {
        AttrDesc attrNorm = {
            MeshInputLayout::NORMAL,
            MeshInputLayout::FLOAT, // 16 bit precision
            offset, // byte offset
            0, // semantic index
            3, // vec3 
            0, // use the first stream
            true, // snorm
        };
        RegisterAttribute(attrNorm);
        offset += 3 * MeshInputLayout::AttrTypeSizes[attrNorm.mType];
    }
    

    if (mask & MeshInputLayout::USE_UV)
    {
        AttrDesc attrUV = {
            MeshInputLayout::UV,
            MeshInputLayout::FLOAT, // 32 bit precision
            offset, // byte offset
            0, // semantic index
            2, // vec2
            0, // use the first stream
            true, // snorm
        };
        RegisterAttribute(attrUV);
    }

}

//----------------------------------------------------------------------------------------

void MeshInputLayout::RegisterAttribute(AttrDesc& attribute)
{
    PG_ASSERTSTR(mAttributeCount < MESH_MAX_ATTRIBUTES, "Attribute max capd! make sure you increase the count");
    PG_ASSERTSTR(attribute.mSemanticIndex < MESH_MAX_SEMANTIC_INDEX, "Semantic index must be between 0 and MESH_MAX_SEMANTIC_INDEX definition");
    PG_ASSERTSTR(attribute.mStreamIndex < MESH_MAX_STREAMS, "Stream index must be between 0 and MESH_MAX_STREAMS");
    mAttributeDescs[mAttributeCount++] = attribute;
}

}   // namespace Mesh
}   // namespace Pegasus
