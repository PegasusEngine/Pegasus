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

#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{

namespace Alloc
{
    class IAllocator;
}

namespace BlockScript
{

class IFileIncluder;

//! Internal compiler preprocessor directive controller / parser
class Preprocessor
{
public:
    enum Cmd
    {
        PP_CMD_NONE, //no command
        PP_CMD_INCLUDE, // include command
        PP_CMD_DEFINE, //define command
        PP_CMD_IF, //ifdef command
        PP_CMD_ELSEIF, //elif command
        PP_CMD_ELSE, //else command
        PP_CMD_ENDIF //endif command
    };

    //! Define container. For now is just a name/value string pair
    struct Definition
    {
        const char* mName;
        int         mBufferSize;
        const char* mValue;
        bool        mIsInclude;
    public:
        Definition() : mName(nullptr), mValue(nullptr), mIsInclude(false), mBufferSize(0) {}
    };

    //! Constructor
    Preprocessor(Alloc::IAllocator* alloc);

    //! Destructor 
    ~Preprocessor();

    //! Push include command
    void IncludeCmd();

    //! Push define command
    void DefineCmd();

    //! Push if command
    void IfCmd();

    //! Push elseif command
    void ElseIfCmd();

    //! Push else command
    void ElseCmd();

    //! Push endif command
    void EndIfCmd();

    //! clear commands
    void NoneCmd();

    //! push a string argument
    void PushString(const char* str);

    //! push a code (bigger string) argument
    void PushCode  (const char* str);

    //! execute the command that has been accumulated argument
    bool FlushCommand(const char** errString);

    //! get the current command
    Cmd GetCmd() const { return Top().mCommand; }

    //! get the number of defines 
    int GetDefinitionCount() const { return mDefinitions.GetSize(); }

    //! get a definition by string name
    const Definition* FindDefinitionByName(const char* name) const;

    //! Sets this code block to active
    void SetIfActive(bool active);

    //!  boolean used to discriminate between if/else statements
    void SetChosePath(bool active);

    //!  returns wether this code path is active
    bool IsIfActive() const;

    //!  returns wether somebody already chose a path before
    bool ChosePath() const;

    //!  pushes a brand new state for this preprocessor controller
    void NewState();

    //!  pops state for this preprocessor controller
    void PopState();

    //!  gets the count of states
    int  StateCount() { return mStateStack.GetSize(); }
    
    //!  returns the String argument pushed
    const char* GetStringArg() const { return Top().mStringArg; }

    //! sets the file includer handler
    void SetFileIncluder(IFileIncluder* fileIncluder) { mFileIncluder = fileIncluder; }

    //! gets the file includer handler
    IFileIncluder* GetFileIncluder() const { return mFileIncluder; }
    
    //! \return true if there is a buffer pending, false otherwise
    bool HasIncludeBuffer() const { return mHasInclude; }

    //! gets the include buffer
    Definition* GetIncludeDefinition() { return mNextIncludeDefinition; }

private:

    //! internal state structure of a preprocessor frame
    struct State
    {
        Cmd mCommand;
        const char* mStringArg;
        const char* mCodeArg;
        bool mIsIfEnabled;
        bool mIsChosePath;
    };

    //! gets the current active preprocessor states
    State& Top();
    const State& Top() const;

    //! vector holding macro definitions
    Utils::Vector<Definition> mDefinitions;

    //! vector holding macro include buffers
    Utils::Vector<Definition> mIncludeDefs;

    //! vector holding states
    Utils::Vector<State> mStateStack;

    //! includer callback
    IFileIncluder* mFileIncluder;

    bool mHasInclude;
    Definition* mNextIncludeDefinition;
};

}

}

#endif
