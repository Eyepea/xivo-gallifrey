/*
 * Copyright (C) 2006,2007 Proformatique
 *
 * Written by Richard Braun <rbraun@proformatique.com>
 *            Guillaume Knispel <gknispel@proformatique.com>
 *
 * Simple Asterisk Channel Protocol, a channel implementing a very simple
 * protocol to learn Asterisk channel developement.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

/**
 * \mainpage chan_sacp
 * 
 * The SACP protocol is a simple trunking protocol to forward calls between
 * two Asterisk instances. It uses a single UDP stream of packets to transmit
 * signals between those instances. Each packet contains a command, the called
 * number and the size of the packet. The called number, as well as any other
 * data that a packet transports, is encapsulated in an information element
 * (IE). An information element is a chunk in the packet that contains an
 * identifier for this IE, the size of the IE, and the data itself. As the
 * protocol only handles signalling, packets are assumed to be small. A packet
 * is at most 512 bytes and an IE is at most 128 bytes.
 * 
 * The called number in packets of the same session is actually used as the
 * session identifier. It is assumed that this number is unique in the network
 * part shared by the two Asterisk instances.
 * 
 * Commands are :
 *  - SACP_CMD_CALL
 *    A new call is initiated. In addition to the called number, the list of
 *    supported codecs is transmitted. The local peer state is set to RING
 *    and the remote peer state is set to RINGING.
 * 
 *  - SACP_CMD_STATE
 *    Tell the remote peer to update its state. The new state is transmitted
 *    in this packet.
 * 
 *  - SACP_CMD_RTP
 *    Setup the RTP session. Both peers exchange their local RTP port.
 *    RTP redirection isn't supported in this version.
 * 
 *  - SACP_CMD_HANGUP
 *    Tell the remote peer to hangup.
 * 
 * States are :
 *  - SACP_STATE_RINGING
 *    The destination is ringing, report it to the caller.
 * 
 *  - SACP_STATE_UP
 *    The destination has answered, begin the data stream.
 * 
 * A single thread demuxes incoming packets, it's called the receiver thread.
 * It lookups the corresponding channel in a linked list, identifying it
 * using the called number.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <asterisk/io.h>
#include <asterisk/pbx.h>
#include <asterisk/rtp.h>
#include <asterisk/sched.h>
#include <asterisk/causes.h>
#include <asterisk/channel.h>
#include <asterisk/config.h>
#include <asterisk/devicestate.h>
#include <asterisk/linkedlists.h>
#include <asterisk/lock.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/options.h>
#include <asterisk/translate.h>

#ifndef UNUSED
#define UNUSED(x)	((void)(x))
#endif /* UNUSED */

#define SACP_NAME "SACP"
#define SACP_VERSION "0.1"
#define SACP_DESCRIPTION "Simple Asterisk Channel Protocol" SACP_VERSION
#define SACP_CONF_FILE "sacp.conf"
#define SACP_PREFIX VERBOSE_PREFIX_3 SACP_NAME ": "
#define SACP_DEFAULT_PORT 16497

/**
 * Helper macro, for debugging.
 */
#define FUNCTION_START ast_log(LOG_DEBUG, "%s() called\n", __FUNCTION__);

/**
 * Helper macro, for debugging.
 */
#define FUNCTION_END ast_log(LOG_DEBUG, "%s() returns\n", __FUNCTION__);

/**
 * Enable this macro to debug locking issues.
 */
#define DEBUG_LOCKING 0

/**
 * Enable this macro to debug I/O processing, i.e. calls to sacp_read() and
 * sacp_write().
 */
#define DEBUG_IO 0

/**
 * Maximum size of a packet. Member size in struct sacp_packet must be large
 * enough to store this size.
 */
#define SACP_PACKET_SIZE_MAX 512

/*
 * SACP commands.
 */
#define SACP_CMD_CALL	1	/**< create a call			*/
#define SACP_CMD_STATE	2	/**< update state of remote peer	*/
#define SACP_CMD_RTP	3	/**< setup the RTP session		*/
#define SACP_CMD_HANGUP	4	/**< hangup up a call			*/

/**
 * Maximum size of an information element. Member size in sacp_ie must be
 * large enough to store this size.
 */
#define SACP_IE_SIZE_MAX 128

/*
 * Information element codes.
 */
#define SACP_IE_DST		1	/**< contains destination	*/
#define SACP_IE_CODEC		2	/**< contains codecs		*/
#define SACP_IE_STATE		3	/**< contains new peer state	*/
#define SACP_IE_RTP_PORT	4	/**< contains local RTP port	*/
#define SACP_IE_HANGUP_CAUSE	5	/**< contains hangup cause	*/

/*
 * Maximum size of a destination string, including the null character.
 */
#define SACP_DST_SIZE_MAX 64

/**
 * Supported codecs. This protocol can actually support any codec, so this
 * list exists for tests.
 */
#define SACP_CAPABILITIES (AST_FORMAT_ULAW | AST_FORMAT_ALAW | AST_FORMAT_GSM)

/*
 * Codec flags.
 */
#define SACP_CODEC_ULAW	0x1	/**< peer supports µ-Law	*/
#define SACP_CODEC_ALAW	0x2	/**< peer supports A-Law	*/
#define SACP_CODEC_GSM	0x4	/**< peer supports GSM		*/

/*
 * Supported states.
 */
#define SACP_STATE_RINGING	1	/**< peer is ringing	*/
#define SACP_STATE_UP		2	/**< peer is ready	*/

/**
 * An information element.
 */
struct sacp_ie
{
  uint8_t ie_code;
  uint8_t size;
  uint8_t data[0];
} __attribute__ ((packed));

typedef struct sacp_ie * sacp_ie_t;
#define SACP_IE_NULL ((sacp_ie_t)0)

/**
 * A SACP packet.
 */
struct sacp_packet
{
  uint16_t cmd;
  uint16_t size;
  uint32_t session_id;
  struct sacp_ie ies[0];
} __attribute__ ((packed));

