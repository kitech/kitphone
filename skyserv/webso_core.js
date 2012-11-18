//
// Copyright tomonline-inc.com
// 2011-01-10
//
// need jquery 1.5.x 
//

function WebSkypeout() {
    this.version = "0.9.85";
    this.debug = true; // LOG_LEVEL_USER and LOG_LEVEL_DEV
    this.com_load = true;
    this.ouser = null;
    this.ocall = null;
    this.wsr = null;
    // this.wsf = null;
    this.wsr_should_close = false;
    // this.wsf_should_close = false;
    this.is_calling = false;
    this.CALL_CTRL_READY = 0;
    this.CALL_CTRL_CONNECTING_WSR = 1;
    this.CALL_CTRL_CONNECTING_WSR_TIMEOUT = 2;
    this.CALL_CTRL_CONNECTED_WSR = 3;
    this.CALL_CTRL_CONNECT_WSR_FAILD = 4;
    this.CALL_CTRL_CONNECTION_WSR_UNEXCEPTED_CLOSE = 5;
    // this.CALL_CTRL_WAITING_WSF_INFO = 6;
    // this.CALL_CTRL_WAIT_WSF_INFO_TIMEOUT = 7;
    // this.CALL_CTRL_CONNECTING_WSF = 8;
    // this.CALL_CTRL_CONNECTING_WSF_TIMEOUT = 9;
    // this.CALL_CTRL_CONNECTED_WSF = 10;
    // this.CALL_CTRL_CONNECT_WSF_FAILD = 11;
    // this.CALL_CTRL_CONNECTION_WSF_UNEXCEPTED_CLOSE = 12;
    this.CALL_CTRL_INCALL = 13;
    this.CALL_CTRL_FINISHED = 14;
    this.call_ctrl_state = this.CALL_CTRL_READY;
    this.MAX_CONN_RETRY_TIMES = 3;
    this.conn_wsr_retry_times = 0;
    // this.conn_wsf_retry_times = 0;
    this.PREPOST_TYPE_AJAX = 1;
    this.PREPOST_TYPE_WEBSOCKET = 2;
    this.prepost_type = this.PREPOST_TYPE_WEBSOCKET;
    this.prepost_button = $("#websocket_send_skypeout_call_button");
    // this.wsr_uri_base = "ws://202.108.12.212:8060/";
    this.wsr_uri_base = "ws://" + GWEB_SOCKET_ADDR + ":8000/";
    // this.wsr_uri_base = "ws://61.138.133.4:8060/";
    // this.wsf_uri_base = ""; // default empty, should like ws://host:port/
    // this.wsf_line_name = null;
    // this.wait_wsf_timer = -1;
    this.wait_conn_wsr_timer = -1;
    this.call_start_time = null;
    this.ajax_request_timer = -1;

    this.init_web_skype = function() {
	if (this.com_load == false) {
	    $("#manual_skype_id_div").attr('style', "display:inline");
	    $("#manual_skype_id_hint").attr('style', "display:inline");
	    return;
	}

	if (!Skype.Client) {
	    log_output("您还没有安装Skype客户端。", 0);
	    return;
	}
	var skype_is_running = false;
	try {
	    skype_is_running = Skype.Client.IsRunning;
	} catch (err) {
	    alert(err.description);
	    return;
	}
	// if (!Skype.Client.IsRunning) {
	if (!skype_is_running) {
	    // $("#skype_log").append("Skype 客户端没有启动，正在尝试启动 ...\n<br>");
	    log_output("Skype 客户端没有启动，正在尝试启动 ...", 0);
	    try {
		var ret = Skype.Client.Start(false);
	    } catch (err) {
		log_output("Skype 客户端启动失败，您还没有安装Skype客户端。", 0);
		return;
	    }
	    // alert(ret);
	    // $("#skype_log").append("Skype 客户端已启动.\n<br>");
	    log_output("Skype 客户端已启动.", 0);
	}

	var retry_times = 3;
	while (retry_times > 0) {
	    var ret = Skype.Attach(8, 0);
	    // alert(Skype.AttachmentStatus);   
	    if (Skype.AttachmentStatus != 1 && Skype.AttachmentStatus != 0) {
		retry_times --;
		if (retry_times == 0) {
		    // $("#skype_log").append('请先登陆Skype，再刷新本页面。' + Skype.AttachmentStatus + "\n<br>");
		    log_output('请先登陆Skype，再刷新本页面。' + Skype.AttachmentStatus, 0);
		    // alert('请先登陆Skype，再刷新本页面。' + Skype.AttachmentStatus);
		    return;
		}
	    } else {
		break;
	    }
	}

	this.ouser = Skype.CurrentUser;
	if (this.ouser == null || this.ouser.Handle == '') {
	    alert('skype not logged in, or auth rejected by you');
	} else {
	    // document.getElementById("handle_name").innerText = this.ouser.Handle;
	    // document.getElementById("display_name").innerText = this.ouser.Fullname;
	    $("#handle_name").attr("innerText", this.ouser.Handle);
	    $("#display_name").attr("innerText", "(" + this.ouser.Fullname + ")");
	}
	log_output("You are using ApiWrapper: " + Skype.ApiWrapperVersion);
	log_output("You are using Skype: " + Skype.Version);
    }

    this.init_web_storage_onloaded = function () {
	var iv = iws_get_item('skid');
	if (iv != null) {
	    $('#skype_id').attr('value', iv);
	} else {
	    $('#skype_id').attr('value', "Guest");
	}
	iv = iws_get_item('lpn');
	if (iv != null) {
	    $('#phone_number').attr('value', iv);
	}
	iv = iws_get_item('lcd');
    }

    this.on_focus_skype_id_input = function () {
	var obj_skype_id = $("#skype_id");
	var cur_val = obj_skype_id.attr("value");
	if (cur_val == '' || cur_val == "Guest") {
	    obj_skype_id.attr("value", "Guest");
	    obj_skype_id.select();
	}
    }

    this.on_blur_skype_id_input = function () {
	var obj_skype_id = $("#skype_id");
	var cur_val = obj_skype_id.attr("value");
	if (cur_val == '') {
	    obj_skype_id.attr("value", "Guest");
	}
    }

    this.on_focus_phone_number_input = function () {
	var obj_phone_number = $("#phone_number");
	var cur_val = obj_phone_number.attr("value");
	if (cur_val.length == 0) {
	    obj_phone_number.attr("value", "星号（*）加电话号码");
	    obj_phone_number.select();
	} else if (cur_val.substr(0, 2) == "*0"
	    || cur_val.substr(0, 7) == "9900866") {
	    // okkkkkk
	} else {
	    // obj_phone_number.attr("value", "星号（*）加电话号码");
	    obj_phone_number.select();
	}
    }
    this.on_blur_phone_number_input = function () {
	var obj_phone_number = $("#phone_number");
	var cur_val = obj_phone_number.attr("value");
	if (cur_val == '') {
	    obj_phone_number.attr("value", "星号（*）加电话号码");
	}
    }

    this.get_usable_skype_id = function() {
	var skype_id = null;
	//
	if (this.com_load == false) {
	    skype_id = $('#skype_id').attr("value");
	    if (skype_id == '' || skype_id == null) {
		alert('请输入您当前登陆的skype账号。');
		return null;
	    }
	} else {
	    skype_id = this.ouser ? this.ouser.Handle : null;
	}
	return skype_id;
    }

    this.get_phone_number = function() {
	var phone_number = $("#phone_number").attr("value");
	if (phone_number == null) {
	
	}
	return phone_number;
    }

    this.check_phone_number = function(phone_number) {
	if (phone_number.length < 8) {
	    return false;
	}
	if (phone_number.length > 26) {
	    return false;
	}
	// 数字或者*
	if (phone_number.substr(0, 2) != '*0'
	    && phone_number.substr(0, 7) != '9900866') {
	    return false;
	}
	
	for (var i = 0; i < phone_number.length; i ++) {
	    if (phone_number.charAt(i) == '*') {
		continue;
	    }
	    if (phone_number.charAt(i) >= '0' && phone_number.charAt(i) <= '9') {
		continue;
	    } else {
		return false;
	    }
	}
	return true;
	return false;
    }

    this.support_com_ctrl = function () {
	return ($.browser.msie && this.com_load);
    }

    // depend: swfobject.js
    this.detect_real_support_websocket = function () {
	// 对于IE6+flash9, 已经测试出来有问题
	var flash_version = swfobject.getFlashPlayerVersion();
	var ie_version = 100;

	if ($.browser.msie) {
	    var re  = new RegExp("MSIE ([0-9]{1,}[\.0-9]{0,})");
	    re.exec(navigator.userAgent);
	    ie_version = parseFloat(RegExp.$1);
	    str_ie_version = RegExp.$1;

	    log_output("Flash version: " + flash_version.major + "." + flash_version.minor);
	    log_output("IE version: " + str_ie_version);

	    if (flash_version.major <= 9 && ie_version <= 6.9) {
		 return false;
	    }
	}
	if (typeof WebSocket == 'undefined') {
	    return false;
	}
	return true;
    }

    // dispatch method
    this.place_call_init = function () {
	if (!this.detect_real_support_websocket()) {
	    this.prepost_type = this.PREPOST_TYPE_AJAX;
	    // this.prepost_button = $("#ajax_send_skypeout_call_button");
	    this.prepost_button = $("#websocket_send_skypeout_call_button");

	    log_output("using ajax mode, feature missed.");
	    this.place_call_init_ajax();
	} else {
	    this.prepost_type = this.PREPOST_TYPE_WEBSOCKET;
	    this.prepost_button = $("#websocket_send_skypeout_call_button");

	    this.place_call_init_websocket();
	}
    }

    // this is really be place_call_init_ajax
    this.place_call_init_ajax = function () {
	var skype_id = null;

	if (this.com_load == false) {
	    skype_id = document.getElementById('skype_id').value;
	    if (skype_id == '' || skype_id == null) {
		alert('请输入您当前登陆的skype账号');
		return;
	    }
	} else {
	    skype_id = this.ouser ? this.ouser.Handle : null;
	}
 	this.prepost_button.attr("disabled", true);

	var phone_number = this.get_phone_number();
	if (phone_number == null) {
	    log_output("电话号码不能为空。", 0);
	    this.prepost_button.attr("disabled", false);
	    return;
	}
	if (!this.check_phone_number(phone_number)) {
	    log_output("电话号码格式不正确。", 0);
	    this.prepost_button.attr("disabled", false);
	    return;
	}

	var turl = "skype_gateway.php?func=notepair&caller_name="
	+ skype_id + "&callee_phone=" + phone_number
	+ '&hinfo=' + platform_info_collection()
	+ "&rand=" + Math.random();
	log_output("Preparing call " + skype_id + " ->  " + phone_number);

	// alert(turl);
	$.ajax({
		type: "GET",
		    dataType: "text",
		    url: turl,
		    cache: false,
		    success: rthis.place_call_ajax_return,
		    error: rthis.place_call_ajax_faild
		    });
    }

    this.place_call_init_websocket = function() {
	log_output("正在呼叫...", 0);

	var skype_id = this.get_usable_skype_id();
	if (skype_id == null) {
	    log_output("Skype 用户名不能为空。", 0);
	    return;
	}
	iws_set_item("skid", skype_id);
	iws_set_item("lcd", new Date());

	//
	this.prepost_button.attr("disabled", true);

	var phone_number = this.get_phone_number();
	if (phone_number == null) {
	    log_output("电话号码不能为空。", 0);
	    this.prepost_button.attr("disabled", false);
	    return;
	}
	if (!this.check_phone_number(phone_number)) {
	    log_output("电话号码格式不正确。", 0);
	    this.prepost_button.attr("disabled", false);
	    return;
	}

// 	var turl = "skype_gateway.php?func=notepair&caller_name="
// 	+ skype_id + "&callee_phone=" + phone_number
// 	+ '&hinfo=' + platform_info_collection()
// 	+ "&rand=" + Math.random();

	if (this.wsr != null) {
	    log_output("wsr is not null. omit request.");
	    log_output("呼叫中，请勿重复点击呼叫按钮。", 0); // ??? 也许不太准确。
	    return;
	}

	iws_set_item("lpn", phone_number);
	log_output("Preparing call " + skype_id + " ->  " + phone_number);

	this.wait_conn_wsr_timer = window.setTimeout(this.wait_conn_wsr_timeout, 5000);
	this.call_ctrl_state_machine(this.CALL_CTRL_CONNECTING_WSR);
	this.wsr_should_close = false;
	var ws_uri = this.wsr_uri_base + skype_id + "/";
	this.wsr = new WebSocket(ws_uri);

	// Set event handlers.
	this.wsr.onopen = this.wsr_cb_onopen;
	this.wsr.onmessage = this.wsr_cb_onmessage;

	this.wsr.onclose = this.wsr_cb_onclose;
	this.wsr.onerror = this.wsr_cb_onerror;

	log_output("Begin connecting to wsr...");
    }

    //// wsr callbacks
    this.wsr_cb_onopen = function () {
	log_output("wrs onopen " + rthis.debug); // this this is not outer's this!!!
	rthis.call_ctrl_state_machine(rthis.CALL_CTRL_CONNECTED_WSR);

	if (rthis.wait_conn_wsr_timer != -1) {
	    window.clearTimeout(rthis.wait_conn_wsr_timer);	    
	    rthis.wait_conn_wsr_timer = -1;
	}

	var skype_id = rthis.get_usable_skype_id();
	log_output("wrs onopen" + skype_id);
	var phone_number = rthis.get_phone_number();
	var cmdline = "101$" + skype_id + "$" + phone_number;
	log_output("wrs onopen" + cmdline);
	log_output("wssending: " + cmdline);

	// rthis.call_ctrl_state_machine(rthis.CALL_CTRL_WAITING_WSF_INFO);
	rthis.wsr.send(cmdline);
    }

    this.wsr_cb_onmessage = function (evt) {
	var msg = evt.data;
	log_output("wrs onmessage: " + msg);

	var fields = msg.split("$");
	log_output("wsr Return value count:" + fields.length);
	switch (parseInt(fields[0])) {
	case 100:
	    // log_output("Stop wait wsf timeout:"+rthis.wait_wsf_timer);
	    // window.clearTimeout(rthis.wait_wsf_timer);
	    // rthis.wait_wsf_timer = -1;

	    // if (rthis.wsf != null) {
		
	    // }

	    // close and new wsf if ok
	    // rthis.wsf_uri_base = "ws://" + fields[3] + ":" + fields[4] + "/";
	    // rthis.wsf_line_name = fields[1];
	    // var ws_uri = rthis.wsf_uri_base + rthis.wsf_line_name + "/";
	    // rthis.wsf = new WebSocket(ws_uri);
	    // Set event handlers.
	    // rthis.wsf.onopen = rthis.wsf_cb_onopen;
	    // rthis.wsf.onmessage = rthis.wsf_cb_onmessage;
	    // rthis.wsf.onclose = rthis.wsf_cb_onclose;
	    // rthis.wsf.onerror = rthis.wsf_cb_onerror;

	    // log_output("Begin connecting to wsf...");
	    break;
	case 102:    
	    if (fields[1] != rthis.get_usable_skype_id()) {
		log_output("Warning: is this your message really.");
	    }
	    rthis.place_call_s2(fields[2]);
	    break;
	case 104:
	    log_output("RCall notice: 线路忙，请稍后再拨。" + fields[3], 0);
	    break;
	case 108:
// 	    if (rthis.wait_wsf_timer != -1) {
// 		log_output("Stop wait wsf timeout:"+rthis.wait_wsf_timer);
// 		window.clearTimeout(rthis.wait_wsf_timer);
// 		rthis.wait_wsf_timer = -1;
// 	    }

	    if (rthis.wsr) {
		log_output("通话已中断。", 0);
		rthis.wsr_should_close = true;
		rthis.wsr.close();
	    } else {
		log_output("hangup some else .");
	    }

	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_FINISHED);
	    break;
	case 106:
	    log_output("FCall notice: " + fields[4]);
	    break;
	case 108:
	    var call_stop_time = new Date();
	    var answered_time;
	    var answered_minite = '';
	    if (rthis.call_start_time != null) {
		answered_time = call_stop_time.getTime() - rthis.call_start_time.getTime();
		answered_minite = answered_time / 1000.0 / 60.0;
		answered_minite = format_time_readable(answered_time);
		log_output("通话结束，通话时间约为: " + answered_minite + " 。", 0);
	    } else {
		log_output("通话结束。", 0);
	    }

	    // rthis.wsf_should_close = true;
	    // if (rthis.wsf) rthis.wsf.close();

	    rthis.wsr_should_close = true;
	    if (rthis.wsr) rthis.wsr.close();

	    rthis.call_start_time = null;
	    call_stop_time = null;
	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_FINISHED);
	    break;
	case 110: // hold/unhold
	    log_output("不支持通话挂起功能，请尽快恢复，否则对方可能因听不到您的声音而挂断。", 0);
	    break;
	case 112: // call answer state begin, answer time start
	    log_output("对方已经接通，计时开始。", 0);
	    rthis.call_start_time = new Date();
	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_INCALL);
	    break;
	case 114: // media active, maybe
	    log_output("可能会有2-3秒静音时间，请稍后。", 0);
	    break;
	default:
	    log_output("Unknown wsr call notice message no:" + fields[0]);
	    break;
	}
    }

    this.wsr_cb_onclose = function() {
	rthis.wsr = null;
	log_output("wrs onclose.");

	if (rthis.wait_conn_wsr_timer != -1) {
	    window.clearTimeout(rthis.wait_conn_wsr_timer);	    
	    rthis.wait_conn_wsr_timer = -1;
	}

	if (rthis.wsr_should_close == true) {
	    rthis.wsr_should_close = false;
	} else {
	    log_output("Unexpected wsr close event.");
	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_CONNECTION_WSR_UNEXCEPTED_CLOSE);
	}
    }

    this.wsr_cb_onerror = function () {
	// 这个函数好象从来没有调用过，应该不会出现这种状态。
	rthis.wsr = null;
	log_output("wrs onerror.");
	rthis.call_ctrl_state_machine(rthis.CALL_CTRL_CONNECT_WSR_FAILD);
    }

    this.wait_conn_wsr_timeout = function() {
	log_output("connect wrs timeout.");
	rthis.call_ctrl_state_machine(rthis.CALL_CTRL_CONNECTING_WSR_TIMEOUT);
    }

    // wsf callbacks
    // depcreated
