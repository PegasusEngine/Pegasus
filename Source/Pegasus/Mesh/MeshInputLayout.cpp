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
            Pegasus::Core::FORMAT_RGBA_32_FLOAT, // 32 bit precision
            16,
            offset, // byte offset
            0, // semantic index            
            0 // use the first stream
        };
        RegisterAttribute(attrPos);
        offset += attrPos.mByteSize;
    }

    
    if (mask & MeshInputLayout::USE_NORMAL)
    {
        AttrDesc attrNorm = {
            MeshInputLayout::NORMAL,
            Pegasus::Core::FORMAT_RGB_32_FLOAT, // 16 bit precision
            12,
            offset, // byte offset
            0, // semantic index
            0 // use the first stream
        };
        RegisterAttribute(attrNorm);
        offset += attrNorm.mByteSize;
    }
    

    if (mask & MeshInputLayout::USE_UV)
    {
        AttrDesc attrUV = {
            MeshInputLayout::UV,
            Pegasus::Core::FORMAT_RG_32_FLOAT, // 32 bit precision
            8,
            offset, // byte offset
            0, // semantic index
            0  // use the first stream
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

//----------------------------------------------------------------------------------------

bool MeshInputLayout::operator == (const MeshInputLayout& other) const
{
    if (mAttributeCount != other.mAttributeCount)
    {
        return false;
    }

    for (int i = 0; i < mAttributeCount; ++i)
    {
        if (mAttributeDescs[i] != other.mAttributeDescs[i])
        {
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------

bool MeshInputLayout::AttrDesc::operator==(const MeshInputLayout::AttrDesc& other) const
{
    return    mSemantic == other.mSemantic
           && mType == other.mType 
           && mByteSize == other.mByteSize 
           && mByteOffset == other.mByteOffset 
           && mSemanticIndex == other.mSemanticIndex 
           && mStreamIndex == other.mStreamIndex
           ;
}


}   // namespace Mesh
}   // namespace Pegasus
