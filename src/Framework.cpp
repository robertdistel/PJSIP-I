#include <pjsua-lib/pjsua.h>

#include <iostream>
#include <vector>
#include <pj/file_access.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>
#include <pjmedia/session.h>

#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include <sched.h>
#include "Enum.h"






#define THIS_FILE "FRAMEWORK"

#define SIP_DOMAIN "127.0.0.1"

namespace po = boost::program_options;
namespace al = boost::algorithm;


/* Transport functions prototypes */
static pj_status_t transport_get_info (pjmedia_transport *tp,
		pjmedia_transport_info *info);
static pj_status_t transport_attach2  (pjmedia_transport *tp,
		pjmedia_transport_attach_param *att_prm);
static void	   transport_detach   (pjmedia_transport *tp,
		void *strm);
static pj_status_t transport_send_rtp( pjmedia_transport *tp,
		const void *pkt,
		pj_size_t size);
static pj_status_t transport_send_rtcp(pjmedia_transport *tp,
		const void *pkt,
		pj_size_t size);
static pj_status_t transport_send_rtcp2(pjmedia_transport *tp,
		const pj_sockaddr_t *addr,
		unsigned addr_len,
		const void *pkt,
		pj_size_t size);
static pj_status_t transport_media_create(pjmedia_transport *tp,
		pj_pool_t *sdp_pool,
		unsigned options,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index);
static pj_status_t transport_encode_sdp(pjmedia_transport *tp,
		pj_pool_t *sdp_pool,
		pjmedia_sdp_session *local_sdp,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index);
static pj_status_t transport_media_start (pjmedia_transport *tp,
		pj_pool_t *pool,
		const pjmedia_sdp_session *local_sdp,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index);
static pj_status_t transport_media_stop(pjmedia_transport *tp);
static pj_status_t transport_simulate_lost(pjmedia_transport *tp,
		pjmedia_dir dir,
		unsigned pct_lost);
static pj_status_t transport_destroy  (pjmedia_transport *tp);


static __thread uint64_t counter;


/* The transport operations */
static struct pjmedia_transport_op tp_adapter_op =
{
		&transport_get_info,
		NULL,
		&transport_detach,
		&transport_send_rtp,
		&transport_send_rtcp,
		&transport_send_rtcp2,
		&transport_media_create,
		&transport_encode_sdp,
		&transport_media_start,
		&transport_media_stop,
		&transport_simulate_lost,
		&transport_destroy,
		&transport_attach2,
};


/* The transport adapter instance */
struct tp_adapter
{
	pjmedia_transport	 base;
	pj_bool_t		 del_base;

	pj_pool_t		*pool;

	/* Stream information. */
	void		*stream_user_data;
	void                *stream_ref;
	void	       (*stream_rtp_cb)(void *user_data,
			void *pkt,
			pj_ssize_t);
	void	       (*stream_rtcp_cb)(void *user_data,
			void *pkt,
			pj_ssize_t);
	void			(* 	stream_rtp_cb2 )(pjmedia_tp_cb_param *param);


	/* Add your own member here.. */
	pjmedia_transport	*slave_tp;
};


/*
 * Create the adapter.
 */
PJ_DEF(pj_status_t) pjmedia_tp_adapter_create( pjmedia_endpt *endpt,
		const char *name,
		pjmedia_transport *transport,
		pj_bool_t del_base,
		pjmedia_transport **p_tp)
		{
	pj_pool_t *pool;
	struct tp_adapter *adapter;

	if (name == NULL)
		name = "tpad%p";

	/* Create the pool and initialize the adapter structure */
	pool = pjmedia_endpt_create_pool(endpt, name, 512, 512);
	adapter = PJ_POOL_ZALLOC_T(pool, struct tp_adapter);
	adapter->pool = pool;
	pj_ansi_strncpy(adapter->base.name, pool->obj_name,
			sizeof(adapter->base.name));
	adapter->base.type = (pjmedia_transport_type)
																					 (PJMEDIA_TRANSPORT_TYPE_USER + 1);
	adapter->base.op = &tp_adapter_op;

	/* Save the transport as the slave transport */
	adapter->slave_tp = transport;
	adapter->del_base = del_base;

	/* Done */
	*p_tp = &adapter->base;
	return PJ_SUCCESS;
		}


/*
 * get_info() is called to get the transport addresses to be put
 * in SDP c= line and a=rtcp line.
 */
