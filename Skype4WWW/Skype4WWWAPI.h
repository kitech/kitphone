/**********************************************************\

  Auto-generated Skype4WWWAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "Skype4WWW.h"

#ifndef H_Skype4WWWAPI
#define H_Skype4WWWAPI

class SkypeClient : public FB::JSAPIAuto
{
public:
    SkypeClient();
    virtual ~SkypeClient();

    // method
    bool Start (const FB::variant& Minimized, const FB::variant& Nosplash);

    int get_IsRunning();
    //// 
    int IsRunning;

    bool on_plugin_ready();
};

class SkypeCall : public FB::JSAPIAuto
{
public:
    SkypeCall();
    virtual ~SkypeCall();

    bool Finish();

    // 
    int get_id();

    std::string call_id;
    std::vector<std::string> targets;

    bool user_hangup;
};

class SkypeAccount : public FB::JSAPIAuto
{
public:
    SkypeAccount();
    virtual ~SkypeAccount();

    std::string get_Handle();
    std::string get_Fullname();

    ////
    std::string Handle;
    std::string Fullname;
};

class Skype4WWWAPI : public FB::JSAPIAuto
{
public:
    Skype4WWWAPI(const Skype4WWWPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~Skype4WWWAPI();

    Skype4WWWPtr getPlugin();

    // method
    FB::variant Attach(const FB::variant& protocol, const FB::variant& wait);
    // method
    FB::JSAPIPtr PlaceCall(const FB::variant& target, const FB::variant& target2 = "",
                           const FB::variant& target3 = "", const FB::variant& target4 = "");

    // SkypeClient *get_Client();
    FB::JSAPIPtr get_Client();
    int get_AttachmentStatus();

    FB::JSAPIPtr get_CurrentUser();
    std::string get_ApiWrapperVersion();
    std::string get_Version();

    ///
    int hangup_call(std::string call_id);

    int connection_status_changed(std::string connection_status);
    int call_status_changed(std::string call_id, std::string call_status);


    bool on_plugin_ready();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    
    // Method test-event
    void testEvent(const FB::variant& s);

public:
    /////////////
    enum TAttachmentStatus {
        apiAttachUnknown = 127,
        apiAttachSuccess = 0,
        apiAttachPendingAuthorization,
        apiAttachRefused,
        apiAttachNotAvailable,
        apiAttachAvailable,
    };

    enum TConnectionStatus {
        conUnknown = 127,
        conOffline = 0,
        conConnecting,
        conPausing,
        conOnline,
    };

    enum TCallStatus {
        clsUnknown = 127,
        clsUnplaced = 0,
        clsRouting,
        clsEarlyMedia,
        clsFailed,
        clsRinging,
        clsInProgress,
        clsOnHold,
        clsFinished,
        clsMissed,
        clsRefused,
        clsBusy,
        clsCancelled,
        clsLocalHold,
        clsRemoteHold,
        clsVoicemailBufferingGreeting,
        clsVoicemailPlayingGreeting,
        clsVoicemailRecording,
        clsVoicemailUploading,
        clsVoicemailSent,
        clsVoicemailCancelled,
        clsVoicemailFailed,
        clsTransferring,
        clsTransferred,
    };

private:
    Skype4WWWWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;

    ////////
    std::string ApiWrapperVersion; //
    std::string Version; // this is skype version
    SkypeClient *ptr_Client;
    FB::JSAPIPtr js_Client;
    int AttachmentStatus;

    SkypeAccount *ptr_CurrentUser;
    FB::JSAPIPtr js_CurrentUser;

    boost::shared_ptr<SkypeClient> boo_client;

    FB::JSAPIPtr js_calls[32];
    FB::JSAPIPtr last_fired_js_call;

    std::map<std::string,int> call_status_map;
    std::map<std::string,int> attachment_status_map;
    std::map<std::string,int> connection_status_map;

};

#endif // H_Skype4WWWAPI

