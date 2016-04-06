/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetEvent.h
//! \author	Kleber Garcia
//! \date	5th March 2016
//! \brief	Events for asset library

#ifndef PEGASUS_ASSETEVENTS_H
#define PEGASUS_ASSETEVENTS_H

#include "Pegasus/Core/Shared/EventDefs.h"

namespace Pegasus
{
    namespace AssetLib
    {
        class IAssetProxy;
        class IObjectProxy;
        class IRuntimeAssetObjectProxy;

        class AssetCreated
        {
        public:
            AssetCreated(IAssetProxy* proxy) : mProxy(proxy) {}
            ~AssetCreated() {}

            IAssetProxy* GetProxy() { return mProxy; }

        private:
            IAssetProxy* mProxy;
        };

        class AssetLinkAdded
        {
        public:
            AssetLinkAdded(IObjectProxy* parent, IAssetProxy* child) : mProxyParent(parent), mProxyChild(child) {}
            ~AssetLinkAdded() {}

            IObjectProxy* GetProxyParent() { return mProxyParent; }
            IAssetProxy* GetProxyChild() { return mProxyChild; }

        private:
            IObjectProxy* mProxyParent;
            IAssetProxy* mProxyChild;
        };

        class RuntimeAssetObjectCreated
        {
        public:
            RuntimeAssetObjectCreated(IRuntimeAssetObjectProxy* proxy) : mProxy(proxy) {}

            ~RuntimeAssetObjectCreated() {}

            IRuntimeAssetObjectProxy* GetProxy() { return mProxy; }

        private:
            IRuntimeAssetObjectProxy* mProxy;
        };

        class AssetDestroyed
        {
        public:
            AssetDestroyed(IAssetProxy* proxy) : mProxy(proxy) {}
            ~AssetDestroyed() {}

            IAssetProxy* GetProxy() { return mProxy; }

        private:
            IAssetProxy* mProxy;
        };

        class RuntimeAssetObjectDestroyed
        {
        public:
            RuntimeAssetObjectDestroyed(IRuntimeAssetObjectProxy* proxy) : mProxy(proxy) {}

            ~RuntimeAssetObjectDestroyed() {}

            IRuntimeAssetObjectProxy* GetProxy() { return mProxy; }

        private:
            IRuntimeAssetObjectProxy* mProxy;
        };

        PEGASUS_EVENT_BEGIN_REGISTRY (IAssetEventListener)
            PEGASUS_EVENT_REGISTER (AssetCreated)
            PEGASUS_EVENT_REGISTER (AssetLinkAdded)
            PEGASUS_EVENT_REGISTER (AssetDestroyed)
            PEGASUS_EVENT_REGISTER (RuntimeAssetObjectCreated)
            PEGASUS_EVENT_REGISTER (RuntimeAssetObjectDestroyed)
        PEGASUS_EVENT_END_REGISTRY

    }
}


#endif
