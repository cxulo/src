/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
/*
 * Copyright (c) 1999, 2000, 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* RCSID("$OpenBSD: channels.h,v 1.31.2.2 2001/09/27 19:03:54 jason Exp $"); */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "buffer.h"

/* Definitions for channel types. */
#define SSH_CHANNEL_X11_LISTENER	1	/* Listening for inet X11 conn. */
#define SSH_CHANNEL_PORT_LISTENER	2	/* Listening on a port. */
#define SSH_CHANNEL_OPENING		3	/* waiting for confirmation */
#define SSH_CHANNEL_OPEN		4	/* normal open two-way channel */
#define SSH_CHANNEL_CLOSED		5	/* waiting for close confirmation */
#define SSH_CHANNEL_AUTH_SOCKET		6	/* authentication socket */
#define SSH_CHANNEL_X11_OPEN		7	/* reading first X11 packet */
#define SSH_CHANNEL_INPUT_DRAINING	8	/* sending remaining data to conn */
#define SSH_CHANNEL_OUTPUT_DRAINING	9	/* sending remaining data to app */
#define SSH_CHANNEL_LARVAL		10	/* larval session */
#define SSH_CHANNEL_RPORT_LISTENER	11	/* Listening to a R-style port  */
#define SSH_CHANNEL_CONNECTING		12
#define SSH_CHANNEL_DYNAMIC		13
#define SSH_CHANNEL_ZOMBIE		14	/* Almost dead. */
#define SSH_CHANNEL_MAX_TYPE		15

#define SSH_CHANNEL_PATH_LEN		30

struct Channel;
typedef struct Channel Channel;

typedef void channel_callback_fn(int, void *);
typedef int channel_filter_fn(struct Channel *, char *, int);

struct Channel {
	int     type;		/* channel type/state */
	int     self;		/* my own channel identifier */
	int     remote_id;	/* channel identifier for remote peer */
	/* peer can be reached over encrypted connection, via packet-sent */
	int     istate;		/* input from channel (state of receive half) */
	int     ostate;		/* output to channel  (state of transmit half) */
	int     flags;		/* close sent/rcvd */
	int     rfd;		/* read fd */
	int     wfd;		/* write fd */
	int     efd;		/* extended fd */
	int     sock;		/* sock fd */
	int     isatty;		/* rfd is a tty */
	int     force_drain;		/* force close on iEOF */
	Buffer  input;		/* data read from socket, to be sent over
				 * encrypted connection */
	Buffer  output;		/* data received over encrypted connection for
				 * send on socket */
	Buffer  extended;
	char    path[SSH_CHANNEL_PATH_LEN];
		/* path for unix domain sockets, or host name for forwards */
	int     listening_port;	/* port being listened for forwards */
	int     host_port;	/* remote port to connect for forwards */
	char   *remote_name;	/* remote hostname */

	int	remote_window;
	int	remote_maxpacket;
	int	local_window;
	int	local_window_max;
	int	local_consumed;
	int	local_maxpacket;
	int     extended_usage;

	char   *ctype;		/* type */

	/* callback */
	channel_callback_fn	*cb_fn;
	void	*cb_arg;
	int	cb_event;
	channel_callback_fn	*detach_user;

	/* filter */
	channel_filter_fn	*input_filter;
};

#define CHAN_EXTENDED_IGNORE		0
#define CHAN_EXTENDED_READ		1
#define CHAN_EXTENDED_WRITE		2

/* default window/packet sizes for tcp/x11-fwd-channel */
#define CHAN_SES_WINDOW_DEFAULT	(32*1024)
#define CHAN_SES_PACKET_DEFAULT	(CHAN_SES_WINDOW_DEFAULT/2)
#define CHAN_TCP_WINDOW_DEFAULT	(32*1024)
#define CHAN_TCP_PACKET_DEFAULT	(CHAN_TCP_WINDOW_DEFAULT/2)
#define CHAN_X11_WINDOW_DEFAULT	(4*1024)
#define CHAN_X11_PACKET_DEFAULT	(CHAN_X11_WINDOW_DEFAULT/2)

