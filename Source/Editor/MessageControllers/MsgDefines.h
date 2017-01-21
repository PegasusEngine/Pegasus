/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	MsgDefines.h
//! \author	Kleber Garca
//! \date	December 27th 2016
//! \brief	message defines and fwd declarations for msg controllers.

#ifndef PEGASUS_EDITOR_IOMSGDEFINES_H
#define PEGASUS_EDITOR_IOMSGDEFINES_H

#include "Pegasus/Application/Shared/ApplicationConfig.h"
#include "Pegasus/Shader/Shared/ShaderDefs.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PegasusAssetTypes.h"
#include <QVariant>
#include <QSet>

//forward declarations

class TimelineIOMessageObserver;
class GraphNodeObserver;
class ViewportWidget;
class AssetTreeObserver;
class PropertyGridObserver;

namespace Pegasus
{
    namespace Timeline
    {
        class ITimelineProxy;
        class IBlockProxy;
        class ILaneProxy;
    }

    namespace PropertyGrid
    {
        class IPropertyGridObjectProxy;
    }
}

//! Qt UI asset instance handle. Represents  a handle to a RuntimeAssetObjectProxy on the render thread side.
//! This is a ui copy of the data that an asset contains to get visualized when opened.
struct AssetInstanceHandle
{
    explicit AssetInstanceHandle(int v) { mValue = v; }

    AssetInstanceHandle() : mValue(-1) {}

    bool operator == (const AssetInstanceHandle& other) const { return other.mValue == mValue;}

    bool operator != (const AssetInstanceHandle& other) const { return !(other == *this); }

    AssetInstanceHandle operator++(int) { return AssetInstanceHandle(mValue++); }

    const AssetInstanceHandle& operator = (const AssetInstanceHandle& other) { mValue = other.mValue; return *this; }

    bool operator < (const AssetInstanceHandle& other) const { return mValue < other.mValue; }

    int InternalValue() const { return mValue; }

    bool IsValid() const { return mValue != -1; }

private:
    int mValue;
};

