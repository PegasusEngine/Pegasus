/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ColorPickerBox.cpp
//! \author	Karolyn Boulanger
//! \date	04th August 2013
//! \brief	Picker box to choose a color (shows the standard color picker when clicking)

#include "Widgets/ColorPickerBox.h"
#include <QColorDialog>


ColorPickerBox::ColorPickerBox(const QColor & color, QWidget * parent)
:   QFrame(parent),
    mColor(color)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setAutoFillBackground(true);
    SetBackgroundColor(color);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(this, SIGNAL(ColorChanged(const QColor &)),
            this, SLOT(SetBackgroundColor(const QColor &)));
}

//----------------------------------------------------------------------------------------

ColorPickerBox::~ColorPickerBox()
{
}

//----------------------------------------------------------------------------------------

void ColorPickerBox::SetColor(const QColor & color)
{
    mColor = color;
    emit ColorChanged(mColor);
}

//----------------------------------------------------------------------------------------

QSize ColorPickerBox::sizeHint() const
{
    return QSize(28, 21);
}

//----------------------------------------------------------------------------------------

void ColorPickerBox::mousePressEvent(QMouseEvent * e)
{
    OpenColorPicker();
}

//----------------------------------------------------------------------------------------

void ColorPickerBox::OpenColorPicker()
{
    QColor newColor = QColorDialog::getColor(mColor, this);
    if (newColor.isValid())
    {
        SetColor(newColor);
    }
}

//----------------------------------------------------------------------------------------
	
void ColorPickerBox::SetBackgroundColor(const QColor & color)
{
    setStyleSheet("ColorPickerBox { background: " + color.name() + "; }");
}
