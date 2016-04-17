/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphGraphicsItemDefs.h
//! \author	Karolyn Boulanger
//! \date	11th April 2016
//! \brief	Global definitions for the graphics item properties

#ifndef EDITOR_GRAPHGRAPHICSITEMDEFS_H
#define EDITOR_GRAPHGRAPHICSITEMDEFS_H


//! Width of a node in pixels, without the input and output connectors
static const float GRAPHITEM_NODE_WIDTHF = 128.0f;

//! Height of a node header in pixels
static const float GRAPHITEM_NODE_HEADER_HEIGHTF = 24.0f;

//! Height of the top image for the node body in pixels
static const float GRAPHITEM_NODE_BODY_TOP_HEIGHTF = 16.0f;

//! Height of a node footer in pixels
static const float GRAPHITEM_NODE_FOOTER_HEIGHTF = 4.0f;

//----------------------------------------------------------------------------------------

//! Font size of the node title in the header
static const int GRAPHITEM_NODE_TITLE_POINT_SIZE = 10;

//! Width in pixels of the margin between the node header edges and the rectangle of the title text
static const float GRAPHITEM_NODE_TITLE_MARGIN_XF = 8.0f;

//! Height in pixels of the margin between the node header top edge and the rectangle of the title text
static const float GRAPHITEM_NODE_TITLE_MARGIN_TOPYF = 6.0f;

//! Height in pixels of the margin between the node header bottom edge and the rectangle of the title text
static const float GRAPHITEM_NODE_TITLE_MARGIN_BOTTOMYF = 2.0f;

//----------------------------------------------------------------------------------------

//! Half width in pixels of the node input connector
static const float GRAPHITEM_INPUT_HALF_WIDTHF = 8.0f;

//! Width in pixels of the node input connector
static const float GRAPHITEM_INPUT_WIDTHF = GRAPHITEM_INPUT_HALF_WIDTHF * 2.0f;

//! Height in pixels between two inputs
static const float GRAPHITEM_INPUT_ROW_HEIGHTF = 16.0f;

//! Half height in pixels between two inputs
static const float GRAPHITEM_INPUT_ROW_HALF_HEIGHTF = GRAPHITEM_INPUT_ROW_HEIGHTF * 0.5f;

//! Reference vertical position for the input and its label within the body,
//! as it would be without offset. The input center position is at (inputIndex + 0.5f) * GRAPHITEM_INPUT_ROW_HEIGHTF
static const float GRAPHITEM_INPUT_REFERENCE_YF = GRAPHITEM_NODE_HEADER_HEIGHTF + GRAPHITEM_INPUT_ROW_HALF_HEIGHTF;

//! Margin between the bottom of the header and the first input row,
//! and between the last input and the top of the footer
static const float GRAPHITEM_INPUT_ROW_MARGIN_YF = 4.0f;

//! Horizontal offset in pixel of the input connector graphics item compared to the left edge of the node
static const float GRAPHITEM_INPUT_OFFSET_XF = 4.0f;

//! Horizontal offset in pixel of the input label compared to the left edge of the node
static const float GRAPHITEM_INPUT_LABEL_OFFSET_XF = 16.0f;

//! Vertical offset in pixel of the input label compared to the center of the input row
static const float GRAPHITEM_INPUT_LABEL_OFFSET_YF = -6.0f;

//----------------------------------------------------------------------------------------

//! Half width in pixels of the node output connector
static const float GRAPHITEM_OUTPUT_HALF_WIDTHF = GRAPHITEM_INPUT_HALF_WIDTHF;

//! Width in pixels of the node output connector
static const float GRAPHITEM_OUTPUT_WIDTHF = GRAPHITEM_OUTPUT_HALF_WIDTHF * 2.0f;

//! Height in pixels between two outputs
static const float GRAPHITEM_OUTPUT_ROW_HEIGHTF = GRAPHITEM_INPUT_ROW_HEIGHTF;

//! Half height in pixels between two outputs
static const float GRAPHITEM_OUTPUT_ROW_HALF_HEIGHTF = GRAPHITEM_OUTPUT_ROW_HEIGHTF * 0.5f;

//! Reference vertical position for the output within the body, as it would be without offset
static const float GRAPHITEM_OUTPUT_REFERENCE_YF = GRAPHITEM_NODE_HEADER_HEIGHTF + GRAPHITEM_OUTPUT_ROW_HALF_HEIGHTF;

//! Margin between the bottom of the header and the first output row,
//! and between the last output and the top of the footer
static const float GRAPHITEM_OUTPUT_ROW_MARGIN_YF = 4.0f;

//! Horizontal offset in pixel of the output connector graphics item compared to the right edge of the node
static const float GRAPHITEM_OUTPUT_OFFSET_XF = -4.0f;

//----------------------------------------------------------------------------------------

//! Width in pixels of the splines for connectors
static const unsigned int GRAPHITEM_CONNECTOR_WIDTH = 4;



#endif // EDITOR_GRAPHGRAPHICSITEMDEFS_H