//     this.wsf_cb_onopen = function (){
// 	log_output("wsf onopen " + rthis.debug); // this this is not outer's this!!!
//     }
    // depcreated
//     this.wsf_cb_onmessage = function (evt) {
// 	var msg = evt.data;
// 	log_output("wsf onmessage: " + msg);

// 	var fields = msg.split("$");
// 	log_output("wsf Return value count:" + fields.length);
// 	switch (parseInt(fields[0])) {
// 	case 106:
// 	    log_output("FCall notice: " + fields[4]);
// 	    break;
// 	case 108:
// 	    var call_stop_time = new Date();
// 	    var answered_time;
// 	    var answered_minite = '';
// 	    if (rthis.call_start_time != null) {
// 		answered_time = call_stop_time.getTime() - rthis.call_start_time.getTime();
// 		answered_minite = answered_time / 1000.0 / 60.0;
// 		answered_minite = format_time_readable(answered_time);
// 		log_output("通话结束，通话时间约为: " + answered_minite + " 。", 0);
// 	    } else {
// 		log_output("通话结束。", 0);
// 	    }

// 	    rthis.wsf_should_close = true;
// 	    if (rthis.wsf) rthis.wsf.close();

// 	    rthis.wsr_should_close = true;
// 	    if (rthis.wsr) rthis.wsr.close();

