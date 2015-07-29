/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptManager.h
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Blockscript manager class. Use this as main entry point for blockscript creation

#ifndef BLOCKSCRIPT_MANAGER_H
#define BLOCKSCRIPT_MANAGER_H


// forward declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }

    namespace BlockScript
    {
        class BlockScript;
        class BlockLib;
    }
}

// declarations
namespace Pegasus
{

namespace BlockScript
{

//! Main blockscript manager class
class BlockScriptManager
{

public:
    //! constructor, initializes class to default null state
    BlockScriptManager(Alloc::IAllocator* allocator);

    //! destructor, destroys blockscript manager
    ~BlockScriptManager();

    //! creates a new block script executable
    //! \return the blockscript executable. Use Compile function to proceed and build internally
    //!         Make sure to link any created libraries before hand. User is responsible of managing dependencies.
    BlockScript* CreateBlockScript();

    //! creates a new block script module
    //! \param the library name. Pointer is cached, so an imm is encouraged.
    //! \return the blockscript library module
    BlockLib*    CreateBlockLib(const char* name);

    //! gets internal runtime library if we desire to add / modify / remove intrinsic functions
    BlockLib*    GetRuntimeLib();

    //! destroys a block script
    //! \param script - the actual script
    void DestroyBlockScript(BlockScript* script);
    
    //! destroys a library module
    //! \param lib - the library
    void DestroyBlockLib(BlockLib* lib);

private:
    //! initializes every runtime library
    void Initialize(Alloc::IAllocator* allocator);

    BlockLib* mInternalRuntimeLib;
    Alloc::IAllocator*     mAllocator;

};

}

}

#endif
