/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeometryTestBlock.h
//! \author	Kleber Garcia
//! \date	16th June 2014
//! \brief	Geometry test

#include "Pegasus/Render/Render.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Mesh/Mesh.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Math/Matrix.h"

// timeline block for GeometryTestBlock

#define MAX_SPHERES 4

class GeometryTestBlock : public Pegasus::Timeline::Block
{

    DECLARE_TIMELINE_BLOCK(GeometryTestBlock, "GeometryTestBlock");

public:
    
    //Constructor
    GeometryTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext);

    //Destructor
    virtual ~GeometryTestBlock();

    //! Initializer
    virtual void Initialize();

    //! Shutdown used by block
    virtual void Shutdown();

    //! Render function
    virtual void Render(float beat, Pegasus::Wnd::Window * window);
    
private:
    
    PG_DISABLE_COPY(GeometryTestBlock)

    Pegasus::Shader::ProgramLinkageRef mBlockProgram;
    Pegasus::Shader::ProgramLinkageRef mDiscoSpeaker;

    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Mesh::MeshRef mCubeMesh;

    
    Pegasus::Mesh::MeshRef mSphereMeshes[MAX_SPHERES];

    // block program uniforms
    Pegasus::Render::Uniform mCubeTextureUniform;
    Pegasus::Render::Uniform mUniformBlock;
    Pegasus::Render::Buffer  mUniformStateBuffer;

    // screen program uniforms
    Pegasus::Render::Uniform mAspectUniform;
    Pegasus::Render::Uniform mTimeUniform;

    Pegasus::Render::RenderTarget mCubeFaceRenderTarget;
    
    PEGASUS_ALIGN_BEGIN(16)
    struct UniformState
    {
        Pegasus::Math::Mat44 mRotation;
    } mState
    PEGASUS_ALIGN_END(16);


};

