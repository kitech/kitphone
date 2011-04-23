/*
  ./configure --enable-epoll
*/

#define PJ_CONFIG_MAXIMUM_SPEED_WITH_STABLE 1

/*
 * Minimum size
 */
#ifdef PJ_CONFIG_MINIMAL_SIZE

#   undef PJ_OS_HAS_CHECK_STACK
#   define PJ_OS_HAS_CHECK_STACK        0
#   define PJ_LOG_MAX_LEVEL             0
#   define PJ_ENABLE_EXTRA_CHECK        0
#   define PJ_HAS_ERROR_STRING          0
#   undef PJ_IOQUEUE_MAX_HANDLES
/* Putting max handles to lower than 32 will make pj_fd_set_t size smaller
 * than native fdset_t and will trigger assertion on sock_select.c.
 */
#   define PJ_IOQUEUE_MAX_HANDLES       32
#   define PJ_CRC32_HAS_TABLES          0
#   define PJSIP_MAX_TSX_COUNT          15
#   define PJSIP_MAX_DIALOG_COUNT       15
#   define PJSIP_UDP_SO_SNDBUF_SIZE     4000
#   define PJSIP_UDP_SO_RCVBUF_SIZE     4000
#   define PJMEDIA_HAS_ALAW_ULAW_TABLE  0

#elif defined(PJ_CONFIG_MAXIMUM_SPEED)
#   define PJ_SCANNER_USE_BITWISE       0
#   undef PJ_OS_HAS_CHECK_STACK
#   define PJ_OS_HAS_CHECK_STACK        0
#   define PJ_LOG_MAX_LEVEL             3
#   define PJ_ENABLE_EXTRA_CHECK        0
#   define PJ_IOQUEUE_MAX_HANDLES       5000
#   define PJSIP_MAX_TSX_COUNT          ((640*1024)-1)
#   define PJSIP_MAX_DIALOG_COUNT       ((640*1024)-1)
#   define PJSIP_UDP_SO_SNDBUF_SIZE     (24*1024*1024)
#   define PJSIP_UDP_SO_RCVBUF_SIZE     (24*1024*1024)
#   define PJ_DEBUG                     0
#   define PJSIP_SAFE_MODULE            0
#   define PJ_HAS_STRICMP_ALNUM         0
#   define PJ_HASH_USE_OWN_TOLOWER      1
#   define PJSIP_UNESCAPE_IN_PLACE      1

#   ifdef PJ_WIN32
#     define PJSIP_MAX_NET_EVENTS       10
#   endif

#   define PJSUA_MAX_CALLS              512

#elif defined(PJ_CONFIG_MAXIMUM_SPEED_WITH_STABLE)
#   define PJ_SCANNER_USE_BITWISE       1
#   undef PJ_OS_HAS_CHECK_STACK
#   define PJ_OS_HAS_CHECK_STACK        0
#   define PJ_LOG_MAX_LEVEL             3
#   define PJ_ENABLE_EXTRA_CHECK        1
#   define PJ_IOQUEUE_MAX_HANDLES       5000
#   define PJSIP_MAX_TSX_COUNT          ((640*1024)-1)
#   define PJSIP_MAX_DIALOG_COUNT       ((640*1024)-1)
#   define PJSIP_UDP_SO_SNDBUF_SIZE     (24*1024*1024)
#   define PJSIP_UDP_SO_RCVBUF_SIZE     (24*1024*1024)
#   define PJ_DEBUG                     1
#   define PJSIP_SAFE_MODULE            1
#   define PJ_HAS_STRICMP_ALNUM         0
#   define PJ_HASH_USE_OWN_TOLOWER      1
#   define PJSIP_UNESCAPE_IN_PLACE      1

#   ifdef PJ_WIN32
#     define PJSIP_MAX_NET_EVENTS       10
#   endif

#   define PJSUA_MAX_CALLS              128

#elif defined(PJ_CONFIG_LIMITS_TEST)
#   define PJSUA_MAX_CALLS			4
#   define PJSUA_MAX_ACC			3

#endif
