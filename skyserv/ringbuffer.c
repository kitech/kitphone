/*
 *   Jackbeat - JACK sequencer
 *
 *   Copyright (c) 2004-2008 Olivier Guilyardi <olivier {at} samalyse {dot} com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   SVN:$Id: ringbuffer.c 779 2011-03-20 10:00:10Z drswinghead $
 */

#include <stdlib.h>
#include <pthread.h>
#include "ringbuffer.h"
#include "pa_ringbuffer.h"

struct ringbuffer_t {
    PaUtilRingBuffer  pa_rb;
    void *            data;
    // pthread_mutex_t mutex;
};

ringbuffer_t *
ringbuffer_create(int size)
{
    ringbuffer_t *rb = malloc(sizeof(ringbuffer_t));
    rb->data = malloc(size);
    // pthread_mutex_init(&rb->mutex, NULL);
    PaUtil_InitializeRingBuffer(&rb->pa_rb, size, rb->data);
    return rb;
}

void
ringbuffer_free(ringbuffer_t *rb)
{
    // pthread_mutex_destroy(&rb->mutex);
    free(rb->data);
    free(rb);
}

int
ringbuffer_read_space(ringbuffer_t *rb)
{
    int ret = 0;

    // pthread_mutex_lock(&rb->mutex);
    ret = PaUtil_GetRingBufferReadAvailable(&rb->pa_rb);
    // pthread_mutex_unlock(&rb->mutex);

    return ret;
}

int
ringbuffer_write_space(ringbuffer_t *rb)
{
    int ret = 0;

    // pthread_mutex_lock(&rb->mutex);
    ret = PaUtil_GetRingBufferWriteAvailable(&rb->pa_rb);
    // pthread_mutex_unlock(&rb->mutex);

    return ret;
}

int
ringbuffer_read(ringbuffer_t *rb, char *dest, int cnt)
{
    int ret = 0;

    // pthread_mutex_lock(&rb->mutex);
    ret = PaUtil_ReadRingBuffer(&rb->pa_rb, (void *) dest, cnt);
    // pthread_mutex_unlock(&rb->mutex);

    return ret;
}

int
ringbuffer_write(ringbuffer_t *rb, const char *src, size_t cnt)
{
    int ret = 0;

    // pthread_mutex_lock(&rb->mutex);
    ret = PaUtil_WriteRingBuffer(&rb->pa_rb, (void *) src, cnt);
    // pthread_mutex_unlock(&rb->mutex);

    return ret;
}