// 	    rthis.call_start_time = null;
// 	    call_stop_time = null;
// 	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_FINISHED);
// 	    break;
// 	case 110: // hold/unhold
// 	    log_output("不支持通话挂起功能，请尽快恢复，否则对方可能因听不到您的声音而挂断。", 0);
// 	    break;
// 	case 112: // call answer state begin, answer time start
// 	    log_output("对方已经接通，计时开始。", 0);
// 	    rthis.call_start_time = new Date();
// 	    rthis.call_ctrl_state_machine(rthis.CALL_CTRL_INCALL);
// 	    break;
// 	case 114: // media active, maybe
// 	    log_output("可能会有2-3秒静音时间，请稍后。", 0);
// 	    break;
// 	default:
// 	    log_output("Unknown wsf call notice message no:" + fields[0]);
// 	    break;
// 	}
//     }
    // depcreated
//     this.wsf_cb_onclose = function () {
// 	rthis.wsf = null;
// 	log_output("wsf onclose.");
// 	if (rthis.wsf_should_close == true) {
// 	    rthis.wsf_should_close = false;
// 	} else {
// 	    log_output("Unexpected wsf close event.");
// 	    rthis.call_ctrl_state_machine(rthis.CONNECTION_WSF_UNEXCEPTED_CLOSE);
// 	}
//     }
    // depcreated
