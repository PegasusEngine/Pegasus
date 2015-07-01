/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   ScriptRenderApi.h
//! \author Kleber Garcia
//! \date   December 4th, 2014
//! \brief  BlockScript registration of Parr callbacks, so scripts can call, use and abuse
//!         of rendering nodes.

#ifndef SCRIPT_RENDER_API_H
#define SCRIPT_RENDER_API_H

//! forward declarations
namespace Pegasus {

    namespace Core {
        class IApplicationContext;
    }

    namespace BlockScript {
        class BlockLib;
    }
}

namespace Pegasus
{
namespace Application 
{
    //! registers the entire render API into blockscript's intrinsic library
    //! \param appContext - the context of the application
    void RegisterRenderApi(BlockScript::BlockLib* blockLib, Core::IApplicationContext* appContext);

}
}

#endif
