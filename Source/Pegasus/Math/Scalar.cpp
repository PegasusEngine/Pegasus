/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Scalar.cpp
//! \author Karolyn Boulanger
//! \date   10th September 2013
//! \brief  Scalar operations

#include "Pegasus/Math/Scalar.h"
//#include "Tools/Threading/Time.h"


namespace Pegasus {
namespace Math {


void SRand()
{
    //! \todo Implement clock-based random number initialization
    //srand((unsigned int)Floor(Mod(GetChrono() * 1000000.0, 4.0e9)));
    srand(0);
}

//----------------------------------------------------------------------------------------

void SRand(PUInt32 seed)
{
    srand((unsigned int)seed);
}


}   // namespace Math
}   // namespace Pegasus