typedef struct sacp_packet * sacp_packet_t;
#define SACP_PACKET_NULL ((sacp_packet_t)0)

/**
 * Handler for a received packet.
 */
typedef void (*sacp_handler_t)(sacp_packet_t packet, const char *dst);
#define SACP_HANDLER_NULL ((sacp_handler_t)0)

/**
 * Association between a command and its handler.
 */
struct sacp_cmd_handler
{
  uint16_t cmd;
  sacp_handler_t handler;
};

typedef struct sacp_cmd_handler * sacp_cmd_handler_t;
#define SACP_CMD_HANDLER_NULL ((sacp_cmd_handler_t)0)

/**
 * Private data associated with a SACP session.
 */
struct sacp_pvt
{
  AST_LIST_ENTRY(sacp_pvt) link;	/**< link for the session list	*/
  char dst[SACP_DST_SIZE_MAX];		/**< identifier of the session	*/
  ast_mutex_t lock;			/**< channel private lock	*/
  struct ast_channel *channel;		/**< channel to which we belong	*/
  struct ast_rtp *rtp;			/**< RTP session		*/
  int devstate;				/**< Asterisk device state	*/
  int rtp_info_sent;			/**< RTP info sent to peer	*/
};

typedef struct sacp_pvt * sacp_pvt_t;
#define SACP_PVT_NULL ((sacp_pvt_t)0)

/*
 * Handlers for incoming packets.
 */
static void handle_call(sacp_packet_t packet, const char *dst);
static void handle_state(sacp_packet_t packet, const char *dst);
static void handle_rtp(sacp_packet_t packet, const char *dst);
static void handle_hangup(sacp_packet_t packet, const char *dst);

/*
 * Asterisk channel technology callbacks. Channel is locked by Asterisk
 * before calling these functions.
 */
static struct ast_channel * sacp_request(const char *type, int format,
                                         void *data, int *cause);
static int sacp_devicestate(void *data);
static int sacp_digit(struct ast_channel *channel, char digit);
static int sacp_call(struct ast_channel *channel, char *dest, int timeout);
static int sacp_hangup(struct ast_channel *channel);
static int sacp_answer(struct ast_channel *channel);
static struct ast_frame * sacp_read(struct ast_channel *channel);
static int sacp_write(struct ast_channel *channel, struct ast_frame *frame);
static int sacp_indicate(struct ast_channel *channel, int condition);

/**
 * SACP channel technology.
 */
static struct ast_channel_tech sacp_tech =
{
  .type = SACP_NAME,
  .description = SACP_DESCRIPTION,
  .capabilities = SACP_CAPABILITIES,
  .properties = AST_CHAN_TP_WANTSJITTER,
  .requester = sacp_request,
  .devicestate = sacp_devicestate,
  .send_digit = sacp_digit,
  /* .send_text = sacp_sendtext,	*/
  /* .send_image = sacp_sendimage,	*/
  .call = sacp_call,
  .hangup = sacp_hangup,
  .answer = sacp_answer,
  .read = sacp_read,
  .write = sacp_write,
  .indicate = sacp_indicate,
  .bridge = ast_rtp_bridge,
  /* .setoption = sacp_setoption,	*/
  /* .transfer = sacp_transfer,		*/
  /* .fixup = sacp_fixup,		*/
  /* .exception = sacp_exception,	*/
};

/**
 * List of handlers, one per command.
 */
static struct sacp_cmd_handler cmd_handlers[] =
{
  {SACP_CMD_CALL, handle_call},
  {SACP_CMD_STATE, handle_state},
  {SACP_CMD_RTP, handle_rtp},
  {SACP_CMD_HANGUP, handle_hangup},
  {0, SACP_HANDLER_NULL}
};

/**
 * Schedule context for RTP packets.
 */
static struct sched_context *sched;

/**
 * I/O context for RTP packets.
 */
static struct io_context *ioctx;

/**
 * Socket descriptor.
 */
static int sd;

/**
 * 1 if the channel technology has been registered, 0 otherwise.
 */
static int registered;

/**
 * Local peer.
 */
struct sockaddr_in local_sa;

/**
 * Remote peer.
 */
struct sockaddr_in remote_sa;

/**
 * List of SACP sessions.
 */
static AST_LIST_HEAD_STATIC(sessions, sacp_pvt);

/**
 * Number of sessions.
 */
static unsigned int session_count;

/*
 * Variables used to control the receiver thread.
 * 
 * Description of the startup process :
 *  1 The receiver thread is started.
 *  2 The main thread waits for receiver_started to become non-zero
 *  3 The receiver thread initializes its data. If an error occurs,
 *    it sets receiver_startup_error to an non-zero value, otherwise
 *    receiver_startup_error is set to 0.
 *  4 The receiver thread sends a signal to the main thread, which reads
 *    receiver_startup_error and prevent the module to load if an error
 *    occurred.
 * 
 * The receiver thread exits when the socket is shut down.
 */

/**
 * Receiver thread.
 */
static pthread_t receiver_thread;

/**
 * Becomes non-zero when the receiver thread has started or ended because of
 * an error.
 */
static int receiver_started;

/**
 * Status of the receiver thread startup: 0 if no error, non-zero otherwise.
 */
static int receiver_startup_error;

/**
 * Condition for receiver_started.
 */
static ast_cond_t receiver_started_cond;

/**
 * Mutex for receiver_started_cond.
 */
static ast_mutex_t receiver_started_lock;

/**
 * Send the given packet to the peer. Return 0 if successful, non-zero
 * otherwise.
 */
static int
packet_send(sacp_packet_t packet)
{
  uint16_t cmd, size;
  ssize_t sent;

  FUNCTION_START

  cmd = packet->cmd;
  size = packet->size;
  packet->cmd = htons(cmd);
  packet->size = htons(size);

  sent = send(sd, packet, size, 0);

  if (sent == -1)
    {
      ast_log(LOG_WARNING, "Unable to send packet: %s\n", strerror(errno));
      return -1;
    }

  ast_log(LOG_DEBUG, "Sent packet, cmd: %u, size: %u\n", cmd, size);

  FUNCTION_END

  return 0;
}

