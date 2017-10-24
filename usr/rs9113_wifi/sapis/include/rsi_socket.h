/**
 * @file    rsi_socket.h
 * @version 0.1
 * @date    15 July,2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain declarations for BSD socket implementation
 *
 *  @section Description  This file contains definitions and declarations for BSD socket layer 
 *  implementation.
 *
 *
 */
#ifndef RSI__SOCKET_H
#define RSI__SOCKET_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifdef fd_set
#undef fd_set
#endif

#define RX_BUFFER_MEM_COPY

#define RSI_SOCKET_FEAT_SSL                 BIT(0)
#define RSI_SOCKET_FEAT_WEBS_SUPPORT        BIT(1)

#define RSI_SOCKET_FEAT_LTCP_ACCEPT         BIT(1)
#define RSI_SOCKET_FEAT_TCP_ACK_INDICATION  BIT(2)
#define RSI_SOCKET_FEAT_TCP_RX_WINDOW       BIT(4)
#define RSI_SOCKET_FEAT_SYNCHRONOUS         BIT(7)

#define RSI_TCP_LISTEN_MIN_BACKLOG          1


#define RSI_LTCP_PRIMARY_SOCKET            1
#define RSI_LTCP_SECONDARY_SOCKET          2

#define BSD_MAX_SOCKETS                    10
#define RSI_CLEAR_ALL_SOCKETS              0xEF
#define RSI_LTCP_PORT_BASED_CLOSE          0xEE
#ifdef   FD_SETSIZE
#undef   FD_SETSIZE                                                  /* May be different in other header files e.g 64 in GNU types.h file    */
#define  FD_SETSIZE                          BSD_MAX_SOCKETS      /* Number of sockets to select on - same is max sockets!                */
#else
#define  FD_SETSIZE                          BSD_MAX_SOCKETS      /* Number of sockets to select on - same is max sockets!                */
#endif


//! Max size of buffer to receive socket data
#define RSI_SOCKET_RECEIVE_BUFFER_SIZE 1500

/* Define some BSD protocol constants.  */

#define SOCK_STREAM                         1                       /* TCP Socket                                                          */
#define SOCK_DGRAM                          2                       /* UDP Socket                                                          */
#define SOCK_RAW                            3                       /* Raw socket                                                          */
#define IPPROTO_TCP                         6                       /* TCP Socket                                                          */
#define IPPROTO_UDP                         17                      /* TCP Socket                                                          */
#define IPPROTO_RAW                         255                     /* Raw Socket                                                          */

/* Address families.  */

#define     AF_UNSPEC                       0                       /* Unspecified.                                                         */
#define     AF_NS                           1                       /* Local to host (pipes, portals).                                      */
#define     AF_INET                         2                       /* IPv4 socket (UDP, TCP, etc)                                          */
#define     AF_INET6                        3                       /* IPv6 socket (UDP, TCP, etc)                                          */

/* Protocol families, same as address families.  */
#define     PF_INET                         AF_INET
#define     PF_INET6                        AF_INET6

#define     INADDR_ANY                      0
#define     BSD_LOCAL_IF_INADDR_ANY      0xFFFFFFFF
/* Define API error codes.  */


#ifndef htons
#define htons(a)                            a
#endif
#ifndef htonl
#define htonl(a)                            a
#endif
#ifndef ntohs
#define ntohs(a)                            a
#endif
#ifndef ntohl
#define ntohl(a)                            a
#endif

/* Define error handling macro.  */



/* Define file descriptor operation flags. */

/* Note: FIONREAD is hardware dependant. The default is for i386 processor. */
#ifndef FIONREAD
#define FIONREAD                            0x541B                  /* Read bytes available for the ioctl() command                         */
#endif

#define F_GETFL                             3                       /* Get file descriptors                                                 */
#define F_SETFL                             4                       /* Set a subset of file descriptors (e.g. O_NONBlOCK                    */
#define O_NONBLOCK                          0x4000                  /* Option to enable non blocking on a file (e.g. socket)                */

#ifndef FIONBIO
#define FIONBIO                             0x5421                  /* Enables socket non blocking option for the ioctl() command            */
#endif