inline uint qHash(const AssetInstanceHandle& h) 
{
    return qHash(h.InternalValue());
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
        PROP_BLOCK_SCRIPT_PATH,
        BLOCK_PROP_COUNT
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
        PROP_LANE_NAME,
        LANE_PROP_COUNT
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

//enumeration indicate the object types, for which parameters are sent for timeline.
enum TimelineIOMCTarget
{
    INVALID_OBJECT,
    TIMELINE_OBJECT,
    LANE_OBJECT
};

//type of operation for timeline msg.
//! Note: only messages such as "Move" and "New" require an ask message.
//! The reason is that when you move an object, you have to query the render app
//! wether if this move is valid. New requires an ASK message, since we need to know
//! the block guid ahead of time. Delete doesnt require an Ask operation
//! since delete and new are guaranteed to always work.
enum TimelineIOMCBlockOp
{
    INVALID_BLOCK_OP,
    NEW_BLOCK,
    DELETE_BLOCKS,
    MOVE_BLOCK, //perform an actual move of a block.
    ASK_NEW_BLOCK,
    ASK_DELETE_BLOCKS,
    ASK_BLOCK_POSITION //request validity of the new position of the block.
};

// a response from a timeline operation
struct TimelineIOMCBlockOpResponse
{
    bool success;
    AssetInstanceHandle timelineHandle;
    TimelineIOMCBlockOp op;
    unsigned newBeat;
    int newLane;  //-1 means unchanged 
    unsigned blockGuid;
    unsigned mouseClickId;
    ShadowLaneState newShadowLaneState;
    QSet<int> lanesFound;
    QMap<int, ShadowLaneState> lanesFoundState;
    QVariant arg;
    bool requiresRefocus;

    TimelineIOMCBlockOpResponse()
    : success(false)
    , op(INVALID_BLOCK_OP)
    , newBeat(0)
    , newLane(-1)
    , blockGuid(0)
    , mouseClickId(0)
    , requiresRefocus(false)
    {
    }
};

// message class for a timeline message.
class TimelineIOMCMessage
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
        TOGGLE_PLAY_MODE,
        //block message operations
        BLOCK_OPERATION
    };


    TimelineIOMCMessage(MessageType type)
    {
        Reset();
        mMessageType = type;
    }

    TimelineIOMCMessage()
    {
        Reset();
    }

    ~TimelineIOMCMessage() {}

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
        mBlockOp = MOVE_BLOCK;
        mMouseClickId = 0;
        mRequiresRefocus = false;
        mIsPlayMode = false;
    }

    //! Setters
    void SetMessageType(MessageType type) { mMessageType = type; }
    void SetBlockGuid(unsigned guid) { mGuid = guid; }
    void SetLaneId(unsigned laneId) { mLaneId = laneId; }
    void SetString(const QString& str) { mString = str; }
    void SetTarget(TimelineIOMCTarget target) { mTarget = target; }
    void SetArg(const QVariant& arg) { mArg = arg; }
    void SetParameterName(unsigned name) { mParamName = name; }
    void SetTimelineHandle(const AssetInstanceHandle& h) { mTimelineHandle = h; }
    void SetObserver(TimelineIOMessageObserver* observer) { mObserver = observer; }
    void SetBlockOp(TimelineIOMCBlockOp op) { mBlockOp = op; }
    void SetMouseClickId(unsigned mouseClickId) {mMouseClickId = mouseClickId;}
    void SetRequiresRefocus(bool requiresRefocus) {mRequiresRefocus = requiresRefocus;}
    void SetIsPlayMode(bool isPlayMode) { mIsPlayMode = isPlayMode; }

    //! Getters
    MessageType GetMessageType() const { return mMessageType; }
    unsigned GetBlockGuid() const { return mGuid; }
    unsigned GetLaneId() const { return mLaneId; }
    unsigned GetParameterName() const { return mParamName; }
    TimelineIOMCTarget GetTarget() const { return mTarget; }
    const QString& GetString() const { return mString; }
    const QVariant& GetArg() const { return mArg; }
    const AssetInstanceHandle& GetTimelineHandle() const { return mTimelineHandle; }
    TimelineIOMessageObserver* GetObserver() const { return mObserver; }
    TimelineIOMCBlockOp GetBlockOp() const { return mBlockOp; }
    unsigned GetMouseClickId() const { return mMouseClickId; }
    bool GetRequiresRefocus() const { return mRequiresRefocus; }
    bool GetIsPlayMode() const { return mIsPlayMode;  }

private:
    TimelineIOMessageObserver* mObserver;
    MessageType mMessageType;
    unsigned mGuid;
    unsigned mLaneId;
    unsigned mParamName;
    unsigned mMouseClickId;
    bool mRequiresRefocus;
    bool mIsPlayMode;
    AssetInstanceHandle mTimelineHandle;
    TimelineIOMCTarget mTarget;
    QVariant mArg;
    QString mString;
    TimelineIOMCBlockOp mBlockOp;
};

//! Structure with the update of an element
struct GraphIOMCUpdateElement
{
    QString mNodeName;

public:

    GraphIOMCUpdateElement()
    {
    }
};


// Accumulation of update elements
struct GraphIOMCUpdateCache
{
    QVector<GraphIOMCUpdateElement> mUpdateCache;
    GraphIOMCUpdateCache()
    { }
};

//! Message container class so the UI can communicate with the application render thread
class GraphIOMCMessage
{
public:
    enum MessageType
    {
        VIEW_GRAPH_ON_VIEWPORT,
        INVALID
    };
    

    //! Constructors
    explicit GraphIOMCMessage(MessageType type)
        : mMessageType(type), mGraphNodeObserver(nullptr), mTargetViewport(nullptr) { }
    
    GraphIOMCMessage()
        : mMessageType(INVALID), mGraphNodeObserver(nullptr), mTargetViewport(nullptr) { }


    // Getters
    inline MessageType GetMessageType() const { return mMessageType; }
    GraphNodeObserver* GetGraphNodeObserver() const { return mGraphNodeObserver; }
    ViewportWidget* GetTargetViewport() const { return mTargetViewport; }
    AssetInstanceHandle GetGraphHandle() const { return mGraphHandle; }

