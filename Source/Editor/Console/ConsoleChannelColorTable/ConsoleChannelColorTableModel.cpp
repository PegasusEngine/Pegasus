/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	ConsoleChannelColorTableModel.cpp
//! \author	Kevin Boulanger
//! \date	06th August 2013
//! \brief	Table model to show the colors associated with the log channels

#include "Console/ConsoleChannelColorTable/ConsoleChannelColorTableModel.h"
#include "Settings/Settings.h"

#include "Pegasus/Core/Log.h"


ConsoleChannelColorTableModel::ConsoleChannelColorTableModel(QObject * parent)
:   QAbstractTableModel(parent)
{

}

//----------------------------------------------------------------------------------------

ConsoleChannelColorTableModel::~ConsoleChannelColorTableModel()
{

}

//----------------------------------------------------------------------------------------

QVariant ConsoleChannelColorTableModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            // Show the log channel name
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
            return Settings::ConvertLogChannelToString(logChannel);
        }
        else if (index.column() == 1)
        {
            // Show only the background color, so no need for text
            return QVariant();
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == 1)
        {
            // Set the background color to the log channel color in the settings
            const Settings * const settings = Editor::GetSettings();
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];

            return QBrush(settings->GetConsoleTextColorForLogChannel(logChannel));
        }
    }

    return QVariant();
}

//----------------------------------------------------------------------------------------

bool ConsoleChannelColorTableModel::setData(const QModelIndex & index,
											const QVariant & value,
                                            int role)
{
	if (index.isValid() && (index.column() == 1) && (role == Qt::EditRole))
    {
        // Set the log channel color (add it if it was using the default color)
        Settings * const settings = Editor::GetSettings();
        const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
        settings->SetConsoleTextColorForLogChannel(logChannel, value.value<QColor>());

        // Emit the dataChanged() signal, required by Qt
        emit(dataChanged(index, index));

    	return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------

Qt::ItemFlags ConsoleChannelColorTableModel::flags(const QModelIndex & index) const
{
	if (index.isValid() && (index.column() == 1))
    {
        // Color cells are editable
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled;
}

//----------------------------------------------------------------------------------------

QVariant ConsoleChannelColorTableModel::headerData(int section,
												   Qt::Orientation orientation,
												   int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        if (section == 0)
        {
            return QString(tr("Log channel"));
        }
        else if (section == 1)
        {
            return QString(tr("Color"));
        }
    }

    return QVariant();
}

//----------------------------------------------------------------------------------------

int ConsoleChannelColorTableModel::rowCount(const QModelIndex & parent) const
{
    return Pegasus::Core::NUM_LOG_CHANNELS;
}

//----------------------------------------------------------------------------------------

int ConsoleChannelColorTableModel::columnCount(const QModelIndex & parent) const
{
	return 2;
}
