<?php
   // echo "正在更新系统，暂停服务。。。<br>\n";
   // print_r($_SERVER);
   $self_domain = '';
   $wso_serv_ipaddr = '';
   // 判断用户来源,决定所使用wso服务器IP
   // 网通用户
   if ($_SERVER['SERVER_ADDR'] == '202.108.15.80') {
       $wso_serv_ipaddr = '202.108.15.81';
       $self_domain = 'gw.skype.tom.com';
   }
   // 电信用户
   if ($_SERVER['SERVER_ADDR'] == '211.100.41.6') {
       $wso_serv_ipaddr = '211.100.41.7';
       $self_domain = 'gw.skype.tom.com';
   }

   // test server
   if ($_SERVER['SERVER_ADDR'] == '202.108.12.211') {
       $wso_serv_ipaddr = '202.108.12.212';
       $self_domain = $_SERVER['SERVER_ADDR'];
   }

   $wso_version = "";
?>
<!DOCTYPE html>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />
<meta name="keywords" content="" />
<meta name="description" content="" />
<meta name="copyright" content="" />
<meta name="robots" content="all" />
<meta name="googlebot" content="all" />
<meta name="baiduspider" content="all" />
<meta name="SKYPE_TOOLBAR" content="SKYPE_TOOLBAR_PARSER_COMPATIBLE" />
<title>Skype 中文官方网站 - Web Skypeout (1.0-BETA)</title>
<link href="css/css.css" rel="stylesheet" type="text/css" />
<!--  -->
<!--   <script language="javascript" src="jquery-1.4.4.min.js" charset="utf-8"></script> -->
  <script language="javascript" src="jquery-1.5.1.min.js" charset="utf-8"></script>
  <script language="javascript" src="jquery.Storage.js" charset="utf-8"></script>

 <script type="text/javascript">
    function loadJs(jsUri) {    
	if(!jsUri) return;    
	var js = document.getElementsByTagName("script");    
	var len = js.length;    
	for(var i=0; i < len; i++) {    
	    if(jsUri == js[i].getAttribute("src")) {    
		js[i].parentNode.removeChild(js[i]);    
		break;    
	    }    
	}    
	var headObj = document.getElementsByTagName("head")[0];    
	var jsLink = document.createElement("script");    
	jsLink.setAttribute("type", "text/javascript");    
	jsLink.setAttribute("src", jsUri);    
	headObj.appendChild(jsLink);    
    }

    // depend: swfobject.js
    function detect_real_support_websocket() {
	// 对于IE6+flash9, 已经测试出来有问题
	var flash_version = swfobject.getFlashPlayerVersion();
	var ie_version = 100;

	if ($.browser.msie) {
	    var re  = new RegExp("MSIE ([0-9]{1,}[\.0-9]{0,})");
	    re.exec(navigator.userAgent);
	    ie_version = parseFloat(RegExp.$1);
	    str_ie_version = RegExp.$1;

	    if (flash_version.major <= 9 && ie_version <= 6.9) {
		 return false;
	    }
	}
	return true;
    }

 </script>

  <!-- Include these three JS files: -->
  <script type="text/javascript" src="swfobject.js" charset="utf-8"></script>
  <!-- 
  <script type="text/javascript" src="FABridge.js" charset="utf-8"></script>
  <script type="text/javascript" src="web_socket.js" charset="utf-8"></script>
  -->
  <script type="text/javascript">

    if (detect_real_support_websocket()) {
        loadJs("FABridge.js");
        loadJs("web_socket.js");
    } else {
	// $("#skype_log_area").prepend("lagecy ie+flash, no websocket");
    }

    // Set URL of your WebSocketMain.swf here:
    WEB_SOCKET_SWF_LOCATION = "WebSocketMain.swf";
    // Set this to dump debug message from Flash to console.log:
    WEB_SOCKET_DEBUG = true;
    WEB_SOCKET_DEBUG = false;
    // Everything below is the same as using standard WebSocket.
    GWEB_SOCKET_ADDR = "<?php echo $wso_serv_ipaddr; ?>";
  </script>

  <script type="text/javascript" src="webso_core.js"></script>

  <script language="Javascript" type="text/Javascript">
  // test area, for easy
  </script>

  <script type="text/javascript" src="js/skype1012.js" charset="utf-8"></script>
