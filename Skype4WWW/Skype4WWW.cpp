/**********************************************************\

  Auto-generated Skype4WWW.cpp

  This file contains the auto-generated main plugin object
  implementation for the Skype API Plugin project

\**********************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "Skype4WWWAPI.h"

#include "Skype4WWW.h"


///////////////////////////////////////////////////////////////////////////////
/// @fn Skype4WWW::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void Skype4WWW::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process

    //    skype_connect();

    // fprintf(stderr, "这能显示吗?能\n");
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Skype4WWW::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void Skype4WWW::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded

    fprintf(stderr, "C: %s\n", __FUNCTION__);
    // sleep(1);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Skype4WWW constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
Skype4WWW::Skype4WWW()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Skype4WWW destructor.
///////////////////////////////////////////////////////////////////////////////
Skype4WWW::~Skype4WWW()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();

    fprintf(stderr, "C: %s\n", __FUNCTION__); // no called
}

void Skype4WWW::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.

    // bool bok = skype_connect();
    
	// gchar *reply = skype_send_message("NAME %s", "SKP4WEB");
	// reply = skype_send_message("PROTOCOL 8");
    // reply = skype_send_message("CALL %s", "liuguangzhao");
    // fprintf(stderr, "这能显示吗?能 %d\n", bok);

    // show all params
    fprintf(stderr, "okkkkkkk: %s\n", __FUNCTION__); // no called    

    FB::JSAPIPtr root_jsapi = this->getRootJSAPI();
    FB::ptr_cast<Skype4WWWAPI>(root_jsapi->shared_ptr())->on_plugin_ready();
}

// get debug like params by JSAPI object
std::string Skype4WWW::get_param(std::string key)
{
    std::string value;

    value = this->m_params[key].cast<std::string>();

    return value;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<Skype4WWW> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr Skype4WWW::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<Skype4WWWAPI>(FB::ptr_cast<Skype4WWW>(shared_from_this()), m_host);
}

bool Skype4WWW::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool Skype4WWW::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool Skype4WWW::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool Skype4WWW::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool Skype4WWW::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}