/**
 * Add an IE to a packet. Return a non-zero value if an error occurred.
 */
static int
packet_add_ie(sacp_packet_t packet, uint8_t ie_code, const void *data,
              uint8_t data_size)
{
  uint8_t ie_size;
  sacp_ie_t ie;

  FUNCTION_START

  ie_size = sizeof(struct sacp_ie) + data_size;

  /*
   * Make sure that data_size isn't 0 and that we didn't wrap.
   */
  if (ie_size <= sizeof(struct sacp_ie))
    {
      ast_log(LOG_WARNING, "Invalid data_size (%u)\n", data_size);
      return -1;
    }

  if ((packet->size + ie_size) > SACP_PACKET_SIZE_MAX)
    {
      ast_log(LOG_WARNING, "Overflow when trying to add IE (%d)\n", ie_code);
      return -1;
    }

  ie = (void *)packet + packet->size;
  ie->ie_code = ie_code;
  ie->size = ie_size;
  memcpy(ie->data, data, data_size);
  packet->size += ie_size;
  ast_log(LOG_DEBUG, "Added IE %u (size: %u, packet cmd: %u)\n", ie_code,
          ie_size, packet->cmd);

  FUNCTION_END

  return 0;
}

/**
 * Make sure the given IE is consistent, to prevent bad memory accesses.
 */
static inline int
ie_check_range(sacp_ie_t ie, sacp_ie_t packet_end)
{
  sacp_ie_t ie_hdr_end, ie_end;

  FUNCTION_START

  ie_hdr_end = (void *)ie + sizeof(struct sacp_ie);

  if (ie_hdr_end > packet_end)
    {
      ast_log(LOG_WARNING, "Received inconsistent IE (size: %u)\n",
              packet_end - ie);
      return -1;
    }

  ie_end = (void *)ie + ie->size;

  if (ie_end > packet_end)
    {
      ast_log(LOG_WARNING, "Received IE advertises a bogus size (%u)\n",
              ie->size);
      return -1;
    }

  FUNCTION_END

  return 0;
}

/**
 * Lookup an information element in a packet. *data_size is the size of
 * the buffer on entry, the size of the copied data on return.
 * 0 is returned if successful, a non-zero value otherwise.
 */
static int
packet_get_ie(sacp_packet_t packet, uint8_t ie_code, void *data,
              uint8_t *data_size)
{
  sacp_ie_t ie, packet_end;
  uint8_t min_size, size;
  int error;

  FUNCTION_START

  packet_end = (void *)packet + packet->size;
  ie = packet->ies;

  while (!(error = ie_check_range(ie, packet_end)) && (ie->ie_code != ie_code))
    ie = (void *)ie + ie->size;

  if (error)
    {
      ast_log(LOG_DEBUG, "Requested IE not found (%u)\n", ie_code);
      return -1;
    }

  size = ie->size - sizeof(struct sacp_ie);
  min_size = (*data_size < size) ? *data_size : size;
  memcpy(data, ie->data, min_size);
  *data_size = min_size;

  FUNCTION_END

  return 0;
}

/**
 * Allocate a SACP packet, creating the DST IE. Return SACP_PACKET_NULL if an
 * error occurred.
 */
static sacp_packet_t
packet_create(uint16_t cmd, const char *dst)
{
  sacp_packet_t packet;
  size_t size;
  int error;

  FUNCTION_START

  size = strlen(dst) + 1;

  if (size > SACP_DST_SIZE_MAX)
    {
      ast_log(LOG_WARNING, "Destination string is too large\n");
      return SACP_PACKET_NULL;
    }

  packet = malloc(SACP_PACKET_SIZE_MAX);

  if (packet == SACP_PACKET_NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate packet");
      return SACP_PACKET_NULL;
    }

  packet->cmd = cmd;
  packet->size = sizeof(struct sacp_packet);
  error = packet_add_ie(packet, SACP_IE_DST, dst, size);

  if (error)
    {
      free(packet);
      return SACP_PACKET_NULL;
    }

  ast_log(LOG_DEBUG, "Packet created, cmd = %u, dst = %s\n", cmd, dst);

  FUNCTION_END

  return packet;
}

/**
 * Release resources associated with a SACP packet previously allocated with
 * packet_create().
 */
static void
packet_destroy(sacp_packet_t packet)
{
  FUNCTION_START
  free(packet);
  FUNCTION_END
}

/**
 * Lock the given pvt.
 */
static inline void
session_lock(sacp_pvt_t pvt)
{
#if DEBUG_LOCKING
  FUNCTION_START
#endif
  ast_mutex_lock(&pvt->lock);
#if DEBUG_LOCKING
  FUNCTION_END
#endif
}

/**
 * Unlock the given pvt.
 */
static inline void
session_unlock(sacp_pvt_t pvt)
{
#if DEBUG_LOCKING
  FUNCTION_START
#endif
  ast_mutex_unlock(&pvt->lock);
#if DEBUG_LOCKING
  FUNCTION_END
#endif
}

/**
 * Search the private data of a session. Return SACP_PVT_NULL if there is no
 * session for this destination. If successful, the returned entry is locked.
 */
static sacp_pvt_t
session_lookup(const char *dst)
{
  sacp_pvt_t pvt;
  int found;

  FUNCTION_START

  found = 0;

  AST_LIST_LOCK(&sessions);

  AST_LIST_TRAVERSE(&sessions, pvt, link)
    {
      session_lock(pvt);

      if (strcmp(pvt->dst, dst) == 0)
        {
          found = 1;
          break;
        }

      session_unlock(pvt);
    }

  AST_LIST_UNLOCK(&sessions);

  if (!found)
    {
      ast_log(LOG_DEBUG, "Session for destination %s not found, maybe already "
              "destroyed\n", dst);
      return SACP_PVT_NULL;
    }

  FUNCTION_END

  return pvt;
}

