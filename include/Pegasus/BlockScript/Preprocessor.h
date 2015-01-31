/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Preprocessor.h
//! \author Kleber Garcia
//! \date   January 31, 2015
//! \brief  Blockscript Internal Compiler Preprocessor directive

#ifndef PEGASUS_PP_H
#define PEGASUS_PP_H

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace BlockScript
{

class Preprocessor
{
public:
    enum Cmd
    {
        PP_CMD_NONE,
        PP_CMD_INCLUDE,
        PP_CMD_DEFINE
    };
    Preprocessor();
    ~Preprocessor();

    void IncludeCmd();
    void DefineCmd();
    void PushString(const char* str);
    void PushCode  (const char* str);
    bool FlushCommand(const char** errString);

    Cmd GetCmd() const { return mCommand; }

private:
    const char* mStringArg;
    const char* mCodeArg;
    Cmd mCommand;
};

}

}

#endif