    // Setters
    void SetMessageType(MessageType t) { mMessageType = t; }
    void SetGraphNodeObserver(GraphNodeObserver* graphNodeObserver) { mGraphNodeObserver = graphNodeObserver; }
    void SetTargetViewport(ViewportWidget* viewport) { mTargetViewport = viewport; }
    void SetGraphHandle(AssetInstanceHandle graphHandle) { mGraphHandle = graphHandle; }
    
private:

    //! Type of the message being sent
    MessageType mMessageType;
    GraphNodeObserver* mGraphNodeObserver;
    ViewportWidget* mTargetViewport;
    AssetInstanceHandle mGraphHandle;
};

class WindowIOMCMessage
{
public:
    enum MessageType
    {
        INVALID = -1,
        INITIALIZE_WINDOW,    
        DRAW_WINDOW,    
        WINDOW_RESIZED,
        ENABLE_DRAW
    };

    //! Constructor
    WindowIOMCMessage() : mMessageType(INVALID),
                mViewportWidget(nullptr),
                mComponentFlags(Pegasus::App::COMPONENT_FLAG_UNUSED_SLOT),
                mWidth(1),
                mHeight(1),
                mEnableDraw(false)
    {} 

    ~WindowIOMCMessage() {}
    
    //! Getters
    MessageType GetMessageType() const { return mMessageType; }
    ViewportWidget* GetViewportWidget() const { return mViewportWidget; }
    Pegasus::App::ComponentTypeFlags GetComponentFlags() const { return mComponentFlags; }
    unsigned GetWidth() const { return mWidth; }
    unsigned GetHeight() const { return mHeight; }
    bool GetEnableDraw() const { return mEnableDraw; }

    //! Setters
    void SetMessageType(MessageType type) { mMessageType = type; }
    void SetViewportWidget(ViewportWidget* vp) { mViewportWidget = vp; }
    void SetComponentFlags(Pegasus::App::ComponentTypeFlags flags)  { mComponentFlags = flags; }
    void SetWidth(unsigned w)  { mWidth = w; }
    void SetHeight(unsigned h) { mHeight = h; }
    void SetEnableDraw(bool enableDraw) { mEnableDraw = enableDraw; }

private:
    MessageType     mMessageType;
    ViewportWidget* mViewportWidget;
    Pegasus::App::ComponentTypeFlags mComponentFlags;
    bool mEnableDraw;
    unsigned mWidth;
    unsigned mHeight;
    
};

//! Program IO controller message definition
class ProgramIOMCMessage 
{
public:

    //! Operation type
    enum MessageType
    {
        INVALID = -1,
        REMOVE_SHADER,
        MODIFY_SHADER
    };

    //!Constructor
    ProgramIOMCMessage() : mMessageType(INVALID), mShaderType(Pegasus::Shader::SHADER_STAGE_INVALID)
    {
    }

    //! Destructor
    ~ProgramIOMCMessage()
    {
    }

    //Getters
    MessageType GetMessageType() const { return mMessageType; }
    const QString& GetShaderPath() const { return mShaderPath; }
    AssetInstanceHandle GetProgram() const { return mProgram; }
    Pegasus::Shader::ShaderType GetShaderType() const { return mShaderType; }
    

    //Setters
    void SetMessageType(MessageType t) { mMessageType = t; }
    void SetShaderPath(const QString& shader) { mShaderPath = shader; }
    void SetProgram(AssetInstanceHandle handle) { mProgram = handle; }
    void SetShaderType(Pegasus::Shader::ShaderType type) { mShaderType = type; }

private:
    MessageType mMessageType;
    QString mShaderPath;
    AssetInstanceHandle mProgram;
    Pegasus::Shader::ShaderType mShaderType;
};

class SourceIOMCMessage
{
public:
    enum MessageType
    {
        INVALID = -1,
        SET_SOURCE_AND_COMPILE_SOURCE,
        COMPILE_SOURCE,
        SET_SOURCE
    };

    //! Constructor
    SourceIOMCMessage() : mType(INVALID) {}

    //! Sets the message type
    void SetMessageType(MessageType type) { mType = type; } 

    //! Sets the source of the shader in the message
    void SetSourceText(const QString& str) { mSourceText = str; }