//     this.wsf_cb_onerror = function () {
// 	rthis.wsf = null;
// 	log_output("wsf onerror.");
//     }
    // depcreated
//     this.wait_wsf_info_timeout = function () {
// 	log_output("progress timeout, check your supplied account and login skype account");
// 	// log_output("T: " + (this == rthis));

// 	rthis.wsr_should_close = true;
// 	if (rthis.wsr) {
// 	    rthis.wsr.close();
// 	}
//     }

    this.call_ctrl_state_machine = function (to_state) {
	var old_state = this.call_ctrl_state;
	switch (to_state) {
	case this.CALL_CTRL_READY:
	    break;
	case this.CALL_CTRL_CONNECTING_WSR:
	    this.call_ctrl_state = to_state;
	    this.wsr_should_close = false;
	    break;
	case this.CALL_CTRL_CONNECTING_WSR_TIMEOUT:
	    this.conn_wsr_retry_times += 1;
	    if (this.conn_wsr_retry_times > this.MAX_CONN_RETRY_TIMES) {
		log_output("Retry conn wsr exceed max times.");
		window.clearTimeout(this.wait_conn_wsr_timeout);
		this.wait_conn_wsr_timer = -1;
		this.wsr = null;
		this.conn_wsr_retry_times = 0;
		this.call_ctrl_state = this.CALL_CTRL_CONNECT_WSR_FAILD;
		log_output("拨打失败，连接服务器1失败。", 0);
		break;
	    }
	    this.wait_conn_wsr_timer = window.setTimeout(this.wait_conn_wsr_timeout, 5000);
	    this.call_ctrl_state = this.CALL_CTRL_CONNECTING_WSR;
	    var skype_id = this.get_usable_skype_id();
	    var ws_uri = this.wsr_uri_base + skype_id + "/";
	    this.wsr = null;
	    this.wsr = new WebSocket(ws_uri);

	    // Set event handlers.
	    this.wsr.onopen = this.wsr_cb_onopen;
	    this.wsr.onmessage = this.wsr_cb_onmessage;

	    this.wsr.onclose = this.wsr_cb_onclose;
	    this.wsr.onerror = this.wsr_cb_onerror;

	    log_output("Begin " + this.conn_wsr_retry_times + "th reconnecting to wsr...");
	    break;
	case this.CALL_CTRL_CONNECTION_WSR_UNEXCEPTED_CLOSE:
	    if (this.call_ctrl_state == this.CALL_CTRL_CONNECTING_WSR) {
		this.call_ctrl_state_machine(this.CALL_CTRL_CONNECTING_WSR_TIMEOUT);
	    } else {
		// if already got server info, can not call  upper procedue
		log_output("task done, but unexecpted close.");
	    }
	    break;
	    // case this.CALL_CTRL_CONNECTING_WSF_TIMEOUT:
	    // 重试一次。
	    // this.conn_wsf_retry_times += 1;
// 	    if (this.conn_wsf_retry_times > this.MAX_CONN_RETRY_TIMES) {
// 		log_output("Retry conn wsf exceed max times.");
// 		this.wsf = null;
// 		this.call_ctrl_state = this.CALL_CTRL_CONNECT_WSF_FAILD;
// 		// log_output("拨打失败，连接服务器2失败。", 0);
// 		this.conn_wsf_retry_times = 0;
// 		log_output("连接服务器失败，您可能无法收到拨打进度通知。", 0);
// 		break;
// 	    }
	    // this.call_ctrl_state = this.CALL_CTRL_CONNECTING_WSF;
	    // var ws_uri = this.wsf_uri_base + this.wsf_line_name + "/";
	    // this.wsf = null;
	    // this.wsf = new WebSocket(ws_uri);
	    // Set event handlers.
	    // this.wsf.onopen = this.wsf_cb_onopen;
	    // this.wsf.onmessage = this.wsf_cb_onmessage;
	    // this.wsf.onclose = this.wsf_cb_onclose;
	    // this.wsf.onerror = this.wsf_cb_onerror;

	    // log_output("Begin " + this.conn_wsf_retry_times + "th reconnecting to wsf...");
	    // break;
	case this.CALL_CTRL_INCALL:
	    this.call_ctrl_state = to_state;
	    break;
	case this.CALL_CTRL_FINISHED:
	    this.call_ctrl_state = this.CALL_CTRL_READY;
	    // this.conn_wsf_retry_times = 0;
	    this.conn_wsr_retry_times = 0;
	    // this.wsf_line_name = null;
	    this.wsr = null;
	    // this.wsf = null;
	    break;
	    // case this.CALL_CTRL_WAITING_WSF_INFO:
	    // this.call_ctrl_state = to_state;
	    // break;
	default:
	    log_output("Unknown/Unsupported Call State: " + to_state + ".");
	    break;
	}
    }

    this.place_call_ajax_return = function (msg, status, obj) {
	// alert("success response:\n" + msg);
	log_output("ajax return:" + msg);
	var lines = msg.split("\n");
	// alert(lines[0]);
	if (lines[0] == "200 OK" && lines[2].length > 0) {
	    // alert("ready for place call...");
	    log_output("Ready for place call, starting call...");
	    // log_output("Start wsf wait timer:" + rthis.wait_wsf_timer);
	    // rthis.wait_wsf_timer = window.setTimeout(rthis.wait_wsf_info_timeout, 8000);
	    // log_output("Start wsf wait timer:" + rthis.wait_wsf_timer);

	    rthis.place_call_real(lines[2]);
	}
	// $("#send_skypeout_call_button").attr("disabled", false);
	// $("#hangup_skypeout_call_button").attr("disabled", false);
	rthis.prepost_button.attr("disabled", false);
    }

    this.place_call_ajax_faild = function (status, msg) {
	var ajax_obj = this;
	log_output("prepost ajax request faild." + status);
    }

    this.place_call_ajax_timeout = function () {
	log_output("prepost ajax request timeout.");
    }

    this.place_call_s2 = function (msg) {
	// alert("success response:\n" + msg);
	var lines = msg.split("\n");
	// alert(lines[0]);
	if (lines[0] == "200 OK" && lines[2].length > 0) {
	    // alert("ready for place call...");
	    log_output("Ready for place call, starting call...");
	    // log_output("Start wsf wait timer:" + this.wait_wsf_timer);
	    // this.wait_wsf_timer = window.setTimeout(this.wait_wsf_info_timeout, 8000);
	    // log_output("Start wsf wait timer:" + this.wait_wsf_timer);

	    this.place_call_real(lines[2]);
	}
	// $("#send_skypeout_call_button").attr("disabled", false);
	// $("#hangup_skypeout_call_button").attr("disabled", false);
	this.prepost_button.attr("disabled", false);
    }

    this.place_call_real = function (skype_id) {
	if (this.com_load == false) {
	    var call_a = document.getElementById("manual_call_a");
	    $("#manual_call_a").attr('href', "skype:" + skype_id + "?call");

	    // log_output("test pause");
	    // return;
	    if ($.browser.mozilla || $.browser.webkit) {
		window.location = $("#manual_call_a").attr('href');
	    } else {
		call_a.click();
	    }
	    return;
	} else {
	    this.ocall = Skype.PlaceCall(skype_id, "", "", "");
	}
    }
    
    this.hangup_call = function () {
	if (this.ocall == null) {
	    if ($.browser.msie) {
		// alert("No active call.");
		log_output("没有正在进行的通话。", 0);
	    } else {
		// send cmd for hangup
		var cmdline = "107$" + this.get_usable_skype_id() + "$fffffff$0";
		if (this.wsr != null) {
		    log_output("发送挂断请求，请稍后...", 0);
		    this.wsr.send(cmdline);
		} else {
		    log_output("trouble hangup here, try hangup by skype.");
		    log_output("由于服务器问题未能挂断通话，请尝试通过skype客户端挂断。", 0);
		}
		// log_output("该浏览器暂不支持页面挂断，请在skype客户中点击挂断。", 0);
	    }
	    return;
	} else {
	    this.ocall.Finish();
	    this.ocall = null;
	}
    }

    this.place_call_test = function () {
	// ocall = Skype.PlaceCall("echo123", "", "", "");
	this.place_call_real("echo123");
    }


    this.skype_com_load_faild = function () {
	this.com_load = false;
	if ($.browser.msie) {
	    document.write("<span>提示：加载Skype web 插件失败。您可以手动输入Skype账号拨打电话。</span><br>");
	} else {
	    // i already known this is not possible.
	    // document.write("failed\n");
	}
	$("#manual_skype_id_div").attr('style', "display:inline");
	$("#manual_skype_id_hint").attr('style', "display:inline");
    }

    this.is_allowed_skype_version = function (cver) {
	return true;
    }

    this.check_browser_compatibility = function () {
	log_output("--- 开始检测...", 0);

	// 对于IE6+flash9, 已经测试出来有问题
	var flash_version = swfobject.getFlashPlayerVersion();
	var ie_version = 100;

	if ($.browser.msie) {
	    // check ie 版本
	    var re  = new RegExp("MSIE ([0-9]{1,}[\.0-9]{0,})");
	    re.exec(navigator.userAgent);
	    ie_version = parseFloat(RegExp.$1);
	    str_ie_version = RegExp.$1;

	    var msg = "IE版本" + str_ie_version;
	    if (ie_version <= 5.9) {
		log_output("ERR " + msg + " 不可用，需要升级IE8", 0);		
	    } else if (ie_version <= 7.9) {
		log_output("OK " + msg + " 可用，建议升级IE8", 0);
	    } else if (ie_version <= 8.9) {
		log_output("OK " + msg + " 可用", 0);
	    } else {
		// >= 9.0
		log_output("ERR " + msg + " 暂不支持", 0);
	    }
	    
	    // check com 
	    msg = "Skype网页插件";
	    if (this.com_load == false) {
		log_output("ERR " + msg + " 没有安装", 0); 
	    } else {
		log_output("OK " + msg + " 已安装", 0); 
	    }
	    
	    // check flash

	    log_output("Flash version: " + flash_version.major + "." + flash_version.minor);
	    log_output("IE version: " + str_ie_version);

	    msg = "Flash网页插件" + flash_version.major + "." + flash_version.minor;
	    if (flash_version.major <= 9 && ie_version <= 6.9) {
		log_output("ERR " + msg + " 未安装，或者版本太低", 0); 
	    } else {
		log_output("OK " + msg + " 可用", 0); 
	    }
	}

	if ($.browser.opera) {

	}

	if ($.browser.mozilla) {

	}

	var msg = "WebSocket功能";
	if (typeof WebSocket == 'undefined') {
	    log_output("ERR " + msg + " 不兼容", 0); 
	    log_output("--- 检测完毕。", 0);
	} else {
	    log_output("OK " + msg + " 可用", 0); 

	    msg = "连接语音服务器";
	    var is_connected = false;
	    var ws_uri = rthis.wsr_uri_base + "tomcheckcn/";

	    var local_wsr = new WebSocket(ws_uri);
	    local_wsr.onopen = function () {
		is_connected = true;
		log_output("OK " + msg + " 连接成功", 0);
		local_wsr.close();
		log_output("--- 检测完毕。", 0);
	    };
	    local_wsr.onmessage = function () {
	    };
	    local_wsr.onclose = function () {
		if (!is_connected) {
		    log_output("ERR " + msg + " 网络错误", 0);
		    log_output("--- 检测完毕。", 0);
		}
	    };
	    local_wsr.onerror = function () {
		if (!is_connected) {
		    log_output("ERR " + msg + " 协议错误", 0);
		    log_output("--- 检测完毕。", 0);
		}
	    };
	}
	
	// log_output("--- 检测完毕。", 0);

	return false;
    }

    // really myself
    var rthis = this;
};

