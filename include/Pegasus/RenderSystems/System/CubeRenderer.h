/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CubeRenderer.h
//! \author Kleber Garcia
//! \date   June 19rd, 2018
//! \brief  Helper class that keeps state and renders cube maps.

#ifndef PEGASUS_RENDER_CUBERENDERER_H
#define PEGASUS_RENDER_CUBERENDERER_H

#include "Pegasus/Render/Render.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus
{
    namespace RenderSystems
    {

        //! CubeRenderer 
        class CubeRenderer
        {
        public:
            //! Constructor
            //! \param allocator the actual allocator used
            explicit CubeRenderer(Alloc::IAllocator* allocator) : mAllocator(allocator) {}

            //! destructor
            virtual ~CubeRenderer() {}
    
            //! tmp function
            void tmp();

        private:
            Alloc::IAllocator* mAllocator;
        };

    }
}

#endif
