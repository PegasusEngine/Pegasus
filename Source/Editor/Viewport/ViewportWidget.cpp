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

#include <QResizeEvent>


ViewportWidget::ViewportWidget(QWidget *parent)
    : QWidget(parent)
{
    //! \todo Remove that temporary background filling with black when we get an actual viewport
    //! \todo Or replace the black background with a dark gray one to indicate that an app is not running
    //! \todo Remove the background refresh completely (see N3D) when an application is open
    setAutoFillBackground(true);
    QPalette palette(palette());
    palette.setBrush(QPalette::Window, Qt::black);
    setPalette(palette);
}

//----------------------------------------------------------------------------------------

ViewportWidget::~ViewportWidget()
{

}

//----------------------------------------------------------------------------------------

unsigned int ViewportWidget::GetWindowHandle() const
{
    WId windowHandle = winId();
    //! \todo Do not use unsigned int, use a Pegasus type instead
    return *reinterpret_cast<unsigned int *>(&windowHandle);
}

//----------------------------------------------------------------------------------------

void ViewportWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);

    emit(ViewportResized(event->size().width(), event->size().height()));
}
