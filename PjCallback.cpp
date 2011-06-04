#include <QtCore>

#include <QList>
#include <QMutex>

#include "pjsip.h"
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include "pjsua.h"
#include <pjmedia/wave.h>
#include <pjmedia/wav_port.h>

#include "simplelog.h"
#include "sipphone.h"
#include "PjCallback.h"

#define THIS_FILE "QjCallback"

// extern QList<int> activeCalls;
// extern QMutex activeCallsMutex;

/* global callback/logger object */
// PjCallback *globalPjCallback = NULL;
void *globalPjCallback = NULL;

PjCallback::PjCallback() {
	globalPjCallback = this;
}

PjCallback::~PjCallback() {
}

void PjCallback::on_call_state(pjsua_call_id call_id, pjsip_event *e, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    emit this->sig_call_state(call_id, e, pci);
}

void PjCallback::on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pj_status_t status;
    pjsua_call_info *pci = (pjsua_call_info*)calloc(1, sizeof(pjsua_call_info));

    status = pjsua_call_get_info(call_id, pci);
    if (pci->state == PJSIP_INV_STATE_DISCONNECTED) {
        qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id<<"GOT DISCONNECTED state";
	
        pjsua_acc_id acc_id;
        acc_id = pjsua_acc_get_default();
        pjsua_acc_del(acc_id);
    }
	/* call the non-static member */
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		// myCb->on_call_state(call_id, e);
        myCb->on_call_state(call_id, e, pci);
	}
}

void PjCallback::on_call_media_state(pjsua_call_id call_id, pjsua_call_info *pci)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    emit this->sig_call_media_state(call_id, pci);
}

void PjCallback::on_call_media_state_wrapper(pjsua_call_id call_id)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    pj_status_t status;
    pjsua_call_info *pci = (pjsua_call_info*)calloc(1, sizeof(pjsua_call_info));

    status = pjsua_call_get_info(call_id, pci);
	/* call the non-static member */
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		// myCb->on_call_media_state(call_id);
        myCb->on_call_media_state(call_id, pci);
	}
}

void PjCallback::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
    emit this->sig_incoming_call(acc_id, call_id, rdata);
}
// static 
void PjCallback::on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<call_id;
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		myCb->on_incoming_call(acc_id, call_id, rdata);
	}
}

void PjCallback::on_user_added(int payload, pjsua_call_id call_id, int tid)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<payload<<call_id;
  emit this->sig_user_added(payload, call_id, tid);
}

// static 
void PjCallback::on_user_added_wrapper(int payload, pjsua_call_id call_id, int tid)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<payload<<call_id;
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		myCb->on_user_added(payload, call_id, tid);
	}
}

void PjCallback::on_exceed_max_call_count(int payload)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<payload;
  emit this->sig_exceed_max_call_count(payload);
}
// static 
void PjCallback::on_exceed_max_call_count_wrapper(int payload)
{
  qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<payload;
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		myCb->on_exceed_max_call_count(payload);
	}
}

// void PjCallback::on_new_connection(void *m_port)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
//     emit sig_new_connection(m_port);
// }

// // static 
// void PjCallback::on_new_connection_wrapper(void *m_port)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_new_connection(m_port);
// 	}
// }

// void PjCallback::on_new_incoming_connection(void *m_port)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
//     emit sig_new_incoming_connection(m_port);
// }

// // static 
// void PjCallback::on_new_incoming_connection_wrapper(void *m_port)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<m_port;
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_new_incoming_connection(m_port);
// 	}
// }

// void PjCallback::on_put_frame(QTcpSocket *sock, QByteArray fba)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<fba.length();
//     emit sig_put_frame(sock, fba);
// }

// // static 
// void PjCallback::on_put_frame_wrapper(QTcpSocket *sock, QByteArray fba)
// {
//     qDebug()<<__FILE__<<__FUNCTION__<<__LINE__<<fba.length();
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_put_frame(sock, fba);
// 	}
// }


// void PjCallback::logger_cb(int level, const char *data, int len) {
// 	PJ_UNUSED_ARG(level);
// 	PJ_UNUSED_ARG(len);
// 	/* optional dump to stdout */
// 	//puts(data);
// 	/* emit signal with log message */
// 	/* paramter will be converted to QString which makes a deep copy */
// emit 		new_log_message(data);
// }

// void PjCallback::logger_cb_wrapper(int level, const char *data, int len) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->logger_cb(level, data, len);
// 	}
// }

