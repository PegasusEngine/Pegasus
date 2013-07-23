/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineSizes.h
//! \author	Kevin Boulanger
//! \date	21st July 2013
//! \brief	Size in pixels of the elements composing the timeline


//! Default length in pixels for one beat (for a zoom of 1.0f)
#define TIMELINE_BEAT_WIDTH                     64.0f

//! Height in pixels of a block (for a zoom of 1.0f)
#define TIMELINE_BLOCK_HEIGHT                   24.0f

//! Height in pixels of the space between blocks and the edge of a lane (for a zoom of 1.0f).
//! There is a margin on top and at the bottom of block within a lane.
#define TIMELINE_BLOCK_MARGIN_HEIGHT            3.0f

//! Total height in pixels of a lane (for a zoom of 1.0f)
#define TIMELINE_LANE_HEIGHT                    (TIMELINE_BLOCK_MARGIN_HEIGHT + TIMELINE_BLOCK_HEIGHT + TIMELINE_BLOCK_MARGIN_HEIGHT)

//! Width of a background measure line
#define TIMELINE_MEASURE_LINE_WIDTH             2.0f