/* Define additional BSD socket errors. */

/* From errno-base.h in /usr/include/asm-generic;  */
#define EPERM        1  /* Operation not permitted */
#define E_MIN        1  /* Minimum Socket/IO error */
#define ENOENT       2  /* No such file or directory */
#define ESRCH        3  /* No such process */
#define EINTR        4  /* Interrupted system call */
#define EIO          5  /* I/O error */
#define ENXIO        6  /* No such device or address */
#define E2BIG        7  /* Argument list too long */
#define ENOEXEC      8  /* Exec format error */
#define EBADF        9  /* Bad file number */
#define ECHILD      10  /* No child processes */
#define EAGAIN      11  /* Try again */
#define ENOMEM      12  /* Out of memory */
#define EACCES      13  /* Permission denied */
#define EFAULT      14  /* Bad address */
#define ENOTBLK     15  /* Block device required */
#define EBUSY       16  /* Device or resource busy */
#define EEXIST      17  /* File exists */
#define EXDEV       18  /* Cross-device link */
#define ENODEV      19  /* No such device */
#define ENOTDIR     20  /* Not a directory */
#define EISDIR      21  /* Is a directory */
#define EINVAL      22  /* Invalid argument */
#define ENFILE      23  /* File table overflow */
#define EMFILE      24  /* Too many open files */
#define ENOTTY      25  /* Not a typewriter */
#define ETXTBSY     26  /* Text file busy */
#define EFBIG       27  /* File too large */
#define ENOSPC      28  /* No space left on device */
#define ESPIPE      29  /* Illegal seek */
#define EROFS       30  /* Read-only file system */
#define EMLINK      31  /* Too many links */
#define EPIPE       32  /* Broken pipe */
#define EDOM        33  /* Math argument out of domain of func */
#define ERANGE      34  /* Math result not representable */

/* From errno.h in /usr/include/asm-generic;   */

