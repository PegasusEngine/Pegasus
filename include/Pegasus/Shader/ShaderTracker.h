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

//! shader tracker class. Contains and tracks of all the shaders and programs in the application
//! \note warning: this container's insertion and deletion operations cost O(n)
class ShaderTracker
{
public:
    ShaderTracker();
    ~ShaderTracker(){}

    //! Maximum amount of elements the static container has
    static const int MAX_SHADER_CONTAINER = 256;

    //! Inserts a program to the container
    //! \param reference to a program
    void InsertProgram(ProgramLinkage * program);

    //! Inserts a shader to the container
    //! \param reference to a shader
    void InsertShader (ShaderStage * shader);    

    //! Deletes a program within the container
    //! \param program reference to a program to remove from container
    void DeleteProgram(ProgramLinkage * program);
    
    //! Deletes a shader from the container
    //! \param shader reference to a shader to remove from container
    void DeleteShader (ShaderStage * shader);

    //! Deletes a shader from the container
    //! \param id the id of the shader to remove
    //! \return returns the deleted shader,
    //!         otherwise returns null if it cant find it
    ShaderStage* DeleteShader (int id);
    
    //! Deletes a program within the container
    //! \param id the id of the program to delete
    //! \return returns the deleted shader,
    //!         otherwise returns null if it cant find it
    ProgramLinkage* DeleteProgram(int id);

    //! Finds the index of a shader
    //! \param shader target shader to find
    //! \return -1 if not found, otherwise the index
    int FindShaderIndex(ShaderStage* shader) const;
    
    //! Finds the index of a program
    //! \param program target program to find
    //! \return -1 if not found, otherwise the index
    int FindProgramIndex(ProgramLinkage* program) const;

    //! \param id index position of program to retrieve
    //! \return the program specified in the index, null otherwise
    ProgramLinkage*  GetProgram(int id) const;

    //! \param id index position of shader to retrieve
    //! \return the shader specified in the index, null otherwise
    ShaderStage*     GetShaderStage(int id) const;

    //! \return returns the number of shaders
    int ShaderSize() const { return mShaderSize; }

    //! \return returns the number of programs
    int ProgramSize() const { return mProgramSize; }

private:
    //! pool of program linkages references
    void* mProgramLinkages[MAX_SHADER_CONTAINER];

    //! pool of shader linkage references
    void* mShaderStages[MAX_SHADER_CONTAINER];

    //! internal int containing sizes of shaders
    int mShaderSize;

    //! internal int containing sizes of programs
    int mProgramSize;
};

}
}

#endif
#endif
