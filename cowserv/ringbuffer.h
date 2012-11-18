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
 *   SVN:$Id $
 */

#ifndef JACKBEAT_RINGBUFFER_H
#define JACKBEAT_RINGBUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct ringbuffer_t ringbuffer_t;

ringbuffer_t *ringbuffer_create(int size);
void          ringbuffer_free(ringbuffer_t *rb);
int           ringbuffer_read_space(ringbuffer_t *rb);
int           ringbuffer_write_space(ringbuffer_t *rb);
int           ringbuffer_read(ringbuffer_t *rb, char *dest, int cnt);
int           ringbuffer_write(ringbuffer_t *rb, const char *src, size_t cnt);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