</head>
<body>
  <object id=Skype name=Skype align=baseline height=0 width=0 border="0"
          CLASSID="CLSID:830690FC-BF2F-47A6-AC2D-330BCB402664"
	  codebase="Skype4COM.dll#VERSION=1,0,35,0">
      <script language="javascript">
      	      wso.skype_com_load_faild();
      </script>
  </object>

    <script type="text/javascript" src="http://skype.tom.com/scripts/hd.js?ver=0701"></script>
    <div class="wrap">
    <div class="p1">
        <p>
        <b>您好：</b><b><span id="handle_name"></span> <span id="display_name"></span></b>
	<span id="manual_skype_id_div" style="display:none">
        <input type="text" name="skype_id" id="skype_id" value="" class="input-text" />
	</span>
        </p>
        <p>
            如果上面正确显示了您的Skype用户名，那么现在您就可以<b>使用计时通</b>通过页面拨打电话了<br />如果显示有误，请 <a href="javascript:void(0);" onclick="on_manual_input_skype_id(); return false;">手动输入</a> 您当前登陆的skype账号。
        </p>
    </div>
    <div class="p2">
        <img src="images/ph.gif" alt="Alternate Text" />
        <span>电话号码：</span>
        <input type="text" name="phone_number" id="phone_number" value="星号（*）加电话号码" class="input-text1" />
        <span>格式： *010678881xx  或 *0138001380xx</span>
        <a class="e1" title="发起呼叫..." id="websocket_send_skypeout_call_button" onclick="wso.place_call_init();" style="display:inline"></a>
        <a class="e2" title="挂断" id="hangup_skypeout_call_button" onclick="wso.hangup_call();" disabled=true></a>
        <a class="e3" title="发起免费测试呼叫" id="test_skypeout_call_button" onclick="wso.place_call_test();"></a>
    </div>

    <div class="p3">
        <div class="cont">
	  <div id="dial_msg_div" style="display:block; position: absolute; width:500px; height: 0px; z-index:999; left: 465px; top: 110px;" >
	      <a id='manual_call_a' href="javascript:void();" style="display:none">Calling ...</a>
	      <textarea id="skype_log_area" rows="32" cols="63" style="overflow-y:hidden;overflow-x:hidden;background-color:transparent; border:0px;" readonly></textarea>
          </div>
            <div id="last_update_div" style="display:block; position: absolute; width:295px; height: 0px;  z-index:999; right: -80px; bottom: -17px;">
              Last update: 2011-03-28 <a href="javascript:void(0);">ChangeLog</a>
            </div>

	  <dl>
            <dt>拨打说明：</dt>
            <dd>
                1、下载安装最新版<a href="http://skype.tom.com/download/">Tom-Skype客户端</a>；
            </dd>
            <dd>
                2、打开页面拨打地址：<a href="http://<?php echo $self_domain;?>/wso_1.0-665/web_skypeout.php">http://<?php echo $self_domain;?>/wso_1.0-665/web_skypeout.php</a>；
            </dd>
            <dd>
                3、以IE浏览器为例：<br />
                <b>选择“运行”ActiveX插件</b><br />
                <img src="images/e1.jpg" alt="Alternate Text" /><br />
                <b class="mg">在Skype客户端中，允许IE对Skype进行访问</b>
                <img src="images/e2.jpg" alt="Alternate Text" />

            </dd>
            <dd>
                4、检查页面顶端是否能正确显示您的Skype用户名和昵称
            </dd>
            <dd>
                5、确保账户内有Skype计时通（<a href="http://skype.tom.com/newhelp/ddd_using_help.html#01">什么是Skype计时通？</a>）
            </dd>
            <dd>
                6、在输入框内输入要拨打的电话号码，点击“呼叫”按钮即可<br />
                <b>  固定电话：星号（*）加座机号码，例：*0411876239xx</b><br />
                <b>  手机：星号（*）加0加手机号码，例：*0138001380xx</b>
            </dd>
            <dd class="nobd">
                7、支持系统及浏览器列表：(只支持原版IE，其他包装IE内核的浏览器可能会有兼容问题。<a href="javascript: void(0);" id="check_coma">检测浏览器兼容性</a>)
                <table border="0" cellpadding="0" cellspacing="0" width="100%">
                    <tr class="thead">
                        <td>
                            <span>系统 </span>
                        </td>
                        <td>
                            <span>IE6、7、8 </span>
                        </td>
                                            <td>
                            <span>Opera</span>
                        </td>
                                            <td>
                            <span>Firefox</span>
                        </td>
                                            <td>
                            <span>Chrome</span>
                        </td>
                        <td>
                            <span>Safari 5</span>
                        </td>
                    </tr>
                    <tr>
                        <td class="c">
                            Windows XP/Vista/7
                        </td>
                        <td>
                            支持
                        </td>
                        <td>    
                            支持
                        </td>
                                                <td>
                            支持
                        </td>
                        <td>    
                            支持
                        </td>
                                                <td>
                            支持
                        </td>
                    </tr>
                    <tr>
                        <td class="c">
                            Mac OS X
                        </td>
                        <td>
                           ____
                        </td>
                        <td>    
                            支持
                        </td>
                                                <td>
                           ____
                        </td>
                        <td>    
                            支持
                        </td>
                                                <td>
                            支持
                        </td>
                    </tr>
                </table>
            </dd>
            <dd>
                8、设置IE控件安全级别，下次使用不再弹出安全设置提示窗口。
		把<a href="skype4com:openzonesettings" target="null_popup">弹出窗口</a>中的选项设置为"Enable"。
		<br>
		<!-- sphoneid telnr="+15551234456" fileas="John Smith" -->  <!-- (555) 1234 456 -->  <!-- sphoneid -->
		<!-- p>+27 11 555 5555</p-->
            </dd>
        </dl>
        </div>
        
    </div>
    </div>

