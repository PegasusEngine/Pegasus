/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportType.h
//! \author	Kevin Boulanger
//! \date	28th November 2013
//! \brief	Declaration of the viewport type enumerant

#ifndef EDITOR_VIEWPORTTYPE_H
#define EDITOR_VIEWPORTTYPE_H


//! Viewport type declaration
//! \warning Can be used only once per viewport
enum ViewportType
{
    VIEWPORTTYPE_FIRST = 0,

    VIEWPORTTYPE_MAIN = VIEWPORTTYPE_FIRST,     // Main demo window
    VIEWPORTTYPE_SECONDARY,                     // Optional secondary demo window
    VIEWPORTTYPE_TEXTURE_EDITOR_PREVIEW,        // Preview window of the texture editor
    VIEWPORTTYPE_MESH_EDITOR_PREVIEW,           // Preview window of the mesh editor

    NUM_VIEWPORT_TYPES
};


#endif  // EDITOR_VIEWPORTTYPE_H