    //! Gets the message type
    MessageType GetMessageType() const { return mType; } 

    //! Gets the source of the shader in the message
    const QString GetSourceText() const { return mSourceText; }

    //! Sets the source asset handle
    void SetHandle(AssetInstanceHandle handle) { mHandle = handle; }

    //! Gets the source asset handle
    AssetInstanceHandle GetHandle() const { return mHandle; } 

private:
    MessageType mType;
    QString     mSourceText;
    AssetInstanceHandle mHandle;
};


//! Represents the information of one asset. Contains list of indexes of children.
struct AssetInformation
{
    AssetInformation() : typeInfo(nullptr) {}

    const Pegasus::PegasusAssetTypeDesc* typeInfo;
    QString displayName;
    QVector<int> children;
    bool isValid;
};

//! Represents an asset category. Contains list of indexes of children.
struct AssetCategory
{
    QString displayName;
    unsigned blockGuid;
    QVector<int> children;
};

//! Tree representing all the assets
struct AssetViewTree
{
    QVector<AssetCategory> categories; //block categories
    QVector<AssetCategory> typeCategories; //typed categoriesss
    QVector<AssetInformation> allAssets;
};

    //! Asset IO controller message definition
class AssetIOMCMessage 
{
public:

    //! Operation type
    enum MessageType
    {
        INVALID = -1,
        OPEN_ASSET,
        CLOSE_ASSET,
        SAVE_ASSET,
        RELOAD_FROM_ASSET,
        NEW_ASSET,
        QUERY_START_VIEW_ASSET_TREE,
        QUERY_REFRESH_VIEW_ASSET_TREE,
        QUERY_STOP_VIEW_ASSET_TREE
    };

    enum IoResponseMessage
    {
        IO_SAVE_SUCCESS,
        IO_SAVE_ERROR,
        IO_NEW_SUCCESS,
        IO_NEW_ERROR
    };

    //!Constructor
    AssetIOMCMessage() : mMessageType(INVALID),
                mTypeDesc(nullptr),
                mTreeObserver(nullptr)
    {
    }

    explicit AssetIOMCMessage(MessageType t) : mMessageType(t),
                                      mTypeDesc(nullptr),
                                      mTreeObserver(nullptr)
    {
    }

    //! Destructor
    ~AssetIOMCMessage()
    {
    }

    //Getters
    MessageType GetMessageType() const { return mMessageType; }
    QString GetString() const { return mString; }        
    AssetInstanceHandle GetObject() const { return mObject; }
    const Pegasus::PegasusAssetTypeDesc* GetTypeDesc() const { return mTypeDesc; }
    AssetTreeObserver* GetTreeObserver() const { return mTreeObserver; }


    //Setters
    void SetMessageType(MessageType t) { mMessageType = t; }
    void SetString(const QString& s) { mString = s; }
    void SetObject(AssetInstanceHandle obj) { mObject = obj; }
    void SetTypeDesc(const Pegasus::PegasusAssetTypeDesc* desc) { mTypeDesc = desc; }
    void SetTreeObserver(AssetTreeObserver* treeObserver) { mTreeObserver = treeObserver; }


private:

    AssetInstanceHandle mObject;
    const Pegasus::PegasusAssetTypeDesc* mTypeDesc;
    MessageType mMessageType;
    QString mString;
    AssetTreeObserver* mTreeObserver;

};

//! Qt handle for property grid proxy. Mimics an integer.
//! Cant be an int typedef, because Qt won't be able to pass it around through signals
struct PropertyGridHandle
{
    explicit PropertyGridHandle(int v) { mValue = v; }

    PropertyGridHandle() : mValue(-1) {}

    bool operator == (const PropertyGridHandle& other) const { return other.mValue == mValue;}

    bool operator != (const PropertyGridHandle& other) const { return !(other == *this); }

    PropertyGridHandle operator++(int) { return PropertyGridHandle(mValue++); }

    const PropertyGridHandle& operator = (const PropertyGridHandle& other) { mValue = other.mValue; return *this; }

    bool operator < (const PropertyGridHandle& other) const { return mValue < other.mValue; }

