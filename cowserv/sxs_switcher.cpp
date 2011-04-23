// sxs_switcher.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-12-09 14:39:15 +0800
// Version: $Id$
// 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <syscall.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <assert.h>
#include <errno.h>
#include <sys/epoll.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// #include <QtCore>

#include "kitserv.h"
#include "PCMSxsHost.h"

#include "sxs_switcher.h"

// #include "siproom.h"

// extern sip_proc_param_t *g_param;

#define THIS_FILE	    "sxs_switcher.cpp"
#define SIGNATURE	    PJMEDIA_PORT_SIGNATURE('F', 'W', 'R', 'T')

#define WAV_TCP_PORT_SERVER 1
#define WAV_TCP_PORT_CLIENT 2

/////// TODO too many rubbish code, should clean right now

//////////
static pj_status_t file_put_frame(pjmedia_port *this_port, 
                                  const pjmedia_frame *frame);
static pj_status_t file_get_frame(pjmedia_port *this_port, 
                                  pjmedia_frame *frame);
static pj_status_t file_on_destroy(pjmedia_port *this_port);


/////////////
pj_status_t pjmedia_sxs_port_server_create(// pj_pool_t *pool,
                                           // const char *filename,
                                            unsigned sampling_rate,
                                            unsigned channel_count,
                                            unsigned samples_per_frame,
                                            unsigned bits_per_sample,
                                            unsigned flags,
                                            pj_ssize_t buff_size,
                                            pjmedia_port **p_port)
{
    sxs_port *fport;
    pjmedia_wave_hdr wave_hdr;
    pj_ssize_t size;
    pj_str_t name;
    pj_status_t status;

    /* Check arguments. */
    // PJ_ASSERT_RETURN(pool && filename && p_port, PJ_EINVAL);

    /* Only supports 16bits per sample for now.
     * See flush_buffer().
     */
    PJ_ASSERT_RETURN(bits_per_sample == 16, PJ_EINVAL);

    fport = (sxs_port_t *)calloc(1, sizeof(sxs_port_t));
    memset(fport, 0, sizeof(sxs_port_t));
    // fport->eph = g_param->epfd;
    fport->in_vocie_frame_rb = ringbuffer_create(1024*1024*2);
    fport->use_frame_buff = true;
    // fport->use_frame_buff = false;

    name = pj_str("hahahahahaha");
    pjmedia_port_info_init(&fport->base.info, &name, SIGNATURE,
                           sampling_rate, channel_count, bits_per_sample,
                           samples_per_frame);

    fport->base.get_frame = &file_get_frame;
    fport->base.put_frame = &file_put_frame;
    fport->base.get_frame = NULL;
    fport->base.put_frame = NULL;
    fport->base.on_destroy = &file_on_destroy;

    if (flags == PJMEDIA_FILE_WRITE_ALAW) {
        fport->fmt_tag = PJMEDIA_WAVE_FMT_TAG_ALAW;
        fport->bytes_per_sample = 1;
    } else if (flags == PJMEDIA_FILE_WRITE_ULAW) {
        fport->fmt_tag = PJMEDIA_WAVE_FMT_TAG_ULAW;
        fport->bytes_per_sample = 1;
    } else {
        fport->fmt_tag = PJMEDIA_WAVE_FMT_TAG_PCM;
        fport->bytes_per_sample = 2;
    }
    

    /* Set buffer size. */
    if (buff_size < 1) buff_size = PJMEDIA_FILE_PORT_BUFSIZE;
    fport->bufsize = buff_size;

    /* Check that buffer size is greater than bytes per frame */
    // pj_assert(fport->bufsize >= fport->base.info.bytes_per_frame);

    // socket, bind, listen, accept
    // g_param->mpo_serv_sock = fport->out_serv_fd = create_tcp_server();
    // g_param->mpi_serv_sock = fport->in_serv_fd = create_tcp_server();

    /* Done. */
    *p_port = &fport->base;

    PJ_LOG(4,(THIS_FILE, 
              "File writer '%.*s' created: samp.rate=%d, bufsize=%uKB, bytes per frame=%d", 
              (int)fport->base.info.name.slen,
              fport->base.info.name.ptr,
              fport->base.info.clock_rate,
              fport->bufsize / 1000, 
              fport->base.info.bytes_per_frame));

    return PJ_SUCCESS;
    return 0;
}

