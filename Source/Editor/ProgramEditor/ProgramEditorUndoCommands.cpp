#include "ProgramEditor/ProgramEditorUndoCommands.h"

static char gUniqueId = 'P';

ProgramEditorModifyShaderCmd::ProgramEditorModifyShaderCmd(
        ProgramEditorWidget* owner,         
        const QString& newShader, 
        const QString& previousShader,
        Pegasus::Shader::ShaderType shaderType) 
: mPrevShader(previousShader),
  mNewShader(newShader),
  mShaderType(shaderType),
  mWidget(owner)
{
    setText("Changing shader.");
}

ProgramEditorModifyShaderCmd::~ProgramEditorModifyShaderCmd()
{
}

void ProgramEditorModifyShaderCmd::undo()
{
    mWidget->SetShader(mPrevShader, mShaderType);
}

void ProgramEditorModifyShaderCmd::redo()
{
    mWidget->SetShader(mNewShader, mShaderType);
}

int ProgramEditorModifyShaderCmd::id() const
{
    return 0x00000000 | gUniqueId | ((short)(mNewShader.size()) << 16);
}

bool ProgramEditorModifyShaderCmd::mergeWith(const QUndoCommand * command)
{
    if (command->id() & gUniqueId)
    {
        const ProgramEditorModifyShaderCmd* peCmd = static_cast<const ProgramEditorModifyShaderCmd*>(command);
        return (
                mShaderType == peCmd->mShaderType &&
                mNewShader  == peCmd->mNewShader
               );
    }
    return false;
}
