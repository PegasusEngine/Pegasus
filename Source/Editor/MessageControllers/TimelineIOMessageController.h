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

// Shadow state objects: these objects make it easy to create internal representations
// of an entire class by utilizing qvariants as the final way of transport.
// These are the ways in which timeline expresses a copy of its state back to the UI.
class ShadowBlockState
{
public:
    enum PropName
    {
        PROP_BLOCK_NAME,
        PROP_CLASS_NAME,
        PROP_BLOCK_COLOR,
        PROP_BLOCK_BEAT,
        PROP_BLOCK_GUID,
        PROP_BLOCK_DURATION,
        PROP_BLOCK_SCRIPT_PATH
    };

    ShadowBlockState();

    QString GetName() const;
    QString GetClassName() const;
    unsigned GetColor() const;
    unsigned GetGuid() const;
    unsigned GetBeat() const;
    unsigned GetDuration() const;
    bool HasBlockScript() const;
    QString GetBlockScriptPath() const;

    QVariantMap& GetRootState() { return mRootState; }
    void Build(const Pegasus::Timeline::IBlockProxy* p);
    void SetState(const QVariantMap& state) { mRootState = state; }
    static QString Str(PropName nm);
    //No need to flush properties for this one.

    void SetScript(const QString& script);
    void RemoveScript();

private:
    QVariantMap mRootState;
};

class ShadowLaneState
{
public:
    enum PropName
    {
        PROP_BLOCK_COUNT,
        PROP_BLOCK_LIST,
        PROP_LANE_NAME
    };

    ShadowLaneState();

    unsigned GetBlockCount() const;
    QString GetName() const;

    QVariantMap& GetRootState() { return mRootState; }
    const QVariantMap& GetRootState() const { return mRootState; }
    void Build(const Pegasus::Timeline::ILaneProxy* p);
    void SetState(const QVariantMap& state) { mRootState = state; }    
    static QString Str(PropName nm);
    static void FlushProp(PropName prop, const QVariant& val, Pegasus::Timeline::ILaneProxy* dest);

private:
    QVariantMap mRootState;
};

class ShadowTimelineState
{
public:
    enum PropName
    {
        PROP_CURR_BEAT,
        PROP_BEATS,
        PROP_BEATS_PER_MIN,
        PROP_TICKS_PER_BEAT,
        PROP_SCRIPT_PATH,
        PROP_LANE_COUNT,
        PROP_LANES,
        PROP_COUNT
    };
    ShadowTimelineState();

    unsigned GetLaneCount() const;
    unsigned GetNumBeats() const;
    unsigned GetNumTicksPerBeat() const;
    float GetBeatsPerMinute() const;
    float GetCurrBeat() const;

    QString GetMasterScriptPath() const;
    bool HasMasterScript() const;

    QVariantMap& GetRootState() { return mRootState; }
    void Build(const Pegasus::Timeline::ITimelineProxy* p);
    void SetState(const QVariantMap& state) { mRootState = state; }
    static QString Str(PropName nm);
    static void FlushProp(PropName prop, const QVariant& val, Pegasus::Timeline::ITimelineProxy* dest);


private:
    QVariantMap mRootState;
};

class TimelineIOMessageObserver;
class TimelineIOMessageController : public QObject, public IAssetTranslator
{

    Q_OBJECT;
public:

    //enumeration indicate the object types, for which parameters are sent.
    enum Target
    {
        INVALID_OBJECT,
        TIMELINE_OBJECT,
        LANE_OBJECT
    };

    enum BlockOp
    {
        INVALID_BLOCK_OP,
        MOVE, //perform an actual move of a block.
        ASK_POSITION //request validity of the new position of the block.
    };

    struct BlockOpResponse
    {
        bool success;
        AssetInstanceHandle timelineHandle;
        BlockOp op;
        unsigned newBeat;
        int newLane;  //-1 means unchanged 
        unsigned blockGuid;
        unsigned mouseClickId;
        BlockOpResponse()
        : success(false)
        , op(INVALID_BLOCK_OP)
        , newBeat(0)
        , newLane(-1)
        , blockGuid(0)
        , mouseClickId(0)
        {
        }
    };

