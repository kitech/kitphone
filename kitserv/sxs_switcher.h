// sxs_switcher.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-09 14:38:28 +0800
// Version: $Id$
// 

#ifndef _SXS_SWITCHER_H_
#define _SXS_SWITCHER_H_

#include "ringbuffer.h"

#include <pjmedia/wav_port.h>
#include <pjmedia/alaw_ulaw.h>
#include <pjmedia/errno.h>
#include <pjmedia/wave.h>
#include <pjmedia/jbuf.h>
#include <pj/assert.h>
#include <pj/file_access.h>
#include <pj/file_io.h>
#include <pj/log.h>
#include <pj/pool.h>
#include <pj/string.h>

#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>

class ServContex;
class PCMSxsHost;

typedef struct sxs_port
{
    pjmedia_port base;
    pjmedia_wave_fmt_tag fmt_tag;
    pj_uint16_t	     bytes_per_sample;

    pj_size_t	     bufsize;
    // char	    *buf;
    char	    *writepos;
    pj_size_t	     total;

    int   direct_type; // as a server 1, or a client 2
    // pj_oshandle_t    fd; // it's tcp socket port now
    ringbuffer_t *in_vocie_frame_rb;
    bool use_frame_buff;

    int             out_serv_fd;
    int             out_cli_fd;
    int             in_serv_fd;
    int             in_cli_fd;

    char rbuf[1024*1280];

    // speed symboles
    int out_read_speed;
    int out_write_speed;
    int in_read_speed;
    int in_write_speed;

    int out_read_total;
    int out_write_total;
    int in_read_total;
    int in_write_total;

    time_t out_write_start_time;
    time_t in_read_start_time;

    pj_size_t	     cb_size;
    pj_status_t	   (*cb)(pjmedia_port*, void*);

    int eph; // epoll handler

    pj_pool_t *pool;

    ServContex *ctx;
    PCMSxsHost *pcm_host;
    
} sxs_port_t;

PJ_DEF(pj_status_t) pjmedia_sxs_port_server_create( // pj_pool_t *pool,
                                                   //     const char *filename,
                                                        unsigned sampling_rate,
                                                        unsigned channel_count,
                                                        unsigned samples_per_frame,
                                                        unsigned bits_per_sample,
                                                        unsigned flags,
                                                        pj_ssize_t buff_size,
                                                        pjmedia_port **p_port);

PJ_DEF(unsigned short) pjmedia_sxs_port_outgoing_server_get_port(pjmedia_port *m_port, unsigned short *n_port);
PJ_DEF(unsigned short) pjmedia_sxs_port_incoming_server_get_port(pjmedia_port *m_port, unsigned short *n_port);
/*
 * Register callback.
 */
PJ_DEF(pj_status_t) pjmedia_sxs_port_set_cb( pjmedia_port *port,
                                                 pj_size_t pos,
                                                 void *user_data,
                                                 pj_status_t(*cb)(pjmedia_port *port,
                                                                  void *usr_data));

PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_sock_connected(pjmedia_port *p_port, int fd);

PJ_DEF(pj_status_t) pjmedia_sxs_incoming_sock_connected(pjmedia_port *p_port, int fd);
PJ_DEF(pj_status_t) pjmedia_sxs_incoming_sock_ready_read(pjmedia_port *p_port, int fd);

PJ_DEF(pj_status_t) pjmedia_sxs_close_outgoing_cli(pjmedia_port *p_port, int fd);
PJ_DEF(pj_status_t) pjmedia_sxs_close_incoming_cli(pjmedia_port *p_port, int fd);

PJ_DEF(pj_status_t) pjmedia_sxs_incoming_media_connected(pjmedia_port *p_port);
PJ_DEF(pj_status_t) pjmedia_sxs_incoming_media_disconnected(pjmedia_port *p_port);
PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_media_connected(pjmedia_port *p_port);
PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_media_disconnected(pjmedia_port *p_port);

PJ_DEF(pj_status_t) pjmedia_sxs_set_pcm_host(pjmedia_port *p_port, ServContex *ctx, PCMSxsHost *pcm_host);

///////////////////////////////////////////// 
#define SUA_SWITCHER_SERVER 1
#define SUA_SWITCHER_CLIENT 2


/*
 * Create a file recorder, and automatically connect this recorder to
 * the conference bridge.
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_create(pj_pool_t *pool,
                                              unsigned enc_type,
                                              void *enc_param,
                                              unsigned options,
                                              pjsua_recorder_id *p_id);

/*
 * Get conference port associated with recorder.
 */
PJ_DEF(pjsua_conf_port_id) pjsua_sxs_switcher_get_conf_port(pjsua_recorder_id id);

/*
 * Get the media port for the recorder.
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_get_port( pjsua_recorder_id id,
                                             pjmedia_port **p_port);
/*
 * Destroy recorder (this will complete recording).
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_destroy(pjsua_recorder_id id);


#endif /* _SXS_SWITCHER_H_ */