<!--
<a href="skype4com:openhostsettings">Open Host Settings</a>
<a href="skype4com:openzonesettings">Open Zone Settings</a>
-->
<iframe name="null_popup" id="null_popup" src="" width="100%" height="0" scrolling="no" style="display:none"></iframe>

    <script type="text/javascript" src="http://skype.tom.com/scripts/ft.js?ver=0608"></script>
<style type="text/css">
/* 遗留 */
.footer{border-top:#e9e9e9 solid 1px; clear:both; text-align:center; padding:10px; background:url(http://skype.tom.com/images/footer_bg.gif) top repeat-x; }
#skypeft{ color:#666; line-height:18px;}
#skypeft A:link,#skypeft A:active{TEXT-DECORATION:underline ;Color:#0083B3}
#skypeft A:visited{TEXT-DECORATION:underline;Color:#0083B3}
#skypeft A:hover{TEXT-DECORATION: none;Color:#ff0000}
#skypeft A.indextop:link,#skypeft A:active{TEXT-DECORATION:none ;}
#skypeft A:indextop{TEXT-DECORATION: none;Color:#ff0000}
.greylink {color: #888883; text-decoration: none}
A.greylink:link {color: #888883; text-decoration: none}
A.greylink:visited {color: #888883; text-decoration: none}
A.greylink:active {color: #888883; text-decoration: none}
A.greylink:hover {color: red; text-decoration: underline}
</style>


  <!-- must put bottom, should init Skype Object first -->
  <script language="JavaScript" type="text/JavaScript" for="Skype" event="AttachmentStatus(Status)">
      if (wso.support_com_ctrl()) 
  	  skype_event_attachment_status(Status);
  </script>

  <script language="JavaScript" type="text/JavaScript" for="Skype" event="ConnectionStatus(Status)">
      if (wso.support_com_ctrl()) 
  	  skype_event_connection_status(Status);
  </script>

  <script language="JavaScript" type="text/JavaScript" for="Skype" event="Error(Cmd, Num, Desc)">
      if (wso.support_com_ctrl()) 
  	  skype_event_error(Cmd, Num, Desc);
  </script>

  <script language="JavaScript" type="text/JavaScript" for="Skype" event="CallStatus(Call, Status)">
      if (wso.support_com_ctrl()) 
  	  skype_event_call_status(Call, Status);
  </script>

  <script language="JavaScript" type="text/JavaScript" for="Skype" event="Reply(Cmd)">
      if (wso.support_com_ctrl()) 
  	  skype_event_reply(Cmd);
  </script>


  <!-- Piwik --> 
  <script type="text/javascript">
    var pkBaseURL = (("https:" == document.location.protocol) ? "https://211.100.41.6/piwik/" : "http://211.100.41.6/piwik/");
    document.write(unescape("%3Cscript src='" + pkBaseURL + "piwik.js' type='text/javascript'%3E%3C/script%3E"));
  </script><script type="text/javascript">
    try {
    var piwikTracker = Piwik.getTracker(pkBaseURL + "piwik.php", 1);
    piwikTracker.trackPageView();
    piwikTracker.enableLinkTracking();
    } catch( err ) {}
  </script><noscript><p><img src="http://211.100.41.6/piwik/piwik.php?idsite=1" style="border:0" alt="" /></p></noscript>
  <!-- End Piwik Tracking Code -->

</body>
</html>