/**
 * Create a channel. Return NULL if an error occurred. Called by handle_call()
 * and sacp_request() when a new call is either received/made.
 */
static struct ast_channel *
sacp_new(const char *dst, int format, int state)
{
  struct ast_channel *channel;
  char format_str[256];
  struct ast_rtp *rtp;
  sacp_pvt_t pvt;

  FUNCTION_START

  pvt = session_lookup(dst);

  if (pvt != SACP_PVT_NULL)
    {
      session_unlock(pvt);
      ast_log(LOG_WARNING, "Session for destination %s already exists (this "
              "can happen because this protocol is a bit crappy :-p\n", dst);
    }

  format &= SACP_CAPABILITIES;
  ast_getformatname_multiple(format_str, 256, format);
  ast_log(LOG_DEBUG, "Format: %s\n", format_str);

  if (format == 0)
    {
      ast_log(LOG_WARNING, "Invalid format\n");
      goto error_format;
    }

  /*
   * XXX Parameter is needalertpipe, investigate on that.
   */
  channel = ast_channel_alloc(1);

  if (channel == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate channel\n");
      goto error_channel;
    }

  pvt = malloc(sizeof(struct sacp_pvt));

  if (pvt == SACP_PVT_NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate private structure\n");
      goto error_pvt;
    }

  rtp = ast_rtp_new_with_bindaddr(sched, ioctx, 1, 0, local_sa.sin_addr);

  if (rtp == NULL)
    {
      ast_log(LOG_WARNING, "Unable to create RTP session\n");
      goto error_rtp;
    }

  snprintf(pvt->dst, sizeof(pvt->dst), "%s", dst);
  ast_mutex_init(&pvt->lock);
  pvt->channel = channel;
  pvt->rtp = rtp;
  pvt->devstate = (state == AST_STATE_DOWN)
                  ? AST_DEVICE_NOT_INUSE
                  : AST_DEVICE_INUSE;
  pvt->rtp_info_sent = 0;

  channel->tech = &sacp_tech;
  snprintf(channel->name, sizeof(channel->name), SACP_NAME "/%s", dst);
  channel->type = SACP_NAME;
  channel->fds[0] = ast_rtp_fd(rtp);
  channel->fds[1] = ast_rtcp_fd(rtp);
  channel->nativeformats = format;
  channel->readformat = ast_best_codec(format);
  channel->writeformat = channel->readformat;
  channel->tech_pvt = pvt;
  strcpy(channel->language, "fr");
  strcpy(channel->context, "from-sacp");
  strcpy(channel->exten, dst);

  /*
   * TODO: fetch real values.
   */
  channel->cid.cid_num = strdup("1337");

  if (channel->cid.cid_num == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate Caller-ID number\n");
      goto error_cid_num;
    }

  channel->cid.cid_dnid = strdup("7350");

  if (channel->cid.cid_dnid == NULL)
    {
      ast_log(LOG_WARNING, "Unable to allocate Caller-ID dialed number ID\n");
      goto error_cid_dnid;
    }

  channel->cid.cid_name = strdup("SACP peer");

  if (channel->cid.cid_name == NULL)
    {
      ast_log(LOG_WARNING, "Unable to duplicate source\n");
      goto error_cid_name;
    }

  ast_setstate(channel, state);

  if (state != AST_STATE_DOWN)
    {
      enum ast_pbx_result res;

      pvt->devstate = AST_DEVICE_INUSE;
      res = ast_pbx_start(channel);

      if (res != AST_PBX_SUCCESS)
        {
          ast_log(LOG_WARNING, "Unable to start PBX on %s\n", channel->name);
          goto error_pbx;
        }
    }

  AST_LIST_LOCK(&sessions);
  AST_LIST_INSERT_HEAD(&sessions, pvt, link);
  session_count++;
  AST_LIST_UNLOCK(&sessions);

  FUNCTION_END

  return channel;

error_pbx:
  free(channel->cid.cid_name);
error_cid_name:
  free(channel->cid.cid_dnid);
error_cid_dnid:
  free(channel->cid.cid_num);
error_cid_num:
  ast_rtp_destroy(rtp);
error_rtp:
  free(pvt);
error_pvt:
  ast_hangup(channel);
error_channel:
error_format:
  return NULL;
}

/**
 * Send a packet to indicate the new state of a peer. Return 0 if successful,
 * non-zero otherwise. pvt must be locked.
 */
static int
indicate_state(sacp_pvt_t pvt, uint8_t state)
{
  sacp_packet_t packet;
  int error;

  FUNCTION_START

  packet = packet_create(SACP_CMD_STATE, pvt->dst);

  if (packet == SACP_PACKET_NULL)
    return -1;

  error = packet_add_ie(packet, SACP_IE_STATE, &state, sizeof(state));

  if (error)
    {
      packet_destroy(packet);
      return -1;
    }

  ast_log(LOG_DEBUG, "Indicating state %u to remote peer\n", state);
  error = packet_send(packet);
  packet_destroy(packet);

  FUNCTION_END

  return error;
}

/*
 * Send information needed by the remote peer to setup the RTP session.
 * Return 0 if successful, non-zero otherwise. pvt must be locked.
 */
static int
setup_rtp(sacp_pvt_t pvt)
{
  struct sockaddr_in local_rtp_sa;
  sacp_packet_t packet;
  struct ast_rtp *rtp;
  uint16_t port;
  int error;

  FUNCTION_START

  if (pvt->rtp_info_sent)
    {
      ast_log(LOG_DEBUG, "RTP info already sent, ignoring request\n");
      return -1;
    }

  rtp = pvt->rtp;
  ast_rtp_get_us(rtp, &local_rtp_sa);
  packet = packet_create(SACP_CMD_RTP, pvt->dst);

  if (packet == SACP_PACKET_NULL)
    return -1;

  error = packet_add_ie(packet, SACP_IE_RTP_PORT, &local_rtp_sa.sin_port,
                        sizeof(local_rtp_sa.sin_port));

  if (error)
    {
      packet_destroy(packet);
      return -1;
    }

  port = ntohs(local_rtp_sa.sin_port);
  ast_log(LOG_DEBUG, "Sending local RTP port (%u) to remote peer\n", port);
  error = packet_send(packet);
  packet_destroy(packet);

  if (!error)
    pvt->rtp_info_sent = 1;

  FUNCTION_END

  return error;
}