/* possible input states */
#define CHAN_INPUT_OPEN			0x01
#define CHAN_INPUT_WAIT_DRAIN		0x02
#define CHAN_INPUT_WAIT_OCLOSE		0x04
#define CHAN_INPUT_CLOSED		0x08

/* possible output states */
#define CHAN_OUTPUT_OPEN		0x10
#define CHAN_OUTPUT_WAIT_DRAIN		0x20
#define CHAN_OUTPUT_WAIT_IEOF		0x40
#define CHAN_OUTPUT_CLOSED		0x80

#define CHAN_CLOSE_SENT			0x01
#define CHAN_CLOSE_RCVD			0x02

/* channel management */

Channel	*channel_lookup(int);
Channel *channel_new(char *, int, int, int, int, int, int, int, char *, int);
void	 channel_set_fds(int, int, int, int, int, int);
void	 channel_free(Channel *);
void	 channel_free_all(void);
void	 channel_detach_all(void);
void	 channel_stop_listening(void);

void	 channel_send_open(int);
void	 channel_request(int, char *, int);
void	 channel_request_start(int, char *, int);
void	 channel_register_callback(int, int mtype, channel_callback_fn *, void *);
void	 channel_register_cleanup(int, channel_callback_fn *);
void	 channel_register_filter(int, channel_filter_fn *);
void	 channel_cancel_cleanup(int);
int	 channel_close_fd(int *);

/* protocol handler */

void	 channel_input_channel_request(int, int, void *);
void	 channel_input_close(int, int, void *);
void	 channel_input_close_confirmation(int, int, void *);
void	 channel_input_data(int, int, void *);
void	 channel_input_extended_data(int, int, void *);
void	 channel_input_ieof(int, int, void *);
void	 channel_input_oclose(int, int, void *);
void	 channel_input_open_confirmation(int, int, void *);
void	 channel_input_open_failure(int, int, void *);
void	 channel_input_port_open(int, int, void *);
void	 channel_input_window_adjust(int, int, void *);

/* file descriptor handling (read/write) */

void	 channel_prepare_select(fd_set **, fd_set **, int *, int*, int);
void     channel_after_select(fd_set *, fd_set *);
void     channel_output_poll(void);

int      channel_not_very_much_buffered_data(void);
void     channel_close_all(void);
void     channel_free_all(void);
int      channel_still_open(void);
char	*channel_open_message(void);
int	 channel_find_open(void);

/* channel_tcpfwd.c */
void     channel_permit_all_opens(void);
void	 channel_add_permitted_opens(char *, int);
void	 channel_clear_permitted_opens(void);
void     channel_input_port_forward_request(int, int);
int	 channel_connect_to(const char *, u_short);
int	 channel_connect_by_listen_address(u_short);
void	 channel_request_remote_forwarding(u_short, const char *, u_short);
int	 channel_request_local_forwarding(u_short, const char *, u_short, int);
int
channel_request_forwarding(const char *, u_short, const char *, u_short, int,
    int);

/* x11 forwarding */

int	 x11_connect_display(void);
char	*x11_create_display(int);
char	*x11_create_display_inet(int, int);
void     x11_input_open(int, int, void *);
void     x11_request_forwarding(void);
void	 x11_request_forwarding_with_spoofing(int, const char *, const char *);
void	 deny_input_open(int, int, void *);

/* agent forwarding */

void	 auth_request_forwarding(void);
char	*auth_get_socket_name(void);
void	 auth_sock_cleanup_proc(void *);
int	 auth_input_request_forwarding(struct passwd *);
void	 auth_input_open_request(int, int, void *);

/* channel close */

int	 chan_is_dead(Channel *);
void	 chan_mark_dead(Channel *);
void 	 chan_init_iostates(Channel *);
void	 chan_init(void);

typedef void    chan_event_fn(Channel *);

/* for the input state */
extern chan_event_fn	*chan_rcvd_oclose;
extern chan_event_fn	*chan_read_failed;
extern chan_event_fn	*chan_ibuf_empty;

/* for the output state */
extern chan_event_fn	*chan_rcvd_ieof;
extern chan_event_fn	*chan_write_failed;
extern chan_event_fn	*chan_obuf_empty;

#endif