PJ_DEF(unsigned short) pjmedia_sxs_port_outgoing_server_get_port(pjmedia_port *m_port, unsigned short *n_port)
{
    sxs_port_t *fport;

    /* Sanity check */
    // PJ_ASSERT_RETURN(m_port, PJ_EINVAL);
    PJ_ASSERT_RETURN(m_port, 0);

    /* Check that this is really a writer port */
    // PJ_ASSERT_RETURN(m_port->info.signature == SIGNATURE, PJ_EINVALIDOP);
    PJ_ASSERT_RETURN(m_port->info.signature == SIGNATURE, 0);

    // fport = (struct tcp_port*) m_port; // standard C struct trick
    fport = (sxs_port_t*)m_port; // standard C struct trick

    struct sockaddr_in serv_addr;
    socklen_t addr_size = sizeof(serv_addr) ;
    ::getsockname(fport->out_serv_fd, (struct sockaddr*)&serv_addr, &addr_size);
    unsigned short s_port = ntohs(serv_addr.sin_port);

    return s_port;
}

PJ_DEF(unsigned short) pjmedia_sxs_port_incoming_server_get_port(pjmedia_port *m_port, unsigned short *n_port )
{
    sxs_port_t *fport;

    /* Sanity check */
    // PJ_ASSERT_RETURN(m_port, PJ_EINVAL);
    PJ_ASSERT_RETURN(m_port, 0);

    /* Check that this is really a writer port */
    // PJ_ASSERT_RETURN(m_port->info.signature == SIGNATURE, PJ_EINVALIDOP);
    PJ_ASSERT_RETURN(m_port->info.signature == SIGNATURE, 0);

    // fport = (struct tcp_port*) m_port; // standard C struct tricky
    fport = (sxs_port_t*)m_port; // standard C struct tricky

    struct sockaddr_in serv_addr;
    socklen_t addrlen = sizeof(serv_addr);
    
    ::getsockname(fport->in_serv_fd, (struct sockaddr*)&serv_addr, &addrlen);
    unsigned short s_port = ntohs(serv_addr.sin_port);

    return s_port;
}

PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_sock_connected(pjmedia_port *p_port, int fd)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;

    fport->out_cli_fd = fd;
    fport->base.put_frame = &file_put_frame;
    fport->out_write_start_time = time(NULL);

    fprintf(stderr, "Info: %s %d enter file_put_frame: size=%d\n", __FILE__, __LINE__, fd);

    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_incoming_sock_connected(pjmedia_port *p_port, int fd)
{
  fprintf(stderr, "Info: %s %d enter file_put_frame: size=%d\n", __FILE__, __LINE__, fd);

    sxs_port_t *fport = (sxs_port_t*)p_port;
    struct epoll_event ev;
    int ret;

    fport->in_cli_fd = fd;
    fport->in_read_start_time = time(NULL);

    if (fport->use_frame_buff) {
        ev.data.fd = fport->in_cli_fd;
        ev.events = EPOLLIN;
        if ((ret = epoll_ctl(fport->eph, EPOLL_CTL_ADD, fport->in_cli_fd, &ev)) != 0) {
            perror("epoll add in cli read event");
            exit(-3);
        }
    }

    fport->base.get_frame = &file_get_frame;

    fprintf(stderr, "Info: %s %d enter file_put_frame: size=%d\n", __FILE__, __LINE__, fd);

    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_close_outgoing_cli(pjmedia_port *p_port, int fd)
{
    (void)(fd);
    sxs_port_t *fport = (sxs_port_t*)p_port;

    fport->out_cli_fd = -1;

    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_close_incoming_cli(pjmedia_port *p_port, int fd)
{
    (void)(fd);
    sxs_port_t *fport = (sxs_port_t*)p_port;

    fport->in_cli_fd = -1;

    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_incoming_sock_ready_read(pjmedia_port *p_port, int fd)
{
    (void)(fd);
    sxs_port_t *fport = (sxs_port_t*)p_port;

    char buf[1000] = {0};
    int ret;
    int rlen;

    rlen = ::read(fport->in_cli_fd, buf, sizeof(buf));
    // fprintf(stderr, "here ready incoming media: %d\n", rlen);

    if (rlen == 0) {
        ret = epoll_ctl(fport->eph, EPOLL_CTL_DEL, fport->in_cli_fd, NULL);
        return -2;
    }

    rlen = ringbuffer_write(fport->in_vocie_frame_rb, buf, rlen);

    if (random() % 10 == 8) {
      // this->in_cli_cond.wakeOne();
    }

//     fprintf(stderr, "Info: %s %d enter ready read: size=%d, total=%d\n", __FILE__, __LINE__, rlen
// 	    , ringbuffer_read_space(fport->in_vocie_frame_rb));

    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_incoming_media_connected(pjmedia_port *p_port)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;
    
    fport->base.get_frame = &file_get_frame;    

    return PJ_SUCCESS;
}
PJ_DEF(pj_status_t) pjmedia_sxs_incoming_media_disconnected(pjmedia_port *p_port)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;
    fport->base.get_frame = NULL;
    return PJ_SUCCESS;
}
PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_media_connected(pjmedia_port *p_port)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;
    
    fport->base.put_frame = &file_put_frame;

    return PJ_SUCCESS;
}
PJ_DEF(pj_status_t) pjmedia_sxs_outgoing_media_disconnected(pjmedia_port *p_port)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;
    fport->base.put_frame = NULL;
    return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) pjmedia_sxs_set_pcm_host(pjmedia_port *p_port, ServContex *ctx, PCMSxsHost *pcm_host)
{
    sxs_port_t *fport = (sxs_port_t*)p_port;

    fport->ctx = ctx;
    fport->pcm_host = pcm_host;
    
    return PJ_SUCCESS;
}