#define EDEADLK         35  /* Resource deadlock would occur */
#define ENAMETOOLONG    36  /* File name too long */
#define ENOLCK          37  /* No record locks available */
#define ENOSYS          38  /* Function not implemented */
#define ENOTEMPTY       39  /* Directory not empty */
#define ELOOP           40  /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          42  /* No message of desired type */
#define EIDRM           43  /* Identifier removed */
#define ECHRNG          44  /* Channel number out of range */
#define EL2NSYNC        45  /* Level 2 not synchronized */
#define EL3HLT          46  /* Level 3 halted */
#define EL3RST          47  /* Level 3 reset */
#define ELNRNG          48  /* Link number out of range */
#define EUNATCH         49  /* Protocol driver not attached */
#define ENOCSI          50  /* No CSI structure available */
#define EL2HLT          51  /* Level 2 halted */
#define EBADE           52  /* Invalid exchange */
#define EBADR           53  /* Invalid request descriptor */
#define EXFULL          54  /* Exchange full */
#define ENOANO          55  /* No anode */
#define EBADRQC         56  /* Invalid request code */
#define EBADSLT         57  /* Invalid slot */
#define EDEADLOCK       EDEADLK
#define EBFONT          59  /* Bad font file format */
#define ENOSTR          60  /* Device not a stream */
#define ENODATA         61  /* No data available */
#define ETIME           62  /* Timer expired */
#define ENOSR           63  /* Out of streams resources */
#define ENONET          64  /* Machine is not on the network */
#define ENOPKG          65  /* Package not installed */
#define EREMOTE         66  /* Object is remote */
#define ENOLINK         67  /* Link has been severed */
#define EADV            68  /* Advertise error */
#define ESRMNT          69  /* Srmount error */
#define ECOMM           70  /* Communication error on send */
#define EPROTO          71  /* Protocol error */
#define EMULTIHOP       72  /* Multihop attempted */
#define EDOTDOT         73  /* RFS specific error */
#define EBADMSG         74  /* Not a data message */
#define EOVERFLOW       75  /* Value too large for defined data type */
#define ENOTUNIQ        76  /* Name not unique on network */
#define EBADFD          77  /* File descriptor in bad state */
#define EREMCHG         78  /* Remote address changed */
#define ELIBACC         79  /* Can not access a needed shared library */
#define ELIBBAD         80  /* Accessing a corrupted shared library */
#define ELIBSCN         81  /* .lib section in a.out corrupted */
#define ELIBMAX         82  /* Attempting to link in too many shared libraries */
#define ELIBEXEC        83  /* Cannot exec a shared library directly */
#define EILSEQ          84  /* Illegal byte sequence */
#define ERESTART        85  /* Interrupted system call should be restarted */
#define ESTRPIPE        86  /* Streams pipe error */
#define EUSERS          87  /* Too many users */
#define ENOTSOCK        88  /* Socket operation on non-socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE        90  /* Message too long */
#define EPROTOTYPE      91  /* Protocol wrong type for socket */
#define ENOPROTOOPT     92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define EOPNOTSUPP      95  /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported by protocol */
#define EADDRINUSE      98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Cannot assign requested address */
#define ENETDOWN        100 /* Network is down */
#define ENETUNREACH     101 /* Network is unreachable */
#define ENETRESET       102 /* Network dropped connection because of reset */
#define ECONNABORTED    103 /* Software caused connection abort */
#define ECONNRESET      104 /* Connection reset by peer */
#define ENOBUFS         105 /* No buffer space available */
#define EISCONN         106 /* Transport endpoint is already connected */
#define ENOTCONN        107 /* Transport endpoint is not connected */
#define ESHUTDOWN       108 /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */
#define ETIMEDOUT       110 /* Connection timed out */
#define ECONNREFUSED    111 /* Connection refused */
#define EHOSTDOWN       112 /* Host is down */
#define EHOSTUNREACH    113 /* No route to host */
#define EALREADY        114 /* Operation already in progress */
#define EINPROGRESS     115 /* Operation now in progress */
#define ESTALE          116 /* Stale NFS file handle */
#define EUCLEAN         117 /* Structure needs cleaning */
#define ENOTNAM         118 /* Not a XENIX named type file */
#define ENAVAIL         119 /* No XENIX semaphores available */
#define EISNAM          120 /* Is a named type file */
#define EREMOTEIO       121 /* Remote I/O error */
#define EDQUOT          122 /* Quota exceeded */
#define ENOMEDIUM       123 /* No medium found */
#define EMEDIUMTYPE     124 /* Wrong medium type */
#define ECANCELED       125 /* Operation Canceled */
#define ENOKEY          126 /* Required key not available */
#define EKEYEXPIRED     127 /* Key has expired */
#define EKEYREVOKED     128 /* Key has been revoked */
#define EKEYREJECTED    129 /* Key was rejected by service */
#define EOWNERDEAD      130 /* Owner died - for robust mutexes*/
#define ENOTRECOVERABLE 131 /* State not recoverable */
#define ERFKILL         132 /* Operation not possible due to RF-kill */


/* List of BSD sock options from socket.h in /usr/include/asm/socket.h and asm-generic/socket.h.
   The first set of socket options take the socket level (category) SOL_SOCKET. */

#define SOL_SOCKET      1   /* Define the socket option category. */
#define IPPROTO_IP      2   /* Define the IP option category.     */
#define SO_MIN          1   /* Minimum Socket option ID */
#define SO_DEBUG        1   /* Debugging information is being recorded.*/
#define SO_REUSEADDR    2   /* Enable reuse of local addresses in the time wait state */
#define SO_TYPE         3   /* Socket type */
#define SO_ERROR        4   /* Socket error status */
#define SO_DONTROUTE    5   /* Bypass normal routing */
#define SO_BROADCAST    6   /* Transmission of broadcast messages is supported.*/
#define SO_SNDBUF       7   /* Enable setting trasnmit buffer size */
#define SO_RCVBUF       8   /* Enable setting receive buffer size */
#define SO_KEEPALIVE    9   /* Connections are kept alive with periodic messages */
#define SO_OOBINLINE    10  /* Out-of-band data is transmitted in line */
#define SO_NO_CHECK     11  /* Disable UDP checksum */
#define SO_PRIORITY     12  /* Set the protocol-defined priority for all packets to be sent on this socket */
#define SO_LINGER       13  /* Socket lingers on close pending remaining send/receive packets. */
#define SO_BSDCOMPAT    14  /* Enable BSD bug-to-bug compatibility */
#define SO_REUSEPORT    15  /* Rebind a port already in use */