    int InternalValue() const { return mValue; }

private:
    int mValue;
};

inline uint qHash(const PropertyGridHandle& h) 
{
    return qHash(h.InternalValue());
}

// invalid handle declaration
const PropertyGridHandle INVALID_PGRID_HANDLE(-1);


//! Structure with the update of an element
struct PropertyGridIOMCUpdateElement
{
    Pegasus::PropertyGrid::PropertyCategory mCategory;
    Pegasus::PropertyGrid::PropertyType mType; //! property type
    int mIndex; // the index position of this property
    union Data //Union representing the data it holds
    {   
        float         f;        //! float
        unsigned int  u;        //! unsigned
        int           i;        //! signed, enum
        bool          b;        //! bool
        char          s64[64];  //! string64
        unsigned char rgba8[4]; //! rgb and rgba
        float         v[4];     //! float2, float3 and float4
    } mData;
public:
    PropertyGridIOMCUpdateElement() :
        mCategory(Pegasus::PropertyGrid::PROPERTYCATEGORY_INVALID),
        mType(Pegasus::PropertyGrid::PROPERTYTYPE_INVALID),
        mIndex(-1)
    {
    }
};


// Accumulation of object data
struct PropertyGridIOMCUpdateCache
{
    PropertyGridHandle mHandle;
    QVector<PropertyGridIOMCUpdateElement> mUpdateCache;
    PropertyGridIOMCUpdateCache() : mHandle(INVALID_PGRID_HANDLE) {}
};

//! Message container class so ui can communicate with render thread
class PropertyGridIOMCMessage
{
public:
    enum MessageType
    {
        OPEN,
        OPEN_FROM_ASSET_HANDLE, //! Open the property grid from an asset handle
        OPEN_BLOCK_FROM_TIMELINE, //! Open the property grid from an asset handle
        CLOSE, //! when a widget will not view a property grid any more
        UPDATE, //! when the widget request a change in the state of a property grid
        INVALID
    };
    
    //! Constructors
    explicit PropertyGridIOMCMessage(MessageType t) : mMessageType(t), mPropGrid(nullptr), mBlockGuid(0xffffff), mPropGridHandle(-1), mObserver(nullptr) {}
    PropertyGridIOMCMessage() : mMessageType(INVALID), mPropGrid(nullptr), mBlockGuid(0xffffffff), mPropGridHandle(-1), mObserver(nullptr) {}

    //! Getters
    unsigned GetBlockGuid() const { return mBlockGuid; }
    AssetInstanceHandle GetAssetHandle() const { return mAssetHandle; }
    MessageType GetMessageType() const { return mMessageType; }
    QVector<PropertyGridIOMCUpdateElement>& GetUpdateBatch() { return mUpdates; }
    const QVector<PropertyGridIOMCUpdateElement>& GetUpdateBatch() const { return mUpdates; }
    PropertyGridHandle GetPropertyGridHandle() const { return mPropGridHandle; }
    Pegasus::PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() const { return mPropGrid; }
    PropertyGridObserver* GetPropertyGridObserver() const { return mObserver; }
    const QString& GetTitle() const { return mTitle; }

    //! Setters
    void SetBlockGuid(unsigned blockGuid) { mBlockGuid = blockGuid; }
    void SetAssetHandle(AssetInstanceHandle handle) { mAssetHandle = handle ; }
    void SetPropertyGridHandle(PropertyGridHandle handle) { mPropGridHandle = handle; }
    void SetPropertyGrid(Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy) { mPropGrid  = proxy; }
    void SetPropertyGridObserver(PropertyGridObserver* observer) { mObserver = observer; }
    void SetMessageType(MessageType t) { mMessageType = t; }
    void SetTitle(const QString& title) { mTitle = title; }
    

private:
    AssetInstanceHandle mAssetHandle;
    MessageType mMessageType;
    QVector<PropertyGridIOMCUpdateElement> mUpdates;
    PropertyGridHandle mPropGridHandle;
    Pegasus::PropertyGrid::IPropertyGridObjectProxy* mPropGrid;
    PropertyGridObserver* mObserver;
    unsigned mBlockGuid;
    QString mTitle;
};
    
#endif
