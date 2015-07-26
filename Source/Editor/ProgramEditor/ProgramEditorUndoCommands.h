/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ProgramEditorUndoCommands.h
//! \author	Kleber Garcia
//! \date	23rd July 2015
//! \brief	List of undo commands for program editor

#ifndef EDITOR_PROGRAMEDITORUNDOCOMMANDS_H
#define EDITOR_PROGRAMEDITORUNDOCOMMANDS_H

#include <QUndoCommand>
#include "Pegasus/Shader/Shared/ShaderDefs.h"

class ProgramEditorWidget;

//! Undo command for shader modification
class ProgramEditorModifyShaderCmd : public QUndoCommand
{
public:
    //! Constructor
    //! \param previousShader the previous shader asset path existed. Must be an empty string if it didn't exist
    //! \param newShader the new shader asset path to add.
    //! \param shaderType the target shader type.
    ProgramEditorModifyShaderCmd(
        ProgramEditorWidget* owner,
        const QString& previousShader, 
        const QString& newShader,
        Pegasus::Shader::ShaderType
    );

    virtual ~ProgramEditorModifyShaderCmd();

    virtual void undo();

    virtual void redo();

    //! Return an ID to uniquely identify this command. Used to enable \a mergeWith()
    //! \return Unique ID derived from the asset strings used
    virtual int id() const;

    //! Merge function to combine the new undo command with the current one if possible
    //! \param command Incoming command
    //! \return False if the commands cannot be merged (accessing a different block for example),
    //!         true if succeeded
    virtual bool mergeWith(const QUndoCommand * command);

private:
    ProgramEditorWidget* mWidget;
    QString mPrevShader;
    QString mNewShader;
    Pegasus::Shader::ShaderType mShaderType;
};

#endif
