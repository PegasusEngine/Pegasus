/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ViewportWidget.h
//! \author	Kevin Boulanger
//! \date	02nd June 2013
//! \brief	Widget containing one application viewport

#ifndef EDITOR_VIEWPORTWIDGET_H
#define EDITOR_VIEWPORTWIDGET_H

#include <QWidget>

class QResizeEvent;


//! Widget containing one application viewport
class ViewportWidget : public QWidget
{
    Q_OBJECT

public:

    ViewportWidget(QWidget *parent);
    virtual ~ViewportWidget();

    //! Get the platform-independent window handle of the widget
    //! \todo Do not use unsigned int, use a Pegasus type instead
    //! \return Platform-independent window handle of the widget
    unsigned int GetWindowHandle() const;

    //! Get the width of the viewport
    //! \return Current width of the viewport in pixels
    inline int GetWidth() const { return size().width(); }

    //! Get the height of the viewport
    //! \return Current height of the viewport in pixels
    inline int GetHeight() const { return size().height(); }

signals:

    //! Signal emitted when the viewport is resized, used to update the application-side viewport
    //! \param width New width of the viewport, in pixels
    //! \param height New height of the viewport, in pixels
    void ViewportResized(int width, int height);

protected:

    //! Called when the viewport is resized, used to update the application-side viewport
    //! \param event Event object containing the new size of the viewport
    void resizeEvent(QResizeEvent * event);
};


#endif  // EDITOR_VIEWPORTWIDGET_H
