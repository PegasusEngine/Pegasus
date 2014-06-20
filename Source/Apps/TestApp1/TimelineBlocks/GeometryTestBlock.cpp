/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GeometryTestBlock.cpp
//! \author	Kleber Garcia
//! \date	16th June 2014
//! \brief	Geometry test

#include "Apps/TestApp1/TimelineBlocks/GeometryTestBlock.h"

#include "../Source/Pegasus/Render/GL/GLEWStaticInclude.h"
#include "Pegasus/Math/Quaternion.h"


//Constructor
GeometryTestBlock::GeometryTestBlock(Pegasus::Alloc::IAllocator * allocator, Pegasus::Wnd::IWindowContext * appContext)
    : Pegasus::Timeline::Block(allocator, appContext)

{

}

//Destructor
GeometryTestBlock::~GeometryTestBlock()
{

}

//! Initializer
void GeometryTestBlock::Initialize()
{
    Pegasus::Shader::ShaderManager * const shaderManager = GetShaderManager();
    mBlockProgram = shaderManager->CreateProgram("CubeProgram");
    
    Pegasus::Shader::ShaderStageFileProperties fileLoadProperties;
    fileLoadProperties.mLoader = GetIOManager();
    fileLoadProperties.mPath = "Shaders\\Cube.vs";
    mBlockProgram->SetShaderStage(  
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );

    fileLoadProperties.mPath = "Shaders\\Cube.ps";
    mBlockProgram->SetShaderStage(  
        shaderManager->LoadShaderStageFromFile(fileLoadProperties)
    );

    bool updated = false; 
    mBlockProgram->GetUpdatedData(updated);

    Pegasus::Render::GetUniformLocation(mBlockProgram, "uniformState", mUniformBlock);
    Pegasus::Render::CreateUniformBuffer(sizeof(mState), mUniformStateBuffer);

    Pegasus::Mesh::MeshManager * const meshManager = GetMeshManager();
    mCubeMesh = meshManager->CreateMeshNode();
    mCubeMesh->SetGeneratorInput(
        meshManager->CreateMeshGeneratorNode("BoxGenerator")
    );

    Pegasus::Math::SetIdentity(mState.mRotation);
}

//! Shutdown used by block
void GeometryTestBlock::Shutdown()
{
    Pegasus::Render::DeleteBuffer(mUniformStateBuffer);
}

//! Render function
void GeometryTestBlock::Render(float beat, Pegasus::Wnd::Window * window)
{
    Pegasus::Render::Dispatch(mBlockProgram);

    Pegasus::Math::Vec3 rotAxis(0.3f, 0.4f, 0.01f);
    Pegasus::Math::SetRotation(mState.mRotation, rotAxis, (float)beat * 0.22f);

    //dispatch uniforms, all packed in a nice cbuffer:
    unsigned int viewportWidth = 0;
    unsigned int viewportHeight = 0;
    window->GetDimensions(viewportWidth, viewportHeight);
    float aspect = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

    // do aspect ratio correctness on y axis
    mState.mRotation.m21 *= aspect;
    mState.mRotation.m22 *= aspect;
    mState.mRotation.m23 *= aspect;

    Pegasus::Render::SetBuffer(mUniformStateBuffer, &mState, sizeof(mState));
    Pegasus::Render::SetUniform(mUniformBlock, mUniformStateBuffer);


    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    glDepthFunc(GL_GREATER);
    glClearDepth(0.0);
    glClear(GL_DEPTH_BUFFER_BIT);

    Pegasus::Render::Dispatch(mCubeMesh);

    Pegasus::Render::Draw();
    
    glDisable(GL_DEPTH_TEST);
}
