/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderTracker.h
//! \author Kleber Garcia
//! \date   3/17/2014 
//! \brief  Container class tracking all the shaders and programs defined in the app	
//!         Only available in dev mode
#ifndef PEGASUS_SHADERTRACKER_H
#define PEGASUS_SHADERTRACKER_H
#if PEGASUS_ENABLE_PROXIES

namespace Pegasus
{
namespace Shader
{

class ProgramLinkage;
class ShaderStage;

class ShaderTracker
{
public:
    ShaderTracker();
    ~ShaderTracker(){}

    //! Maximum amount of elements the static container has
    static const int MAX_SHADER_CONTAINER = 256;

    //! Inserts a program to the container
    void InsertProgram(ProgramLinkage * program);

    //! Inserts a shader to the container
    void InsertShader (ShaderStage * shader);    

    //! Deletes a program within the container
    void DeleteProgram(ProgramLinkage * program);
    
    //! Deletes a shader from the container
    void DeleteShader (ShaderStage * shader);

    //! Deletes a shader from the container
    //! \return returns the deleted shader,
    //!         otherwise returns null if it cant find it
    ShaderStage* DeleteShader (int id);
    
    //! Deletes a program within the container
    //! \return returns the deleted shader,
    //!         otherwise returns null if it cant find it
    ProgramLinkage* DeleteProgram(int id);

    //! Finds the index of a shader
    //! \return -1 if not found, otherwise the index
    int FindShaderIndex(ShaderStage* shader) const;
    
    //! Finds the index of a program
    //! \return -1 if not found, otherwise the index
    int FindProgramIndex(ProgramLinkage* program) const;

    //! Returns the program specified in the index, null otherwise
    ProgramLinkage*  GetProgram(int id) const;

    //! Returns the shader specified in the index, null otherwise
    ShaderStage*     GetShaderStage(int id) const;

    //! returns the number of shaders
    int ShaderSize() const { return mShaderSize; }

    //! returns the number of programs
    int ProgramSize() const { return mProgramSize; }

private:
    void* mProgramLinkages[MAX_SHADER_CONTAINER];
    void* mShaderStages[MAX_SHADER_CONTAINER];
    int mShaderSize;
    int mProgramSize;
};

}
}

#endif
#endif