var wso = new WebSkypeout();

$(document).ready(function () {
	// log_output("document ready.");
	wso.init_web_skype();
	wso.init_web_storage_onloaded();

	if (!wso.detect_real_support_websocket()) {
	    log_output("提示，您的IE浏览器和Flash插件版本太低，高级功能无法使用。", 0);
	    log_output("...   推荐升级到IE 8 和 Flashplayer 10.0以上版本。", 0);
	}

	// $("#phone_number").focus(wso.on_focus_phone_number_input);
	$("#skype_id").click(wso.on_focus_skype_id_input);
	$("#skype_id").blur(wso.on_blur_skype_id_input);
	$("#phone_number").click(wso.on_focus_phone_number_input);
	$("#phone_number").blur(wso.on_blur_phone_number_input);

	// 
	$("#check_coma").click(wso.check_browser_compatibility);
    });

$(window).load(function () {
	// log_output("window load.");
	// call from here is not very realtime
	bind_all_skype_event();
    });

$(window).unload(function () {
	// IE OK
	// log_output("window unload.");
	// alert("Are you want exit?");
    });

// call on window.load
function bind_all_skype_event()
{
    if (typeof(Skype) == 'undefined' || Skype == null) {
	log_output("unable to load activex");
    } else {
	var s = null;
	// s = Skype.attachEvent('AttachmentStatus', skype_event_attachment_status);
	// s = Skype.attachEvent('ConnectionStatus', skype_event_connection_status);
	// s = Skype.attachEvent('Error', skype_event_connection_status);
	// s = Skype.attachEvent('CallStatus', skype_event_call_status);
	// s = Skype.attachEvent('Reply', skype_event_reply);
    }
}
function skype_event_attachment_status(Status)
{
    if (wso.com_load == true) {
	var log = '#' + "Attach to Skype " + Status;
	log_output(log);
    }
}

