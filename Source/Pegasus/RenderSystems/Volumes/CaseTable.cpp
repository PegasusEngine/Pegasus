/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CaseTable.cpp
//! \author Kleber Garcia
//! \date   June 25rd, 2016
//! \brief  Lookup table of all the case patterns (each case means how a set of densities intersects
//!         a box.

#include "Pegasus/RenderSystems/Volumes/CaseTable.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Math/Vector.h"
#include "Pegasus/Utils/Memcpy.h"

#if RENDER_SYSTEM_CONFIG_ENABLE_VOLUMES

using namespace Pegasus;
using namespace Pegasus::RenderSystems;
using namespace Pegasus::Math;

//hard coded graph that represents the cube.
//
// 8e------    _~ 4----4e---5
//            0----0e----1 /|   ------9e
//            |   |      |  5e
//            3e  7e     1e |      
//            |   |      |  |
//            |_~ 7---6e-|--6
// 11e-----   3----2e----2/    -------10e
//
//   numbers - vertices
//   numbers with e - edges

static const float sEdgeCoords[][3] = 
{
    {  0.0f,  1.0f,  1.0f },
    {  1.0f,  0.0f,  1.0f },
    {  0.0f, -1.0f,  1.0f },
    { -1.0f,  0.0f,  1.0f },
    {  0.0f,  1.0f, -1.0f },
    {  1.0f,  0.0f, -1.0f },
    {  0.0f, -1.0f, -1.0f },
    { -1.0f,  0.0f, -1.0f },
    { -1.0f,  1.0f,  0.0f },
    {  1.0f,  1.0f,  0.0f },
    {  1.0f, -1.0f,  0.0f },
    { -1.0f, -1.0f,  0.0f },
};

static const float sCubePoints[][3] =
{
    { -1.0f, 1.0f, 1.0f },
    {  1.0f, 1.0f, 1.0f },
    {  1.0f,-1.0f, 1.0f },
    { -1.0f,-1.0f, 1.0f },
    { -1.0f, 1.0f,-1.0f },
    {  1.0f, 1.0f,-1.0f },
    {  1.0f,-1.0f,-1.0f },
    { -1.0f,-1.0f,-1.0f }    
};

// this algorithm sorts points clock wise or counter clockwise (in a convex hull fasion)
// following the right hand rule of a normal
static void SortSurfaceConvexHull(char* surface, char surfaceCount, Vec3& n)
{
    PG_ASSERT(surfaceCount >= 3);   
    char outSurfaceList[15];
    char outSurfaceCount = 0;    
    char ptInHull = surface[0];    //we pick a starting point
    
    do
    {
        outSurfaceList[outSurfaceCount++] = ptInHull;
        char endEdge = surface[0];
        for (char i = 1; i < surfaceCount; ++i)
        {
            if (endEdge == ptInHull)
            {
                endEdge = surface[i];
                continue;
            }

            if (endEdge != surface[i] && ptInHull != surface[i])
            {
                Vec3 A(sEdgeCoords[ptInHull]);
                Vec3 B(sEdgeCoords[surface[i]]);
                Vec3 C(sEdgeCoords[endEdge]);
                Vec3 d1 = B - A;
                Vec3 d2 = C - A;
                Vec3 c = Normalize(Cross(d1, d2));
                bool isBetterCandidate = Dot(c,n) > 0.0f;
                if (isBetterCandidate)
                {
                    endEdge = surface[i];
                }
            }
        }
        ptInHull = endEdge;
    }while(ptInHull != outSurfaceList[0]);
    PG_ASSERT(outSurfaceCount == surfaceCount);
    Utils::Memcpy(surface, outSurfaceList, surfaceCount);    
}

static int CountBits(unsigned char b)
{
    int bits = 0;
    while(b) {bits += b & 0x1; b >>= 0x1;}
    return bits;
}

static char SurfaceToTriangles(char* surface, char surfaceCount, char* outTriangles)
{
    if (surfaceCount == 3)
    {
        outTriangles[0] = surface[0];
        outTriangles[1] = surface[1];
        outTriangles[2] = surface[2];
        return 1;
    }
    else if (surfaceCount == 4)
    {
        outTriangles[0] = surface[0];
        outTriangles[1] = surface[1];
        outTriangles[2] = surface[2];
        outTriangles[3] = surface[2];
        outTriangles[4] = surface[3];
        outTriangles[5] = surface[0];
        return 2;
    }
    else if (surfaceCount ==5)
    {
        outTriangles[0] = surface[0];
        outTriangles[1] = surface[1];
        outTriangles[2] = surface[2];
        outTriangles[3] = surface[3];
        outTriangles[4] = surface[4];
        outTriangles[5] = surface[0];
        outTriangles[6] = surface[0];
        outTriangles[7] = surface[2];
        outTriangles[8] = surface[3];
        return 3;
    }
    else if (surfaceCount ==6)
    {
        outTriangles[0] = surface[0];
        outTriangles[1] = surface[1];
        outTriangles[2] = surface[2];
        outTriangles[3] = surface[2];
        outTriangles[4] = surface[3];
        outTriangles[5] = surface[4];
        outTriangles[6] = surface[4];
        outTriangles[7] = surface[5];
        outTriangles[8] = surface[0];
        outTriangles[9] = surface[0];
        outTriangles[10] = surface[2];
        outTriangles[11] = surface[4];
        return 4;
    }
    PG_FAIL();
    return 0;
}