/*
 * Register callback.
 */
PJ_DEF(pj_status_t) pjmedia_sxs_port_set_cb( pjmedia_port *port,
                                             pj_size_t pos,
                                             void *user_data,
                                             pj_status_t (*cb)(pjmedia_port *port,
                                                               void *usr_data))
{
    sxs_port_t *fport;

    /* Sanity check */
    PJ_ASSERT_RETURN(port && cb, PJ_EINVAL);

    /* Check that this is really a writer port */
    PJ_ASSERT_RETURN(port->info.signature == SIGNATURE, PJ_EINVALIDOP);

    // fport = (struct tcp_port*) port; // standard C struct tricky
    fport = (sxs_port_t*)port; // standard C struct tricky

    fport->cb_size = pos;
    fport->base.port_data.pdata = user_data;
    fport->cb = cb;

    return PJ_SUCCESS;
}


#if defined(PJ_IS_BIG_ENDIAN) && PJ_IS_BIG_ENDIAN!=0
static void swap_samples(pj_int16_t *samples, unsigned count)
{
	unsigned i;
	for (i=0; i<count; ++i) {
	    samples[i] = pj_swap16(samples[i]);
	}
}
#else
#   define swap_samples(samples,count)
#endif

#if defined(PJ_IS_BIG_ENDIAN) && PJ_IS_BIG_ENDIAN!=0
    static void samples_to_host(pj_int16_t *samples, unsigned count)
    {
        unsigned i;
        for (i=0; i<count; ++i) {
           samples[i] = pj_swap16(samples[i]);
        }
    }
#else
#   define samples_to_host(samples,count)
#endif

/*
 * Put a frame into the buffer. When the buffer is full, flush the buffer
 * to the file.
 */
