/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ColorPickerBox.h
//! \author	Kevin Boulanger
//! \date	04th August 2013
//! \brief	Picker box to choose a color (shows the standard color picker when clicking)

#ifndef EDITOR_COLORPICKERBOX_H
#define EDITOR_COLORPICKERBOX_H

#include <QFrame>


class ColorPickerBox : public QFrame
{
    Q_OBJECT

public:
    
    //! Constructor
	//! \param color Initial color of the color box
	//! \param parent Parent of the widget
	ColorPickerBox(const QColor & color = Qt::black,
				   QWidget * parent = 0);

	//! Destructor
	virtual ~ColorPickerBox();

	//! Get the current color
	//! \return Current color (in Qt format)
	inline const QColor & GetColor() const { return mColor; }

	//------------------------------------------------------------------------------------

public slots:
		
	//! Set the current color
	//! \param color New color
	void SetColor(const QColor & color);

	//------------------------------------------------------------------------------------

signals:

	//! Emitted when the color has changed
	//! \param color New color
	void ColorChanged(const QColor & color);

	//------------------------------------------------------------------------------------

public:

	//! Preferred size of the widget
	//! \return Preferred size of the widget
	QSize sizeHint() const;

	//! Called when a mouse button is pressed
	//! \param e Parameters of the event
	void mousePressEvent(QMouseEvent * e);

	//------------------------------------------------------------------------------------

private slots:

	//! Open the color picker and update the current color
	void OpenColorPicker();

	//! Set the background color of the color box
	//! \param New color
	void SetBackgroundColor(const QColor & color);

	//------------------------------------------------------------------------------------

private:

	//! Current color
	QColor mColor;
};

#endif  // EDITOR_COLORPICKERBOX_H
