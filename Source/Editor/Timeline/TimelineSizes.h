/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineSizes.h
//! \author	Karolyn Boulanger
//! \date	21st July 2013
//! \brief	Size in pixels of the elements composing the timeline


//! Default length in pixels for one beat (for a zoom of 1.0f)
#define TIMELINE_BEAT_WIDTH                     32.0f

//! Height in pixels of a block (for a zoom of 1.0f)
#define TIMELINE_BLOCK_HEIGHT                   16.0f

//! Height in pixels of the space between blocks and the edge of a lane (for a zoom of 1.0f).
//! There is a margin on top and at the bottom of block within a lane.
#define TIMELINE_BLOCK_MARGIN_HEIGHT            1.0f

//! Height of the timeline block font in pixels
#define TIMELINE_BLOCK_FONT_HEIGHT              10

//! Total height in pixels of a lane (for a zoom of 1.0f)
#define TIMELINE_LANE_HEIGHT                    (TIMELINE_BLOCK_MARGIN_HEIGHT + TIMELINE_BLOCK_HEIGHT + TIMELINE_BLOCK_MARGIN_HEIGHT)

//! Width of a background measure line
#define TIMELINE_MEASURE_LINE_WIDTH             2.0f

//! Height of the beat number font in pixels
#define TIMELINE_BEAT_NUMBER_FONT_HEIGHT        10

//! Width of the beat number block in pixels
#define TIMELINE_BEAT_NUMBER_BLOCK_WIDTH        20.0f

//! Height of the beat number block in pixels (>= TIMELINE_BEAT_NUMBER_FONT_HEIGHT)
#define TIMELINE_BEAT_NUMBER_BLOCK_HEIGHT       14.0f

//! Width in pixels of a lane header (for a zoom of 1.0f)
#define TIMELINE_LANE_HEADER_WIDTH              96.0f

//! Height of the lane header font in pixels
#define TIMELINE_LANE_HEADER_FONT_HEIGHT        10

//! Width of the cursor
#define TIMELINE_CURSOR_LINE_WIDTH              2.0f