static pj_status_t transport_get_info(pjmedia_transport *tp,
		pjmedia_transport_info *info)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* Since we don't have our own connection here, we just pass
	 * this function to the slave transport.
	 */
	return pjmedia_transport_get_info(adapter->slave_tp, info);
}


/* This is our RTP callback, that is called by the slave transport when it
 * receives RTP packet.
 */
static void transport_rtp_cb(void *user_data, void *pkt, pj_ssize_t size)
{
	struct tp_adapter *adapter = (struct tp_adapter*)user_data;

	//pj_assert(adapter->stream_rtp_cb != NULL);

	/* Call stream's callback */
	adapter->stream_rtp_cb(adapter->stream_user_data, pkt, size);
}

static void transport_rtp_cb2(pjmedia_tp_cb_param *param)
{
	struct tp_adapter *adapter = (struct tp_adapter*)param->user_data;
	//pj_assert(adapter->stream_rtp_cb2 != NULL);
	pjmedia_tp_cb_param new_param;
	new_param=*param;
	new_param.user_data=adapter->stream_user_data;


	/* Call stream's callback */
	adapter->stream_rtp_cb2(&new_param);
	counter++;
}


/* This is our RTCP callback, that is called by the slave transport when it
 * receives RTCP packet.
 */
static void transport_rtcp_cb(void *user_data, void *pkt, pj_ssize_t size)
{
	struct tp_adapter *adapter = (struct tp_adapter*)user_data;

	pj_assert(adapter->stream_rtcp_cb != NULL);

	/* Call stream's callback */
	adapter->stream_rtcp_cb(adapter->stream_user_data, pkt, size);
}



/*
 * attach2() is called by stream to register callbacks that we should
 * call on receipt of RTP and RTCP packets.
 */
static pj_status_t transport_attach2(pjmedia_transport *tp,
		pjmedia_transport_attach_param *att_param)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;
	pj_status_t status;

	/* In this example, we will save the stream information and callbacks
	 * to our structure, and we will register different RTP/RTCP callbacks
	 * instead.
	 */
	pj_assert(adapter->stream_user_data == NULL);
	adapter->stream_user_data = att_param->user_data;
	adapter->stream_rtp_cb = att_param->rtp_cb;
	adapter->stream_rtp_cb2 = att_param->rtp_cb2;
	adapter->stream_rtcp_cb = att_param->rtcp_cb;
	adapter->stream_ref = att_param->stream;

	att_param->rtp_cb = &transport_rtp_cb;
	att_param->rtp_cb2 = &transport_rtp_cb2;
	att_param->rtcp_cb = &transport_rtcp_cb;
	att_param->user_data = adapter;

	status = pjmedia_transport_attach2(adapter->slave_tp, att_param);
	if (status != PJ_SUCCESS) {
		adapter->stream_user_data = NULL;
		adapter->stream_rtp_cb = NULL;
		adapter->stream_rtp_cb2 = NULL;
		adapter->stream_rtcp_cb = NULL;
		adapter->stream_ref = NULL;
		return status;
	}

	return PJ_SUCCESS;
}

/*
 * detach() is called when the media is terminated, and the stream is
 * to be disconnected from us.
 */
static void transport_detach(pjmedia_transport *tp, void *strm)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	PJ_UNUSED_ARG(strm);

	if (adapter->stream_user_data != NULL) {
		pjmedia_transport_detach(adapter->slave_tp, adapter);
		adapter->stream_user_data = NULL;
		adapter->stream_rtp_cb = NULL;
		adapter->stream_rtp_cb2 = NULL;
		adapter->stream_rtcp_cb = NULL;
		adapter->stream_ref = NULL;
	}
}


/*
 * send_rtp() is called to send RTP packet. The "pkt" and "size" argument
 * contain both the RTP header and the payload.
 */
static pj_status_t transport_send_rtp( pjmedia_transport *tp,
		const void *pkt,
		pj_size_t size)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* You may do some processing to the RTP packet here if you want. */

	/* Send the packet using the slave transport */
	return pjmedia_transport_send_rtp(adapter->slave_tp, pkt, size);
}


/*
 * send_rtcp() is called to send RTCP packet. The "pkt" and "size" argument
 * contain the RTCP packet.
 */
static pj_status_t transport_send_rtcp(pjmedia_transport *tp,
		const void *pkt,
		pj_size_t size)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* You may do some processing to the RTCP packet here if you want. */

	/* Send the packet using the slave transport */
	return pjmedia_transport_send_rtcp(adapter->slave_tp, pkt, size);
}


