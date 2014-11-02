/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   CodeTextEditorTreeWidget.cpp
//! \author	Kleber Garcia
//! \date	July 6th, 2014
//! \brief	Code Editor Tree of views. Allows recursive subdivision of views

#include "CodeEditor/CodeTextEditorTreeWidget.h"
#include "CodeEditor/CodeTextEditorWidget.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QPushButton>


void CodeTextEditorTreeWidget::SignalCombination::Initialize(QSignalMapper * textChanged, QSignalMapper * selected)
{
    mTextChangedMapper = textChanged;
    mSelectedSignalMapper = selected;
}

void CodeTextEditorTreeWidget::SignalCombination::Bind(CodeTextEditorWidget * editor)
{
    mTextChangedMapper->setMapping(editor, static_cast<QWidget*>(editor));
    connect ( editor, SIGNAL(textChanged()),
             mTextChangedMapper, SLOT(map()) );

    mSelectedSignalMapper->setMapping(editor, static_cast<QWidget*>(editor));
    connect (editor, SIGNAL(SignalSelected()),
            mSelectedSignalMapper, SLOT(map()));

}

void CodeTextEditorTreeWidget::SignalCombination::Unbind(CodeTextEditorWidget * editor)
{
    disconnect ( editor, SIGNAL(textChanged()),
             mTextChangedMapper, SLOT(map()) );
    mTextChangedMapper->removeMappings(editor);

    disconnect ( editor, SIGNAL(Selected()),
            mSelectedSignalMapper, SLOT(map()));
    mSelectedSignalMapper->removeMappings(editor);
}

CodeTextEditorTreeWidget::CodeTextEditorTreeWidget(
    CodeTextEditorTreeWidget::SignalCombination& signalCombo,
    Qt::Orientation orientation, 
    QWidget * parent
)
: QSplitter(orientation, parent), mOrientation(orientation), mChildrenCount(0)
{
    mSignalCombo = signalCombo;
    PushLeafChild();
}

CodeTextEditorTreeWidget::CodeTextEditorTreeWidget(
    CodeTextEditorTreeWidget::SignalCombination& signalCombo,
    QWidget * parent
)
: QSplitter(Qt::Vertical, parent), mOrientation(Qt::Vertical), mChildrenCount(0)
{
    mSignalCombo = signalCombo;
    PushLeafChild();
}

CodeTextEditorTreeWidget::~CodeTextEditorTreeWidget()
{
}

void CodeTextEditorTreeWidget::ForceUpdateAllStyles()
{
    for (int i = 0; i < mChildrenCount; ++i)
    {
        if (mChildren[i].mIsLeaf)
        {
            mChildren[i].mLeafChild->UpdateAllDocumentSyntax();
        }
        else
        {
            mChildren[i].mTreeChild->ForceUpdateAllStyles();
        }
    }
}

void CodeTextEditorTreeWidget::InternalSplit(int childIndex, Qt::Orientation orientation)
{
    if (orientation == mOrientation)
    {
        if (PushLeafChild(childIndex))
        {
            ED_ASSERT(childIndex + 1 < mChildrenCount);
            //the child in front, which is the new one
            mChildren[childIndex + 1].mLeafChild->UpdateAllDocumentSyntax();
        }
    }
    else
    {
        //remove the current child
        ED_ASSERT(mChildren[childIndex].mIsLeaf);
        CodeTextEditorWidget * oldWidget = mChildren[childIndex].mLeafChild;

        //replace with a new tree
        mChildren[childIndex].mIsLeaf = false;
        mChildren[childIndex].mTreeChild = new CodeTextEditorTreeWidget(mSignalCombo, orientation, this);
        mChildren[childIndex].mTreeChild->ForceUpdateAllStyles();

        insertWidget(childIndex, mChildren[childIndex].mTreeChild);

        //reinsert in the child
        mChildren[childIndex].mTreeChild->PushLeafChild(0, oldWidget);
    }
}

bool CodeTextEditorTreeWidget::RecurseSplit(Qt::Orientation orientation)
{
    //do we have within our children a selected view?    
    for (int i = 0; i < mChildrenCount; ++i)
    {
        CodeTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (child.mLeafChild->IsFocus())
            {
                CodeTextEditorWidget * cachedLeaf = child.mLeafChild;
                InternalSplit(i, orientation);
                cachedLeaf->setFocus();//refocus this widget
                return true;
            }
        }
        else
        {
            if (child.mTreeChild->RecurseSplit(orientation))
            {
                return true;
            }
        }
    }
    return false;
}

void CodeTextEditorTreeWidget::RemoveChild(int i)
{
    ED_ASSERT(i < mChildrenCount && mChildrenCount > 0);
    for (; i < mChildrenCount - 1; ++i)
    {
        mChildren[i] = mChildren[i + 1];
    }
    --mChildrenCount;
}