CaseTable::CaseTable()
{
    Utils::Memset8(&mCases, 0, sizeof(mCases));
    Utils::Memset8(&mPackedCaseCountInfo, 0, sizeof(mPackedCaseCountInfo));
    Utils::Memset32(&mPackedIndexCases, 0, sizeof(mPackedIndexCases));
    BuildCubeGraph();
}


void CaseTable::BuildCubeGraph()
{

    //create the graph for a cube.
    for (char i = 0; i < 4; ++i)
    {
        mEdges[i][0] = i;
        mEdges[i][1] = (i + 1) % 4;
    }

    for (char i = 4; i < 8; ++i)
    {
        mEdges[i][0] = i;
        mEdges[i][1] = (i - 3) % 4 + 4;
    }

    for (char i = 8; i < 12; ++i)
    {
        mEdges[i][0] = i - 8;
        mEdges[i][1] = i - 4;
    }

    for (char i = 0; i < 8; ++i)
    {
        char c = 0;
        for (unsigned e = 0; e < 12; ++e)
        {
            if (mEdges[e][0] == i || mEdges[e][1] == i)
            {
                mVertexConnections[i][c++] = e;
            }
        }
        PG_ASSERT(c == 3);      
    }
}

void CaseTable::BuildCase(char signature, Case& c)
{
    //  _~ 4---------5
    // 0----------1 /|  
    // |   |      |  |  
    // |   |      |  |  
    // |   |      |  |
    // |_~ 7------|--6
    // 3----------2/    

    #define GET_BIT(s, i) ((s >> i) & 0x01)
    float normalSign = 1.0;
    if (CountBits(signature) > 4)
    {
        signature = ~signature;
        normalSign = -1.0;
    }
    static const char VISITED_STATE_WHITE = 0;
    static const char VISITED_STATE_GRAY = 1;
    static const char VISITED_STATE_BLACK = 2;

    char visited[8];
    char stack[8];
    Utils::Memset8(visited, VISITED_STATE_WHITE, sizeof(visited));
    char el = 0;
    char surface[15]; //each element is a vertex in the middle of an edge. This holds the edge Id.
    Utils::Memset8(surface, -1, sizeof(surface));
    Utils::Memset8(c.triangles, -1, sizeof(c.triangles));
    c.triangleCount = 0;
    char surfaceOffset = 0;
    for (char visitedIt = 0; visitedIt < 8; ++visitedIt)
    {
        if (visited[visitedIt] == VISITED_STATE_BLACK) continue;
        stack[el++] = visitedIt;
        char surfaceCount = 0;
        char inVertexCount = 0;
        Vec3 n(0.0f);
        while (el > 0)
        {
            char v = stack[--el];
            if (GET_BIT(signature, v) == 1) // if this vertex is inside the terrain density.
            {
                n += Vec3(sCubePoints[v]); ++inVertexCount;
                for (char neighbor = 0; neighbor < 3; ++neighbor)
                {
                    char edgeId = mVertexConnections[v][neighbor];
                    char neighborId = mEdges[edgeId][0] != v ? mEdges[edgeId][0] : mEdges[edgeId][1];
                    if (GET_BIT(signature, neighborId) == 0)
                    {
                            surface[surfaceOffset + surfaceCount++] = mVertexConnections[v][neighbor];                            
                    }
                    else
                    {
                        if (visited[neighborId] == VISITED_STATE_WHITE)
                        {
                            visited[neighborId] = VISITED_STATE_GRAY;
                            stack[el++] = neighborId;
                        } 
                    }
                }
            }
            visited[v] = VISITED_STATE_BLACK;
        }
        
        //build the triangle soup.
        if (surfaceCount > 0)
        {
            n /= inVertexCount;
            n *= normalSign;
            // just pick a normal behind the surface
            SortSurfaceConvexHull(surface + surfaceOffset, surfaceCount,n);
            char trianglesAdded = SurfaceToTriangles(surface + surfaceOffset, surfaceCount, c.triangles + c.triangleCount*3);
            c.triangleCount += trianglesAdded;
        }
        surfaceOffset += surfaceCount;
        PG_ASSERT(surfaceCount <= 15);
    }
    PG_ASSERT(c.triangleCount <= 5);

    #undef GET_BIT

}

void CaseTable::BuildCaseCountInfo(const Case& c, unsigned int* outPackedInfo)
{
    unsigned int activeEdges = 0x0;
    
    for (int i = 0; i < 3*c.triangleCount; ++i)
    {
        int edgeIndex = c.triangles[i]; 
        activeEdges |= (1 << edgeIndex);
    }

    //two halfs. Aligned at the MSB is the size infos.
    // Aligned at the LSB is the active edge mask.
    *outPackedInfo = (c.triangleCount << 16) | (activeEdges);

}

void CaseTable::BuildCaseIndexPackedBuffer(const Case& c, unsigned int* outPackedInfoArray)
{
    for (char t = 0; t < c.triangleCount * 3; ++t)
    {
        outPackedInfoArray[t] = c.triangles[t];
    }
}

void CaseTable::Initialize()
{
    for (short caseId = 0; caseId < 256; ++caseId)
    {
        Case& c = mCases[caseId];
        BuildCase((char)caseId, c);
        BuildCaseCountInfo(c, &mPackedCaseCountInfo[caseId]);
        BuildCaseIndexPackedBuffer(c, &mPackedIndexCases[caseId * 16]); //the extra index is for padding for a uint4
    }
}

const float* CaseTable::GetEdgeCoordinates() const
{
    return reinterpret_cast<const float*>((const void*)sEdgeCoords);
}

const float* CaseTable::GetCubePoints() const
{
    return reinterpret_cast<const float*>((const void*)sCubePoints);
}



#else
    PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