// void PjCallback::on_pager(pjsua_call_id call_id, const pj_str_t *from,
// 		const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
// 		const pj_str_t *text) {
// 	/* Note: call index may be -1 */
// 	PJ_UNUSED_ARG(call_id);
// 	PJ_UNUSED_ARG(to);
// 	PJ_UNUSED_ARG(contact);
// 	PJ_UNUSED_ARG(mime_type);
	
// 	emit new_im(QString::fromAscii(from->ptr,from->slen), 
// 			QString::fromAscii(text->ptr,text->slen));
// }

// /** callback wrapper function called by pjsip
//  * Incoming IM message (i.e. MESSAGE request)!*/
// void PjCallback::on_pager_wrapper(pjsua_call_id call_id, const pj_str_t *from,
// 		const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
// 		const pj_str_t *text) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_pager(call_id, from, to, contact, mime_type, text);
// 	}
// }

// /* Notify application which NAT type was detected
//  */
void PjCallback::on_nat_detect(const pj_stun_nat_detect_result *res) {
	QString description;
	switch (res->nat_type) {
	case PJ_STUN_NAT_TYPE_UNKNOWN:
		description="PJ_STUN_NAT_TYPE_UNKNOWN:\r\n\r\n"
			"NAT type is unknown because the detection has not been performed.";
		break;
	case PJ_STUN_NAT_TYPE_ERR_UNKNOWN:
		description="PJ_STUN_NAT_TYPE_ERR_UNKNOWN:\r\n\r\n"
			"NAT type is unknown because there is failure in the detection process, \r\n"
			"possibly because server does not support RFC 3489.";
		break;
	case PJ_STUN_NAT_TYPE_OPEN:
		description="PJ_STUN_NAT_TYPE_OPEN:\r\n\r\n"
			"This specifies that the client has open access to Internet (or at \r\n"
			"least, its behind a firewall that behaves like a full-cone NAT, but \r\n"
			"without the translation)";
		break;
	case PJ_STUN_NAT_TYPE_BLOCKED:
		description="PJ_STUN_NAT_TYPE_BLOCKED:\r\n\r\n"
			"This specifies that communication with server has failed, probably \r\n"
			"because UDP packets are blocked.";
		break;
	case PJ_STUN_NAT_TYPE_SYMMETRIC_UDP:
		description="PJ_STUN_NAT_TYPE_SYMMETRIC_UDP:\r\n\r\n"
			"Firewall that allows UDP out, and responses have to come back to the \r\n"
			"source of the request (like a symmetric NAT, but no translation.";
		break;
	case PJ_STUN_NAT_TYPE_FULL_CONE:
		description="PJ_STUN_NAT_TYPE_FULL_CONE:\r\n\r\n"
			"A full cone NAT is one where all requests from the same internal IP \r\n"
			"address and port are mapped to the same external IP address and port. \r\n"
			"Furthermore, any external host can send a packet to the internal host, \r\n"
			"by sending a packet to the mapped external address.";
		break;
	case PJ_STUN_NAT_TYPE_SYMMETRIC:
		description="PJ_STUN_NAT_TYPE_SYMMETRIC:\r\n\r\n"
			"A symmetric NAT is one where all requests from the same internal IP \r\n"
			"address and port, to a specific destination IP address and port, are \r\n"
			"mapped to the same external IP address and port. If the same host \r\n"
			"sends a packet with the same source address and port, but to a different \r\n"
			"destination, a different mapping is used. Furthermore, only the external \r\n"
			"host that receives a packet can send a UDP packet back to the internal host.";
		break;
	case PJ_STUN_NAT_TYPE_RESTRICTED:
		description="PJ_STUN_NAT_TYPE_RESTRICTED:\r\n\r\n"
			"A restricted cone NAT is one where all requests from the same internal \r\n"
			"IP address and port are mapped to the same external IP address and port. \r\n"
			"Unlike a full cone NAT, an external host (with IP address X) can send a \r\n"
			"packet to the internal host only if the internal host had previously \r\n"
			"sent a packet to IP address X.";
		break;
	case PJ_STUN_NAT_TYPE_PORT_RESTRICTED:
		description="PJ_STUN_NAT_TYPE_PORT_RESTRICTED:\r\n\r\n"
			"A port restricted cone NAT is like a restricted cone NAT, but the \r\n"
			"restriction includes port numbers. Specifically, an external host \r\n"
			"can send a packet, with source IP address X and source port P, to \r\n"
			"the internal host only if the internal host had previously sent a \r\n"
			"packet to IP address X and port P.: ";
		break;
	default:
		description="Error: unknown type detected!";
	}
	// emit nat_detect(QString(res->nat_type_name), description);
    qLogx()<<QString(res->nat_type_name)<<description;
    emit sig_nat_detect(QString(res->nat_type_name), description);
}

