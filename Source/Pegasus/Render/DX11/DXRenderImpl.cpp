/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   DXRenderImpl.cpp
//! \author Kleber Garcia
//! \date   15th July of 2014
//! \brief  DirectX 11 implementation of PARR

#if PEGASUS_GAPI_DX

#include "Pegasus/Render/Render.h"

//////////////////        GLOBALS CODE BLOCK     //////////////////////////////
//         All globals holding state data are declared on this block       ////
///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DISPATCH FUNCTIONS IMPLEMENTATION /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void Pegasus::Render::Dispatch (Pegasus::Shader::ProgramLinkageInOut program)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch (Pegasus::Mesh::MeshInOut mesh)
{
}


// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch(Pegasus::Render::RenderTarget& renderTarget, const Viewport& viewport, int renderTargetSlot)
{
}


// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchNullRenderTarget()
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::DispatchDefaultRenderTarget(const Pegasus::Render::Viewport& viewport)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   Clear Functions                         ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Clear(bool color, bool depth, bool stencil)
{
}

void Pegasus::Render::SetClearColorValue(const Pegasus::Math::ColorRGBA& color)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETRASTERIZERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetRasterizerState(const RasterizerState& rasterState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBLENDINGSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::SetBlendingState(const Pegasus::Render::BlendingState& blendingState)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::SetDepthClearValue(float d)
{
}

// ---------------------------------------------------------------------------

void Pegasus::Render::Dispatch(Pegasus::Render::RenderTarget& renderTarget)
{
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   DRAW FUNCTION IMPLEMENTATION      /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::Draw()
{
}

// ---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/////////////   GET UNIFORM FUNCTION IMPLEMENTATIONS    ///////////////////////
///////////////////////////////////////////////////////////////////////////////

bool Pegasus::Render::GetUniformLocation(Pegasus::Shader::ProgramLinkageInOut program, const char * name, Pegasus::Render::Uniform& outputUniform)
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEUNIFORMBUFFER IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateUniformBuffer(int size, Pegasus::Render::Buffer& outputBuffer)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateRasterizerState(Pegasus::Render::RasterizerConfig& config, Pegasus::Render::RasterizerState& rasterizerState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   CREATEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::CreateBlendingState(Pegasus::Render::BlendingConfig& config, Pegasus::Render::BlendingState& blendingState)
{
}
///////////////////////////////////////////////////////////////////////////////
/////////////   DELETERASTERSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteRasterizerState(Pegasus::Render::RasterizerState& state)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETEBLENDIGNSTATE IMPLEMENTATION      ///////////////////////
///////////////////////////////////////////////////////////////////////////////
void Pegasus::Render::DeleteBlendingState(Pegasus::Render::BlendingState& blendingState)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETBUFFER IMPLEMENTATION                ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::SetBuffer(Pegasus::Render::Buffer& dstBuffer, void * src, int size, int offset)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   DELETE BUFFER IMPLEMENTATION            ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void Pegasus::Render::DeleteBuffer(Pegasus::Render::Buffer& buffer)
{
}

///////////////////////////////////////////////////////////////////////////////
/////////////   SETUNIFORMS IMPLEMENTATIONS             ///////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, float value)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, Pegasus::Texture::TextureInOut texture)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, const Buffer& buffer)
{
    return false;
}

// ---------------------------------------------------------------------------

bool Pegasus::Render::SetUniform(Pegasus::Render::Uniform& u, const RenderTarget& renderTarget)
{
    return false;
}


#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif
