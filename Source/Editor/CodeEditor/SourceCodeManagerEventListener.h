/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   SourceCodeManagerEventListener.h
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Source Code Manager Event Listener	

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "MessageControllers/AssetIOMessageController.h"
#include <QObject>
#include <QSet>
#include <QMap>

#ifndef EDITOR_CODEMANAGEREVENTLISTENER_H
#define EDITOR_CODEMANAGEREVENTLISTENER_H

//! User interface state user data for code
class CodeUserData : public Pegasus::Core::IEventUserData
{
public:
    //! constructor
    //! \param handle parent handle
    //! \param codeProxy parent code
    explicit CodeUserData(Pegasus::Core::ISourceCodeProxy * codeProxy);

    //! constructor
    //! \param handle parent handle
    //! \param programProxy parent code
    explicit CodeUserData(Pegasus::Shader::IProgramProxy * programProxy);

    //! destructor
    virtual ~CodeUserData(){}

    //! gets the code that owns this user data
    //! \return the code
    Pegasus::Core::ISourceCodeProxy * GetSourceCode() const { return mData.mSourceCode; }

    //! gets the program that owns this user data
    //! \return the program
    Pegasus::Shader::IProgramProxy * GetProgram() const { return mData.mProgram; }

	//! gets the program that owns this user data
	//! \return the program
	Pegasus::Core::IBasicSourceProxy * GetBasicAsset() const { return mData.mBasicAsset; }

    //! true if this holds a program, false if it holds a source code
    bool IsProgram() const { return mIsProgram; }

    //! sets wether this is a valid code or an invalid (compilation or loading error)
    //! \param the value set
    void SetIsValid(bool value) { mIsValid = value; }

    //! whether a code is valid or not
    //! \return validity return value: true if code runs, false if its errord
    bool IsValid() const { return mIsValid; }

    //! Return the asset handle of this instance
    AssetInstanceHandle GetHandle() const { return mHandle; }

    //! Sets the handle of this instance
    void SetHandle(const AssetInstanceHandle& handle) { mHandle = handle; }


private:
    AssetInstanceHandle mHandle;
    bool mIsValid;
    bool mIsProgram;
    
    union {
        Pegasus::Shader::IProgramProxy* mProgram;
        Pegasus::Core::ISourceCodeProxy* mSourceCode;
		Pegasus::Core::IBasicSourceProxy* mBasicAsset;
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
    SourceCodeManagerEventListener();
    virtual ~SourceCodeManagerEventListener();
    
    //! called in the constructor of a node requiring user data injection
    virtual void OnInitUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name);

    //! called in the destructor of a node requiring user data destruction
    virtual void OnDestroyUserData(Pegasus::Core::IBasicSourceProxy* proxy, const char* name);

    //! Dispatch event callback on a compilation event
    virtual void OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::CompilationEvent& e);

    //! Dispatch event callback on a linker event
    virtual void OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::LinkingEvent& e);

    //! Dispatch event callback on a loading event
    virtual void OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::SourceLoadedEvent& e);

    //! Dispatch event callback on a compilation notification event
    virtual void OnEvent(Pegasus::Core::IEventUserData * userData, Pegasus::Core::CompilerEvents::CompilationNotification& e);

signals:
    //! triggered when any compilation state changes. Use a queued connection
    void CompilationResultsChanged(bool success);

    //! triggered when any compilation error is posted. This event can be triggered
    //! several times during the same compilation
    void OnCompilationError(AssetInstanceHandle handle, int row, QString message);

    //! triggered when compilation begins
    void OnCompilationBegin(AssetInstanceHandle handle);

    //! triggered when compilation ends, posts the log string
    void OnCompilationEnd(QString log);
    
    //! triggered when a linking event occurs (error, warning or an actual positive linking)
    void OnLinkingEvent(QString message, int messageType);

    //! triggered when a file has been saved successfully
    void OnSignalSaveSuccess();

    //! triggered when a file has been not saved and there is an io error
    void OnSignalSavedFileError(int ioError, QString msg);

    //! triggered when any compilation state changes. Use a queued connection
    void OnTagValidity(QString assetPath, bool success);

};

#endif