/**
 * Process the CALL command.
 */
static void
handle_call(sacp_packet_t packet, const char *dst)
{
  struct ast_channel *channel;
  uint8_t codecs, codecs_len;
  int error, format;

  FUNCTION_START

  codecs_len = sizeof(codecs);
  error = packet_get_ie(packet, SACP_IE_CODEC, &codecs, &codecs_len);

  if (error)
    return;

  format = 0;

  if (codecs & SACP_CODEC_ULAW)
    format |= AST_FORMAT_ULAW;

  if (codecs & SACP_CODEC_ALAW)
    format |= AST_FORMAT_ALAW;

  if (codecs & SACP_CODEC_GSM)
    format |= AST_FORMAT_GSM;

  channel = sacp_new(dst, format, AST_STATE_RING);

  if (channel == NULL)
    return;

  FUNCTION_END
}

/**
 * Process the STATE command.
 */
static void
handle_state(sacp_packet_t packet, const char *dst)
{
  uint8_t state, state_len;
  sacp_pvt_t pvt;
  int error;

  FUNCTION_START

  state_len = sizeof(state);
  error = packet_get_ie(packet, SACP_IE_STATE, &state, &state_len);

  if (error)
    return;

  pvt = session_lookup(dst);

  if (pvt == SACP_PVT_NULL)
    return;

  ast_log(LOG_DEBUG, "Updating state to %u\n", state);

  switch (state)
    {
      case SACP_STATE_RINGING:
        ast_queue_control(pvt->channel, AST_CONTROL_RINGING);
        break;
      case SACP_STATE_UP:
        ast_setstate(pvt->channel, AST_STATE_UP);
        ast_queue_control(pvt->channel, AST_CONTROL_ANSWER);
        break;
      default:
        ast_log(LOG_WARNING, "Unknown state: %u\n", state);
        break;
    }

  session_unlock(pvt);

  FUNCTION_END
}

/**
 * Process the RTP command.
 */
static void
handle_rtp(sacp_packet_t packet, const char *dst)
{
  struct sockaddr_in remote_rtp_sa;
  struct ast_rtp *rtp;
  uint8_t port_len;
  sacp_pvt_t pvt;
  uint16_t port;
  int error;

  FUNCTION_START

  pvt = session_lookup(dst);

  if (pvt == SACP_PVT_NULL)
    return;

  port_len = sizeof(port);
  error = packet_get_ie(packet, SACP_IE_RTP_PORT, &port, &port_len);

  if (error)
    {
      session_unlock(pvt);
      return;
    }

  ast_log(LOG_DEBUG, "Remote peer sent us RTP port %u\n", ntohs(port));
  rtp = pvt->rtp;
  remote_rtp_sa.sin_family = AF_INET;
  remote_rtp_sa.sin_port = port;
  remote_rtp_sa.sin_addr = remote_sa.sin_addr;
  ast_rtp_set_peer(rtp, &remote_rtp_sa);

  if (!pvt->rtp_info_sent)
    setup_rtp(pvt);

  session_unlock(pvt);

  FUNCTION_END
}

/**
 * Process the HANGUP command.
 */
static void
handle_hangup(sacp_packet_t packet, const char *dst)
{
  struct ast_channel *channel;
  uint8_t cause, cause_len;
  sacp_pvt_t pvt;
  int error;

  FUNCTION_START

  pvt = session_lookup(dst);

  if (pvt == SACP_PVT_NULL)
    return;

  cause_len = sizeof(cause);
  error = packet_get_ie(packet, SACP_IE_HANGUP_CAUSE, &cause, &cause_len);

  if (error)
    {
      session_unlock(pvt);
      return;
    }

  if (cause == AST_CAUSE_NOTDEFINED)
    {
      ast_log(LOG_DEBUG, "Received cause AST_CAUSE_NOTDEFINED, "
              "changed to AST_CAUSE_NOANSWER to avoid hangup problems\n");
      cause = AST_CAUSE_NOANSWER;
    }

  ast_log(LOG_DEBUG, "Hangup cause: %u\n", cause);
  channel = pvt->channel;
  session_unlock(pvt);
  ast_softhangup(channel, cause);

  FUNCTION_END
}

/**
 * Return the handler for the given command, or SACP_HANDLER_NULL if the
 * command is unknown. There is no locking for cmd_handlers, as this is a
 * read-only data structure, only used by a single thread.
 */
static sacp_handler_t
handler_lookup(uint16_t cmd)
{
  sacp_cmd_handler_t cmd_handler;

  FUNCTION_START

  cmd_handler = cmd_handlers;

  while (cmd_handler->handler != SACP_HANDLER_NULL)
    {
      if (cmd_handler->cmd == cmd)
        {
          FUNCTION_END
          return cmd_handler->handler;
        }

      cmd_handler++;
    }

  ast_log(LOG_WARNING, "Unable to find handler, unknown command (%u)\n", cmd);

  return SACP_HANDLER_NULL;
}

/**
 * Function called by the receiver thread to tell the main thread that it has
 * finished startup. error is the status of the startup.
 */
static void
receiver_startup_end(int error)
{
  FUNCTION_START

  ast_mutex_lock(&receiver_started_lock);
  receiver_startup_error = error;
  receiver_started = 1;
  ast_cond_signal(&receiver_started_cond);
  ast_mutex_unlock(&receiver_started_lock);

  FUNCTION_END
}

/**
 * Body of the receiver thread.
 */
