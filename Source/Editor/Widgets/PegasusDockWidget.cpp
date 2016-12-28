/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PegasusDockWidget.cpp
//! \author	Kleber Garcia
//! \date	July 20th, 2015
//! \brief	Master widget class with common functions required by dock widgets

#include "Widgets/PegasusDockWidget.h"
#include "Editor.h"
#include "Application/Application.h"
#include "Application/ApplicationManager.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"

#include <QFocusEvent>

PegasusDockWidget::PegasusDockWidget(QWidget* parent, Editor* editor)
: QDockWidget(parent), mEditor(editor)
{
}

void PegasusDockWidget::Initialize()
{
    setFocusPolicy(Qt::StrongFocus);
	setFeatures(  QDockWidget::DockWidgetClosable
				| QDockWidget::DockWidgetMovable
				| QDockWidget::DockWidgetFloatable);
    SetupUi();
    
    setWindowTitle(tr(GetTitle()));
    setObjectName(GetName());
}

PegasusDockWidget::~PegasusDockWidget()
{
}

void PegasusDockWidget::PerformFocus()
{
    emit(OnFocus(this));
}

void PegasusDockWidget::focusInEvent(QFocusEvent * event)
{
    QDockWidget::focusInEvent(event);
    emit(OnFocus(this));
}

void PegasusDockWidget::focusOutEvent(QFocusEvent * event)
{
    QDockWidget::focusOutEvent(event);
    emit(OutFocus(this));
}

void PegasusDockWidget::UpdateUIForAppClosed()
{
    OnUIForAppClosed();
}

void PegasusDockWidget::UpdateUIForAppLoaded()
{
    OnUIForAppLoaded(GetEditor()->GetApplicationManager().GetApplication()->GetApplicationProxy());
}

void PegasusDockWidget::SendAssetIoMessage(const AssetIOMCMessage& msg)
{
    emit(OnSendAssetIoMessage(this, msg));
}

void PegasusDockWidget::SendPropertyGridIoMessage(const PropertyGridIOMCMessage& msg)
{
    emit(OnSendPropertyGridIoMessage(msg));
}

void PegasusDockWidget::SendGraphIoMessage(const GraphIOMCMessage& msg)
{
    emit(OnSendGraphIoMessage(msg));
}
void PegasusDockWidget::SendTimelineIoMessage(const TimelineIOMCMessage& msg)
{
    emit(OnSendTimelineIoMessage(msg));
}

