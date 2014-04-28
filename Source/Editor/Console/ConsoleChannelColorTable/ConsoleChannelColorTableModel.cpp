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

#include "Pegasus/Core/Shared/LogChannel.h"


static const int CHANNELNAME_COLUMN_INDEX = 0;
static const int COLOR_COLUMN_INDEX = 1;

//----------------------------------------------------------------------------------------

ConsoleChannelColorTableModel::ConsoleChannelColorTableModel(QObject * parent)
:   QAbstractTableModel(parent)
{

}

//----------------------------------------------------------------------------------------

ConsoleChannelColorTableModel::~ConsoleChannelColorTableModel()
{

}

//----------------------------------------------------------------------------------------

void ConsoleChannelColorTableModel::Refresh()
{
    QModelIndex firstIndex = createIndex(0, COLOR_COLUMN_INDEX);
    QModelIndex lastIndex  = createIndex(Pegasus::Core::NUM_LOG_CHANNELS - 1, COLOR_COLUMN_INDEX);
    emit(dataChanged(firstIndex, lastIndex));
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
        if (index.column() == CHANNELNAME_COLUMN_INDEX)
        {
            // Show the log channel name
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
            return Settings::ConvertLogChannelToString(logChannel);
        }
        else if (index.column() == COLOR_COLUMN_INDEX)
        {
            // Show only the background color, so no need for text
            return QVariant();
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == CHANNELNAME_COLUMN_INDEX)
        {
            // Set the check box for the filtering state of the log channel color from the settings
            const Settings * const settings = Editor::GetSettings();
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];

            return settings->GetConsoleFilterStateForLogChannel(logChannel) ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == COLOR_COLUMN_INDEX)
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
	if (index.isValid())
    {
        if ((index.column() == CHANNELNAME_COLUMN_INDEX) && (role == Qt::CheckStateRole))
        {
            // Set the log channel filtering state
            Settings * const settings = Editor::GetSettings();
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
            const bool isEnabled = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked);
            settings->SetConsoleFilterStateForLogChannel(logChannel, isEnabled);

            // Emit the dataChanged() signal, required by Qt
            emit(dataChanged(index, index));

    	    return true;
        }
        else if ((index.column() == COLOR_COLUMN_INDEX) && (role == Qt::EditRole))
        {
            // Set the log channel color (add it if it was using the default color)
            Settings * const settings = Editor::GetSettings();
            const Pegasus::Core::LogChannel logChannel = Pegasus::Core::sLogChannels[index.row()];
            settings->SetConsoleTextColorForLogChannel(logChannel, value.value<QColor>());

            // Emit the dataChanged() signal, required by Qt
            emit(dataChanged(index, index));

    	    return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------------------

Qt::ItemFlags ConsoleChannelColorTableModel::flags(const QModelIndex & index) const
{
	if (index.isValid())
    {
        if (index.column() == CHANNELNAME_COLUMN_INDEX)
        {
            // Channel name cell is non-editable, but it contains a checkbox
            return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
        }
        else if (index.column() == COLOR_COLUMN_INDEX)
        {
            // Color cells are editable
            return Qt::ItemIsEnabled | Qt::ItemIsEditable;
        }
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
        if (section == CHANNELNAME_COLUMN_INDEX)
        {
            return QString(tr("Log channel"));
        }
        else if (section == COLOR_COLUMN_INDEX)
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