static void *
receiver(void *arg)
{
  char dst[SACP_DST_SIZE_MAX];
  sacp_handler_t handler;
  sacp_packet_t packet;
  uint8_t data_size;
  ssize_t received;
  int error;

  UNUSED(arg);

  FUNCTION_START

  packet = malloc(SACP_PACKET_SIZE_MAX);

  if (packet == SACP_PACKET_NULL)
    {
      ast_log(LOG_ERROR, "Unable to allocate receive buffer\n");
      receiver_startup_end(-1);
      return NULL;
    }

  receiver_startup_end(0);

  do
    {
      received = recv(sd, packet, SACP_PACKET_SIZE_MAX, 0);

      if (received == 0)
        {
          ast_log(LOG_DEBUG, "Socket closed, receiver thread exiting\n");
          break;
        }

      else if (received < 0)
        {
          ast_log(LOG_WARNING, "I/O error while receiving data: %s\n",
                  strerror(errno));
          continue;
        }

      else if ((size_t)received < sizeof(struct sacp_packet))
        {
          ast_log(LOG_WARNING, "Received inconsistent packet (size: %d), "
                  "ignoring\n", received);
          continue;
        }

      packet->cmd = ntohs(packet->cmd);
      packet->size = ntohs(packet->size);

      if (received != packet->size)
        {
          ast_log(LOG_WARNING, "Received packet advertises a bogus size "
                  "(packet size: %u, advertised size: %u), ignoring\n",
                  received, packet->size);
          continue;
        }

      data_size = sizeof(dst);
      error = packet_get_ie(packet, SACP_IE_DST, dst, &data_size);

      if (error)
        {
          ast_log(LOG_WARNING, "Unable to retreive destination IE\n");
          continue;
        }

      ast_log(LOG_DEBUG, "Packet received, cmd: %u, size: %u, dst = %s\n",
              packet->cmd, packet->size, dst);

      handler = handler_lookup(packet->cmd);

      if (handler != SACP_HANDLER_NULL)
        handler(packet, dst);
    }
  while (1);

  packet_destroy(packet);

  FUNCTION_END

  return NULL;
}

/**
 * This callback is used by Asterisk when a call originates from the PBX.
 */
static struct ast_channel *
sacp_request(const char *type, int format, void *data, int *cause)
{
  struct ast_channel *channel;

  UNUSED(type);
  UNUSED(cause);

  FUNCTION_START

  if (data == NULL)
    {
      ast_log(LOG_WARNING, "Invalid dial string\n");
      return NULL;
    }

  channel = sacp_new(data, format, AST_STATE_DOWN);

  if (channel == NULL)
    return NULL;

  /*
   * TODO: all this native or not format thing is weird, understand it.
   */
  if (channel->nativeformats & format)
    channel->nativeformats &= format;

  else
    {
      int error, tmpfmt, tmpnatfmt;

      tmpnatfmt = channel->nativeformats;
      tmpfmt = format;
      error = ast_translator_best_choice(&tmpfmt, &tmpnatfmt);

      if (error)
        {
          ast_log(LOG_WARNING, "Unable to create translator path for %s to %s"
                  " on %s\n", ast_getformatname(channel->nativeformats),
                  ast_getformatname(format), channel->name);
          ast_hangup(channel);
          return NULL;
        }

      channel->nativeformats = tmpnatfmt;
    }

  channel->readformat = ast_best_codec(channel->nativeformats);
  channel->writeformat = channel->readformat;

  FUNCTION_END

  return channel;
}

/**
 * This callback is used by Asterisk to request link state updates. Note that
 * this module has no user accounts, and thus no such link. If a session
 * exists, its state is returned, otherwise, AST_DEVICE_NOT_INUSE is returned.
 */
static int
sacp_devicestate(void *data)
{
  sacp_pvt_t pvt;
  int devstate;

  FUNCTION_START

  pvt = session_lookup(data);

  /*
   * This function can be called after session is destroyed.
   */
  if (pvt == SACP_PVT_NULL)
    return AST_DEVICE_NOT_INUSE;

  devstate = pvt->devstate;
  session_unlock(pvt);

  FUNCTION_END

  return devstate;
}

/**
 * Callback used by Asterisk when DTMF digits must be transfered.
 */
static int
sacp_digit(struct ast_channel *channel, char digit)
{
  sacp_pvt_t pvt;
  int error;

  FUNCTION_START

  pvt = channel->tech_pvt;
  session_lock(pvt);
  error = ast_rtp_senddigit(pvt->rtp, digit);
  session_unlock(pvt);

  FUNCTION_END

  return error;
}

/**
 * Callback used by Asterisk in order to create a new call.
 */
static int
sacp_call(struct ast_channel *channel, char *dest, int timeout)
{
  sacp_packet_t packet;
  int error, format;
  sacp_pvt_t pvt;
  uint8_t codecs;

  UNUSED(dest);
  UNUSED(timeout);

  FUNCTION_START

  pvt = channel->tech_pvt;
  session_lock(pvt);
  packet = packet_create(SACP_CMD_CALL, pvt->dst);

  if (packet == SACP_PACKET_NULL)
    {
      session_unlock(pvt);
      return -1;
    }

  format = channel->nativeformats;
  codecs = 0;

  if (format & AST_FORMAT_ULAW)
    codecs |= SACP_CODEC_ULAW;

  if (format & AST_FORMAT_ALAW)
    codecs |= SACP_CODEC_ALAW;

  if (format & AST_FORMAT_GSM)
    codecs |= SACP_CODEC_GSM;

  error = packet_add_ie(packet, SACP_IE_CODEC, &codecs, sizeof(codecs));

  if (error)
    {
      session_unlock(pvt);
      packet_destroy(packet);
      return -1;
    }

  error = packet_send(packet);
  packet_destroy(packet);

  if (error)
    {
      session_unlock(pvt);
      return -1;
    }

  /* Consider the remote end is ringing as soon as we send him the "call" msg */
  ast_setstate(channel, AST_STATE_RINGING);
  pvt->devstate = AST_DEVICE_INUSE;
  session_unlock(pvt);

  FUNCTION_END

  return error;
}