void PjCallback::on_nat_detect_wrapper(const pj_stun_nat_detect_result *res) {
	/* call the non-static member */
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		myCb->on_nat_detect(res);
	}
}

// void PjCallback::on_call_state(pjsua_call_id call_id, pjsip_event *e) {
// 	PJ_UNUSED_ARG(e);

// 	PJ_LOG(3,(THIS_FILE, "trying to lock ...."));
// 	PJ_LOG(3,(THIS_FILE, "trying to lock .... locked"));
// 	// if (activeCalls.empty()) {
// 	// 	PJ_LOG(3,(THIS_FILE, "Call %d not found as callList is empty; new incoming call? ... ignoring", call_id));
// 	// 	return;	
// 	// }
// 	// if (!activeCalls.contains(call_id)) {
// 	// 	PJ_LOG(3,(THIS_FILE, "Call %d not found in callList; new incoming call? ... ignoring", call_id));
// 	// 	return;	
// 	// }

// 	pj_status_t status;
// 	pjsua_call_info ci;
// 	status = pjsua_call_get_info(call_id, &ci);
// 	if (status != PJ_SUCCESS) {
// 		PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
// 		return;
// 	}
// 	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id, 
// 			(int)ci.state_text.slen, ci.state_text.ptr));

// 	QString state_text = QString::fromAscii(ci.state_text.ptr,(int)ci.state_text.slen);
// 	emit setCallState(state_text);

// 	switch(ci.state) {
// 	case PJSIP_INV_STATE_DISCONNECTED:
// 		// activeCalls.removeAt(activeCalls.indexOf(call_id));
// 		emit setCallButtonText("call buddy");
// 		break;
// 	default:
// 		;
// 	}
// }

// void PjCallback::on_call_state_wrapper(pjsua_call_id call_id, pjsip_event *e) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_call_state(call_id, e);
// 	}	
// }

// void PjCallback::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {

// 	PJ_UNUSED_ARG(acc_id);
// 	PJ_UNUSED_ARG(rdata);

// 	// activeCallsMutex.lock();
// 	// if (!activeCalls.empty()) {
// 	// 	PJ_LOG(3,(THIS_FILE, "new incoming Call %d, but we already have a call ... reject", call_id));
// 	// 	activeCallsMutex.unlock();
// 	// 	pjsua_call_hangup(call_id, 486, NULL, NULL);
// 	// 	return;	
// 	// }
	
// 	pj_status_t status;
// 	pjsua_call_info ci;
// 	status = pjsua_call_get_info(call_id, &ci);
// 	if (status != PJ_SUCCESS) {
// 		PJ_LOG(3,(THIS_FILE, "ERROR retrieveing info for Call %d ... ignoring", call_id));
// 		// activeCallsMutex.unlock();
// 		return;
// 	}
// 	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id, 
// 			(int)ci.state_text.slen, ci.state_text.ptr));

// 	QString state_text = QString::fromAscii(ci.state_text.ptr,(int)ci.state_text.slen);
// 	emit setCallState(state_text);

// 	emit setCallState(state_text);
// 	// activeCalls << call_id;
// 	emit setCallButtonText("answer call");
// 	// activeCallsMutex.unlock();
// }

// void PjCallback::on_incoming_call_wrapper(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_incoming_call(acc_id, call_id, rdata);
// 	}	
// }

// void PjCallback::on_call_media_state(pjsua_call_id call_id) {
// 	pjsua_call_info ci;

// 	pjsua_call_get_info(call_id, &ci);
// 	switch (ci.media_status) {
// 	case PJSUA_CALL_MEDIA_NONE: 
// 		PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
// 				"PJSUA_CALL_MEDIA_NONE: Call currently has no media", call_id));
// 		break;
// 	case PJSUA_CALL_MEDIA_ACTIVE: 
// 		PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
// 				"PJSUA_CALL_MEDIA_ACTIVE: The media is active", call_id));
// 		// When media is active, connect call to sound device.
// 		pjsua_conf_connect(ci.conf_slot, 0);
// 		pjsua_conf_connect(0, ci.conf_slot);
// 		break;
// 	case PJSUA_CALL_MEDIA_LOCAL_HOLD:
// 		PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
// 				"PJSUA_CALL_MEDIA_LOCAL_HOLD: The media is currently put on hold by local endpoint", call_id));
// 		break;
// 	case PJSUA_CALL_MEDIA_REMOTE_HOLD:
// 		PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
// 				"PJSUA_CALL_MEDIA_REMOTE_HOLD: The media is currently put on hold by remote endpoint", call_id));
// 		break;
// 	case PJSUA_CALL_MEDIA_ERROR:
// 		PJ_LOG(3,(THIS_FILE, "on_call_media_state: call_id %d: "
// 				"PJSUA_CALL_MEDIA_ERROR: The media has reported error (e.g. ICE negotiation)", call_id));
// 		break;
// 	}
// }

