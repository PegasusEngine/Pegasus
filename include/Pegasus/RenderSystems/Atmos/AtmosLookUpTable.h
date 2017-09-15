/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AtmosLookUpTable.h
//! \author Arthur Homs
//! \date   June 25rd, 2016
//! \brief  Lookup table for atmosphere rendering

#ifndef PEGASUS_ATMOS_LOOK_UP_TABLE_H
#define PEGASUS_ATMOS_LOOK_UP_TABLE_H

#include "Pegasus/RenderSystems/Config.h"

#if 0 //RENDER_SYSTEM_CONFIG_ENABLE_ATMOS

namespace Pegasus
{
namespace RenderSystems
{

// struct representing a particular case.
struct AtmosLookUpTableBuffer
{
};


//! Class that holds the case patterns.
class AtmosLookUpTable
{
public:

    //! Constructor. All static baby!
    AtmosLookUpTable();

    //! initialize. Builds the marching cubes.
    void Initialize();


    

private:
    void UpdateInternal();
    
};

}
}

#endif
#endif
