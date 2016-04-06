/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorViewStrategy.h
//! \author	Kleber Garcia
//! \date	24th May of 2016
//! \brief	Texture graph editor specific code.

#ifndef PEGASUS_EDITOR_TEXTURE_EDITOR_VIEW_STRATEGY_H
#define PEGASUS_EDITOR_TEXTURE_EDITOR_VIEW_STRATEGY_H

#include "Graph/GraphEditorDockWidget.h"

//! Interface that defines specific visualization patterns
//! for a graph editor widget.
class TextureGraphEditorViewStrategy : public IGraphEditorViewStrategy
{
public:
    TextureGraphEditorViewStrategy(){}
    virtual ~TextureGraphEditorViewStrategy(){}

    //! Returns the title of this widget
    virtual const char* GetWidgetTitle() const;

    //! Returns the name this widget
    virtual const char* GetWidgetName()  const;

    //! Switch that holds every pegasus asset type that this dock widget can open for edit.
    //! Asset types that get this type association, will be the ones passed through OnOpenRequest function 
    virtual const Pegasus::PegasusAssetTypeDesc** GetTargetAssetTypes() const;

    //! Returns the components used for the viewport on this view strategy
    virtual Pegasus::App::ComponentTypeFlags GetViewportComponents() const;
};

#endif //PEGASUS_EDITOR_TEXTURE_EDITOR_VIEW_STRATEGY_H