/*
 * This is another variant of send_rtcp(), with the alternate destination
 * address in the argument.
 */
static pj_status_t transport_send_rtcp2(pjmedia_transport *tp,
		const pj_sockaddr_t *addr,
		unsigned addr_len,
		const void *pkt,
		pj_size_t size)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;
	return pjmedia_transport_send_rtcp2(adapter->slave_tp, addr, addr_len,
			pkt, size);
}

/*
 * The media_create() is called when the transport is about to be used for
 * a new call.
 */
static pj_status_t transport_media_create(pjmedia_transport *tp,
		pj_pool_t *sdp_pool,
		unsigned options,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* if "rem_sdp" is not NULL, it means we are UAS. You may do some
	 * inspections on the incoming SDP to verify that the SDP is acceptable
	 * for us. If the SDP is not acceptable, we can reject the SDP by
	 * returning non-PJ_SUCCESS.
	 */

	/* Once we're done with our initialization, pass the call to the
	 * slave transports to let it do it's own initialization too.
	 */
	return pjmedia_transport_media_create(adapter->slave_tp, sdp_pool, options,
			rem_sdp, media_index);
}

/*
 * The encode_sdp() is called when we're about to send SDP to remote party,
 * either as SDP offer or as SDP answer.
 */
static pj_status_t transport_encode_sdp(pjmedia_transport *tp,
		pj_pool_t *sdp_pool,
		pjmedia_sdp_session *local_sdp,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* If "rem_sdp" is not NULL, it means we're encoding SDP answer. You may
	 * do some more checking on the SDP's once again to make sure that
	 * everything is okay before we send SDP.
	 */


	/* And then pass the call to slave transport to let it encode its
	 * information in the SDP. You may choose to call encode_sdp() to slave
	 * first before adding your custom attributes if you want.
	 */
	return pjmedia_transport_encode_sdp(adapter->slave_tp, sdp_pool, local_sdp,
			rem_sdp, media_index);
}

/*
 * The media_start() is called once both local and remote SDP have been
 * negotiated successfully, and the media is ready to start. Here we can start
 * committing our processing.
 */
static pj_status_t transport_media_start(pjmedia_transport *tp,
		pj_pool_t *pool,
		const pjmedia_sdp_session *local_sdp,
		const pjmedia_sdp_session *rem_sdp,
		unsigned media_index)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* Do something.. */

	/* And pass the call to the slave transport */
	return pjmedia_transport_media_start(adapter->slave_tp, pool, local_sdp,
			rem_sdp, media_index);
}

/*
 * The media_stop() is called when media has been stopped.
 */
static pj_status_t transport_media_stop(pjmedia_transport *tp)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* Do something.. */

	/* And pass the call to the slave transport */
	return pjmedia_transport_media_stop(adapter->slave_tp);
}

/*
 * simulate_lost() is called to simulate packet lost
 */
static pj_status_t transport_simulate_lost(pjmedia_transport *tp,
		pjmedia_dir dir,
		unsigned pct_lost)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;
	return pjmedia_transport_simulate_lost(adapter->slave_tp, dir, pct_lost);
}

/*
 * destroy() is called when the transport is no longer needed.
 */
static pj_status_t transport_destroy  (pjmedia_transport *tp)
{
	struct tp_adapter *adapter = (struct tp_adapter*)tp;

	/* Close the slave transport */
	if (adapter->del_base) {
		pjmedia_transport_close(adapter->slave_tp);
	}

	/* Self destruct.. */
	pj_pool_release(adapter->pool);

	return PJ_SUCCESS;
}

ENUM(eCallType, uint16_t, UNINITIALISED, SIMULATED_AXE_CALL_OFFER, SIMULATED_S10_CALL_OFFER, RECEIVED_CALL );
ENUM(eSimulatorDirectionType, uint16_t, SIMULATED_UNI_DIRECTIONAL, SIMULATE_BI_DIRECTIONAL);
ENUM(eAgentDirectionType, uint16_t,  ANSWER_UNI_DIRECTIONAL, ANSWER_BI_DIRECTIONAL );

//grab bag for anything required on a per call basis - sorta C/C++ halfway house of yuk...
class LocalCallUserData {
public:
	pj_pool_t* pool;
	eCallType  callType;
	eSimulatorDirectionType simDir;
	eAgentDirectionType agentDir;
	pjsua_call_id call_id;