static pj_status_t file_put_frame(pjmedia_port *this_port, 
                                  const pjmedia_frame *frame)
{
    fprintf(stderr, "(T:%ld Info: %s %d enter file_put_frame: size=%ld\n", 
            syscall(__NR_gettid), __FILE__, __LINE__, frame->size);
    sxs_port_t *fport = (sxs_port_t*)this_port;
    unsigned frame_size;
    ServContex *ctx = fport->ctx;
    int fd = ctx->m_sock_pair_sv[0];
    PCMSxsHost *pcmif = (PCMSxsHost*)fport->pcm_host;
    assert(pcmif != NULL);

    if (ctx != NULL) {
        ctx->m_sip_media_thread_id = syscall(__NR_gettid);
    }

    if (frame->size == 0) {
        return PJ_SUCCESS;
    }

    if (fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_PCM)
        frame_size = frame->size;
    else
        frame_size = frame->size >> 1;

    int wlen = 0;
    /* Copy frame to buffer. */
    assert(fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_PCM);
    if (fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_PCM) {
        // default socketpair mode
        if (1) {
            wlen = ::write(fd, frame->buf, frame->size);
            if (wlen != frame->size) {
                fprintf(stderr, "Put frame package lost: should=%ld, lost=%ld\n", frame->size, 
                        wlen < 0 ? frame->size : (frame->size - wlen));
            }
            if (wlen > frame->size) {
                fprintf(stderr, "Ooops, what's problem: %d,%ld\n", wlen, frame->size);
                assert(1 == 2);
            } else if (wlen <= frame->size && wlen > 0) {
                fport->out_write_total += wlen;
                fport->out_write_speed = fport->out_write_total
                    / (time(NULL) - fport->out_write_start_time + 1) / 1000;
                if (random() % 12 == 10) {
                    fprintf(stderr, "T%ld,Random show out write speed: %d KB/s\n", syscall(__NR_gettid), 
                            fport->out_write_speed);
                }
            } else if (wlen <= 0) {
	    
            }
            // using shared ringbuffer mode
        } else {
            pcmif->input_append_frame((char*)frame->buf, frame->size);
        }
    } else {
        assert(1 == 2);
    }
    fport->writepos += frame_size;

    /* Increment total written, and check if we need to call callback */
    fport->total += frame_size;
    if (fport->cb && fport->total >= fport->cb_size) {
        pj_status_t (*cb)(pjmedia_port*, void*);
        pj_status_t status;

        cb = fport->cb;
        fport->cb = NULL;

        // status = (*cb)(this_port, this_port->port_data.pdata);
        return status;
    }

    return PJ_SUCCESS;
}

/*
 * Get frame, basicy is a no-op operation.
 */
