/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshInputLayout.h
//! \author	Kleber Garcia
//! \date   11th May 2014	
//! \brief	Mesh input layout

#ifndef PEGASUS_MESH_MESHINPUTLAYOUT_H
#define PEGASUS_MESH_MESHINPUTLAYOUT_H

//Increase this number if we are to use more than 32 attributes

//! Total maximum number of attributes allowed per mesh
#define MESH_MAX_ATTRIBUTES 32

//! Maximum semantic index per attribute. For example, if 4, means that you have t0, t1, t2, and t3.
//! If this value goes beyond 10 then the binding algorithm in Render might need adjustment
#define MESH_MAX_SEMANTIC_INDEX 4

//! Maximum number of vertex streams
#define MESH_MAX_STREAMS 8

namespace Pegasus {
namespace Mesh {


//! Mesh configuration used by all mesh nodes, which must be compatible
//! between nodes to link them
class MeshInputLayout
{
public:

    //! Convinience mask used for the editor layout
    
    static const int USE_POSITION = 0x1;
    static const int USE_NORMAL = 0x2;
    static const int USE_UV = 0x4;
    typedef int LayoutUsageBitMask;
    

    //! semantic description of a vertex attribute
    enum Semantic
    {
        POSITION,  //! Position, used by pegasus to know where the mesh position is determined
        NORMAL,    //! Normal, used by pegasus to know where the normal is located
        TANGENT,   //! Tangent, used by pegasus to know where the tangent is located
        BITANGENT, //! Bitangent, used by pegasus to know where the bitangent is located
        UV,        //! UV coordinates, used by pegasus to know where the UVs are located 
        COLOR,     //! Color attribute of mesh, used by pegasus to know where color is located
        BLEND_INDEX,  //! Blend index, used by pegasus to know where the blend index is located
        BLEND_WEIGHT, //! Blend weight, used by pegasus to know where the blend weight is located
        USER_GENERIC, //! user generic, used for any generic custom attribute type
        SEMANTIC_COUNT//! count of semantics
    };



    //! vertex attribute name in shader. Position must match VertexSemanticType enum values.
    //! in the shader, vertex attributes must be the attribute string, with a numerical index concatenated.
    //! for example, we want to put two position inputs in the shader, it would be: 
    //! "attribute vec3 p0;" in an opengl shader
    //! Conversely, it would be "float3 p0 : POSITION" in direct x
    //! Matrices must be packed in group of float4s
    static const char SemanticNames[SEMANTIC_COUNT];

    //! the attribute type enum list. These types get translated into dx or opengl types internally
    enum AttrType
    {
        FLOAT,
        INT,
        UINT,
        SHORT,
        USHORT,
        BOOL,
        ATTRTYPE_COUNT
    };

    //! type sizes, order must match those of the enum values of AttributeType
    static const int AttrTypeSizes[ATTRTYPE_COUNT];

    struct AttrDesc
    {
        Semantic mSemantic; //! The pegasus semantic. Used to bind to shader inputs. The shader inputs must match the description of VertexSemanticAttributeNames list
        AttrType      mType;     //! the internal type of the buffer containing the semantic
        int           mByteOffset; //! the offset position of the particular stream
        int           mSemanticIndex      : 8;  //! the index followed by the semantic. For example "p0" or "p1", would be semantic POSITION, with semanticIndex 0 and semanticIndex 1
        int           mAttributeTypeCount : 8;  //! 1 if is a scalar, 2,3 or 4 if is a vector
        int           mStreamIndex        : 8 ; //! the stream index where this attribute belongs to.
        bool          mIsNormalized       : 1;  //! if the types of this mesh are UNORMS. Do not set to true if mType is FLOAT
    };

    //! Default constructor
    //! Creates an empty input layout
    MeshInputLayout();

    //! Set layout to the editor style settings.
    //! This layout consistency must be perseverant on all the mesh nodes.
    //! Violation of this layout in mesh operations will result in mesh compositing errors
    //! \param mask the attribute components to use. The basic formats are:
    //!        float4 for position, ushort3(normalized) for normals and float2 for UVs
    void GenerateEditorLayout(LayoutUsageBitMask mask);

    //! Copy constructor
    //! \param other Other configuration to copy from
    explicit MeshInputLayout(const MeshInputLayout & other);

    //! Registers an attribute
    //! \param the attribute description structure
    void RegisterAttribute(AttrDesc& attribute);

    //! Get an attribute description
    //! \return gets a vertex attribute description. Use GetAttributeCount to iterate over the attribute descriptions
    const AttrDesc& GetAttributeDesc(int index) const { PG_ASSERT(index >= 0 && index < mAttributeCount); return mAttributeDescs[index]; }

    //! Gets the count of attributes this mesh is holding 
    const int GetAttributeCount() const { return mAttributeCount; }

    //! Assignment operator
    //! \param other Other configuration to copy from
    MeshInputLayout & operator=(const MeshInputLayout & other);

    //! Tests if the incoming mesh type can be absorbed, meaning that the output mesh is at least a sub
    
private:
    int      mAttributeCount;
    AttrDesc mAttributeDescs[MESH_MAX_ATTRIBUTES];
};


}   // namespace Mesh
}   // namespace Pegasus

#endif  // PEGASUS_MESH_MESHINPUTLAYOUT_H
