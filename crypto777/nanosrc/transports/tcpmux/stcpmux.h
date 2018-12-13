/*
    Copyright (c) 2013-2014 Martin Sustrik  All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef NN_STCPMUX_INCLUDED
#define NN_STCPMUX_INCLUDED

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../utils/streamhdr.h"

#include "../../utils/msg.h"

/*  This state machine handles TCPMUX connection from the point where it is
    established to the point when it is broken. */

#define NN_STCPMUX_ERROR 1
#define NN_STCPMUX_STOPPED 2

struct nn_stcpmux {

    /*  The state machine. */
    struct nn_fsm fsm;
    int state;

    /*  The underlying socket. */
    struct nn_usock *usock;

    /*  Child state machine to do protocol header exchange. */
    struct nn_streamhdr streamhdr;

    /*  The original owner of the underlying socket. */
    struct nn_fsm_owner usock_owner;

    /*  Pipe connecting this TCPMUX connection to the nanomsg core. */
    struct nn_pipebase pipebase;

    /*  State of inbound state machine. */
    int instate;

    /*  Buffer used to store the header of incoming message. */
    uint8_t inhdr [8];

    /*  Message being received at the moment. */
    struct nn_msg inmsg;

    /*  State of the outbound state machine. */
    int outstate;

    /*  Buffer used to store the header of outgoing message. */
    uint8_t outhdr [8];

    /*  Message being sent at the moment. */
    struct nn_msg outmsg;

    /*  Event raised when the state machine ends. */
    struct nn_fsm_event done;
};

void nn_stcpmux_init (struct nn_stcpmux *self, int src,
    struct nn_epbase *epbase, struct nn_fsm *owner);
void nn_stcpmux_term (struct nn_stcpmux *self);

int nn_stcpmux_isidle (struct nn_stcpmux *self);
void nn_stcpmux_start (struct nn_stcpmux *self, struct nn_usock *usock);
void nn_stcpmux_stop (struct nn_stcpmux *self);

#endif