static pj_status_t file_get_frame(pjmedia_port *this_port, 
                                  pjmedia_frame *frame)
{
    fprintf(stderr, "T: %ld Info: %s %d enter file_get_frame: size=%ld\n",
            syscall(__NR_gettid), __FILE__, __LINE__, frame->size);
    // struct tcp_port *fport = (struct tcp_port*)this_port;
    sxs_port_t *fport = (sxs_port_t*)this_port;
    unsigned frame_size;
    pj_status_t status;
    ServContex *ctx = fport->ctx;
    int fd = ctx->m_sock_pair_sv[0];
    PCMSxsHost *pcmif = (PCMSxsHost*)fport->pcm_host;
    assert(pcmif != NULL);

    pj_assert(fport->base.info.signature == SIGNATURE);
    // pj_assert(frame->size <= fport->bufsize);

    //pj_assert(frame->size == fport->base.info.bytes_per_frame);
    if (fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_PCM) {
        frame_size = frame->size;
        //frame->size = frame_size;
    } else {
        /* Must be ULAW or ALAW */
        pj_assert(fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_ULAW || 
                  fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_ALAW);

        frame_size = frame->size >> 1;
        frame->size = frame_size << 1;
    }

    /* Copy frame from buffer. */
    frame->type = PJMEDIA_FRAME_TYPE_AUDIO;
    frame->timestamp.u64 = 0;

    int read_cycle_cnt = 1;
    int rlen = 0;
    //*
    // if (fport->use_frame_buff == true) {
    //     while (true) {
    //         if (ringbuffer_read_space(fport->in_vocie_frame_rb) < frame_size) {
	//       break;
    //             if (read_cycle_cnt ++ > 3) {
	//             break;
	//         }

	//         usleep(10);
    //             continue;
    //             // fprintf(stderr, "read frame size: zero, buff size = %d\n", fport->incoming_media_frame_buffer.size());
    //             // frame->type = PJMEDIA_FRAME_TYPE_NONE;
    //             // frame->size = 0;
    //             // return PJ_EEOF;
    //         } else {
    //             rlen = ringbuffer_read(fport->in_vocie_frame_rb, (char*)frame->buf, frame_size);
	// 	if (rlen > 0) {
	// 	  fport->in_read_total += rlen;
	// 	  fport->in_read_speed = fport->in_read_total / (time(NULL) - fport->in_read_start_time + 1) / 1000;
	// 	  if (random() % 20 == 5) {
	// 	    fprintf(stderr, "T%u, Random show input read speed: %d KB/s\n", syscall(__NR_gettid),
	// 		    fport->in_read_speed);
	// 	  }
	// 	}
    //             break;
    //         }
    //     }
    // } else {
    //     fport->total = 0;
    //     // frame_size = frame->size = 8000;
    //     while (fport->total < frame_size) {
    //         rlen = ::read(fport->in_cli_fd, fport->rbuf, frame_size - fport->total);
    //         if (rlen > 0) {
    //             fport->in_read_total += rlen;
    //             fport->in_read_speed = fport->in_read_total / (time(NULL) - fport->in_read_start_time + 1) / 1000;
    //             if (random() % 52 == 5) {
	// 	  fprintf(stderr, "T%u, Random show input read speed: %d KB/s\n", syscall(__NR_gettid),
	// 		  fport->in_read_speed);
    //             }
    //         } else if (rlen < 0) {
    //             if (errno == EBADF) {
    //                 // qDebug()<<__FILE__<<__LINE__<<"may be in cli disconnected";
    //             }
    //             break;
    //         } else if (rlen == 0) {
    //             break;
    //         }
    //         memcpy((char*)frame->buf + fport->total, fport->rbuf, rlen);
    //         fport->total += rlen;
    //         // break; // 
    //         read_cycle_cnt ++;
    //     }
    //     rlen = fport->total;
    // }

    // socketpair mode
    if (1) {
        fport->total = 0;
    //     // frame_size = frame->size = 8000;
        while (fport->total < frame_size) {
#ifdef HAVE_SOCK_NONBLOCK
            rlen = ::read(fd, fport->rbuf, frame->size);
#else
        retry_select:
            fd_set rd_fd_set;
            FD_ZERO(&rd_fd_set);
            FD_SET(fd, &rd_fd_set);

            struct timeval timev;
            timev.tv_sec = 0;
            timev.tv_usec = 1000 * 10; // 10ms, 最多可阻塞等待10ms
        
            rlen = ::select(fd + 1, &rd_fd_set, 0, 0, &timev);
            if (rlen < 0) {
                // error
                if (errno == EINTR) {
                    goto retry_select;
                } else {
                    assert(1 == 2);
                }
            } else if (rlen == 0) {
                // timeout, no readable fd
            } else {
                rlen = ::read(fd, fport->rbuf, frame->size);
            }
#endif
            if (rlen > 0) {
                fport->in_read_total += rlen;
                fport->in_read_speed = fport->in_read_total / (time(NULL) - fport->in_read_start_time + 1) / 1000;
                if (random() % 52 == 5) {
                    fprintf(stderr, "T%u, Random show input read speed: %d KB/s\n", syscall(__NR_gettid),
                            fport->in_read_speed);
                }
            } else if (rlen < 0) {
                if (errno == EBADF) {
                    // qDebug()<<__FILE__<<__LINE__<<"may be in cli disconnected";
                }
                break;
            } else if (rlen == 0) {
                break;
            }
            memcpy((char*)frame->buf + fport->total, fport->rbuf, rlen);
            fport->total += rlen;
            // break; // 
            read_cycle_cnt ++;
        }
        rlen = fport->total;

        // share ringbuffer mode
    } else {
        while (true) {
            rlen = pcmif->output_levy_frame((char*)frame->buf, frame->size);
            if (rlen == 0) {
                if (read_cycle_cnt ++ > 3) {
                    break;
                }

                usleep(10);
                continue;
                // fprintf(stderr, "read frame size: zero, buff size = %d\n", fport->incoming_media_frame_buffer.size());
                // frame->type = PJMEDIA_FRAME_TYPE_NONE;
                // frame->size = 0;
                // return PJ_EEOF;
            } else if (rlen == frame->size) {
                break;
            } else {
                assert(1 == 2); // not possible
            }
        }
    }

    if (rlen < 0) {
        // how to do
        fprintf(stderr, "get frame read error: -1\n");

        frame->type = PJMEDIA_FRAME_TYPE_NONE;
        frame->size = 0;

        return PJ_SUCCESS;
    } else if (rlen == 0) {
        // qDebug()<<__FILE__<<__LINE__<<"may be in cli disconnected";
        frame->type = PJMEDIA_FRAME_TYPE_NONE;
        frame->size = 0;

        if (0) {
            rlen = fport->in_cli_fd;
            fport->in_cli_fd = -1;

            epoll_ctl(fport->eph, EPOLL_CTL_DEL, rlen, NULL);
            close(rlen);
            fport->base.get_frame = NULL;
        }
        return PJ_SUCCESS;
        return PJ_EEOF;
    } else if (rlen < frame_size) {
        // fprintf(stderr, "read frame size: invalid %d, want %d, total %d\n", rlen, frame_size, fport->incoming_media_frame_buffer.size());
        fprintf(stderr, "read frame size: invalid %d, want %d, total %d\n", rlen, frame_size, ringbuffer_read_space(fport->in_vocie_frame_rb));
        frame->type = PJMEDIA_FRAME_TYPE_NONE;
        frame->size = 0;
        return PJ_SUCCESS;
        return PJ_EEOF;
    } else {
        // fprintf(stderr, "read frame size: %d, left buf size: %d\n", rlen, fport->incoming_media_frame_buffer.size());
        if (random() % 30 == 3) {
            fprintf(stderr, "read frame size: %d , cycle: %d, avg bytes per read: %d\n",
                    rlen, read_cycle_cnt, rlen/read_cycle_cnt);
        }
    }

    if (fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_ULAW ||
        fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_ALAW) {
        unsigned i;
        pj_uint16_t *dst;
        pj_uint8_t *src;

        dst = (pj_uint16_t*)frame->buf + frame_size - 1;
        src = (pj_uint8_t*)frame->buf + frame_size - 1;

        if (fport->fmt_tag == PJMEDIA_WAVE_FMT_TAG_ULAW) {
            for (i = 0; i < frame_size; ++i) {
                *dst-- = (pj_uint16_t) pjmedia_ulaw2linear(*src--);
            }
        } else {
            for (i = 0; i < frame_size; ++i) {
                *dst-- = (pj_uint16_t) pjmedia_alaw2linear(*src--);
            }
        }
    }

    return PJ_SUCCESS;
}