#ifndef SO_PASSCRED          /* Used for passing credentials. Not currently in use. */
#define SO_PASSCRED     16   /* Enable passing local user credentials  */
#define SO_PEERCRED     17   /* Obtain the process, user and group ids of the other end of the socket connection */
#define SO_RCVLOWAT     18   /* Enable receive "low water mark" */
#define SO_SNDLOWAT     19   /* Enable send "low water mark" */
#define SO_RCVTIMEO     20   /* Enable receive timeout */
#define SO_SNDTIMEO     21   /* Enable send timeout */
#endif  /* SO_PASSCRED */
#define SO_SNDBUFFORCE  22  /* Enable setting trasnmit buffer size overriding user limit (admin privelege) */
#define SO_RCVBUFFORCE  23  /* Enable setting trasnmit buffer size overriding user limit (admin privelege) */
#define SO_MAX          SO_RCVBUFFORCE /* Maximum Socket option ID      */

/*  This second set of socket options take the socket level (category) IPPROTO_IP. */

#define IP_MULTICAST_IF     27 /* Specify outgoing multicast interface */
#define IP_MULTICAST_TTL    28 /* Specify the TTL value to use for outgoing multicast packet. */
#define IP_MULTICAST_LOOP   29 /* Whether or not receive the outgoing multicast packet, loopbacloopbackk mode. */
#define IP_BLOCK_SOURCE     30 /* Block multicast from certain source. */
#define IP_UNBLOCK_SOURCE   31 /* Unblock multicast from certain source. */
#define IP_ADD_MEMBERSHIP   32 /* Join IPv4 multicast membership */
#define IP_DROP_MEMBERSHIP  33 /* Leave IPv4 multicast membership */
#define IP_HDRINCL          34 /* Raw socket IPv4 header included. */
#define IP_RAW_RX_NO_HEADER 35 /* proprietary socket option that does not include 
                                  IPv4/IPv6 header (and extension headers) on received raw sockets.*/
#define IP_RAW_IPV6_HDRINCL 36 /* Transmitted buffer over IPv6 socket contains IPv6 header. */
#define IP_OPTION_MAX       IP_RAW_IPV6_HDRINCL



/*
 * User-settable options (used with setsockopt).
 */
#define TCP_NODELAY 0x01    /* don't delay send to coalesce packets     */
#define TCP_MAXSEG  0x02    /* set maximum segment size                 */
#define TCP_NOPUSH  0x04    /* don't push last block of write           */
#define TCP_NOOPT   0x08    /* don't use TCP options                    */


#define RSI_SOCKET_TCP_CLIENT             0x0000
#define RSI_SOCKET_UDP_CLIENT             0x0001 
#define RSI_SOCKET_TCP_SERVER             0x0002 
#define RSI_SOCKET_LUDP                   0x0004 

#define RSI_NULL                   NULL
#define RSI_IPV4_ADDRESS_LENGTH    4
#define RSI_IPV6_ADDRESS_LENGTH    16

#define RSI_SOCK_ERROR             -1 
//! send Defines

//! UDP Frame header is 42 bytes, padding bytes are extra
#define RSI_UDP_FRAME_HEADER_LEN          44

//! TCP Frame header is 54 bytes, padding bytes are extra
#define RSI_TCP_FRAME_HEADER_LEN          56 

//!  Frame header is 42 bytes, padding bytes are extra
#define RSI_UDP_V6_FRAME_HEADER_LEN       64

//! TCP Frame header is 54 bytes, padding bytes are extra
#define RSI_TCP_V6_FRAME_HEADER_LEN       76 

/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                   Enumerations
 * ******************************************************/
typedef enum rsi_ip_version_e
{
  RSI_IP_VERSION_4=4,
  RSI_IP_VERSION_6=6
}rsi_ip_version_t;

