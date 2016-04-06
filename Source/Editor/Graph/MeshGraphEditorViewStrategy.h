/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshGraphEditorViewStrategy.h
//! \author	Kleber Garcia
//! \date	24th May of 2016
//! \brief	Mesh graph editor specific code.

#ifndef PEGASUS_EDITOR_MESH_EDITOR_VIEW_STRATEGY_H
#define PEGASUS_EDITOR_MESH_EDITOR_VIEW_STRATEGY_H

#include "Graph/GraphEditorDockWidget.h"

//! Interface that defines specific visualization patterns
//! for a graph editor widget.
class MeshGraphEditorViewStrategy : public IGraphEditorViewStrategy
{
public:
    MeshGraphEditorViewStrategy(){}
    virtual ~MeshGraphEditorViewStrategy(){}

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

#endif //PEGASUS_EDITOR_MESH_EDITOR_VIEW_STRATEGY_H
