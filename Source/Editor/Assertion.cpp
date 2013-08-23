/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Assertion.cpp
//! \author	Kevin Boulanger
//! \date	30th July 2013
//! \brief	Assertion test macros and manager

#include "Assertion.h"

#include <QString>
#include <QMessageBox>
#include <QPushButton>


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

bool AssertionManager::AssertionError(const char * testStr,
                                      const char * fileStr,
                                      int line,
                                      const char * msgStr)
{
    ED_ASSERTSTR(testStr != nullptr, "The test string has to be defined for an assertion error");
    ED_ASSERTSTR(fileStr != nullptr, "The file name string has to be defined for an assertion error");
    ED_ASSERTSTR(line >= 0, "Invalid line number for an assertion error");

    // If Ignore All has been selected, ignore the current assertion error
    if (mIgnoreAll)
    {
        return false;
    }

    //! \todo Add the assertion text to the log

    // Convert the strings to QString
    const QString testString(testStr);
    QString fileString(fileStr);
    fileString.remove("..\\", Qt::CaseSensitive);
    QString lineString;
    lineString.setNum(line);
    const QString msgString(msgStr);

    // Prepare the content of the dialog box
    QString titleText;
    QString assertionText;
    QString assertionMsgText;
    if (testString == "FAILURE")
    {
        titleText = tr("Failure");
    }
    else
    {
        titleText = tr("Assertion error");
        assertionText += tr("Test: ");
        assertionText += testString;
        assertionText += "\n";
    }
    assertionText += tr("File: ");
    assertionText += fileString;
    assertionText += "\n";
    assertionText += tr("Line: ");
    assertionText += lineString;
    if (!msgString.isNull() && !msgString.isEmpty())
    {
        assertionMsgText = msgString;
    }

    // Show the dialog box
    QMessageBox msgBox(mEditor->centralWidget());
    msgBox.setWindowTitle(titleText);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(assertionText);
    if (!assertionMsgText.isNull() && !assertionMsgText.isEmpty())
    {
        msgBox.setInformativeText(assertionMsgText);
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
    if (returnButton == breakButton)
    {
        // Break into the debugger
        __debugbreak();
    }
    else if (returnButton == ignoreButton)
    {
        // Ask the assertion error to be ignored in the future
        return true;
    }
    else if (returnButton == ignoreAllButton)
    {
        // Make all assertions ignored from now
        mIgnoreAll = true;
    }

    return false;
}