typedef enum rsi_ip_config_mode_e
{
  RSI_STATIC = 0,
  RSI_DHCP,
  RSI_DHCP_HOSTNAME,
  RSI_DHCP_RESERVED,
  RSI_DHCP_OPTION81
}rsi_ip_config_mode_t;
typedef enum rsi_socket_state_e
{
  RSI_SOCKET_STATE_INIT = 0,
  RSI_SOCKET_STATE_CREATE,
  RSI_SOCKET_STATE_BIND,
  RSI_SOCKET_STATE_LISTEN,
  RSI_SOCKET_STATE_CONNECTED
}rsi_socket_state_t;

 
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
typedef uint32_t       rsi_time_t;

typedef uint32_t       rsi_suseconds_t; 

typedef uint32_t       socklen_t;

struct timeval
{
  rsi_time_t          tv_sec;             /* Seconds      */
  rsi_suseconds_t     tv_usec;            /* Microseconds */
};


struct sockaddr
{
  uint16_t          sa_family;              /* Address family (e.g. , AF_INET).                 */
  uint8_t           sa_data[14];            /* Protocol- specific address information.          */
};

struct in6_addr 
{
  union 
  {
    uint8_t _S6_u8[16];
    uint32_t _S6_u32[4];
  } _S6_un;
};

#define s6_addr     _S6_un._S6_u8
#define s6_addr32   _S6_un._S6_u32

struct sockaddr_in6 
{
  uint16_t          sin6_family;                 /* AF_INET6 */
  uint16_t          sin6_port;                   /* Transport layer port.  */
  uint32_t          sin6_flowinfo;               /* IPv6 flow information. */
  struct in6_addr   sin6_addr;                   /* IPv6 address. */
  uint32_t          sin6_scope_id;               /* set of interafces for a scope. */

};

/* Internet address (a structure for historical reasons).  */

struct in_addr
{
  uint32_t           s_addr;             /* Internet address (32 bits).                         */        
};

#define in_addr_t   uint32_t

/* Socket address, Internet style. */

struct sockaddr_in
{
  uint16_t              sin_family;         /* Internet Protocol (AF_INET).                    */
  uint16_t              sin_port;           /* Address port (16 bits).                         */
  struct in_addr        sin_addr;           /* Internet address (32 bits).                     */
  int8_t                sin_zero[8];        /* Not used.                                       */
};



typedef struct FD_SET_STRUCT                /* The select socket array manager.                                                             */
{ 
  int32_t                  fd_count;           /* How many are SET?                                                                            */
  uint32_t                fd_array[(FD_SETSIZE + 31)/32]; /* Bit map of SOCKET Descriptors.                                                   */
} fd_set;



typedef struct BSD_SOCKET_SUSPEND_STRUCT
{
  uint32_t            bsd_socket_suspend_actual_flags;
  fd_set              bsd_socket_suspend_read_fd_set;
  fd_set              bsd_socket_suspend_write_fd_set;
  fd_set              bsd_socket_suspend_exception_fd_set;

} BSD_SOCKET_SUSPEND;


struct ip_mreq 
{
  struct in_addr imr_multiaddr;     /* The IPv4 multicast address to join. */
  struct in_addr imr_interface;     /* The interface to use for this group. */
};


/* Define additional BSD data structures for supporting socket options.  */

struct sock_errno
{
  int32_t error;                              /* default = 0; */
};

struct linger
{
  int32_t l_onoff;                            /* 0 = disabled; 1 = enabled; default = 0;*/
  int32_t l_linger;                           /* linger time in seconds; default = 0;*/
};

struct sock_keepalive
{
  int32_t keepalive_enabled;                  /* 0 = disabled; 1 = enabled; default = 0;*/
};

struct sock_reuseaddr
{
  int32_t reuseaddr_enabled;                  /* 0 = disabled; 1 = enabled; default = 1; */
};

struct sock_winsize
{
  int32_t winsize;                            /* receive window size for TCP sockets   ; */
};

typedef union rsi_ip_addr_u
{
  uint8_t ipv4[4];
  uint8_t ipv6[16];
}rsi_ip_addr_t;

