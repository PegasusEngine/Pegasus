/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ShaderManager.h
//! \author	Kleber Garcia
//! \date	17th October 2013
//! \brief	Shader Manager entry point class. This class serves as major factory towards 
//          shaders. Also manages shader files.

#ifndef PEGASUS_SHADERMANAGER_H
#define PEGASUS_SHADERMANAGER_H

#include "Pegasus/Shader/EventDispatcher.h"
#include "Pegasus/Shader/ShaderStage.h"

//Forward Declarations
namespace Pegasus
{
namespace Shader
{

class ShaderManager : private EventDispatcher
{
public:
    ShaderManager();
    virtual ~ShaderManager();

private:
    //no copy constructor
    explicit ShaderManager(const ShaderManager& other);
    ShaderManager operator=(const ShaderManager& other);
    static const int MAXIMUM_SHADER_STAGES = 50;    
    static const int MAXIMUM_PROGRAMS = 50; 

    class ShaderPoolStage
    {
    public:
        ShaderPoolStage(); 
        ~ShaderPoolStage();
    private:
        //no copy constructor
        explicit ShaderPoolStage(const ShaderPoolStage& other);
        ShaderPoolStage& operator=(const ShaderPoolStage& other); 
        int mSize;
        ShaderStage * mPoolSize;
         
    }; 
    
    ShaderPoolStage mShaderBuckets[SHADER_STAGES_COUNT];
    
};


}
}

#endif