function skype_event_connection_status(Status)
{
    if (wso.com_load == true) {
	var log = '#' + "Connect to Skype " + Status;
	// $("#skype_log").prepend(log);
	log_output(log);
	if (Status == 0) {
	    // 掉线或者登出,离线
	    log = "Skype 已离线。";
	    log_output(log, 0);
	} else if (Status == 3) {
	    // 上线
	}
    }
}

function skype_event_error(Cmd, Num, Desc)
{
    log_output("my event handle" + Status);
    if (wso.com_load == true) {
	var log = '#' + "Error: " + Cmd.id + ' ' + Cmd.Command + ' ' + Num + ' ' + Desc;
	// $("#skype_log").prepend(log);
	log_output(log);
    }
}

function skype_event_call_status(Call, Status)
{
    if (wso.com_load == true) {
        var log = '' + Call.id + ' STATUS ' + Status;
        log_output(log);
    }
    if (wso.support_com_ctrl()) {
        var istatus = parseInt(Status);
        // call stop
        if (istatus == 7 || istatus == 9 || istatus == 11) {
            $("#ajax_send_skypeout_call_button").attr('disabled', false);
            $("#websocket_send_skypeout_call_button").attr('disabled', false);
            $("#test_skypeout_call_button").attr('disabled', false);
	    $("#hangup_skypeout_call_button").attr('disabled', true);

	    if (istatus == 7) {
		wso.wsr_cb_onmessage({data: "108$123456$78910$from_this_hangup_call"});
	    }

// 	    if (wso.wsf != null) {
// 		wso.wsf.close();
// 		wso.wsf = null;
// 	    }
	    if (wso.wsr != null) {
		wso.wsr.close();
		wso.wsr = null;
	    }

	    ocall = null;
        }
        if (istatus == 0 || istatus == 1) {
            $("#ajax_send_skypeout_call_button").attr('disabled', true);
            $("#websocket_send_skypeout_call_button").attr('disabled', true);
            $("#test_skypeout_call_button").attr('disabled', true);
	    $("#hangup_skypeout_call_button").attr('disabled', false);
        }
    }
}