/*
 * Close the port, modify file header with updated file length.
 */
static pj_status_t file_on_destroy(pjmedia_port *this_port)
{
    // qDebug()<<__FILE__<<__LINE__<<"here";

    enum { FILE_LEN_POS = 4, DATA_LEN_POS = 40 };
    sxs_port_t *fport = (sxs_port_t*)this_port;
    epoll_ctl(fport->eph, EPOLL_CTL_DEL, fport->out_serv_fd, NULL);

    epoll_ctl(fport->eph, EPOLL_CTL_DEL, fport->in_serv_fd, NULL);
    // some case it has
    // epoll_ctl(fport->eph, EPOLL_CTL_DEL, fport->in_cli_fd, NULL);

    int tfd;

    tfd = fport->out_serv_fd;
    fport->out_serv_fd = -1;
    ::close(tfd);
    tfd = fport->out_cli_fd;
    fport->out_cli_fd = -1;
    ::close(tfd);

    ::close(fport->in_serv_fd);
    ::close(fport->in_cli_fd);

    free(fport);

    /* Done. */
    return PJ_SUCCESS;
}


//////////////

///////////////

/*
 * Create a file recorder, and automatically connect this recorder to
 * the conference bridge.
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_create(pj_pool_t *pool,
                                              unsigned enc_type,
                                              void *enc_param,
                                              unsigned options,
                                              pjsua_recorder_id *p_id)
{
    enum Format
    {
        FMT_UNKNOWN,
        FMT_WAV,
        FMT_MP3,
    };
    unsigned slot, file_id;
    // char path[PJ_MAXPATH];
    // pj_str_t ext;
    int file_format;
    pjmedia_port *port;
    pj_status_t status;

    /* Filename must present */
    // PJ_ASSERT_RETURN(filename != NULL, PJ_EINVAL);

    /* Don't support encoding type at present */
    PJ_ASSERT_RETURN(enc_type == 0, PJ_EINVAL);

    if (pjsua_var.rec_cnt >= PJ_ARRAY_SIZE(pjsua_var.recorder))
        return PJ_ETOOMANY;

    file_format = FMT_WAV;

    PJSUA_LOCK();

    for (file_id=0; file_id<PJ_ARRAY_SIZE(pjsua_var.recorder); ++file_id) {
        if (pjsua_var.recorder[file_id].port == NULL)
            break;
    }

    if (file_id == PJ_ARRAY_SIZE(pjsua_var.recorder)) {
        /* This is unexpected */
        PJSUA_UNLOCK();
        pj_assert(0);
        return PJ_EBUG;
    }

    if (file_format == FMT_WAV) {
        status = pjmedia_sxs_port_server_create(
                                                pjsua_var.media_cfg.clock_rate, 
                                                pjsua_var.mconf_cfg.channel_count,
                                                pjsua_var.mconf_cfg.samples_per_frame,
                                                pjsua_var.mconf_cfg.bits_per_sample, 
                                                options, 0, &port);
        fprintf(stderr, "samples_per_frame=%d, bits_per_samples=%d\n",
                pjsua_var.mconf_cfg.samples_per_frame,
                pjsua_var.mconf_cfg.bits_per_sample);
    } else {
        PJ_UNUSED_ARG(enc_param);
        port = NULL;
        status = PJ_ENOTSUP;
    }

    if (status != PJ_SUCCESS) {
        PJSUA_UNLOCK();
        pjsua_perror(THIS_FILE, "Unable to open file for recording", status);
        return status;
    }

    pjmedia_sxs_port_outgoing_server_get_port(port, 0);

    // Const pj_str_t filename = pj_str(path);
    // assert(g_param->pool);
    status = pjmedia_conf_add_port(pjsua_var.mconf, pool, port, 0, &slot);

     if (status != PJ_SUCCESS) {
         pjmedia_port_destroy(port);
         PJSUA_UNLOCK();
         return status;
     }

    pjsua_var.recorder[file_id].port = port;
    pjsua_var.recorder[file_id].slot = slot;

    if (p_id) *p_id = file_id;

    ++pjsua_var.rec_cnt;

    PJSUA_UNLOCK();
    return PJ_SUCCESS;
}


