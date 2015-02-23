/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   SourceCodeManagerEventListener.h
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Source Code Manager Event Listener	

#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include <QObject>
#include <QSet>
#include <QMap>

#ifndef EDITOR_CODEMANAGEREVENTLISTENER_H
#define EDITOR_CODEMANAGEREVENTLISTENER_H

class AssetLibraryWidget;

class QTextDocument;

class QSemaphore;

//! User interface state user data for code
class CodeUserData : public Pegasus::Graph::IGraphUserData
{
public:
    //! constructor
    //! \param parent code
    explicit CodeUserData(Pegasus::Core::ISourceCodeProxy * codeProxy);

    //! constructor
    //! \param parent code
    explicit CodeUserData(Pegasus::Shader::IProgramProxy * programProxy);

    //! destructor
    virtual ~CodeUserData(){}

    //! gets the code that owns this user data
    //! \return the code
    Pegasus::Core::ISourceCodeProxy * GetSourceCode() const { return mData.mSourceCode; }

    //! gets the program that owns this user data
    //! \return the program
    Pegasus::Shader::IProgramProxy * GetProgram() const { return mData.mProgram; }

    //! Clears any reference data
    void ClearData() { mData.mSourceCode = nullptr; }

    //! true if this holds a program, false if it holds a source code
    bool IsProgram() const { return mIsProgram; }

    //! sets wether this is a valid code or an invalid (compilation or loading error)
    //! \param the value set
    void SetIsValid(bool value) { mIsValid = value; }

    //! whether a code is valid or not
    //! \return validity return value: true if code runs, false if its errord
    bool IsValid() const { return mIsValid; }

    //! clears all the invalid lines regustered in the code
    void ClearInvalidLines() { mInvalidLinesSet.clear(); mMessagesMap.clear(); }

    //TODO: mix these two into the same data structure
    //! invalidates a line in the code
    //! \param line to tag as invalid for syntax highlighing and presentation
    void InvalidateLine(int line) { mInvalidLinesSet.insert(line); }
    
    //! registers a message for a single compiled line
    //! \param the line that is invalid
    //! \param the string to record for this invalid line
    void InsertMessage(int line, const QString& str) { mMessagesMap.insert(line, str); }

    //! tests whether the current line is valid
    //! \param the line to test
    //! \return true if the line contains a compilation or warning error
    bool IsInvalidLine (int line) const { return mInvalidLinesSet.contains(line); }

    //! \return gets the set of invalid lines (with compilation errors)
    QSet<int>& GetInvalidLineSet() { return mInvalidLinesSet; }

    //! \return gets the map with lines & messages to display in case of compilation errors
    QMap<int, QString>& GetMessageMap() { return mMessagesMap; }

    //! gets the error message that this program has
    //! \return the last error message that this program had (in case of linking errors)
    const QString& GetErrorMessage() const { return mErrorMessage; }

    //! Sets an error message to be carried. 
    void SetErrorMessage(const QString& message) { mErrorMessage = message; }

    //! sets the intermediate document
    void SetDocument(QTextDocument* document) { mIntermediateDocument = document; }

    //! gets the intermediate document
    QTextDocument* GetDocument() const { return mIntermediateDocument; }

    //! an int attached to the user data. 0 by default
    void SetDispatchType(int value) { mDispatchType = value; }

    //! returns the dispatch type. 0 by default
    int GetDispatchType() const { return mDispatchType; }

private:
    bool mIsValid;
    QSet<int> mInvalidLinesSet;
    QMap<int, QString> mMessagesMap;
    QString mErrorMessage;
    QTextDocument* mIntermediateDocument;
    bool mIsProgram;
    int  mDispatchType;
    
    union {
        Pegasus::Shader::IProgramProxy* mProgram;
        Pegasus::Core::ISourceCodeProxy* mSourceCode;
    } mData;
    
};

//! event listener bridge, which implements the app's code compiler event listener
//! this class serves as a bridge across the app and the user interface to present data
//! all the signals of this app must be queued connections, and no state should be set within
//! these event callbacks
//! this event must be pushed before the application loads any data
class SourceCodeManagerEventListener : public QObject, public Pegasus::Core::CompilerEvents::ICompilerEventListener
{
    Q_OBJECT;

public:
    explicit SourceCodeManagerEventListener(AssetLibraryWidget * widget);
    virtual ~SourceCodeManagerEventListener();
    
    //! called in the constructor of a node requiring user data injection
    virtual void OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name);

    //! called in the destructor of a node requiring user data destruction
    virtual void OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name);

    //! Dispatch event callback on a compilation event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::CompilationEvent& e);

    //! Dispatch event callback on a linker event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::LinkingEvent& e);

    //! Dispatch event callback on a file operation event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::FileOperationEvent& e);

    //! Dispatch event callback on a loading event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e);

    //! Dispatch event callback on a compilation notification event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Core::CompilerEvents::CompilationNotification& e);

signals:
    //! triggered when any compilation state changes. Use a queued connection
    void CompilationResultsChanged();

    //! triggered when any compilation error is posted. This event can be triggered
    //! several times during the same compilation
    void OnCompilationError(CodeUserData* code, int row, QString message);

    //! triggered when compilation begins
    void OnCompilationBegin(CodeUserData* code);

    //! triggered when compilation ends, posts the log string
    void OnCompilationEnd(QString log);
    
    //! triggered when a linking event occurs (error, warning or an actual positive linking)
    void OnLinkingEvent(QString message, int messageType);

    //! triggered when a file has been saved successfully
    void OnSignalSaveSuccess();

    //! triggered only when blockscript renews the initialization screen and adds new / removes old nodes
    void OnSignalUpdateUIViews();

    //! triggered when a file has been not saved and there is an io error
    void OnSignalSavedFileError(int ioError, QString msg);

    //! request ui thread to bless user data with ui specific stuff
    void OnBlessUserData(CodeUserData* userData);
    
    //! request ui thread to unbless user data with ui specific stuff
    void OnUnblessUserData(CodeUserData* userData);

public slots:
    //! safe call of user data destruction once the ui thread removes any references
    void SafeDestroyUserData(CodeUserData* userData);
    

private:
    AssetLibraryWidget * mLibraryWidget;

};

#endif