typedef struct rsi_socket_info_s
{
  //! socket ID
  uint8_t  sock_id;

  //! socket state
  uint8_t  sock_state;

  //! socket type
  uint8_t  sock_type;

  //! opcode used in websocket
  uint8_t  opcode;
  
  //! Local port number
  uint16_t source_port;

  //! Destination port number
  uint16_t destination_port;

  //! Destination IP address
  rsi_ip_addr_t destination_ip_addr;

  //! LTCP master socket
  uint8_t  ltcp_socket_type;

  //! Total backlogs
  uint8_t backlogs;

  //! Current backlog count
  uint8_t backlog_current_count;

  //! Socket bitmap
  uint8_t sock_bitmap;

#ifdef RX_BUFFER_MEM_COPY
  //! socket recieve buffer
  uint8_t  sock_recv_buffer[RSI_SOCKET_RECEIVE_BUFFER_SIZE];

  //! Socket receive buffer offset
  uint32_t sock_recv_buffer_offset;

  //! Socket receive available length
  uint32_t sock_recv_available_length;
#else
  //! Application receive buffer
  uint8_t *recv_buffer;

  //! Application receive length
  uint32_t recv_buffer_length;
#endif

  uint8_t max_available_buffer_count;

  uint8_t current_available_buffer_count;

  uint8_t rx_window_buffers;

  //! Socket recieve callback
  void (* sock_receive_callback)(uint32_t sock_no, const struct sockaddr *source_addr, uint8_t *buffer, uint32_t length);

  uint16_t read_time_out;
}rsi_socket_info_t;

extern rsi_socket_info_t *rsi_socket_pool;
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t  socket(int32_t protocolFamily, int32_t type, int32_t protocol);
int32_t  bind(int32_t sockID, struct sockaddr *localAddress, int32_t addressLength);
int32_t  connect(int32_t sockID, struct sockaddr *remoteAddress, int32_t addressLength);
int32_t  listen(int32_t sockID, int32_t backlog);
int32_t  accept(int32_t sockID, struct sockaddr *ClientAddress, int32_t *addressLength);
int32_t  recvfrom(int32_t sockID, int8_t *buffer, int32_t buffersize, int32_t flags,struct sockaddr *fromAddr, int32_t *fromAddrLen);
int32_t  recv(int32_t sockID, void *rcvBuffer, int32_t bufferLength, int32_t flags);
int32_t  sendto(int32_t sockID, int8_t *msg, int32_t msgLength, int32_t flags, struct sockaddr *destAddr, int32_t destAddrLen);
int32_t  send(int32_t sockID, const int8_t *msg, int32_t msgLength, int32_t flags);
int32_t  select(int32_t nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int32_t  shutdown(int32_t sockID,int32_t how);
int32_t  socket_async(int32_t protocolFamily, int32_t type, int32_t protocol, void (*)(uint32_t sock_no, const struct sockaddr* addr, uint8_t *buffer, uint32_t length));
int32_t  rsi_config_ipaddress(rsi_ip_version_t version, uint8_t mode,uint8_t *ip_addr,uint8_t *mask,uint8_t *gw,uint8_t *ipconfig_rsp,uint16_t length, uint8_t vap_id);
int32_t  rsi_get_primary_socket_id(uint16_t port_number);
int32_t  rsi_check_state(int32_t type);
int32_t  rsi_socket_create(int32_t sockID, int32_t type, int32_t backlog);
int32_t  rsi_get_application_socket_descriptor(int32_t sock_id);
void rsi_clear_sockets(uint8_t sockID);
uint8_t calculate_buffers_required(uint8_t type, uint16_t length);
uint16_t calculate_length_to_send(uint8_t type, uint8_t buffers);
int32_t send_async(int32_t sockID, const int8_t *msg, int32_t msgLength, int32_t flags,
                     void(*data_transfer_complete_handler)(uint8_t sockId, const uint16_t length));
int32_t sendto_async(int32_t sockID, int8_t *msg, int32_t msgLength, int32_t flags, struct sockaddr *destAddr, int32_t destAddrLen, 
                     void(*data_transfer_complete_handler)(uint8_t sockId, const uint16_t length));
int setsockopt(int socket, int level, int option_name,const void *option_value, socklen_t option_len);

#endif