	std::shared_ptr<std::function<void(void)>> clearCallTimerCB;


	LocalCallUserData():callType(eCallType::UNINITIALISED),
			simDir(eSimulatorDirectionType::SIMULATED_UNI_DIRECTIONAL),
			agentDir(eAgentDirectionType::ANSWER_UNI_DIRECTIONAL),call_id(-1)
	{
		pool = pjmedia_endpt_create_pool(pjsua_get_pjmedia_endpt(), "USER_CALL_%p", 512, 512);
	}

	~LocalCallUserData()
	{
		pj_pool_release(pool); //this frees up the memory under the object!
	}

	static LocalCallUserData* LookupByCall(pjsua_call_id call_id)
	{
		return (LocalCallUserData*)pjsua_call_get_user_data (call_id);
	}

	void BindToCall(pjsua_call_id _call_id)
	{
		call_id=_call_id;
		pjsua_call_set_user_data (call_id,this);
	}

	static void timer_heap_callback(void *user_data)
	{
		LocalCallUserData* callUserData = (LocalCallUserData*)user_data;
		(*(callUserData->clearCallTimerCB))();
	}

	void SetHangupTimer(uint32_t sec_timeout)
	{
		pjsua_call_id c = call_id; //capture c by value
		std::function<void(void)>* lambda = new std::function<void(void)>([c](void)
				{
			if (c !=-1)
				pjsua_call_hangup(c,200,0,0);
				});
		clearCallTimerCB=std::shared_ptr<std::function<void(void)>>(lambda);
		pjsua_schedule_timer2(timer_heap_callback,this,sec_timeout*1000);
	}


};

void add_SIP_I_AXE_IAM_Mime(pj_pool_t* pool, pjsua_msg_data *msg_data)
{
	pjsip_multipart_part *alt_part;
	const char* content_type_string = "application";
	const char* content_subtype_string = "ISUP; version=itu-t \nContent-Disposition: signal; handling=optional";
	const char* content_string = "THIS WAS AN AXE IAM A bunch of zeros \0\0\0\0 And after the zeros - ";

	const pj_str_t content_type=pj_str((char*)content_type_string);
	const pj_str_t content_subtype=pj_str((char*)content_subtype_string);
	const pj_str_t content={(char*)content_string,sizeof(content_string)}; //note NOT STRLEN!!!

	alt_part = pjsip_multipart_create_part(pool);
	alt_part->body = pjsip_msg_body_create(pool, &content_type,
			&content_subtype, &content);

	pjsua_msg_data_init(msg_data);

	msg_data->multipart_ctype.type = pj_str((char*)"multipart");
	msg_data->multipart_ctype.subtype = pj_str((char*)"mixed");
	pj_list_push_back(&msg_data->multipart_parts, alt_part);
}

void add_SIP_I_S12_IAM_Mime(pj_pool_t* pool, pjsua_msg_data *msg_data)
{
	pjsip_multipart_part *alt_part;
	const char* content_type_string = "application";
	const char* content_subtype_string = "ISUP; version=itu-t \nContent-Disposition: signal; handling=optional";
	const char* content_string = "THIS WAS AN S12 IAM A bunch of zeros \0\0\0\0 And after the zeros - ";

	const pj_str_t content_type=pj_str((char*)content_type_string);
	const pj_str_t content_subtype=pj_str((char*)content_subtype_string);
	const pj_str_t content={(char*)content_string,sizeof(content_string)}; //note NOT STRLEN!!!

	alt_part = pjsip_multipart_create_part(pool);
	alt_part->body = pjsip_msg_body_create(pool, &content_type,
			&content_subtype, &content);

	pjsua_msg_data_init(msg_data);

	msg_data->multipart_ctype.type = pj_str((char*)"multipart");
	msg_data->multipart_ctype.subtype = pj_str((char*)"mixed");
	pj_list_push_back(&msg_data->multipart_parts, alt_part);
}

