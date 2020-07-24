/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus                                           */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusAssetTypes.inl
//! \author	Kleber Garcia
//! \date	30th July 2015
//! \brief	high level description of all the assets this app can have.
//!         \note Edit this file to add new asset bindings with the runtime.

                               ////////////////////////////////////////////////
                               //                 , .-.-,_,                  //
                               //                 )`-.>'` (                  //
                               //                /     `\  |                 //
                               //                |       | |                 //
                               //                 \     / /                  //
                               //                 `=(\ /.=`                  //
                               //                  `-;`.-'                   //
                               //                    `)|     ,               //
                               //                     ||  .-'|               //
                               //                   ,_||  \_,/               //
                               //             ,      \|| .'                  //
                               //             |\|\  , ||/                    //
                               //            ,_\` |/| |Y_,                   //
                               //             '-.'-._\||/                    //
                               //                >_.-`Y|                     //
                               //                ` .-"||"-.                  //
                               //                  \'----'/                  //
                               //                   |:.  |                   //
                               //                   |::. |                   //
                               //                  /::::  \                  //
                               //                .:::'     '.                //
                               //               /:::         \               //
                               //              ;:::'          ;              //
                               //              |:::           |              //
                               //              |:::           |              //
                               //              |:::           |              //
                               //              ;:::           ;              //
                               //              \:::.          /              //
                               //               ':::..      .'               //
                               //                `""-----""`                 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////--------------------------------------Asset Registration Table----------------------------------------------//////
/////----------------------------------------(a table... get it??)-----------------------------------------------//////
//-------------------------------------------------------------------------------------------------------------------//
// Runtime Name  - Use this to access a struct directly in the runtime. For example, ASSET_TYPE_H_SHADER.mTypeGuid   //
//                 will give you the type guid registered here.                                                      //
// Type GUID     - Must be a unique identifier for any asset. The runtime uses this internally to identify           //
//                 assets quickly without having to do string compares.                                              //
// Type Name     - Humanly readable type name. Used to debug json files, useless for non structured assets.          //
// Is Structured - if true, the assets are json files. If false the assets are raw files                             //
// NOTE: DO NOT REGISTER TWO OR MORE ASSET TYPES THAT HAVE THE SAME EXTENSION AND DIFFERENT Is Structured value.     //
//            Pegasus will check for structureness the first asset type it finds.                                    //  
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////                           Runtime Name         |  TypeGuid |   TypeName          | mExtension | Is Structured | Icon path////
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_SHADER_SRC   ,  'HLSL'   ,  "HlslSource"       ,    "hlsl"  ,    false     , ":TypeIcons/shader.png") /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_PROGRAM      ,  'PRAS'   ,  "Program"          ,    "pas"   ,    true      , ":TypeIcons/program.png") /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_TEXTURE      ,  'TEAS'   ,  "Texture"          ,    "pas"   ,    true      , ":TypeIcons/texture.png") /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_MESH         ,  'MEAS'   ,  "Mesh"             ,    "pas"   ,    true      , ":TypeIcons/mesh.png") /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_TIMELINE     ,  'TIAS'   ,  "Timeline"         ,    "pas"   ,    true      , ":TypeIcons/timeline.png") /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_BLOCKSCRIPT  ,  'BLAS'   ,  "Blockscript"      ,    "bs"    ,    false     , ":TypeIcons/timelinescript.png")  /**/
/**/   REGISTER_ASSET_TYPE( ASSET_TYPE_H_BLOCKSCRIPT,  'BHAS'   ,  "BlockscriptHeader",    "bsh"   ,    false     , ":TypeIcons/timelinescript.png")  /**/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////-------------------------------------Asset Registration Table End------------------------------------------//////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                               
