/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CaseTable.h
//! \author Kleber Garcia
//! \date   June 25rd, 2016
//! \brief  Lookup table of all the case patterns (each case means how a set of densities intersects
//!         a box.

#ifndef PEGASUS_CASE_TABLE_H
#define PEGASUS_CASE_TABLE_H

#include "Pegasus/RenderSystems/Config.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_VOLUMES

namespace Pegasus
{
namespace RenderSystems
{

// struct representing a particular case.
struct Case
{
    char  triangleCount; // cout of triangles. Up to 5. 
    char  triangles[15]; //list of (max) 5 triangles. Each triangle is three ints, representing the edges where it lives.
};


//! Class that holds the case patterns.
class CaseTable
{
public:

    //! Constructor. All static baby!
    CaseTable();

    //! initialize. Builds the marching cubes.
    void Initialize();

    //! returns a single case.
    const Case& GetCase(unsigned int i) const { return mCases[i]; }

    //! returns a buffer of a case count information. Each case count info contains the following bit count info (starting with LSB)
    //! bit size (bit offset)
    //! 2(0): count of the vertices in edges 0, 3, and 8.
    //! 2(2): count of the vertices in edges 9 and 1
    //! 2(4): count of the vertices in edges 9, 1 and 10
    //! 2(6): count of the vertices in edges 11 and 2
    //! 2(8): count of the vertices in edges 11, 2 and 6
    //! 2(10): count of the vertices in edges 4 and 7
    //! 2(12): count of the vertices in edges 4, 7 and 5
    //! 3(14): total count of triangles (up to 5)
    //! 12(17): vertex mask per triangle (one bit per edge active).
    const unsigned int * GetPackedCaseCountInfoBuffer() const { return mPackedCaseCountInfo; }

    //! returns a buffer for a case index information with the corresponding 15 indices (at max).
    const unsigned int * GetPackedIndexCases() const { return mPackedIndexCases; }

    //! returns the byte size of the packed index case
    const unsigned int GetPackedIndexCaseByteSize() const { return sizeof(mPackedIndexCases); }
    
    //! returns the case count;
    int GetCaseCount() const { return 256; }

    //! returns an  array of points, each point is a float3 element specifying the position in the marching cubes.
    const float* GetEdgeCoordinates() const;

    //! Only 12 edges in a cube.
    int GetEdgeCount() const { return 12; }

    //! returns an array of points, each point is a float3 representing a vertex in a cube
    const float* GetCubePoints() const;
    
    //! Only 8 poitns in a cube.
    int GetCubePointsCount() { return 8; }

    

private:
    void BuildCubeGraph();
    void Build();
    void BuildCase(char signature, Case& c);
    void BuildCaseCountInfo(const Case& c, unsigned int* outputPackedInfo);
    void BuildCaseIndexPackedBuffer(const Case& c, unsigned int* outPackedInfoArray);

    
    // cached intersections, to prevent triangles from intersecting

    //cube graph.
    char mEdges[12][2]; // array of edges (12 edges in a cube). Each edge has 2 points, start and end point. Stores these edge indices.
    char mVertexConnections[8][3]; // array of vertexes (8 vertex in a cube). Each vertex has 3 edge neighbors. The edge indices are stored.
    Case mCases[256]; //all possible 255 cases (marching cubes algorithm).
    unsigned int mPackedCaseCountInfo[256]; // buffer countaining the packed information for cases counts (the index count).
    unsigned int mPackedIndexCases[16 * 256]; //buffer containing the packed index information for cases.
    
};

}
}

#endif
#endif