void add_SIP_I_CON_Mime(pj_pool_t* pool, pjsua_msg_data *msg_data)
{
	pjsip_multipart_part *alt_part;
	const char* content_type_string = "application";
	const char* content_subtype_string = "ISUP; version=itu-t \nContent-Disposition: signal; handling=optional";
	const char* content_string = "HIS WAS A CON A bunch of zeros \0\0\0\0 And after the zeros - T";

	const pj_str_t content_type=pj_str((char*)content_type_string);
	const pj_str_t content_subtype=pj_str((char*)content_subtype_string);
	const pj_str_t content={(char*)content_string,sizeof(content_string)}; //note NOT STRLEN!!!

	alt_part = pjsip_multipart_create_part(pool);
	alt_part->body = pjsip_msg_body_create(pool, &content_type,
			&content_subtype, &content);

	pjsua_msg_data_init(msg_data);

	msg_data->multipart_ctype.type = pj_str((char*)"multipart");
	msg_data->multipart_ctype.subtype = pj_str((char*)"mixed");
	pj_list_push_back(&msg_data->multipart_parts, alt_part);
}


/*
 * This callback is called when media transport needs to be created.
 */

static pjmedia_transport* on_create_media_transport(pjsua_call_id call_id,
		unsigned media_idx,
		pjmedia_transport *base_tp,
		unsigned flags)
{
	pjmedia_transport *adapter;
	pj_status_t status;

	/* Create the adapter */
	status = pjmedia_tp_adapter_create(pjsua_get_pjmedia_endpt(),
			NULL, base_tp,
			(flags & PJSUA_MED_TP_CLOSE_MEMBER),
			&adapter);
	if (status != PJ_SUCCESS) {
		PJ_PERROR(1,(THIS_FILE, status, "Error creating adapter"));
		return NULL;
	}

	PJ_LOG(3,(THIS_FILE, "Media transport is created for call %d media %d",
			call_id, media_idx));

	return adapter;
}

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
		pjsip_rx_data *rdata)
{
	pjsua_call_info ci;
	char buffer[1024];

	pjsip_media_type ISUP_TYPE;
	ISUP_TYPE.type = pj_str((char*)"application");
	ISUP_TYPE.subtype = pj_str((char*)"ISUP");



	PJ_UNUSED_ARG(acc_id);


	pjsua_call_get_info(call_id, &ci);

	PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			(int)ci.remote_info.slen,
			ci.remote_info.ptr));
	pjsip_multipart_part* part = pjsip_multipart_find_part(rdata->msg_info.msg->body,&ISUP_TYPE,0);
	pj_memcpy(buffer,part->body->data,part->body->len);


	pjsua_msg_data msg_data;

	//NOTE THAT USER DATA IS ALREADY ALLOCATED in on_call_sdp_created
	add_SIP_I_CON_Mime(LocalCallUserData::LookupByCall(call_id)->pool,&msg_data);

	/* Automatically answer incoming calls with 200/OK */
	pjsua_call_answer(call_id, 200, NULL, &msg_data);
}



/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	pjsua_call_info ci;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);
	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
			(int)ci.state_text.slen,
			ci.state_text.ptr));
	switch (ci.state)
	{
	case PJSIP_INV_STATE_DISCONNECTED :
		delete LocalCallUserData::LookupByCall(call_id);
		break;
	default:
		break;

	}
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info ci;
	pjsua_stream_info si;

	pjsua_call *call;
	pjsua_call_media *call_med;
	pj_status_t status;


	unsigned med_idx=0;

	pjsua_call_get_info(call_id, &ci);


	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		// When media is active, connect call to sound device.
		// pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);


		PJSUA_LOCK();

		call = &pjsua_var.calls[call_id];


		call_med = &call->media[med_idx];

		pjmedia_stream_pause(call_med->strm.a.stream,PJMEDIA_DIR_DECODING);
		//pjmedia_stream_pause(call_med->strm.a.stream,PJMEDIA_DIR_ENCODING_DECODING);

		PJSUA_UNLOCK();

		//pjsua_call_get_stream_info(call_id,0,&si);

	}
}





