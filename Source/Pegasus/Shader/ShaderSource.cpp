/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file ShaderSource.cpp
//! \author Kleber Garcia
//! \date 3rd March 2015
//! \brief Pegasus Shader include source file

#include "Pegasus/Shader/ShaderSource.h"
#include "Pegasus/Shader/ProgramLinkage.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::Shader;

ShaderSource::ShaderSource (Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
: Core::SourceCode(nodeAllocator, nodeDataAllocator)
 ,mAllocator(nodeAllocator)
 ,mIsDirty(false)
#if PEGASUS_ENABLE_PROXIES
 ,mProxy(this)
#endif
{
    PEGASUS_EVENT_INIT_DISPATCHER
}

ShaderSource::~ShaderSource()
{
#if PEGASUS_ENABLE_PROXIES
    PEGASUS_EVENT_DESTROY_USER_DATA(&mProxy, "ShaderSource", GetEventListener());
#endif
}

void Pegasus::Shader::ShaderSource::RegisterParent(ProgramLinkage* programLinkage)
{
    if (mLockParentArray)
        return;
    mProgramParents.PushEmpty() = programLinkage;
}

void Pegasus::Shader::ShaderSource::UnregisterParent(ProgramLinkage* programLinkage)
{
    if (mLockParentArray)
        return;

    for (unsigned int i = 0; i < mProgramParents.GetSize(); ++i)
    {
        if (mProgramParents[i] == programLinkage)
        {
            mProgramParents.Delete(i);
            return;
        }
    }
    PG_FAILSTR("Trying to unregister a program linkage that has no parent?");
}

Pegasus::Graph::NodeReturn Pegasus::Shader::ShaderSource::CreateNode(Graph::NodeManager* nodeManager, Alloc::IAllocator* nodeAllocator, Alloc::IAllocator* nodeDataAllocator)
{
    return PG_NEW(nodeAllocator, -1, "ShaderSource", Pegasus::Alloc::PG_MEM_TEMP) Pegasus::Shader::ShaderSource(nodeAllocator, nodeDataAllocator);
}

void ShaderSource::Compile()
{
    if (!mIsDirty)
        return;

    SourceCode::Compile();

    mLockParentArray = true;
    for (unsigned int i = 0; i < mProgramParents.GetSize(); ++i)
    {
        mProgramParents[i]->InvalidateData();
        mProgramParents[i]->Compile();
    }
    mLockParentArray = false;
    mIsDirty = false;
}

void ShaderSource::GenerateData()
{
}

Pegasus::Graph::NodeData * ShaderSource::AllocateData() const
{
    return nullptr;
}
