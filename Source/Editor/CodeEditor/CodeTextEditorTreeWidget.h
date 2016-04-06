/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file   CodeTextEditorTreeWidget.h
//! \author	Kleber Garcia
//! \date	July 6th, 2014
//! \brief	Code Editor Tree of views. Allows recursive subdivision of views

#ifndef EDITOR_CODETEXTEDITORTREEWIDGET_H
#define EDITOR_CODETEXTEDITORTREEWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QSignalMapper>


class CodeTextEditorWidget;
class QLayout;
struct SourceState;

//! the class represents a view that can be splitted recursively
class CodeTextEditorTreeWidget : public QSplitter
{
    Q_OBJECT;

public:

    //! subclass that contains the signal mappers for event dispatching of the leafs of this tree
    class SignalCombination
    {
    public:
        SignalCombination()
        : mTextChangedMapper(nullptr), mSelectedSignalMapper(nullptr)
        {
        }

        //! initializer function
        //! \param textChanged the signal mapper that triggers text change signal from leaf
        //! \param selected the signal mapper that triggers when the user focuses and clicks on a leaf
        void Initialize(QSignalMapper * textChanged, QSignalMapper * selected);

        //! Binds the signals and slots to the mappers
        //! \param source code text editor widget target (leaf)
        void Bind(CodeTextEditorWidget * editor);

        //! Unbinds the signals and slots to the mappers
        //! \param source code text editor widget target (leaf)
        void Unbind(CodeTextEditorWidget * editor);

    private:
        QSignalMapper * mTextChangedMapper;
        QSignalMapper * mSelectedSignalMapper;
    };

    //! Constructor
    CodeTextEditorTreeWidget(
        SignalCombination& signalCombo, 
        Qt::Orientation orientation, 
        QWidget * parent
    );

    //! Constructor
    CodeTextEditorTreeWidget(
        SignalCombination& signalCombo,
        QWidget * parent
    );

    //! Destructor
    ~CodeTextEditorTreeWidget();

    //! \return true if any child has focus, false otherwise
    bool HasAnyChildFocus() const;

    //! Displays a source code in the current text view
    //! \param source code the source code node
    void DisplayCode(SourceState * code);

    //! Finds the last leaf editor in the entire tree.
    CodeTextEditorWidget* FindLastLeafEditor();

    //! Takes the source code out of the tree view
    //! \param source code target source code to show
    void HideCode(SourceState * code);

    //! searches for the leaf that contains the current source code 
    //! \param proxy the source code to find
    //! \return the editor containign the source code, null if there isn't
    CodeTextEditorWidget * FindCodeInEditors(SourceState * code);

     

signals:
    //! triggers when the text has changed on the CodeTextEditor
    //! \param sender the text editor leaf that changed
    void OnTextChanged(CodeTextEditorWidget * sender);
    
public slots:
    //! splits the current selected view in half recursively (horizontally)
    void HorizontalSplit();

    //! splits the current selected view in half recursively (vertically)
    void VerticalSplit();

    //! removes and merges the current view selected
    void CloseSplit();

    //! updates and forces a syntax highlight on all child views
    void ForceUpdateAllStyles();

private:

    //! removes a child from the child list
    //! \param i the child index
    void RemoveChild(int i);

    //! gets the orientation of this tree 
    //! \return gets the orientation
    Qt::Orientation GetOrientation() const { return mOrientation; }

    //! pushes (or creates) a child
    //! \param i the place to push the child into. -1 if we append it to the end
    //! \allocation if we want to allocate the child leaf outside or internally (by passing nullptr)
    //! \return true if success, false otherwise
    bool PushLeafChild(int i = -1, CodeTextEditorWidget * allocation = nullptr);

    //! splits recursively the current tree view
    //! \param orientation the orientation
    bool RecurseSplit(Qt::Orientation orientation);
    
    //! \return true if success on splitting (a focused view was split)
    bool RecurseCloseSplit();

    //! \param proxy the source code to display
    //! \return true if success (a focused view grabbed it) false otherwise
    bool RecurseDisplayCode(SourceState * code);

    //! signal set for events in the leafs
    SignalCombination mSignalCombo;

    //! up to 6 splits per tree view
    static const int MAX_CHILDREN = 6;

    //! child structure
    struct Child
    {
    public:
        bool mIsLeaf;

        union {
            CodeTextEditorTreeWidget * mTreeChild;
            CodeTextEditorWidget     * mLeafChild;
            QWidget                    * mWidget;
        };
        
        Child()
        {
            mIsLeaf = false;
            mTreeChild = nullptr;
        };
        
    } mChildren[MAX_CHILDREN + 1]; //the extra space is to allow temporary insertion of 1 element when reshaping the tree

    //! number of children
    int mChildrenCount;

    //! \param c the child to push
    //! \param i the index (position) to insert it
    void InternalPushChild(Child& c, int i);

    //! \param childIndex the child to internally split
    //! \param the orientation
    void InternalSplit(int childIndex, Qt::Orientation orientation);

    //! orientation we point to
    Qt::Orientation mOrientation;
};


#endif
