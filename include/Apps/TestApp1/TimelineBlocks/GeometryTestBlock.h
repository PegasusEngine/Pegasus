/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeometryTestBlock.h
//! \author	Kleber Garcia
//! \date	16th June 2014
//! \brief	Geometry test

#include "Pegasus/Core/IApplicationContext.h"
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
    GeometryTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Core::IApplicationContext * appContext);

    //Destructor
    virtual ~GeometryTestBlock();


    //! Initialize the data of the block
    virtual void Initialize();

    //! Deallocate the data used by the block
    virtual void Shutdown();

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Update(float beat);

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void Render(float beat, Pegasus::Wnd::Window * window);
    
private:
    
    PG_DISABLE_COPY(GeometryTestBlock)

    Pegasus::Shader::ProgramLinkageRef mBlockProgram;
    Pegasus::Shader::ProgramLinkageRef mDiscoSpeaker;

    Pegasus::Shader::ProgramLinkageRef mBlurHorizontal;
    Pegasus::Shader::ProgramLinkageRef mBlurVertical;
    Pegasus::Shader::ProgramLinkageRef mComposite;

    Pegasus::Mesh::MeshRef mQuad;
    Pegasus::Mesh::MeshRef mCubeMesh;

    
    Pegasus::Mesh::MeshRef mSphereMeshes[MAX_SPHERES];

    // block program uniforms
    Pegasus::Render::Uniform mCubeTextureUniform;
    Pegasus::Render::Uniform mUniformBlock;
    Pegasus::Render::BufferRef  mUniformStateBuffer;

    Pegasus::Render::Uniform mHorizontalInput;
    Pegasus::Render::Uniform mVerticalInput;
    Pegasus::Render::Uniform mCompositeInput1;
    Pegasus::Render::Uniform mCompositeInput2;

    // screen program uniforms
    Pegasus::Render::Uniform mSpeakerUniformBlock;
    Pegasus::Render::BufferRef  mSpeakerStateBuffer;
    Pegasus::Render::CubeMapRef mCubeMap;
    Pegasus::Render::RenderTargetRef mCubeMapTarget;
    PEGASUS_ALIGN_BEGIN(16)
    struct SpeakerUniformState
    {
        float time;
    } mSpeakerState
    PEGASUS_ALIGN_END(16);

    Pegasus::Render::RenderTargetRef mCubeFaceRenderTarget;

    Pegasus::Render::RenderTargetRef mTempTarget1;
    Pegasus::Render::RenderTargetRef mTempTarget2;
    Pegasus::Render::RenderTargetRef mTempTarget3;

    Pegasus::Render::RasterizerStateRef mDefaultRasterState;
    Pegasus::Render::RasterizerStateRef mCurrentBlockRasterState;
    
    PEGASUS_ALIGN_BEGIN(16)
    struct UniformState
    {
        Pegasus::Math::Mat44 mRotation;
    } mState
    PEGASUS_ALIGN_END(16);

};

