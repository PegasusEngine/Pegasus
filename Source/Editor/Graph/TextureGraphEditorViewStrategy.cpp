/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TextureGraphEditorViewStrategy.cpp
//! \author	Kleber Garcia
//! \date	24th May of 2016
//! \brief	Texture graph editor specific code.

#include "Graph/TextureGraphEditorViewStrategy.h"
#include "Pegasus/PegasusAssetTypes.h"

const char* TextureGraphEditorViewStrategy::GetWidgetTitle() const
{
    return "Texture Graph Editor";
}

const char* TextureGraphEditorViewStrategy::GetWidgetName()  const
{
    return "TextureGraphEditor";
}

Pegasus::App::ComponentTypeFlags TextureGraphEditorViewStrategy::GetViewportComponents() const
{
    return Pegasus::App::COMPONENT_FLAG_TEXTURE_VIEW;    
}

const Pegasus::PegasusAssetTypeDesc** TextureGraphEditorViewStrategy::GetTargetAssetTypes() const
{
    static const Pegasus::PegasusAssetTypeDesc* gAssets[] = {
        &Pegasus::ASSET_TYPE_TEXTURE,
        nullptr
    };

    return gAssets;
}
