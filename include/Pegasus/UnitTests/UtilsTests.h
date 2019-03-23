/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Unit Tests                                */
/*                                                                                      */
/****************************************************************************************/

//! \file   UtilsTests.h
//! \author Kleber Garcia
//! \date   30th March 2014
//! \brief  Pegasus Unit tests for the Utils package

//! make sure your unit test returns true if pass, false if fail

#pragma once

namespace Pegasus
{
namespace Tests
{
    class TestHarness;
    TestHarness* createUtilsHarness();
}
}

