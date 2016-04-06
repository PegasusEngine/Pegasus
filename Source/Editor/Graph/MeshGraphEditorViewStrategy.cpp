/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	MeshGraphEditorViewStrategy.cpp
//! \author	Kleber Garcia
//! \date	24th May of 2016
//! \brief	Mesh graph editor specific code.

#include "Graph/MeshGraphEditorViewStrategy.h"
#include "Pegasus/PegasusAssetTypes.h"

const char* MeshGraphEditorViewStrategy::GetWidgetTitle() const
{
    return "Mesh Graph Editor";
}

const char* MeshGraphEditorViewStrategy::GetWidgetName()  const
{
    return "MeshGraphEditor";
}

const Pegasus::PegasusAssetTypeDesc** MeshGraphEditorViewStrategy::GetTargetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gAssets[] = {
        &Pegasus::ASSET_TYPE_MESH,
        nullptr
    };

    return gAssets;
}

Pegasus::App::ComponentTypeFlags MeshGraphEditorViewStrategy::GetViewportComponents() const
{
    return Pegasus::App::COMPONENT_FLAG_WORLD;    
}
