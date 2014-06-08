/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   ShaderManagerEventListener.h
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Shader Manager Event Listener	

#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include <QObject>
#include <QSet>
#include <QMap>

#ifndef EDITOR_SHADERMANAGEREVENTLISTENER_H
#define EDITOR_SHADERMANAGEREVENTLISTENER_H

class ShaderLibraryWidget;

//! User interface state user data for shaders
class ShaderUserData : public Pegasus::Graph::IGraphUserData
{
public:
    //! constructor
    //! \param parent shader
    explicit ShaderUserData(Pegasus::Shader::IShaderProxy * shader);

    //! destructor
    virtual ~ShaderUserData(){}

    //! gets the shader that owns this user data
    //! \return the shader returned
    Pegasus::Shader::IShaderProxy * GetShader() const { return mShader; }

    //! sets wether this is a valid shader or an invalid (compilation or loading error)
    //! \param the value set
    void SetIsValid(bool value) { mIsValid = value; }

    //! whether a shader is valid or not
    //! \return validity return value: true if shader runs, false if its errord
    bool IsValid() const { return mIsValid; }

    //! clears all the invalid lines regustered in the shader
    void ClearInvalidLines() { mInvalidLinesSet.clear(); mMessagesMap.clear(); }

    //TODO: mix these two into the same data structure
    //! invalidates a line in the shader
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

private:
    Pegasus::Shader::IShaderProxy * mShader;
    bool mIsValid;
    QSet<int> mInvalidLinesSet;
    QMap<int, QString> mMessagesMap;
    
};

//! User interface state user data for shader programs
class ProgramUserData : public Pegasus::Graph::IGraphUserData
{
public:
    //! constructor
    //! \param owner program of this user data
    explicit ProgramUserData(Pegasus::Shader::IProgramProxy * program);

    //! destructor
    virtual ~ProgramUserData(){}

    //! gets the owner program of this user data
    //! \param the program that has this user data
    Pegasus::Shader::IProgramProxy * GetProgram() const { return mProgram; }

    //! checks wether this program is valid or invalid
    //! \param true if this program can run, false otherwise
    bool IsValid() const { return mIsValid; }

    //! gets the error message that this program has
    //! \return the last error message that this program had (in case of linking errors)
    const QString& GetErrorMessage() const { return mErrorMessage; }

    //! sets validity of program
    //! \param determines if this is a valid program
    void SetIsValid(bool valid) { mIsValid = valid; }
    void SetErrorMessage(const QString& message) { mErrorMessage = message; }
private:
    Pegasus::Shader::IProgramProxy * mProgram;
    bool mIsValid;
    QString mErrorMessage;
};

//! event listener bridge, which implements the app's shader compiler event listener
//! this class serves as a bridge across the app and the user interface to present data
//! all the signals of this app must be queued connections, and no state should be set within
//! these event callbacks
//! this event must be pushed before the application loads any data
class ShaderManagerEventListener : public QObject, public Pegasus::Shader::IShaderEventListener 
{
    Q_OBJECT;

public:
    explicit ShaderManagerEventListener(ShaderLibraryWidget * widget);
    virtual ~ShaderManagerEventListener();

    //! Dispatch event callback on a compilation event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::CompilationEvent& e);

    //! Dispatch event callback on a linker event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::LinkingEvent& e);

    //! Dispatch event callback on a file operation event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::FileOperationEvent& e);

    //! Dispatch event callback on a loading event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::ShaderLoadedEvent& e);

    //! Dispatch event callback on a compilation notification event
    virtual void OnEvent(Pegasus::Graph::IGraphUserData * userData, Pegasus::Shader::CompilationNotification& e);

signals:
    //! triggered when any compilation state changes. Use a queued connection
    void CompilationResultsChanged(void* shaderPointer);

    //! triggered when any compilation error is posted. This event can be triggered
    //! several times during the same compilation
    void OnCompilationError(void* shaderPointer, int row, QString message);

    //! triggered when compilation begins
    void OnCompilationBegin(void* shaderPointer);
    
    //! triggered when a linking event occurs (error, warning or an actual positive linking)
    void OnLinkingEvent(void* shaderPointer, QString message, int messageType);

private:
    ShaderLibraryWidget * mLibraryWidget;

};

#endif
