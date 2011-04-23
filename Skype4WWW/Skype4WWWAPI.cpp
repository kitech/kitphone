/**********************************************************\

  Auto-generated Skype4WWWAPI.cpp

\**********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// #include "X11/skype_messaging.c"
#include "X11/libskypeapi.h"

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

#include "Skype4WWWAPI.h"

Skype4WWWAPI *root_jsapi_skw = NULL;

// TODO 添加debug param
//
// TODO 放在头文件中的函数，会在共享库.so中暴出。
// TODO 在skype未登陆时，页面会加载一半停止很长时间，需要有一连接skype超时功能。
// TODO 连接skype的时机不对，在插件没有加载完成前即连接，可能导致页面没加载完而暂停
// got skype running: 1
// okkkkkkk: onPluginReady
///////////////////////////////////////////////////////////////////////////////
/// @fn Skype4WWWAPI::Skype4WWWAPI(const Skype4WWWPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
Skype4WWWAPI::Skype4WWWAPI(const Skype4WWWPtr& plugin, const FB::BrowserHostPtr& host)
    : FB::JSAPIAuto("<JSAPI-Auto-Skype4WWW Javascript Object>")
    , m_plugin(plugin), m_host(host)
{
    // fprintf(stderr, "Plugin running host: %s\n", host);
    root_jsapi_skw = this;
    this->AttachmentStatus = apiAttachUnknown;

    // this->ptr_client = new SkypeClient();
    this->js_Client = boost::make_shared<SkypeClient>();
    this->ptr_Client = static_cast<SkypeClient*>((this->js_Client->shared_ptr()).get());

    this->js_CurrentUser = boost::make_shared<SkypeAccount>();
    this->ptr_CurrentUser = static_cast<SkypeAccount*>((this->js_CurrentUser->shared_ptr()).get());

    //// init js_calls list
    for (int i = 0; i < sizeof(this->js_calls)/sizeof(FB::JSAPIPtr); i++) {
        this->js_calls[i] = boost::shared_ptr<FB::JSAPI>();
        // this->js_calls[i] = 0;
    }
    this->last_fired_js_call = boost::shared_ptr<FB::JSAPI>();

    registerMethod("Attach",    make_method(this, &Skype4WWWAPI::Attach));
    registerMethod("PlaceCall", make_method(this, &Skype4WWWAPI::PlaceCall));
    registerProperty("Client", make_property(this, &Skype4WWWAPI::get_Client));

    registerProperty("AttachmentStatus", make_property(this, &Skype4WWWAPI::get_AttachmentStatus));

    registerProperty("CurrentUser", make_property(this, &Skype4WWWAPI::get_CurrentUser));
    registerProperty("ApiWrapperVersion", make_property(this, &Skype4WWWAPI::get_ApiWrapperVersion));
    registerProperty("Version", make_property(this, &Skype4WWWAPI::get_Version));

    registerEvent("onAttachmentStatus");
    registerEvent("onConnectionStatus");

    ////////// 
    registerMethod("echo",      make_method(this, &Skype4WWWAPI::echo));
    registerMethod("testEvent", make_method(this, &Skype4WWWAPI::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &Skype4WWWAPI::get_testString,
                        &Skype4WWWAPI::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &Skype4WWWAPI::get_version));
    
    
    registerEvent("onfired");
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Skype4WWWAPI::~Skype4WWWAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
Skype4WWWAPI::~Skype4WWWAPI()
{
    fprintf(stderr, "~~~~%s\n", __FUNCTION__);

    // if not connected, do not call disconnect
    if (this->AttachmentStatus == apiAttachSuccess
	|| this->AttachmentStatus == apiAttachAvailable
	|| this->AttachmentStatus == apiAttachPendingAuthorization) {
      skype_disconnect();
    }

    // 这些都不需要，并且在Linux firefox 3.6导致浏览器刷新时死掉
    // 但可正常释放这些变量。
    // FB::JSAPIPtr shptr = this->js_Client;
    // this->js_Client = boost::shared_ptr<FB::JSAPI>();
    // delete this->ptr_Client; // 不能这么做，否则会引起其他的问题。
    // this->ptr_Client = NULL;
    // this->m_host->releaseJSAPIPtr(this->js_Client);

    fprintf(stderr, "~~~~%s\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Skype4WWWPtr Skype4WWWAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
Skype4WWWPtr Skype4WWWAPI::getPlugin()
{
    Skype4WWWPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

FB::variant Skype4WWWAPI::Attach(const FB::variant& protocol, const FB::variant& wait)
{
    bool bok = skype_connect();
    if (!bok) {
        skype_debug_info("skw", "Connect to skype error\n");
        this->ptr_Client->IsRunning = 0;
        this->AttachmentStatus = apiAttachNotAvailable;
        return -1;
    }
    this->AttachmentStatus = apiAttachPendingAuthorization;

    this->ptr_Client->IsRunning = 1;
    gchar *reply = skype_send_message("NAME %s", "SKP4WEB");
    if (reply == NULL || strlen(reply) == 0) {
        skype_debug_info("skw", "Auth to skype error\n");
        this->AttachmentStatus = apiAttachRefused;
        return -2;
    }

	if (g_str_equal(reply, "CONNSTATUS OFFLINE"))
	{
		//this happens if we connect before skype has connected to the network
		// purple_timeout_add_seconds(1, skype_login_cb, acct);
		g_free(reply);
		return -3;
	}
    g_free(reply);
    this->AttachmentStatus = apiAttachAvailable;

    reply = skype_send_message("PROTOCOL 8");
    if (reply == NULL || strlen(reply) == 0) {
        skype_debug_info("skw", "Exchange protocol to skype error\n");
        return -4;
    }
    g_free(reply);

    this->AttachmentStatus = apiAttachSuccess;

    this->FireJSEvent("onAttachmentStatus", FB::variant_list_of("param1")(apiAttachSuccess));
    this->FireEvent("onAttachmentStatus", FB::variant_list_of("param1")(apiAttachSuccess));

    // reply = skype_send_message("CALL %s", "liuguangzhao");

	const char *username = skype_get_account_username();
    const char *display_name = skype_get_account_alias();
    this->ptr_CurrentUser->Handle = std::string(username);
    this->ptr_CurrentUser->Fullname = std::string(display_name);

    return 0;
}

FB::JSAPIPtr Skype4WWWAPI::get_Client()
{
    // return &this->Client;
    // return this->Client.shared_ptr();
    // FB::JSAPI * ptr = & this->Client;
    // return ptr;
    // return boost::make_shared<SkypeClient>();
    // return boost::make_shared<Skype4WWWAPI>(FB::ptr_cast<Skype4WWW>(shared_from_this()), m_host);
    // return this->Client;
    return this->js_Client;
}
int Skype4WWWAPI::get_AttachmentStatus()
{
    return this->AttachmentStatus;
}

FB::JSAPIPtr Skype4WWWAPI::get_CurrentUser()
{
    return this->js_CurrentUser;
}

std::string Skype4WWWAPI::get_ApiWrapperVersion()
{
    this->ApiWrapperVersion = std::string("1.0.3.26");
    return this->ApiWrapperVersion;
}
std::string Skype4WWWAPI::get_Version()
{
    // this->Version = std::string("2.1.0.81");
    if (this->Version.empty()) {
        gchar *temp = skype_send_message("GET SKYPEVERSION");
        gchar *version = g_strdup(&temp[13]);
        g_free(temp);
        this->Version = std::string(version);
        g_free(version);
    } else {
    }
    
    return this->Version;
}

FB::JSAPIPtr Skype4WWWAPI::PlaceCall(const FB::variant& target, const FB::variant& target2,
                                     const FB::variant& target3, const FB::variant& target4)
{
    // this->js_calls[0] = boost::shared_ptr<FB::JSAPI>();
    // fprintf(stderr, "a null call: %d\n", this->js_calls[0] == boost::shared_ptr<FB::JSAPI>());
    const char *who;
    std::string str_who;
    fprintf(stderr, "target,%d, %d\n", target.empty(), target.empty());

    // const std::type_info t_info(target.get_type());
    fprintf(stderr, "target is type of: char *= %d\n", target.is_of_type<char *>());
    fprintf(stderr, "target is type of: std::string= %d\n", target.is_of_type<std::string>());

    str_who = target.cast<std::string>();
    fprintf(stderr, "target std::string is: %s\n", str_who.c_str());

    // who = target.cast<const char*>();
    // try {
    // } catch (std::Exception *e) {
    // }
    
    who = str_who.c_str();
    fprintf(stderr, "target,%s\n", who);
    const char *call_id = skype_callto(who);

    fprintf(stderr, "callto: %s, call id: %s\n", who, call_id);
    SkypeCall *skp_call = new SkypeCall();
    skp_call->targets.insert(skp_call->targets.end(), std::string(who));
    skp_call->call_id = call_id;

    // this->js_calls[0] = skp_call;
    // boost::shared_ptr<FB::JSAPI> js_call = boost::shared_ptr<FB::JSAPI>(skp_call);
    this->js_calls[0] = boost::shared_ptr<FB::JSAPI>(skp_call);

    fprintf(stderr, "find call object with id: %s, ref: %d\n", 
            skp_call->call_id.c_str(), this->js_calls[0].use_count());

    if (0) {
        // for test
        this->m_host->releaseJSAPIPtr(this->js_calls[0]);
    }
    return this->js_calls[0];
    // return js_call;
}

int Skype4WWWAPI::hangup_call(std::string call_id)
{
    SkypeCall *ocall = NULL;
    boost::shared_ptr<FB::JSAPI> shptr, tmptr;
    bool found = false;

    fprintf(stderr, "%s vv %s, %d\n", __FUNCTION__, call_id.c_str()
            , sizeof(this->js_calls)/sizeof(FB::JSAPIPtr));

    for (int i = 0; i < sizeof(this->js_calls)/sizeof(FB::JSAPIPtr); i++) {
        shptr = this->js_calls[i];
        if (shptr == boost::shared_ptr<FB::JSAPI>()) {
            continue;
        }
        // ocall = this->js_calls[i];
        // fprintf(stderr, "ocall %d, %p\n", i, ocall);
        ocall = static_cast<SkypeCall*>(shptr->shared_ptr().get());
        if (ocall != NULL) {
            if (ocall->call_id == call_id) {
                fprintf(stderr, "find call object with id: %s, ref: %d\n", 
                        call_id.c_str(), shptr.use_count());

                this->js_calls[i] = boost::shared_ptr<FB::JSAPI>();
                // this->js_calls[i] = 0;
                /// free it
                // delete ocall;
                found = true;

                fprintf(stderr, "find call object with id: %s, ref: %d\n", 
                        call_id.c_str(), shptr.use_count());

                fprintf(stderr, "ocall info: hangup by user: %d\n", ocall->user_hangup);

                tmptr = this->last_fired_js_call;
                this->last_fired_js_call = boost::shared_ptr<FB::JSAPI>();
                if (tmptr != boost::shared_ptr<FB::JSAPI>()) {
                    this->m_host->releaseJSAPIPtr(tmptr);
                }
                // 就这样清理，那么浏览器的js变量值会变成null了，没有问题。
                if (1) {
                    this->m_host->releaseJSAPIPtr(shptr);
                    // this->m_host->DoDeferredRelease ();
                }
                fprintf(stderr, "find call object with id: %s, ref: %d\n", 
                        call_id.c_str(), shptr.use_count());
                break; // not dup call id, can break now
            }
        }
    }

    if (!found) {
        // 
        fprintf(stderr, "Can not find call object with id: %s\n", call_id.c_str());
    } else {
    }

    return 0;
}

int Skype4WWWAPI::connection_status_changed(std::string connection_status)
{
    if (this->connection_status_map.empty()) {
        this->connection_status_map.insert(std::make_pair<std::string,int>(std::string("OFFLINE"), conOffline));
        this->connection_status_map.insert(std::make_pair<std::string,int>(std::string("ONLINE"), conOnline));
        this->connection_status_map.insert(std::make_pair<std::string,int>(std::string("CONNECTING"), conConnecting));
        this->connection_status_map.insert(std::make_pair<std::string,int>(std::string("PAUSING"), conPausing));
    }

    int istatus = this->connection_status_map.count(connection_status) == 0 ?
        conUnknown : this->connection_status_map[connection_status];

    this->FireEvent("onConnectionStatus", FB::variant_list_of(istatus));

    fprintf(stderr, "return connection status changed: %s,%d\n", connection_status.c_str(), istatus);    

    return 0;
}

// 所有被fire的JSAPIPtr要记住，并在下次状态变化时释放上一次fire的引用。
int Skype4WWWAPI::call_status_changed(std::string call_id, std::string call_status)
{
    boost::shared_ptr<FB::JSAPI> shptr, tmptr;
    SkypeCall *ocall = NULL;

    shptr = this->js_calls[0];

    if (this->call_status_map.empty()) {
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("UNPLACED"), clsUnplaced));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("ROUTING"), clsRouting));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("RINGING"), clsRinging));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("FINISHED"), clsFinished));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("CANCELLED"), clsCancelled));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("REFUSED"), clsRefused));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("MISSED"), clsMissed));
        this->call_status_map.insert(std::make_pair<std::string,int>(std::string("FAILED"), clsFailed));
    }
    int istatus = this->call_status_map.count(call_status)==0 ?
        clsUnknown: this->call_status_map[call_status];
    fprintf(stderr, "call call_id status changed: %s,%d\n", call_status.c_str(), istatus);
    if (shptr != boost::shared_ptr<FB::JSAPI>()) {
        ocall = static_cast<SkypeCall*>((shptr->shared_ptr().get()));
        if (ocall->call_id == call_id) {
            this->FireEvent("onCallStatus", FB::variant_list_of(shptr)(istatus));
            fprintf(stderr, "fired call call_id status changed: %s,%d, ref:%d\n", call_status.c_str(), istatus, shptr.use_count());

            tmptr = this->last_fired_js_call;
            this->last_fired_js_call = shptr;
            if (tmptr != boost::shared_ptr<FB::JSAPI>()) {
                this->m_host->releaseJSAPIPtr(tmptr);
            }
        }
    }

    fprintf(stderr, "return call call_id status changed: %s,%d\n", call_status.c_str(), istatus);
    return 0;
}

bool Skype4WWWAPI::on_plugin_ready()
{
    this->ptr_Client->on_plugin_ready();
    return true;
}

// Read/Write property testString
std::string Skype4WWWAPI::get_testString()
{
    return m_testString;
}
void Skype4WWWAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string Skype4WWWAPI::get_version()
{
    // return "CURRENT_VERSION";
    return "1.0.3.23";
}

// Method echo
FB::variant Skype4WWWAPI::echo(const FB::variant& msg)
{
    // throw FB::script_error("hhhhhhhhhhhhhhahaha");
    return msg;
}

void Skype4WWWAPI::testEvent(const FB::variant& var)
{
    FireEvent("onfired", FB::variant_list_of(var)(true)(1));
}

////////////////////////////
//////
/////////////////////////////
SkypeClient::SkypeClient()
    : FB::JSAPIAuto("<JSAPI-Auto-SkypeClient Javascript Object>")
{
    this->IsRunning = 0;
    // this->IsRunning = is_skype_running();
    // 这个在插件ready之前执行，时机不太好，还是在之后再更新这个值。
    // fprintf(stderr, "got skype running: %d\n", this->IsRunning);    
    
    registerMethod("Start", make_method(this, &SkypeClient::Start));
    registerProperty("IsRunning", make_property(this, &SkypeClient::get_IsRunning));
}

SkypeClient::~SkypeClient()
{
    fprintf(stderr, "~~~~%s\n", __FUNCTION__);
}

int SkypeClient::get_IsRunning()
{
    fprintf(stderr, "client object prop return\n");

    return this->IsRunning;
}

bool SkypeClient::Start(const FB::variant& Minimized, const FB::variant& Nosplash)
{
    fprintf(stderr, "client object Start return\n");
    bool bret = exec_skype();
    return bret;
    return true;
}

bool SkypeClient::on_plugin_ready()
{
    this->IsRunning = is_skype_running();
    fprintf(stderr, "got skype running: %d\n", this->IsRunning);
    return true;
}

///////////////////////////
////////
///////////////////////////
SkypeAccount::SkypeAccount()
    : FB::JSAPIAuto("<JSAPI-Auto-SkypeAccount Javascript Object>")
{
    registerProperty("Handle", make_property(this, &SkypeAccount::get_Handle));
    registerProperty("Fullname", make_property(this, &SkypeAccount::get_Fullname));
}
SkypeAccount::~SkypeAccount()
{
}

std::string SkypeAccount::get_Handle()
{
    return this->Handle;
}

std::string SkypeAccount::get_Fullname()
{
    return this->Fullname;
}

////////////////////////////
//////
/////////////////////////////
SkypeCall::SkypeCall()
    : FB::JSAPIAuto("<JSAPI-Auto-SkypeCall Javascript Object>")
{
    this->user_hangup = false;

    registerMethod("Finish", make_method(this, &SkypeCall::Finish));

    registerProperty("id", make_property(this, &SkypeCall::get_id));
}

SkypeCall::~SkypeCall()
{
    fprintf(stderr, "~~~~%s , %s, %d\n", __FUNCTION__, this->call_id.c_str(), this->user_hangup);
}

// 怎么把这个变量删掉呢
// 在接收到挂断信息的时候删除，应该更稳妥，只要浏览器不出问题，挂断信息总会收到
// 而如果浏览器崩溃，那么，一切皆空
// 在这只是设置一个状态位。叫用户挂断。
bool SkypeCall::Finish()
{
    fprintf(stderr, "hhhhh %s\n", __FUNCTION__);
    this->user_hangup = true;

    skype_send_message_nowait("ALTER CALL %s HANGUP", this->call_id.c_str());

    return true;
}

int SkypeCall::get_id()
{
    int id = atoi(this->call_id.c_str());
    fprintf(stderr, "return call id: %d\n", id);
    return id;
    return -1;
}

