/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.cpp
//! \author	Karolyn Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#include "Viewport/ViewportWidget.h"

#include <QResizeEvent>


ViewportWidget::ViewportWidget(ViewportType viewportType, QWidget * parent)
:   QWidget(parent)
{
    if (viewportType >= NUM_VIEWPORT_TYPES)
    {
        ED_FAILSTR("Invalid viewport type (%d), it should be < %d. Switching to MAIN type", viewportType, NUM_VIEWPORT_TYPES);
        mViewportType = VIEWPORTTYPE_MAIN;
    }
    else
    {
        mViewportType = viewportType;
    }

    // Set the size policy of the widget
    QSizePolicy newSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    newSizePolicy.setHorizontalStretch(0);
    newSizePolicy.setVerticalStretch(0);
    newSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
    setSizePolicy(newSizePolicy);
    setMinimumSize(QSize(128, 128));

    //! \todo Remove that temporary background filling with black when we get an actual viewport
    //! \todo Or replace the black background with a dark gray one to indicate that an app is not running
    //! \todo Remove the background refresh completely (see N3D) when an application is open
    setAutoFillBackground(true);
    QPalette palette(palette());
    palette.setBrush(QPalette::Window, Qt::black);
    setPalette(palette);

    //! Get the native window ID
    //! \todo Do not use unsigned int, use a Pegasus type instead
    WId windowHandle = winId();
    mWindowHandle = *reinterpret_cast<unsigned int *>(&windowHandle);
}

//----------------------------------------------------------------------------------------

ViewportWidget::~ViewportWidget()
{
}

//----------------------------------------------------------------------------------------

void ViewportWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);

    emit(ViewportResized(mViewportType, event->size().width(), event->size().height()));
}
