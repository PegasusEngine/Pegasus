/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Assertion.h
//! \author	Karolyn Boulanger
//! \date	30th July 2013
//! \brief	Assertion test macros and manager

#ifndef EDITOR_ASSERTION_H
#define EDITOR_ASSERTION_H

#include "Editor.h"

#include <QObject>
#include <QtCore/qobjectdefs.h>


//! Use this macro for hard failures, typically code paths that should not be reached
#define ED_FAIL()                   { static bool ignore = false;                                               \
                                      if (!ignore)                                                              \
                                          ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                        "FAILURE", __FILE__, __LINE__)                          \
                                                    == AssertionManager::ASSERTION_IGNORE;              \
                                    }

//! Use this macro for hard failures, typically code paths that should not be reached,
//! with a formatted message to explain the failure
#define ED_FAILSTR(str, ...)        { static bool ignore = false;                                               \
                                      if (!ignore)                                                              \
                                          ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                        "FAILURE", __FILE__, __LINE__, str, __VA_ARGS__)        \
                                                    == AssertionManager::ASSERTION_IGNORE;              \
                                    }

//! Use this macro for simple assertion tests, which do not require a message to explain the test
#define ED_ASSERT(t)                { static bool ignore = false;                                               \
                                      if (!ignore && !(t))                                                      \
                                          ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                        #t, __FILE__, __LINE__)                                 \
                                                    == AssertionManager::ASSERTION_IGNORE;              \
                                    }

//! Use this macro for assertion tests, with a formatted message that describes the test
#define ED_ASSERTSTR(t, str, ...)   { static bool ignore = false;                                               \
                                      if (!ignore && !(t))                                                      \
                                          ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                        #t, __FILE__, __LINE__, str, __VA_ARGS__)               \
                                                    == AssertionManager::ASSERTION_IGNORE;              \
                                    }

//----------------------------------------------------------------------------------------

//! Assertion manager (singleton) that shows a dialog box when an assertion test fails
class AssertionManager : public QObject
{
    Q_OBJECT

public:

    AssertionManager(Editor * parent);
    virtual ~AssertionManager();

    //! Return codes for the assertion error functions below
    enum ReturnCode {
        ASSERTION_CONTINUE = 0,
        ASSERTION_IGNORE,
        ASSERTION_IGNOREALL,
        ASSERTION_BREAK,

        ASSERTION_NUM_VALUES,
        ASSERTION_INVALID = ASSERTION_NUM_VALUES
    };

    //! Throw an assertion error with a formatted string.
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand,
    //!               with the same formatting syntax as printf(). nullptr if no message is defined.
    //! \return ASSERTION_xxx constant, chosen from the button the user clicked on
    //! \warning The number of parameters following msgStr must match the list of formatting
    //!          strings inside msgStr. No extra parameter is required when msgStr == nullptr.
    //! \warning Do not use QString directly as a parameter, call .toLatin1().constData() first.
    ReturnCode AssertionError(const char * testStr,
                              const char * fileStr,
                              int line,
                              const char * msgStr = nullptr, ...);

    //! Throw an assertion error with an unformatted string.
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr Optional string of a message making the assertion test easier to understand.
    //!               Empty string if no message is defined.
    //! \param allowDebugBreak True to enable breaking into the debugger from inside this function.
    //!                        Otherwise the debug break needs to happen externally, based on the return code
    //! \return ASSERTION_xxx constant, chosen from the button the user clicked on
    ReturnCode AssertionErrorNoFormat(const QString & testStr,
                                      const QString & fileStr,
                                      int line,
                                      const QString & msgStr,
                                      bool allowDebugBreak = true);

private:

    //! Pointer to the parent Editor object
    Editor * mEditor;

    //! True if all assertions are ignored
    bool mIgnoreAll;
};


#endif  // EDITOR_ASSERTION_H
