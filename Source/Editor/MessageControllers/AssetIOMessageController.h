/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetIOMessageController.h
//! \author	Kleber Garcia
//! \date	31st March 2015
//! \brief	Asset IO controller - keeps state of assets / dirty / on loading etc

#ifndef EDITOR_ASSET_IO_CONTROLLER_H
#define EDITOR_ASSET_IO_CONTROLLER_H

#include <QObject>
#include "Pegasus/Core/Shared/IoErrors.h"

//forward declarations
class PegasusDockWidget;

namespace Pegasus
{
    struct PegasusAssetTypeDesc;

    namespace App
    {
        class IApplicationProxy;
    }

    namespace AssetLib
    {
        class IAssetProxy;
        class IRuntimeAssetObjectProxy;
    }

    namespace Core
    {
        class ISourceCodeProxy;
    }
}

//! Asset IO controller class
class AssetIOMessageController : public QObject
{
    Q_OBJECT;
public:
    //! Asset IO controller message definition
    class Message 
    {
    public:
    
        //! Operation type
        enum MessageType
        {
            INVALID = -1,
            OPEN_ASSET,
            CLOSE_ASSET,
            SAVE_ASSET,
            NEW_ASSET,
        };

        enum IoResponseMessage
        {
            IO_SAVE_SUCCESS,
            IO_SAVE_ERROR,
            IO_NEW_SUCCESS,
            IO_NEW_ERROR
        };
    
        //!Constructor
        Message() : mMessageType(INVALID),
                    mObject(nullptr),
                    mTypeDesc(nullptr)
        {
        }
    
        explicit Message(MessageType t) : mMessageType(t),
                                          mObject(nullptr),
                                          mTypeDesc(nullptr)
        {
        }
    
        //! Destructor
        ~Message()
        {
        }
    
        //Getters
        MessageType GetMessageType() const { return mMessageType; }
        QString GetString() const { return mString; }        
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* GetObject() const { return mObject; }
        const Pegasus::PegasusAssetTypeDesc* GetTypeDesc() const { return mTypeDesc; }
    
    
        //Setters
        void SetMessageType(MessageType t) { mMessageType = t; }
        void SetString(const QString& s) { mString = s; }
        void SetObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* obj) { mObject = obj; }
        void SetTypeDesc(const Pegasus::PegasusAssetTypeDesc* desc) { mTypeDesc = desc; }
    
    
    private:
    
        Pegasus::AssetLib::IRuntimeAssetObjectProxy  * mObject;
        const Pegasus::PegasusAssetTypeDesc* mTypeDesc;
        MessageType mMessageType;
        QString mString;
    
    };

    //! Constructor
    explicit AssetIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~AssetIOMessageController();

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(PegasusDockWidget* sender, const Message& msg);

signals:

    //! Signal triggered when an object is requested for opening. Calls the UI to open on a proper editor.
    void SignalOpenObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);
    
    //! Signal triggered when the UI needs to update the active node views
    void SignalUpdateNodeViews();

    //! Signal triggered when an error has occured.
    void SignalOnErrorMessagePopup(const QString& message);

    //! Signal triggered when a message is sent to the code editor.
    void SignalPostMessage(PegasusDockWidget* sender, AssetIOMessageController::Message::IoResponseMessage id);

private:
    
    //! Opens a serialized object stored in the asset path passed.
    //! \param path the file path containing the asset
    void OnRenderRequestOpenObject(const QString& path);

    //! Closes (decreases ref count) on serialized object
    //! \param object to close
    void OnRenderRequestCloseObject(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Saves object to disk.
    //! \param sender dock widget - for message responses
    //! \param object - the object to save
    void OnSaveObject(PegasusDockWidget* sender, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object);

    //! Requests a new object internally
    //! \param path - the path to use to create the object
    //! \param desc - the description of the object used
    void OnRenderRequestNewObject(PegasusDockWidget* sender, const QString& path, const Pegasus::PegasusAssetTypeDesc* desc);

    Pegasus::App::IApplicationProxy* mApp;
    
};


#endif