/**
 * Callback used by Asterisk when a channel must hang up.
 */
static int
sacp_hangup(struct ast_channel *channel)
{
  sacp_packet_t packet;
  sacp_pvt_t pvt;
  uint8_t cause;
  int error;

  FUNCTION_START

  pvt = channel->tech_pvt;

  AST_LIST_LOCK(&sessions);
  AST_LIST_REMOVE(&sessions, pvt, link);
  session_count--;
  AST_LIST_UNLOCK(&sessions);

  session_lock(pvt);
  packet = packet_create(SACP_CMD_HANGUP, pvt->dst);

  if (packet == SACP_PACKET_NULL)
    {
      session_unlock(pvt);
      return 0;
    }

  /*
   * Being lazy, send the raw Asterisk value.
   */
  cause = channel->hangupcause;
  error = packet_add_ie(packet, SACP_IE_HANGUP_CAUSE, &cause, sizeof(cause));

  if (error)
    {
      session_unlock(pvt);
      packet_destroy(packet);
      return 0;
    }

  session_unlock(pvt);

  ast_rtp_destroy(pvt->rtp);
  free(pvt);
  channel->tech_pvt = NULL;

  ast_log(LOG_DEBUG, "Hangup cause: %u\n", cause);
  error = packet_send(packet);
  packet_destroy(packet);

  FUNCTION_END

  return 0;
}

/**
 * Callback used by Asterisk when the call has been answered.
 */
static int
sacp_answer(struct ast_channel *channel)
{
  int error;

  FUNCTION_START

  if (channel->_state != AST_STATE_UP)
    {
      sacp_pvt_t pvt;

      pvt = channel->tech_pvt;
      ast_setstate(channel, AST_STATE_UP);
      session_lock(pvt);
      error = setup_rtp(pvt);

      if (!error)
        error = indicate_state(pvt, SACP_STATE_UP);

      session_unlock(pvt);
    }

  else
    error = 0;

  FUNCTION_END

  return error;
}

/**
 * Callback used by Asterisk to read data from the channel.
 */
static struct ast_frame *
sacp_read(struct ast_channel *channel)
{
  struct ast_frame *frame;
  sacp_pvt_t pvt;

#if DEBUG_IO
  FUNCTION_START
#endif

  pvt = channel->tech_pvt;
  session_lock(pvt);

  switch (channel->fdno)
    {
      case 0:
        frame = ast_rtp_read(pvt->rtp);
        break;
      case 1:
        frame = ast_rtcp_read(pvt->rtp);
        break;
      default:
        ast_log(LOG_WARNING, "Invalid file descriptor when reading (%d)\n",
                channel->fdno);
        frame = NULL;
	break;
    }

  session_unlock(pvt);

#if DEBUG_IO
  FUNCTION_END
#endif

  return frame;
}

/**
 * Callback used by Asterisk to write data to the channel.
 */
static int
sacp_write(struct ast_channel *channel, struct ast_frame *frame)
{
  struct ast_rtp *rtp;
  sacp_pvt_t pvt;
  int error;

#if DEBUG_IO
  FUNCTION_START
#endif

  pvt = channel->tech_pvt;
  session_lock(pvt);
  rtp = pvt->rtp;

  switch (frame->frametype)
    {
      case AST_FRAME_VOICE:
        if (!(frame->subclass & channel->nativeformats))
          {
            ast_log(LOG_WARNING, "Frame format is %d while channel supports %d "
                    "(read/write = %d/%d)\n", frame->subclass,
                    channel->nativeformats, channel->readformat,
                    channel->writeformat);
            error = 0;
          }

        else
          error = ast_rtp_write(rtp, frame);

        break;
      default:
        ast_log(LOG_DEBUG, "SACP only supports audio frames\n");
        error = 0;
	break;
    }

  session_unlock(pvt);

#if DEBUG_IO
  FUNCTION_END
#endif

  return error;
}

/**
 * Callback used by Asterisk to inform this channel that the state of the
 * call has changed.
 */
static int
sacp_indicate(struct ast_channel *channel, int condition)
{
  sacp_pvt_t pvt;
  int error;

  FUNCTION_START

  ast_log(LOG_DEBUG, "Condition: %d\n", condition);
  pvt = channel->tech_pvt;
  session_lock(pvt);

  switch (condition)
    {
      case AST_CONTROL_RINGING:
        if (channel->_state != AST_STATE_RING)
          {
            ast_log(LOG_WARNING, "Condition is AST_CONTROL_RINGING but state "
                    "isn't AST_STATE_RING\n");
            error = -1;
          }

        else
          error = indicate_state(pvt, SACP_STATE_RINGING);

        break;

      /*
       * TODO: find out why we get this one and why returning -1 in this case
       * doesn't make Asterisk unhappy.
       */
      case -1:
        error = -1;
        break;
      default:
        ast_log(LOG_WARNING, "Unknown condition %d\n", condition);
        error = -1;
	break;
    }

  session_unlock(pvt);

  FUNCTION_END

  return error;
}

/**
 * Called before anything else.
 * Must return something negative on failure.
 */
