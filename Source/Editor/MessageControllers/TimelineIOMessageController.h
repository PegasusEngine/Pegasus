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

    struct PegasusAssetTypeDesc;
}

class TimelineIOMessageController : public QObject, public IAssetTranslator
{
    Q_OBJECT;
public:
    class Message
    {
    public:
        enum MessageType
        {
            INVALID = -1,
            SET_BLOCKSCRIPT,
            CLEAR_BLOCKSCRIPT,
            SET_MASTER_BLOCKSCRIPT,
            CLEAR_MASTER_BLOCKSCRIPT 
        };

        Message() : mMessageType(INVALID), mGuid(0){}
        Message(MessageType type) : mMessageType(type), mGuid(0){}
        ~Message() {}

        //! Setters
        void SetMessageType(MessageType type) { mMessageType = type; }
        void SetBlockGuid(unsigned guid) { mGuid = guid; }
        void SetString(const QString& str) { mString = str; }

        //! Getters
        MessageType GetMessageType() const { return mMessageType; }
        unsigned GetBlockGuid() const { return mGuid; }
        const QString& GetString() const { return mString; }
    
    private:
        MessageType mMessageType;
        unsigned mGuid;
        QString mString;
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
    void OnSetBlockscript(const QString& str, unsigned blockGuid);

    void OnClearBlockscript(unsigned blockGuid);

    void OnSetMasterBlockscript(const QString& str);

    void OnClearMasterBlockscript();

    Pegasus::App::IApplicationProxy* mApp;

};

#endif
