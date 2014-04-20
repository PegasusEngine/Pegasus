/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   ShaderManagerEventListener.h
//! \author Kleber Garcia
//! \date   4rth April 2014
//! \brief  Pegasus Shader Manager Event Listener	

#include "Pegasus/Shader/Shared/IEventListener.h"
#include "Pegasus/Shader/Shared/ShaderEvent.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include <QObject>

#ifndef EDITOR_SHADERMANAGEREVENTLISTENER_H
#define EDITOR_SHADERMANAGEREVENTLISTENER_H

class ShaderLibraryWidget;

class ShaderUserData : public Pegasus::Shader::IUserData
{
public:
    explicit ShaderUserData(Pegasus::Shader::IShaderProxy * shader);
    virtual ~ShaderUserData(){}
    Pegasus::Shader::IShaderProxy * GetShader() const { return mShader; }
    void SetIsValid(bool value) { mIsValid = value; }
    bool IsValid() const { return mIsValid; }
private:
    Pegasus::Shader::IShaderProxy * mShader;
    bool mIsValid;
    
};

class ProgramUserData : public Pegasus::Shader::IUserData
{
public:
    explicit ProgramUserData(Pegasus::Shader::IProgramProxy * program);
    virtual ~ProgramUserData(){}
    Pegasus::Shader::IProgramProxy * GetProgram() const { return mProgram; }
private:
    Pegasus::Shader::IProgramProxy * mProgram;
};

class ShaderManagerEventListener : public QObject, public Pegasus::Shader::IEventListener 
{
    Q_OBJECT;

public:
    explicit ShaderManagerEventListener(ShaderLibraryWidget * widget);
    virtual ~ShaderManagerEventListener();

    //! Dispatch event callback on a compilation event
    virtual void OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::CompilationEvent& e);

    //! Dispatch event callback on a linker event
    virtual void OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::LinkingEvent& e);

    //! Dispatch event callback on a file operation event
    virtual void OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::FileOperationEvent& e);

    //! Dispatch event callback on a loading event
    virtual void OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::ShaderLoadedEvent& e);

    //! Dispatch event callback on a compilation notification event
    virtual void OnEvent(Pegasus::Shader::IUserData * userData, Pegasus::Shader::CompilationNotification& e);

signals:
    void CompilationResultsChanged();

private:
    ShaderLibraryWidget * mLibraryWidget;

};

#endif
