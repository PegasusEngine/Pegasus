/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Program.h
//! \author	Kleber Garcia
//! \date	17th October 2013
//! \brief	Represents an opengl program

#ifndef PEGASUS_SHADERPROGRAM_H
#define PEGASUS_SHADERPROGRAM_H

#include "Pegasus/Shader/ShaderStage.h"

namespace Pegasus
{
namespace Shader
{

class Program
#if PEGASUS_SHADER_USE_EDIT_EVENTS
    : public EventDispatcher
#endif
{
public:
    Program();
    ~Program();
    void SetStage(ShaderStage * stage);
    void RemoveStage(ShaderType stageType);
    bool IsValid() const { return mIsValidProgram; }
    bool Link();
    void Bind() const;
	
	GLuint GetProgramHandle() const { return mOgl.mProgramHandle; }

private:
    //no copy constructor
    explicit Program(const Program& other);
    Program& operator=(const Program& other);
    bool mIsValidProgram;
    ShaderStage* mStages[SHADER_STAGES_COUNT];
    
    struct OglState
    {
        GLuint mProgramHandle;
        OglState()
        : mProgramHandle(0)
        {
        }
        ~OglState()
        {
        }
    } mOgl;
};

}
}
#endif
