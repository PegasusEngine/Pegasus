/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   ShaderTextEditorTreeWidget.cpp
//! \author	Kleber Garcia
//! \date	July 6th, 2014
//! \brief	Shader Editor Tree of views. Allows recursive subdivision of views

#include "ShaderLibrary/ShaderTextEditorTreeWidget.h"
#include "ShaderLibrary/ShaderTextEditorWidget.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QPushButton>


void ShaderTextEditorTreeWidget::SignalCombination::Initialize(QSignalMapper * textChanged, QSignalMapper * selected)
{
    mTextChangedMapper = textChanged;
    mSelectedSignalMapper = selected;
}

void ShaderTextEditorTreeWidget::SignalCombination::Bind(ShaderTextEditorWidget * editor)
{
    mTextChangedMapper->setMapping(editor, static_cast<QWidget*>(editor));
    connect ( editor, SIGNAL(textChanged()),
             mTextChangedMapper, SLOT(map()) );

    mSelectedSignalMapper->setMapping(editor, static_cast<QWidget*>(editor));
    connect (editor, SIGNAL(SignalSelected()),
            mSelectedSignalMapper, SLOT(map()));

}

void ShaderTextEditorTreeWidget::SignalCombination::Unbind(ShaderTextEditorWidget * editor)
{
    disconnect ( editor, SIGNAL(textChanged()),
             mTextChangedMapper, SLOT(map()) );
    mTextChangedMapper->removeMappings(editor);

    disconnect ( editor, SIGNAL(Selected()),
            mSelectedSignalMapper, SLOT(map()));
    mSelectedSignalMapper->removeMappings(editor);
}

ShaderTextEditorTreeWidget::ShaderTextEditorTreeWidget(
    ShaderTextEditorTreeWidget::SignalCombination& signalCombo,
    Qt::Orientation orientation, 
    QWidget * parent
)
: QSplitter(orientation, parent), mOrientation(orientation), mChildrenCount(0)
{
    mSignalCombo = signalCombo;
    PushLeafChild();
}

ShaderTextEditorTreeWidget::ShaderTextEditorTreeWidget(
    ShaderTextEditorTreeWidget::SignalCombination& signalCombo,
    QWidget * parent
)
: QSplitter(Qt::Vertical, parent), mOrientation(Qt::Vertical), mChildrenCount(0)
{
    mSignalCombo = signalCombo;
    PushLeafChild();
}

ShaderTextEditorTreeWidget::~ShaderTextEditorTreeWidget()
{
}

void ShaderTextEditorTreeWidget::ForceUpdateAllStyles()
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

void ShaderTextEditorTreeWidget::InternalSplit(int childIndex, Qt::Orientation orientation)
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
        ShaderTextEditorWidget * oldWidget = mChildren[childIndex].mLeafChild;

        //replace with a new tree
        mChildren[childIndex].mIsLeaf = false;
        mChildren[childIndex].mTreeChild = new ShaderTextEditorTreeWidget(mSignalCombo, orientation, this);
        mChildren[childIndex].mTreeChild->ForceUpdateAllStyles();

        insertWidget(childIndex, mChildren[childIndex].mTreeChild);

        //reinsert in the child
        mChildren[childIndex].mTreeChild->PushLeafChild(0, oldWidget);
    }
}

bool ShaderTextEditorTreeWidget::RecurseSplit(Qt::Orientation orientation)
{
    //do we have within our children a selected view?    
    for (int i = 0; i < mChildrenCount; ++i)
    {
        ShaderTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (child.mLeafChild->IsFocus())
            {
                ShaderTextEditorWidget * cachedLeaf = child.mLeafChild;
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

void ShaderTextEditorTreeWidget::RemoveChild(int i)
{
    ED_ASSERT(i < mChildrenCount && mChildrenCount > 0);
    for (; i < mChildrenCount - 1; ++i)
    {
        mChildren[i] = mChildren[i + 1];
    }
    --mChildrenCount;
}

bool ShaderTextEditorTreeWidget::RecurseCloseSplit()
{
    //find the child that is focused 
    for (int i = 0; i < mChildrenCount; ++i)
    {
        ShaderTextEditorTreeWidget::Child& child = mChildren[i];
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

bool ShaderTextEditorTreeWidget::RecurseDisplayShader(Pegasus::Shader::IShaderProxy * proxy)
{
    for (int i = 0; i < mChildrenCount; ++i)
    {
        ShaderTextEditorTreeWidget::Child& child = mChildren[i];
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
            if (child.mTreeChild->RecurseDisplayShader(proxy))
            {
                return true;
            }
        }
    }
    return false;
}

ShaderTextEditorWidget * ShaderTextEditorTreeWidget::FindShadersInEditors(Pegasus::Shader::IShaderProxy * proxy)
{
    for (int i = 0; i < mChildrenCount; ++i)
    {
        ShaderTextEditorTreeWidget::Child& child = mChildren[i];
        if (child.mIsLeaf)
        {
            if (
                (proxy == nullptr && child.mLeafChild->GetShader() == nullptr) || //next available editor slot is empty?
                (proxy != nullptr && child.mLeafChild->GetShader() != nullptr && child.mLeafChild->GetShader()->GetGuid() == proxy->GetGuid())
               )
            {
                return child.mLeafChild;
            }
        }
        else
        {
            ShaderTextEditorWidget * editor = child.mTreeChild->FindShadersInEditors(proxy);
            if (editor != nullptr)
            {
                return editor;
            }
        }
    }
    return nullptr;
}

void ShaderTextEditorTreeWidget::InternalPushChild(ShaderTextEditorTreeWidget::Child& c, int i)
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

bool ShaderTextEditorTreeWidget::PushLeafChild(int i, ShaderTextEditorWidget * allocation)
{
    ED_ASSERT(i < mChildrenCount);
    if (mChildrenCount < MAX_CHILDREN)
    {
        ShaderTextEditorTreeWidget::Child child;
        child.mIsLeaf = true;
        child.mLeafChild = allocation;
        if (child.mLeafChild == nullptr)
        {
            child.mLeafChild = new ShaderTextEditorWidget(this);
            mSignalCombo.Bind(child.mLeafChild);
        };
        InternalPushChild(child, i);
        return true;
    }

    return false;
}

void ShaderTextEditorTreeWidget::DisplayShader(Pegasus::Shader::IShaderProxy * proxy, ShaderTextEditorWidget * finalEditor)
{
    ShaderTextEditorWidget * editor = FindShadersInEditors(proxy);
    if (editor != nullptr)
    {
        editor->setFocus();
    }
    else
    {
        bool foundShader = RecurseDisplayShader(proxy);
    
        if (!foundShader)
        {
            //no text editor in focus? no problem. Lets find the next available empty slot
            editor = FindShadersInEditors(nullptr);
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

void ShaderTextEditorTreeWidget::HideShader(Pegasus::Shader::IShaderProxy * proxy)
{
    ShaderTextEditorWidget * editor = FindShadersInEditors(proxy);
    if (editor != nullptr)
    {
        editor->Initialize(nullptr);
    }
}

void ShaderTextEditorTreeWidget::CloseSplit()
{
    if (
        mChildrenCount > 1 ||
        (mChildrenCount == 1 && !mChildren[0].mIsLeaf)
       )
    {
        RecurseCloseSplit();
    }
}

void ShaderTextEditorTreeWidget::HorizontalSplit()
{
    RecurseSplit(Qt::Horizontal);
}

void ShaderTextEditorTreeWidget::VerticalSplit()
{
    RecurseSplit(Qt::Vertical);
}