function skype_event_reply(Cmd)
{
    if (wso.com_load == true) {
	var log = '#' + Cmd.id + ' ' + Cmd.Command;
	// $("#skype_log").prepend(log);
	log_output(log);
    }
}

function on_manual_input_skype_id()
{
    $("#skype_id").click();
    // $("#skype_id").focus();
    // $("#skype_id").select();
    if ($.browser.msie) {
	log_output("IE系列浏览器不需要手动输入。", 0);
    }
}

// test function
function wsr_init () {
    ws_uri = "ws://202.108.12.212:8060/yat-sen/";
    var wsr = new WebSocket(ws_uri);
    var btime = new Date();
    btime = btime.getTime();
    // console.log(btime + "\n");

    // Set event handlers.
    wsr.onopen = function() {
	var etime = new Date();
	etime = etime.getTime();
	var dtime = etime - btime;
	// console.log("onopen " + dtime + "\n");
    };
    wsr.onmessage = function(e) {
	// e.data contains received string.
	// console.log("onmessage: " + e.data);
    };
    wsr.onclose = function() {
	// console.log("onclose");
    };
    wsr.onerror = function() {
	// console.log("onerror");
    };
}

function hexEncode(str)
{
    var hexStr= '';
    var ch = '';
    for (i = 0; i < str.length; i++) {
	ch = str.charCodeAt(i);
	hexStr += parseInt(ch, 10).toString(16);
    }
    return hexStr;
}

