/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   IApplication.h
//! \author Kevin Boulanger
//! \date   09th July 2013
//! \brief  Interface returned to the launcher or editor when compiling an app as a dynamic library

#ifndef PEGASUS_IAPPLICATION_H
#define PEGASUS_IAPPLICATION_H


namespace Pegasus{
    struct WindowConfig;
    class Window;
}


namespace Pegasus {


class IApplication
{
public:

    virtual ~IApplication() { };

    // Window API
    //! \todo
    //Window* AttachWindow(const WindowConfig& config);
    //void DetachWindow(Window* wnd);

    // Update API
    virtual int Run() = 0;
    //! \todo Set update mode

    //virtual void Initialize(const ApplicationConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual void Render() = 0;
};


}   // namespace Pegasus

#endif  // PEGASUS_IAPPLICATION_H