bool CodeTextEditorTreeWidget::RecurseCloseSplit()
{
    //find the child that is focused 
    for (int i = 0; i < mChildrenCount; ++i)
    {
        CodeTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (child.mLeafChild->IsFocus())
            {
                mSignalCombo.Unbind(child.mLeafChild);
                delete child.mLeafChild; //this removes it from the UI
                RemoveChild(i);  //update the count list
                return true;
            }
        }
        else
        {
            if (child.mTreeChild->RecurseCloseSplit())
            {
                //is it containing only one leaf?
                if (child.mTreeChild->mChildrenCount == 1)
                {
                    //add its leaf to the current tree
                    InternalPushChild(child.mTreeChild->mChildren[0], i);
                    delete child.mTreeChild; //remove redundant empty child from the UI
                    RemoveChild(i);
                    return true;
                }
            }
        }
    }

    return false;
}

bool CodeTextEditorTreeWidget::RecurseDisplayCode(Pegasus::Core::ISourceCodeProxy * proxy)
{
    for (int i = 0; i < mChildrenCount; ++i)
    {
        CodeTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (child.mLeafChild->IsFocus())
            {
                child.mLeafChild->Initialize(proxy);
                return true;
            }
        }
        else
        {
            if (child.mTreeChild->RecurseDisplayCode(proxy))
            {
                return true;
            }
        }
    }
    return false;
}

CodeTextEditorWidget * CodeTextEditorTreeWidget::FindCodeInEditors(Pegasus::Core::ISourceCodeProxy * proxy)
{
    for (int i = 0; i < mChildrenCount; ++i)
    {
        CodeTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (
                (proxy == nullptr && child.mLeafChild->GetCode() == nullptr) || //next available editor slot is empty?
                (proxy != nullptr && child.mLeafChild->GetCode() != nullptr && child.mLeafChild->GetCode()->GetGuid() == proxy->GetGuid())
               )
            {
                return child.mLeafChild;
            }
        }
        else
        {
            CodeTextEditorWidget * editor = child.mTreeChild->FindCodeInEditors(proxy);
            if (editor != nullptr)
            {
                return editor;
            }
        }
    }
    return nullptr;
}

void CodeTextEditorTreeWidget::InternalPushChild(CodeTextEditorTreeWidget::Child& c, int i)
{
    ED_ASSERTSTR(mChildrenCount <= MAX_CHILDREN, "Memory stomp! do not call InternalPushChild if the tree is at its maximum capacity");
    int targetIndex = i + 1;
    if (i == -1)
    {
        targetIndex = mChildrenCount;
    }

    //forward delete
    for (int j = mChildrenCount - 1; j > targetIndex - 1; --j)
    {
        mChildren[j + 1] = mChildren[j];
    }

    mChildren[targetIndex] = c;
    
    insertWidget(targetIndex, mChildren[targetIndex].mWidget);

    mChildrenCount++;
}

bool CodeTextEditorTreeWidget::PushLeafChild(int i, CodeTextEditorWidget * allocation)
{
    ED_ASSERT(i < mChildrenCount);
    if (mChildrenCount < MAX_CHILDREN)
    {
        CodeTextEditorTreeWidget::Child child;
        child.mIsLeaf = true;
        child.mLeafChild = allocation;
        if (child.mLeafChild == nullptr)
        {
            child.mLeafChild = new CodeTextEditorWidget(this);
            mSignalCombo.Bind(child.mLeafChild);
        };
        InternalPushChild(child, i);
        return true;
    }

    return false;
}

void CodeTextEditorTreeWidget::DisplayCode(Pegasus::Core::ISourceCodeProxy * proxy, CodeTextEditorWidget * finalEditor)
{
    CodeTextEditorWidget * editor = FindCodeInEditors(proxy);
    if (editor != nullptr)
    {
        editor->setFocus();
    }
    else
    {
        bool foundCode = RecurseDisplayCode(proxy);
    
        if (!foundCode)
        {
            //no text editor in focus? no problem. Lets find the next available empty slot
            editor = FindCodeInEditors(nullptr);
            if (editor != nullptr)
            {
                editor->Initialize(proxy);
                editor->setFocus();
            }
            else if (finalEditor != nullptr)
            {
                finalEditor->Initialize(proxy);
                finalEditor->setFocus();
            }
        }
    }
}

void CodeTextEditorTreeWidget::HideCode(Pegasus::Core::ISourceCodeProxy * proxy)
{
    CodeTextEditorWidget * editor = FindCodeInEditors(proxy);
    if (editor != nullptr)
    {
        editor->Initialize(nullptr);
    }
}

void CodeTextEditorTreeWidget::CloseSplit()
{
    if (
        mChildrenCount > 1 ||
        (mChildrenCount == 1 && !mChildren[0].mIsLeaf)
       )
    {
        RecurseCloseSplit();
    }
}

void CodeTextEditorTreeWidget::HorizontalSplit()
{
    RecurseSplit(Qt::Horizontal);
}

void CodeTextEditorTreeWidget::VerticalSplit()
{
    RecurseSplit(Qt::Vertical);
}