/*
 * Get conference port associated with recorder.
 */
PJ_DEF(pjsua_conf_port_id) pjsua_sxs_switcher_get_conf_port(pjsua_recorder_id id)
{
    PJ_ASSERT_RETURN(id>=0 && id<(int)PJ_ARRAY_SIZE(pjsua_var.recorder), 
                     PJ_EINVAL);
    PJ_ASSERT_RETURN(pjsua_var.recorder[id].port != NULL, PJ_EINVAL);

    return pjsua_var.recorder[id].slot;
}

/*
 * Get the media port for the recorder.
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_get_port( pjsua_recorder_id id,
                                             pjmedia_port **p_port)
{
    PJ_ASSERT_RETURN(id>=0 && id<(int)PJ_ARRAY_SIZE(pjsua_var.recorder), 
                     PJ_EINVAL);
    PJ_ASSERT_RETURN(pjsua_var.recorder[id].port != NULL, PJ_EINVAL);
    PJ_ASSERT_RETURN(p_port != NULL, PJ_EINVAL);

    *p_port = pjsua_var.recorder[id].port;
    return PJ_SUCCESS;
}

/*
 * Destroy recorder (this will complete recording).
 */
PJ_DEF(pj_status_t) pjsua_sxs_switcher_destroy(pjsua_recorder_id id)
{
    PJ_ASSERT_RETURN(id>=0 && id<(int)PJ_ARRAY_SIZE(pjsua_var.recorder), 
                     PJ_EINVAL);
    PJ_ASSERT_RETURN(pjsua_var.recorder[id].port != NULL, PJ_EINVAL);

    PJSUA_LOCK();

    if (pjsua_var.recorder[id].port) {
        pjsua_conf_remove_port(pjsua_var.recorder[id].slot);
        pjmedia_port_destroy(pjsua_var.recorder[id].port);
        pjsua_var.recorder[id].port = NULL;
        pjsua_var.recorder[id].slot = 0xFFFF;
        pj_pool_release(pjsua_var.recorder[id].pool);
        pjsua_var.recorder[id].pool = NULL;
        pjsua_var.rec_cnt--;
    }

    PJSUA_UNLOCK();

    return PJ_SUCCESS;
}

