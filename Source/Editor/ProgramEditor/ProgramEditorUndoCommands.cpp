#include "ProgramEditor/ProgramEditorUndoCommands.h"
#include "ProgramEditor/ProgramEditorWidget.h"

static char gUniqueId = 'P';

ProgramEditorModifyShaderCmd::ProgramEditorModifyShaderCmd(
        ProgramEditorWidget* owner,         
        const QString& newShader, 
        const QString& previousShader)
: mPrevShader(previousShader),
  mNewShader(newShader),
  mWidget(owner)
{
    setText("Changing shader.");
}

ProgramEditorModifyShaderCmd::~ProgramEditorModifyShaderCmd()
{
}

void ProgramEditorModifyShaderCmd::undo()
{
    mWidget->SetShader(mPrevShader);
}

void ProgramEditorModifyShaderCmd::redo()
{
    mWidget->SetShader(mNewShader);
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
                mNewShader  == peCmd->mNewShader
               );
    }
    return false;
}