    class Message
    {
    public:
        enum MessageType
        {
            INVALID = -1,
            SET_BLOCKSCRIPT,
            CLEAR_BLOCKSCRIPT,
            SET_MASTER_BLOCKSCRIPT,
            SET_PARAMETER,
            CLEAR_MASTER_BLOCKSCRIPT,

            //block message operations
            BLOCK_OPERATION
        };


        Message(MessageType type)
        {
            Reset();
            mMessageType = type;
        }
        Message()
        {
            Reset();
        }

        ~Message() {}

        // Sets all default params
        void Reset()
        {
            mGuid = 0;
            mLaneId = 0;
            mTarget = INVALID_OBJECT;
            mArg = 0;
            mString = "";
            mObserver = nullptr;
            mMessageType = INVALID;
            mBlockOp = MOVE;
            mMouseClickId = 0;
        }

        //! Setters
        void SetMessageType(MessageType type) { mMessageType = type; }
        void SetBlockGuid(unsigned guid) { mGuid = guid; }
        void SetLaneId(unsigned laneId) { mLaneId = laneId; }
        void SetString(const QString& str) { mString = str; }
        void SetTarget(Target target) { mTarget = target; }
        void SetArg(const QVariant& arg) { mArg = arg; }
        void SetParameterName(unsigned name) { mParamName = name; }
        void SetTimelineHandle(const AssetInstanceHandle& h) { mTimelineHandle = h; }
        void SetObserver(TimelineIOMessageObserver* observer) { mObserver = observer; }
        void SetBlockOp(BlockOp op) { mBlockOp = op; }
        void SetMouseClickId(unsigned mouseClickId) {mMouseClickId = mouseClickId;}

        //! Getters
        MessageType GetMessageType() const { return mMessageType; }
        unsigned GetBlockGuid() const { return mGuid; }
        unsigned GetLaneId() const { return mLaneId; }
        unsigned GetParameterName() const { return mParamName; }
        Target GetTarget() const { return mTarget; }
        const QString& GetString() const { return mString; }
        const QVariant& GetArg() const { return mArg; }
        const AssetInstanceHandle& GetTimelineHandle() const { return mTimelineHandle; }
        TimelineIOMessageObserver* GetObserver() const { return mObserver; }
        BlockOp GetBlockOp() const { return mBlockOp; }
        unsigned GetMouseClickId() const { return mMouseClickId; }

    private:
        TimelineIOMessageObserver* mObserver;
        MessageType mMessageType;
        unsigned mGuid;
        unsigned mLaneId;
        unsigned mParamName;
        unsigned mMouseClickId;
        AssetInstanceHandle mTimelineHandle;
        Target mTarget;
        QVariant mArg;
        QString mString;
        BlockOp mBlockOp;

    };

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
    void OnRenderThreadProcessMessage(const Message& msg);

signals:
    void NotifyMasterScriptState(bool isScriptLoaded, QString path);

    void NotifyRepaintTimeline();

private:
    Pegasus::Timeline::ITimelineProxy* ResolveTimeline(const AssetInstanceHandle& assetHandle);

    void OnSetBlockscript(const QString& str, unsigned blockGuid);

    void OnClearBlockscript(unsigned blockGuid);

    void OnSetMasterBlockscript(const QString& str);

    void OnClearMasterBlockscript();

    void OnSetParameter(Target targetObject, const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned parameterName, const QVariant& paramValue, TimelineIOMessageObserver* observer);

    void OnBlockOp(const AssetInstanceHandle& timelineHandle, BlockOp blockOp, unsigned blockGuid, unsigned targetLaneId, const QVariant& arg, unsigned mouseClickId, TimelineIOMessageObserver* observer );

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

    virtual void OnBlockOpResponse(const TimelineIOMessageController::BlockOpResponse& response) = 0;

signals:
    void SignalParameterUpdated(AssetInstanceHandle timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, QVariant parameterValue);

    void SignalBlockOpResponse(TimelineIOMessageController::BlockOpResponse blockOpResponse);

private slots:
    void SlotParameterUpdated(AssetInstanceHandle timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, QVariant parameterValue);

    void SlotBlockOpResponse(TimelineIOMessageController::BlockOpResponse blockOpResponse);
};

#endif