//NOTE THAT THIS IS CALLED PRIOR TO on_incoming_call - so YOU HAVE NOT HAD A CHANCE TO SETUP CALL USER DATA YET
static void on_call_sdp_created (pjsua_call_id call_id, pjmedia_sdp_session *sdp, pj_pool_t *pool, const pjmedia_sdp_session *rem_sdp)
{

	LocalCallUserData* userData = LocalCallUserData::LookupByCall(call_id);

	//if there is no user data allocated it is because we are answering a call - and call_sdp_created is called PRIOR to on_incoming call
	//so we need to allocate a call structure and save it
	if (!userData)
	{
		userData = new LocalCallUserData;

		userData->callType=eCallType::RECEIVED_CALL;

		userData->BindToCall(call_id);
	}

	if (userData->callType == +eCallType::RECEIVED_CALL)
	{
		if(userData->agentDir == +eAgentDirectionType::ANSWER_UNI_DIRECTIONAL)
		{
			//need to generate an SDP suppressing the TX audio
			//for each media line change a=sendrecv to a=sendonly
			for (uint16_t i=0; i<sdp->media_count; i++)
			{
				auto attr = pjmedia_sdp_media_find_attr2(sdp->media[i],"sendrecv",0);
				if (attr)
				{
					pjmedia_sdp_media_remove_attr(sdp->media[i],attr);
					attr=pjmedia_sdp_attr_create(pool,"recvonly",0);
					pjmedia_sdp_media_add_attr (sdp->media[i],attr);
				}
			}

		}


	}
	else
	{
		if(userData->simDir == +eSimulatorDirectionType::SIMULATED_UNI_DIRECTIONAL)
		{
			//need to generate an SDP suppressing the RX audio
			//for each media line change a=sendrecv to a=recvonly
			for (uint16_t i=0; i<sdp->media_count; i++)
			{
				auto attr = pjmedia_sdp_media_find_attr2(sdp->media[i],"sendrecv",0);
				if (attr)
				{
					pjmedia_sdp_media_remove_attr(sdp->media[i],attr);
					attr=pjmedia_sdp_attr_create(pool,"sendonly",0);
					pjmedia_sdp_media_add_attr (sdp->media[i],attr);
				}
			}
		}

	}
}









