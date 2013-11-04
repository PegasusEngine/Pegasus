/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   AssertReturnCode.h
//! \author David Worsham
//! \date   03 Nov 2013
//! \brief  Return code from the assertion manager.

#ifndef PEGASUS_CORE_ASSERTRETURNCODE_H
#define PEGASUS_CORE_ASSERTRETURNCODE_H

namespace Pegasus {
namespace Core {

//! Return codes for the assertion error functions
enum AssertReturnCode {
    ASSERTION_CONTINUE = 0, //!< Skip assert
    ASSERTION_IGNORE, //!< Ignore the assert
    ASSERTION_IGNOREALL, //!< Ignore  all asserts
    ASSERTION_BREAK, //!< Break into debugger

    ASSERTION_NUM_VALUES,
    ASSERTION_INVALID = ASSERTION_NUM_VALUES
};


//! Callback function declaration.
//! One function with this type needs to be declared in the user application
//! to handle assertion errors.
//! \param testStr String representing the assertion test itself
//! \param fileStr String with the filename where the assertion test failed
//! \param line Line number where the assertion test failed
//! \param msgStr String of an optional message making the assertion test easier to understand.
//!               nullptr if the message is not defined
//! \return ASSERTION_xxx constant, chosen from the button the user clicked on
typedef AssertReturnCode (* AssertionHandlerFunc)(const char * testStr,
                                                  const char * fileStr,
                                                  int line,
                                                  const char * msgStr);


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_CORE_ASSERTRETURNCODE_H