// void PjCallback::on_call_media_state_wrapper(pjsua_call_id call_id) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_call_media_state(call_id);
// 	}	
// }

// void PjCallback::on_buddy_state(pjsua_buddy_id buddy_id) {
// 	emit new_log_message("on_buddy_state called for buddy_id " + QString::number(buddy_id));
// 	emit buddy_state(buddy_id);
// }

// void PjCallback::on_buddy_state_wrapper(pjsua_buddy_id buddy_id) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_buddy_state(buddy_id);
// 	}
// }

// void PjCallback::on_reg_state(pjsua_acc_id acc_id) {
// 	emit reg_state_signal(acc_id);
// }

// void PjCallback::on_reg_state_wrapper(pjsua_acc_id acc_id) {
// 	/* call the non-static member */
// 	if (globalPjCallback) {
// 		PjCallback *myCb = (PjCallback*) globalPjCallback;
// 		myCb->on_reg_state(acc_id);
// 	}
// }
void PjCallback::on_reg_state(pjsua_acc_id acc_id) {
	emit sig_reg_state(acc_id);
}

void PjCallback::on_reg_state_wrapper(pjsua_acc_id acc_id) {
	/* call the non-static member */
	if (globalPjCallback) {
		PjCallback *myCb = (PjCallback*) globalPjCallback;
		myCb->on_reg_state(acc_id);
	}
}

//void PjCallback::on_pager2(pjsua_call_id call_id, const pj_str_t *from,
//		const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
//		const pj_str_t *text, pjsip_rx_data *rdata) {
//	/* Note: call index may be -1 */
//	PJ_UNUSED_ARG(call_id);
//	PJ_UNUSED_ARG(to);
//	PJ_UNUSED_ARG(contact);
//	PJ_UNUSED_ARG(mime_type);
//	PJ_UNUSED_ARG(rdata);
//	
//	char *cfrom, *ctext, *cfromuri;
//	cfrom = (char*) malloc((from->slen)+1);
//	if (!cfrom) {
//		puts("cfrom memory allocation error");
//		return;
//	}
//	memcpy(cfrom, from->ptr, from->slen);
//	memcpy(cfrom+(from->slen), "", 1);
//	ctext = (char*) malloc((text->slen)+1);
//	if (!ctext) {
//		puts("ctext memory allocation error");
//		return;
//	}
//	memcpy(ctext, text->ptr, text->slen);
//	memcpy(ctext+(text->slen), "", 1);
//
////rdata->msg_info.from->uri->vptr->
////	cfromuri = (char*) malloc((from->slen)+1);
////	if (!cfromuri) {
////		puts("cfrom memory allocation error");
////		return;
////	}
////	memcpy(cfromuri, from->ptr, from->slen);
////	memcpy(cfromuri+(from->slen), "", 1);
//
////	emit new_im(QString(cfrom), QString(cfromuri), QString(ctext));
//	emit new_im(QString(cfrom), QString(ctext));
//}
//
///** callback wrapper function called by pjsip
// * Incoming IM message (i.e. MESSAGE request)!*/
//void PjCallback::on_pager2_wrapper(pjsua_call_id call_id, const pj_str_t *from,
//		const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
//		const pj_str_t *text, pjsip_rx_data *rdata) {
//	/* call the non-static member */
//	if (globalPjCallback) {
//		PjCallback *myCb = (PjCallback*) globalPjCallback;
//		myCb->on_pager2(call_id, from, to, contact, mime_type, text, rdata);
//	}
//}

void PjCallback::on_make_call_impl(int reqno, pjsua_acc_id acc_id, const QString &sip_uri)
{
    pj_status_t status;
    pjsua_call_id call_id;
    char buf[5120] = {0};

    strncpy(buf, sip_uri.toAscii().data(), sip_uri.length());
    pj_str_t uri = pj_str(buf);
    
    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, &call_id);

    emit this->sig_make_call_done(reqno, status, call_id);
}


