/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Assertion.h
//! \author	Kevin Boulanger
//! \date	30th July 2013
//! \brief	Assertion test macros and manager

#ifndef EDITOR_ASSERTION_H
#define EDITOR_ASSERTION_H

#include "Editor.h"

#include <QObject>


//! Use this macro for hard failures, typically code paths that should not be reached
#define ED_FAIL()               { static bool ignore = false;   \
                                  if (!ignore)  \
                                      ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                    "FAILURE", __FILE__, __LINE__);         \
                                }

//! Use this macro for hard failures, typically code paths that should not be reached,
//! with a message to explain the failure
#define ED_FAILSTR(str)         { static bool ignore = false;   \
                                  if (!ignore)  \
                                      ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                    "FAILURE", __FILE__, __LINE__, str);    \
                                }

//! Use this macro for simple assertion tests, which do not require a message to explain the test
#define ED_ASSERT(t)            { static bool ignore = false;   \
                                  if (!ignore && !(t))  \
                                      ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                    #t, __FILE__, __LINE__);        \
                                }

//! Use this macro for assertion tests, with a message that describes the test
#define ED_ASSERTSTR(t, str)    { static bool ignore = false;   \
                                  if (!ignore && !(t))  \
                                      ignore = Editor::GetInstance().GetAssertionManager().AssertionError(  \
                                                    #t, __FILE__, __LINE__, str);   \
                                }

//----------------------------------------------------------------------------------------

//! Assertion manager (singleton) that shows a dialog box when an assertion test fails
class AssertionManager : public QObject
{
    Q_OBJECT

public:

    AssertionManager(Editor * parent);
    ~AssertionManager();


    //! Throw an assertion error.
    //! \param testStr String representing the assertion test itself
    //! \param fileStr String with the filename where the assertion test failed
    //! \param line Line number where the assertion test failed
    //! \param msgStr String of an optional message making the assertion test easier to understand
    //! \return True if the assertion test is ignored in the future
    bool AssertionError(const char * testStr,
                        const char * fileStr,
                        int line,
                        const char * msgStr = nullptr);

private:

    //! Pointer to the parent Editor object
    Editor * mEditor;

    //! True if all assertions are ignored
    bool mIgnoreAll;
};


#endif  // EDITOR_ASSERTION_H
