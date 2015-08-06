/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Assertion.cpp
//! \author	Karolyn Boulanger
//! \date	30th July 2013
//! \brief	Assertion test macros and manager

#include "Assertion.h"
#include "Log.h"

#include <QString>
#include <QMessageBox>
#include <QPushButton>

#include <stdio.h>
#include <stdarg.h>


//! Maximum size of the buffer containing one assertion error message
static const size_t ASSERTIONERRORARGS_BUFFER_SIZE = 1024; 

//----------------------------------------------------------------------------------------

AssertionManager::AssertionManager(Editor * parent)
:   QObject(parent),
    mEditor(parent),
    mIgnoreAll(false)
{
}

//----------------------------------------------------------------------------------------

AssertionManager::~AssertionManager()
{
}

//----------------------------------------------------------------------------------------

AssertionManager::ReturnCode AssertionManager::AssertionError(const char * testStr,
                                                              const char * fileStr,
                                                              int line,
                                                              const char * msgStr, ...)
{
    ED_ASSERTSTR(testStr != nullptr, "The test string has to be defined for an assertion error.");
    ED_ASSERTSTR(fileStr != nullptr, "The file name string has to be defined for an assertion error.");
    ED_ASSERTSTR(line >= 0, "Invalid line number for an assertion error.");

    // If Ignore All has been selected, ignore the current assertion error
    // (this call is redundant with the one inside AssertionErrorNoFormat, but it avoids formatting the input string for no reason)
    if (mIgnoreAll)
    {
        return ASSERTION_IGNOREALL;
    }

    // If a message string is present, format it with the extra parameters if there are any
    char * formattedString = nullptr;
    if (msgStr != nullptr)
    {
        static char buffer[ASSERTIONERRORARGS_BUFFER_SIZE];
        va_list args;
        va_start(args, msgStr);
        vsnprintf_s(buffer, ASSERTIONERRORARGS_BUFFER_SIZE, ASSERTIONERRORARGS_BUFFER_SIZE - 1, msgStr, args);
        va_end(args);

        formattedString = buffer;
    }

    return AssertionErrorNoFormat(testStr, fileStr, line, formattedString, true);
}

//----------------------------------------------------------------------------------------

AssertionManager::ReturnCode AssertionManager::AssertionErrorNoFormat(const QString & testStr,
                                                                      const QString & fileStr,
                                                                      int line,
                                                                      const QString & msgStr,
                                                                      bool allowDebugBreak)
{
    ED_ASSERTSTR(!testStr.isNull() && !testStr.isEmpty(), "The test string has to be defined for an assertion error.");
    ED_ASSERTSTR(!fileStr.isNull() && !fileStr.isEmpty(), "The file name string has to be defined for an assertion error.");
    ED_ASSERTSTR(line >= 0, "Invalid line number for an assertion error.");

    // If Ignore All has been selected, ignore the current assertion error
    if (mIgnoreAll)
    {
        return ASSERTION_IGNOREALL;
    }

    QString fileString(fileStr);
    fileString.remove("..\\", Qt::CaseSensitive);

    QString lineString;
    lineString.setNum(line);

    // Prepare the content of the dialog box
    QString titleText;
    QString assertionText;
    if (testStr == "FAILURE")
    {
        titleText = tr("Failure");
    }
    else
    {
        titleText = tr("Assertion error");
        assertionText += tr("Test: ");
        assertionText += testStr;
        assertionText += "\n";
    }
    assertionText += tr("File: ");
    assertionText += fileString;
    assertionText += "\n";
    assertionText += tr("Line: ");
    assertionText += lineString;

    //! Send the text of the assertion error to the log
    if (!msgStr.isNull() && !msgStr.isEmpty())
    {
        Editor::GetInstance().GetLogManager().Log('ASRT', (titleText + ". " + assertionText + ". " + msgStr).toLatin1().constData());
    }
    else
    {
        Editor::GetInstance().GetLogManager().Log('ASRT', (titleText + ". " + assertionText).toLatin1().constData());
    }

    // Show the dialog box
    QMessageBox msgBox(mEditor->centralWidget());
    msgBox.setWindowTitle(titleText);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(assertionText);
    if (!msgStr.isNull() && !msgStr.isEmpty())
    {
        msgBox.setInformativeText(msgStr);
    }
    QPushButton * continueButton  = msgBox.addButton(tr("Continue"  ), QMessageBox::AcceptRole);
    QPushButton * ignoreButton    = msgBox.addButton(tr("Ignore"    ), QMessageBox::AcceptRole);
    QPushButton * ignoreAllButton = msgBox.addButton(tr("Ignore All"), QMessageBox::AcceptRole);
    QPushButton * breakButton     = msgBox.addButton(tr("Break"     ), QMessageBox::RejectRole);
    msgBox.setDefaultButton(continueButton);
    msgBox.setEscapeButton(continueButton);
    msgBox.exec();

    // React based on the buttons of the dialog box
    QAbstractButton * returnButton = msgBox.clickedButton();
    if (returnButton == ignoreButton)
    {
        // Ask the assertion error to be ignored in the future
        return ASSERTION_IGNORE;
    }
    else if (returnButton == ignoreAllButton)
    {
        // Make all assertions ignored from now
        mIgnoreAll = true;
        return ASSERTION_IGNOREALL;
    }
    else if (returnButton == breakButton)
    {
        if (allowDebugBreak)
        {
#if PEGASUS_COMPILER_MSVC
            // Break into the debugger
            __debugbreak();
#else
#error "Debug break is not implemented on this platform"
#endif
        }

        return ASSERTION_BREAK;
    }

    return ASSERTION_CONTINUE;
}