int main(int argc, char** argv)
{

	unsigned short port;

	std::string uri_to_call_string;

	po::options_description desc;
	desc.add_options()
	    																																						  ("help,h", "Help screen")
																																								  ("port,p",po::value(&port)->default_value(5060),"sip port to listen on")
																																								  //																						  ("transport,t",po::value(&transport_string),"transport to use, tcp/udp")
																																								  ("server", "activate server thread")
																																								  ("client", po::value(&uri_to_call_string)->default_value(std::string("sip:127.0.0.1")),"activate client thread")
																																								  ;


	po::variables_map vm;

	try
	{
		po::store(parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

	}
	catch(...)
	{
		exit(-1);
	}

	{
		struct sched_param schedule;
		schedule.__sched_priority = 10;
		auto rc = sched_setscheduler(0,SCHED_RR,&schedule);
		if (rc==-1)
		{
			perror("sched_setscheduler failed with");
			exit(-1);
		}
	}


	enum pjsip_transport_type_e transport;

	transport=PJSIP_TRANSPORT_TCP;

	uri_to_call_string=uri_to_call_string + std::string(";transport=tcp");

	pjsua_acc_id acc_id;

	{
		//initialise realtime subsystem

	}

	/* Create pjsua first! */
	pjsua_create();


	/* Init pjsua */
	{
		pjsua_config ua_cfg;
		pjsua_logging_config log_cfg;
		pjsua_media_config media_cfg;


		pjsua_config_default(&ua_cfg);
		pjsua_logging_config_default(&log_cfg);
		pjsua_media_config_default(&media_cfg);
		ua_cfg.cb.on_incoming_call = &on_incoming_call;
		ua_cfg.cb.on_call_media_state = &on_call_media_state;
		ua_cfg.cb.on_call_state = &on_call_state;
		ua_cfg.cb.on_create_media_transport=&on_create_media_transport;
		ua_cfg.cb.on_call_sdp_created=&on_call_sdp_created;

		ua_cfg.max_calls = 1200;
		ua_cfg.thread_cnt=4;

		log_cfg.console_level = 3;

		media_cfg.no_vad = 1; //disable VAD
		media_cfg.thread_cnt=8;

		pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
	}

	pjsua_verify_url(uri_to_call_string.c_str());

	{
		/* Add transport. */
		pjsua_transport_config cfg;

		pjsua_transport_config_default(&cfg);
		cfg.port = port;
		pjsua_transport_create(transport, &cfg, NULL);

	}



	/* Initialization is done, now start pjsua */
	pjsua_start() ;

	{
		/* Register to SIP server by creating SIP account. */
		pjsua_acc_config cfg;

		pjsua_acc_config_default(&cfg);
		cfg.id = pj_str((char*)"sip:" SIP_DOMAIN);

		pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	}
	/* If URL is specified, make call to the URL. */

	pjsua_set_null_snd_dev();



	if (vm.count("server")==0)
	{
		//this is bullshit - what we want to do is force the use of A law for testing - lots of bandwidth but only small CPU load
		pjmedia_codec_info* inf;

		//first find the codec manager singleton
		pjmedia_codec_mgr* codec_mgr = pjmedia_endpt_get_codec_mgr(pjsua_get_pjmedia_endpt());

		//the only way to set codec priority is by identifying it by name (actually a PJ_STR)
		//but we cant it directly - first get codec info using the static enum that actually defines the codec
		pjmedia_codec_mgr_get_codec_info(codec_mgr,  (unsigned int)PJMEDIA_RTP_PT_PCMA , (const pjmedia_codec_info**)&inf);

		//and then use the name to look up the info again and set priority
		pjmedia_codec_mgr_set_codec_priority(codec_mgr,&(inf->encoding_name), PJMEDIA_CODEC_PRIO_HIGHEST);



		for (int i=0; i<550; i++)
		{

			pjsua_msg_data msg_data;
			pjsua_call_id call_id;

			LocalCallUserData* callUserData = new LocalCallUserData;
			add_SIP_I_AXE_IAM_Mime(callUserData->pool,&msg_data);
			pj_str_t uri = pj_str((char *)uri_to_call_string.c_str());
			callUserData->callType=eCallType::SIMULATED_AXE_CALL_OFFER;
			pjsua_call_make_call(acc_id, &uri, 0, callUserData, &msg_data, &(callUserData->call_id));
			callUserData->SetHangupTimer(120);
			usleep(100000);
		}

	}

	/* Wait until user press "q" to quit. */
	for (;;) {
		char option[10];

		puts("Press 'h' to hangup all calls, 'q' to quit");
		if (fgets(option, sizeof(option), stdin) == NULL) {
			puts("EOF while reading stdin, will quit now..");
			break;
		}

		if (option[0] == 'q')
			break;

		if (option[0] == 'h')
			pjsua_call_hangup_all();

		if (option[0] == 'l')
		{
			auto call_count = pjsua_call_get_count()+10; //reserve a little extra space, can 10 calls turn up in the meantime....
			if (call_count>0)
			{
				std::vector<pjsua_call_id> calls(call_count);
				pjsua_enum_calls(calls.data(),&call_count); //may update call count on another thread - we send in the max size and get back the real number
				calls.resize(call_count);
				for (auto call: calls)
				{
					pjsua_call_info info;
					if(pjsua_call_get_info(call,&info) == PJ_SUCCESS) //get info on the call - but it may disappear while iterating - so not finding it is not an error
					{

						printf("ID: %d\nFrom: %.*s To: %.*s\nCallID: %.*s\nState: %.*s\nConnect Duration: %ld.%03lds Total Duration %ld.%03lds\n",
								call,
								(int)info.local_contact.slen,
								info.local_contact.ptr,
								(int)info.remote_contact.slen,
								info.remote_contact.ptr,
								(int)info.call_id.slen,
								info.call_id.ptr,
								(int)info.state_text.slen,
								info.state_text.ptr,
								info.connect_duration.sec,
								info.connect_duration.msec,
								info.total_duration.sec,
								info.total_duration.msec);


						PJSUA_LOCK();

						pjsua_call* pcall = &pjsua_var.calls[call];

						if (pcall) //was it still there??
						{


							pjsua_call_media* call_med = &pcall->media[0];  //this breaks when there are multiple media types...
							pjmedia_rtcp_stat rtcp_stats;
							pjmedia_stream_get_stat(call_med->strm.a.stream,&rtcp_stats);
							PJSUA_UNLOCK();
							printf ("rx stats\npkts: %d bytes: %d\ndiscarded: %d lost: %d\nreordered: %d dup: %d\njitter (us) min/mean/max %d/%d/%d\n",
									rtcp_stats.rx.pkt,
									rtcp_stats.rx.bytes,
									rtcp_stats.rx.discard,
									rtcp_stats.rx.loss,
									rtcp_stats.rx.reorder,
									rtcp_stats.rx.dup,
									rtcp_stats.rx.jitter.min,
									rtcp_stats.rx.jitter.mean,
									rtcp_stats.rx.jitter.max);


						}
						else
						{
							PJSUA_UNLOCK();
						}



					}

				}
			}
		}
	}

	return 0;
}









