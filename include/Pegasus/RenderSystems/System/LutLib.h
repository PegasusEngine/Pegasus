/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   LutLib.h
//! \author Kleber Garcia
//! \date   January 21st, 2018
//! \brief  A lib bag of luts, accessible for the whole up.

#ifndef PEGASUS_RENDER_SYSTEMS_LUTLIB_H
#define PEGASUS_RENDER_SYSTEMS_LUTLIB_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Texture/Texture.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{

    namespace Alloc
    {
        class IAllocator;
    }

    namespace Core        
    {
        class IApplicationContext;
    }

    namespace BlockScript
    {
        class BlockScriptManager;
        class BlockLib;
    }

    namespace RenderSystems
    {

        //! LutLib, a bag of luts
        class LutLib
        {

        public:

            enum LutEnum {
                ACOS
            };

            //! Constructor
            //! \param allocator the actual allocator used to register a system
            explicit LutLib(Alloc::IAllocator* allocator);

            //! destructor
            ~LutLib() {}

            //! Internal function, called to initialize and load any render resources required.
            void Load(Core::IApplicationContext* appContext);

            //! Internal function, called to initialize and load any render resources required.
            void RegisterBlockscript(BlockScript::BlockLib* bsManager);

            Pegasus::Texture::TextureRef GetLut(LutEnum lut) { return mLuts[(unsigned int)lut]; }

        protected:
            Alloc::IAllocator* mAllocator;
            Pegasus::Utils::Vector<Pegasus::Texture::TextureRef> mLuts;

        };

    }
}

#endif