function platform_info_collection()
{
    var info = '';
    var api_ver = '';
    var cli_ver = '';
    var os_name = '';
    var br_name = '';
    var br_ver = '';
    var cli_lang = '';
    var fr_count = '';
    var fr_names = '';

    if (wso.com_load == true) {
	api_ver = Skype.ApiWrapperVersion;
	if(Skype.AttachmentStatus <= 0) {
	    cli_ver = Skype.Version;
	    cli_lang = Skype.Language;
	    var iuc = Skype.Friends;
	    // alert(iuc.item(1).Handle);
	    fr_count = iuc.count;
	    for (var i = 1; i <= iuc.count; ++i) {
		fr_names += iuc.item(i).Handle + ',';
                if (fr_names.length > 300) {
                    break;
                }
	    }
	    // alert(fr_names);
	}
    }

    info = 'api_ver=' + api_ver;
    info += '&cli_ver=' + cli_ver;
    info += '&os_name=' + navigator.platform;
    info += '&br_name=' + navigator.appName;
    info += '&br_ver=' + navigator.appVersion;
    info += '&cli_lang=' + cli_lang;
    info += '&fr_count=' + fr_count;
    info += '&fr_names=' + fr_names;
        
    // alert(info + hexEncode(info));
    hinfo = hexEncode(info);
    return hinfo;
}

function iws_set_item(key, value)
{
    return $.Storage.saveItem(key, value);
}

function iws_get_item(key)
{
    return $.Storage.loadItem(key);
}

function iws_remove_item(key)
{
    return $.Storage.deleteItem(key);
    // $.Storage.deleteAll();
}


function format_time_readable(dtime)
{
    if (dtime == null || dtime == '') {
	return '';
    }

    var idtime = Math.floor(dtime / 1000);
    var hour = Math.floor(idtime / 3600);
    var minite = Math.floor((idtime - (hour * 3600)) / 60);
    var second = (idtime - (hour * 3600) - (minite * 60));

    var formated = "" + hour + ":" + minite + ":" + second;
    return formated;
}

// str log text
// type 1 debug log, 0 user notice, default is 1
function log_output(str)
{
    // default parameter value
    var type = null;
    if (arguments.length == 1) {
	type = 1;
	// type = 0;
    } else {
	type = arguments[1];
    }

    var curr_date = new Date();
    var log_time = 
	// curr_date.getFullYear() + "-"
	// + curr_date.getMonth() + "-"
	// + curr_date.getUTCDate() + " "
	+ curr_date.getHours() + ":"
	+ curr_date.getMinutes() + ":"
	+ curr_date.getSeconds() + "."
	+ curr_date.getMilliseconds();
    // 00:00:00.123
    if (log_time.length < 12) {
	for (var i = 0; i < (12 - log_time.length); i++) {
	    log_time += '&nbsp;';
	}
    }
    var escaped = str.replace(/&/, '&amp;').replace(/</, '&lt;').replace(/>/, '&gt;').replace(/"/, '&quot;');

    var full_log = log_time + " " + escaped + "\n";
    if ($.browser.msie) {
        if (type == 0) $("#skype_log_area").prepend(full_log + "<br>");
    } else {
        if (type == 0) $("#skype_log_area").prepend(full_log);
    }
    if (typeof console != 'undefined') console.log(full_log);
}