int
load_module(void)
{
  struct ast_config *config;
  struct ast_variable *var;
  struct in_addr peer_addr;
  char *peer;
  int error;

  FUNCTION_START

  /*
   * Set a few variables now in case our module is unloaded prematurely.
   */
  sched = NULL;
  ioctx = NULL;
  sd = -1;
  registered = 0;
  receiver_started = 0;

  sched = sched_context_create();

  if (sched == NULL)
    {
      ast_log(LOG_WARNING, "Unable to create schedule context\n");
      goto error_sched;
    }

  ioctx = io_context_create();

  if (ioctx == NULL)
    {
      ast_log(LOG_WARNING, "Unable to create I/O context\n");
      goto error_io;
    }

  peer = NULL;
  config = ast_config_load(SACP_CONF_FILE);

  if (config == NULL)
    {
      ast_log(LOG_ERROR, "Unable to load " SACP_CONF_FILE "\n");
      goto error_config;
    }

  for (var = ast_variable_browse(config, "general");
       var != NULL;
       var = var->next)
    {
      if (strcasecmp(var->name, "peer") == 0)
        {
          if (peer != NULL)
            ast_log(LOG_WARNING, "Parameter peer specified more than once\n");

          else
            {
              peer = strdup(var->value);

              if (peer == NULL)
                {
                  ast_log(LOG_ERROR, "Unable to allocate peer address\n");
                  goto error_parse;
                }
            }
        }

      else
        ast_log(LOG_WARNING, "Unknown parameter: %s\n", var->name);
    }

  ast_config_destroy(config);
  config = NULL;

  if (peer == NULL)
    {
      ast_log(LOG_ERROR, "Required parameter undefined: peer\n");
      goto error_peer;
    }

  error = !inet_aton(peer, &peer_addr);

  if (error)
    {
      ast_log(LOG_ERROR, "Invalid peer address: %s\n", peer);
      goto error_conversion;
    }

  sd = socket(PF_INET, SOCK_DGRAM, 0);

  if (sd == -1)
    {
      ast_log(LOG_ERROR, "Unable to create socket: %s\n", strerror(errno));
      goto error_socket;
    }

  memset(&local_sa, 0, sizeof(local_sa));
  local_sa.sin_family = AF_INET;
  local_sa.sin_port = htons(SACP_DEFAULT_PORT);
  local_sa.sin_addr.s_addr = INADDR_ANY;
  error = bind(sd, (struct sockaddr *)&local_sa, sizeof(local_sa));

  if (error == -1)
    {
      ast_log(LOG_ERROR, "Unable to bind socket: %s\n", strerror(errno));
      goto error_bind;
    }

  memset(&remote_sa, 0, sizeof(remote_sa));
  remote_sa.sin_family = AF_INET;
  remote_sa.sin_port = htons(SACP_DEFAULT_PORT);
  remote_sa.sin_addr.s_addr = peer_addr.s_addr;
  error = connect(sd, (struct sockaddr *)&remote_sa, sizeof(remote_sa));

  if (error)
    {
      ast_log(LOG_ERROR, "Unable to connect to remote socket: %s\n",
              strerror(errno));
      goto error_connect;
    }

  ast_verbose(SACP_PREFIX "UDP socket created, port: %u\n", SACP_DEFAULT_PORT);
  ast_verbose(SACP_PREFIX "Connected to remote peer: %s:%u\n", peer, SACP_DEFAULT_PORT);
  free(peer);
  peer = NULL;

  error = ast_channel_register(&sacp_tech);

  if (error)
    {
      ast_log(LOG_ERROR, "Unable to register " SACP_NAME " channel driver\n");
      goto error_register;
    }

  registered = 1;

  ast_mutex_init(&receiver_started_lock);
  ast_cond_init(&receiver_started_cond, NULL);
  ast_log(LOG_DEBUG, "Starting receiver thread...\n");
  error = pthread_create(&receiver_thread, NULL, receiver, NULL);

  if (error)
    {
      ast_log(LOG_ERROR, "Unable to start receiver thread: %s\n",
              strerror(error));
      goto error_receiver1;
    }

  ast_mutex_lock(&receiver_started_lock);

  while (!receiver_started)
    ast_cond_wait(&receiver_started_cond, &receiver_started_lock);

  ast_mutex_unlock(&receiver_started_lock);

  if (receiver_startup_error)
    {
      ast_log(LOG_DEBUG, "An error occurred in the receiver thread, "
              "closing socket and waiting for receiver thread to terminate\n");
      goto error_receiver2;
    }

  ast_log(LOG_DEBUG, "Receiver thread started\n");

  session_count = 0;

  FUNCTION_END

  return 0;

error_receiver2:
  if (receiver_started)
    pthread_join(receiver_thread, NULL);
error_receiver1:
  if (registered)
    ast_channel_unregister(&sacp_tech);
error_register:
error_connect:
error_bind:
  if (sd != -1)
    {
      shutdown(sd, SHUT_RDWR);
      close(sd);
    }
error_conversion:
error_socket:
error_parse:
  if (config != NULL)
    ast_config_destroy(config);

  if (peer != NULL)
    free(peer);
error_peer:
error_config:
  if (ioctx != NULL)
    io_context_destroy(ioctx);
  ioctx = NULL;
error_io:
  if (sched != NULL)
    sched_context_destroy(sched);
  sched = NULL;
error_sched:
  return -1;
}

/**
 * Called to unload the module from the Asterisk process.
 * WARNING: this function is called even if load_module()
 * returned with a failure.
 */
int
unload_module(void)
{
  FUNCTION_START

  if (registered)
    ast_channel_unregister(&sacp_tech);

  if (sd != -1)
    {
      ast_log(LOG_DEBUG, "Closing socket and waiting for receiver thread to "
              "terminate\n");
      shutdown(sd, SHUT_RDWR);
      close(sd);
    }

  if (receiver_started)
    pthread_join(receiver_thread, NULL);

  if (ioctx != NULL)
    io_context_destroy(ioctx);
  ioctx = NULL;
  if (sched != NULL)
    sched_context_destroy(sched);
  sched = NULL;

  FUNCTION_END

  return 0;
}

char *
description(void)
{
  FUNCTION_START
  FUNCTION_END
  return SACP_DESCRIPTION;
}

int
usecount(void)
{
  unsigned int count;

  FUNCTION_START

  AST_LIST_LOCK(&sessions);
  count = session_count;
  AST_LIST_UNLOCK(&sessions);

  FUNCTION_END

  return count;
}

char *
key(void)
{
  FUNCTION_START
  FUNCTION_END
  return ASTERISK_GPL_KEY;
}
