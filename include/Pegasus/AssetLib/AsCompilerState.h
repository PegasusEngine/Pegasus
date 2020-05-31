/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AsCompilerState.h
//! \author Kleber Garcia
//! \date   February 8 2015
//! \brief  AssetScript Compiler State.

#ifndef PEGASUS_AS_COMPILER_STATE_H
#define PEGASUS_AS_COMPILER_STATE_H

//fwd declarations
namespace Pegasus
{
    namespace Io
    {
        class FileBuffer;
    }

    namespace AssetLib
    {
        class AssetBuilder;
    }
}

namespace Pegasus
{
namespace AssetLib
{
    class AsCompilerState
    {
    public:
        size_t mBufferPosition;
        const Io::FileBuffer* mFileBuffer;
        AssetBuilder*  mBuilder;
        char mStringAccumulator[512];
        int  mStringAccumulatorPos;
        
        AsCompilerState()
        : mBufferPosition(0),
          mFileBuffer(nullptr),
          mBuilder(nullptr),
          mStringAccumulatorPos(0)
        { 
        }
            
    };
}
}

#endif
