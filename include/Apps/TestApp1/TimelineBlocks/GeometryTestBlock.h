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

// timeline block for GeometryTestBlock

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
    Pegasus::Mesh::MeshRef mCubeMesh;

    Pegasus::Render::Uniform mUniformBlock;
    Pegasus::Render::Buffer  mUniformStateBuffer;
    
    struct UniformState
    {
        float mTime;
        float mAspect;
    } mState;


};

