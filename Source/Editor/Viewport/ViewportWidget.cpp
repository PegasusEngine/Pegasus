/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.cpp
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#include "Viewport/ViewportWidget.h"


ViewportWidget::ViewportWidget(QWidget *parent)
    : QWidget(parent)
{
    //! \todo Remove that temporary background filling with black when we get an actual viewport
    setAutoFillBackground(true);
    QPalette palette(palette());
    palette.setBrush(QPalette::Window, Qt::black);
    setPalette(palette);
}

//----------------------------------------------------------------------------------------

ViewportWidget::~ViewportWidget()
{

}