/////////////////////////
////////////////////////
int PjsipCallFront::m_reqno = 0;
PjsipCallFront::PjsipCallFront(QObject *parent)
    :QThread(parent)
{

    
}

PjsipCallFront::~PjsipCallFront()
{
}

int PjsipCallFront::mystart(pjsua_config *ua_cfg, pjsua_logging_config *log_cfg, pjsua_media_config *media_cfg,
          pjsua_transport_config *tcp_tp_cfg, pjsua_transport_config *udp_tp_cfg,
                pjsua_transport_id *tcp_tp_id, pjsua_transport_id *udp_tp_id)
{
    m_ua_cfg = ua_cfg;
    m_log_cfg = log_cfg;
    m_media_cfg = media_cfg;
    
    m_tcp_tp_cfg = tcp_tp_cfg;
    m_udp_tp_cfg = udp_tp_cfg;

    m_tcp_tp_id = tcp_tp_id;
    m_udp_tp_id = udp_tp_id;

    this->start();

    return PJ_SUCCESS;
}

pj_status_t PjsipCallFront::sip_init_misc()
{
    pj_status_t status;

    

    return status;
}

void PjsipCallFront::run()
{
    pj_thread_desc initdec;
    pj_thread_t* thread = 0;
    pj_status_t status;
    int evt_cnt = 0;

    Q_ASSERT(::globalPjCallback == NULL);
    ::globalPjCallback = new PjCallback();

    status = pjsua_create();
    Q_ASSERT(status == PJ_SUCCESS);

    PJSipEventThread *ethread = new PJSipEventThread();
    ethread->start();
    while (!ethread->isRunning()) {
        msleep(5);
    }

    PjCallback *myCb = (PjCallback*) globalPjCallback;
    QObject::connect(this, SIGNAL(invoke_make_call_fwd(int, pjsua_acc_id, const QString&)),
                     myCb, SLOT(on_make_call_impl(int, pjsua_acc_id, const QString&)));


    QObject::connect(myCb, SIGNAL(sig_make_call_done(int, pj_status_t, pjsua_call_id)),
                     this, SIGNAL(invoke_make_call_result(int, pj_status_t, pjsua_call_id)));


    status = pjsua_init(m_ua_cfg, m_log_cfg, m_media_cfg);
    Q_ASSERT(status == PJ_SUCCESS);

    pjsua_var.mconf_cfg.samples_per_frame = 8000; // pjsua_var from 

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;
        pjsua_transport_config rtp_cfg;
        pjsua_transport_id udp_tp_id;
        pjsua_transport_id tcp_tp_id;
       
        // 创建指定端口的RTP/RTCP层media后端
        pjsua_transport_config_default(&rtp_cfg);
        rtp_cfg.port = 8050;
        status = pjsua_media_transports_create(&rtp_cfg);

        // SIP 层初始化，可指定端口
        pjsua_transport_config_default(&cfg);
        cfg.port = 15678; // if not set , use random big port 
        // cfg.public_addr = pj_str("123.1.2.3"); // 与上面的port一起可用于穿透，指定特定的公共端口!!!
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, m_udp_tp_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating udp transport", status);
            // error_exit("Error creating transport", status);
        }

        // TCP transport
        pjsua_transport_config_default(&cfg);
        cfg.port = 56789;
        status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &cfg, m_tcp_tp_id);
        if (status != PJ_SUCCESS) {
            pjsua_perror(__FILE__, "Error creating tcp transport", status);
            // error_exit("Error creating transport", status);
        }
    }
    
    status = pjsua_start();
    Q_ASSERT(status == PJ_SUCCESS);

    qLogx()<<"";
    emit this->realStarted(status);
    this->exec();
}

void PjsipCallFront::dump_info(pj_thread_t *thread)
{
    Q_ASSERT(thread != NULL);

    qLogx()<<"pj_thread_is_registered:"<<pj_thread_is_registered();
    qLogx()<<"pj_thread_get_prio:"<<pj_thread_get_prio(thread);
    qLogx()<<"pj_thread_get_prio_min:"<<pj_thread_get_prio_min(thread);
    qLogx()<<"pj_thread_get_prio_max:"<<pj_thread_get_prio_max(thread);
    qLogx()<<"pj_thread_get_name:"<<pj_thread_get_name(thread);
    qLogx()<<"pj_getpid:"<<pj_getpid();
}

int PjsipCallFront::invoke_make_call(pjsua_acc_id acc_id, const QString &sip_uri)
{
    int reqno = ++ this->m_reqno;

    emit this->invoke_make_call_fwd(reqno, acc_id, sip_uri);

    return reqno;
}

