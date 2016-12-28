/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineIOMessageController.h
//! \author	Kleber Garcia
//! \date	21st Februarty 2016
//! \brief	Timeline Message controller

#ifndef EDITOR_TIMELINE_IO_MSG_CONTROLLER
#define EDITOR_TIMELINE_IO_MSG_CONTROLLER

#include <QObject>
#include <QVariant>
#include "MessageControllers/MsgDefines.h"
#include "MessageControllers/AssetIOMessageController.h"

//! Forward declarations
namespace Pegasus
{
    namespace App
    {
        class IApplicationProxy;
    }

    namespace AssetLib
    {
        class IRuntimeAssetObjectProxy;
    }

    namespace Timeline
    {
        class ITimelineProxy;
        class ILaneProxy;
        class IBlockProxy;
    }

    struct PegasusAssetTypeDesc;
}

class TimelineIOMessageController : public QObject, public IAssetTranslator
{

    Q_OBJECT;
public:

    //! Constructor
    explicit TimelineIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~TimelineIOMessageController();

    //! \param handle - handle to used for this object. Use this to set the user data,
    //!                 so the event listeners can send messages with the handle to the ui.
    //! \param object input asset to open
    //! \return a qvariant that contains the representation of this asset for the ui to read.
    virtual QVariant TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! \return null terminated list that contains all the necessary asset data so assets can be translated.
    virtual const Pegasus::PegasusAssetTypeDesc** GetTypeList() const;

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(const TimelineIOMCMessage& msg);

signals:
    void NotifyMasterScriptState(bool isScriptLoaded, QString path);

    void NotifyRepaintTimeline();

private:
    Pegasus::Timeline::ITimelineProxy* ResolveTimeline(const AssetInstanceHandle& assetHandle);

    void OnSetBlockscript(const QString& str, unsigned blockGuid);

    void OnClearBlockscript(unsigned blockGuid);

    void OnSetMasterBlockscript(const QString& str);

    void OnClearMasterBlockscript();

    void OnSetParameter(TimelineIOMCTarget targetObject, const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned parameterName, const QVariant& paramValue, TimelineIOMessageObserver* observer);

    void OnBlockOp(const AssetInstanceHandle& timelineHandle, TimelineIOMCBlockOp blockOp, unsigned blockGuid, unsigned targetLaneId, const QVariant& arg, unsigned mouseClickId, TimelineIOMessageObserver* observer );

    Pegasus::App::IApplicationProxy* mApp;

};

//Safe callback mechanism from Render -> to UI.
class TimelineIOMessageObserver : public QObject
{
    Q_OBJECT;
public:
    TimelineIOMessageObserver();
    virtual ~TimelineIOMessageObserver() {}

    virtual void OnParameterUpdated(const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, const QVariant& parameterValue) = 0;

    virtual void OnBlockOpResponse(const TimelineIOMCBlockOpResponse& response) = 0;

signals:
    void SignalParameterUpdated(AssetInstanceHandle timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, QVariant parameterValue);

    void SignalBlockOpResponse(TimelineIOMCBlockOpResponse blockOpResponse);

private slots:
    void SlotParameterUpdated(AssetInstanceHandle timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, QVariant parameterValue);

    void SlotBlockOpResponse(TimelineIOMCBlockOpResponse blockOpResponse);
};

#endif
