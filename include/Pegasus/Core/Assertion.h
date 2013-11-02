/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Assertion.h
//! \author Kevin Boulanger
//! \date   24th June 2013
//! \brief  Assertion test macros and manager

#ifndef PEGASUS_CORE_ASSERTION_H
#define PEGASUS_CORE_ASSERTION_H

#include "Pegasus/Preprocessor.h"

#if PEGASUS_ENABLE_ASSERT

//! Use this macro for hard failures, typically code paths that should not be reached
#define PG_FAIL()                   { static bool ignore = false;                                                   \
                                      if (!ignore)                                                                  \
                                          ignore = Pegasus::Core::AssertionManager::GetInstance().AssertionError(   \
                                                        "FAILURE", __FILE__, __LINE__)                              \
                                                    == Pegasus::Core::AssertionManager::ASSERTION_IGNORE;           \
                                    }

//! Use this macro for hard failures, typically code paths that should not be reached,
//! with a formatted message to explain the failure
#define PG_FAILSTR(str, ...)        {  static bool ignore = false;                                                  \
                                       if (!ignore)                                                                 \
                                           ignore = Pegasus::Core::AssertionManager::GetInstance().AssertionError(  \
                                                        "FAILURE", __FILE__, __LINE__, str, __VA_ARGS__)            \
                                                    == Pegasus::Core::AssertionManager::ASSERTION_IGNORE;           \
                                    }

//! Use this macro for simple assertion tests, which do not require a message to explain the test
#define PG_ASSERT(t)                { static bool ignore = false;                                                   \
                                      if (!ignore && !(t))                                                          \
                                          ignore = Pegasus::Core::AssertionManager::GetInstance().AssertionError(   \
                                                        #t, __FILE__, __LINE__)                                     \
                                                    == Pegasus::Core::AssertionManager::ASSERTION_IGNORE;           \
                                    }

//! Use this macro for assertion tests, with a formatted message that describes the test
#define PG_ASSERTSTR(t, str, ...)   { static bool ignore = false;                                                   \
                                      if (!ignore && !(t))                                                          \
                                          ignore = Pegasus::Core::AssertionManager::GetInstance().AssertionError(   \
                                                        #t, __FILE__, __LINE__, str, __VA_ARGS__)                   \
                                                    == Pegasus::Core::AssertionManager::ASSERTION_IGNORE;           \
                                    }

#else

#define PG_FAIL()
#define PG_FAILSTR(str, ...)
#define PG_ASSERT(t)
#define PG_ASSERTSTR(t, str, ...)

#endif  // PEGASUS_ENABLE_ASSERT

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_ASSERT

#include "Pegasus/Core/Singleton.h"

namespace Pegasus {
namespace Core {

//! Assertion manager (singleton) that redirects the macros to the assertion handler
class AssertionManager : public AutoSingleton<AssertionManager>
{
public:
    //! Constructor
    AssertionManager();

    //! Destructor
    virtual ~AssertionManager();

    //! Return codes for the assertion error functions below
    enum ReturnCode {
        ASSERTION_CONTINUE = 0, //<! Skip assert
        ASSERTION_IGNORE, //<! Ignore the assert
        ASSERTION_IGNOREALL, //<! Ignore  all asserts
        ASSERTION_BREAK, //<! Break into debugger

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
    typedef ReturnCode (* Handler)(const char * testStr,
                                   const char * fileStr,
                                   int line,
                                   const char * msgStr);

    //! Register the assertion error handler
    //! \warning To be called at least once by the user application. Otherwise,
    //!          a crash will purposely occur to show where the assertion error happened
    //! \param handler Function pointer of the assertion handler (!= nullptr)
    void RegisterHandler(Handler handler);

    //! Unregister the assertion handler if defined
    void UnregisterHandler();

    //! Test if AssertionError() if currently being executed
    //! \return True if currently inside the execution of the assertion handler in \a AssertionError()
    inline bool IsAssertionBeingHandled() const { return mAssertionBeingHandled; }

    //! Throw an assertion error with a formatted string.
    //! The handler registered with \a RegisterHandle is called with the provided parameters
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand,
    //!               with the same formatting syntax as printf(). nullptr if no message is defined.
    //! \warning The number of parameters following msgStr must match the list of formatting
    //!          strings inside msgStr. No extra parameter is required when msgStr == nullptr.
    //! \return ASSERTION_xxx constant, chosen from the button the user clicked on
    ReturnCode AssertionError(const char * testStr,
                              const char * fileStr,
                              int line,
                              const char * msgStr = nullptr, ...);

private:
    // No copies allowed
    PG_DISABLE_COPY(AssertionManager);


    Handler mHandler; //!< Function pointer of the assertion error handler, nullptr by default
    bool mAssertionBeingHandled; //!< True if inside the execution of the assertion handler in \a AssertionError()
};


}   // namespace Core
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_ASSERT

#endif  // PEGASUS_CORE_ASSERTION_H
