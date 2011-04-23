// xmessages.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-06-28 22:11:39 +0800
// Version: $Id: xmessages.cpp 142 2010-06-28 14:42:09Z drswinghead $
// 
/****************************************************************************

 Copyright (C) 2001-2003 Lubos Lunak        <l.lunak@kde.org>

 Modifications copyright (c) 2006 Skype Limited

 Modifications copyright (c) 2008 Jonathan Verner <jonathan.verner@matfyz.cz>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

****************************************************************************/
#include <QtCore/QString>
#include <QtGui/QWidget>
#include <QtCore/QMap>
#include <QtGui/QApplication>

#ifdef Q_WS_X11

#include <QtGui/QX11Info>

#include <X11/Xlib.h>
#include<X11/X.h>

#include "xmessages.h"


// for broadcasting
const long BROADCAST_MASK = PropertyChangeMask;
// CHECKME

static QList<XMessages *> xm_instances;


XMessages::XMessages( const char* accept_broadcast_P, QWidget* parent_P ): QWidget( parent_P ) {
        QByteArray tmp ( accept_broadcast_P );
	tmp = tmp + "_BEGIN";

	if( accept_broadcast_P != NULL )
	{
		/*( void ) qApp->desktop(); //trigger desktop widget creation to select root window events*/
	        ( void ) QApplication::desktop();
		accept_atom2 = XInternAtom( QX11Info::display(), accept_broadcast_P, false );
		accept_atom1 = XInternAtom( QX11Info::display(), tmp.data(), false );
	}
	else
	{
		accept_atom1 = accept_atom2 = None;
	}
	handle = new QWidget( this );
	xm_instances.append(this);
}

XMessages::~XMessages()
{
    xm_instances.removeAll(this);
}


bool XMessages::broadcastMessage( const char* msg_type_P, const QString& message_P, int screen_P ) {
        QByteArray tmp ( msg_type_P );
	tmp = tmp + "_BEGIN";

	Atom a2 = XInternAtom( QX11Info::display(), msg_type_P, false );
	Atom a1 = XInternAtom( QX11Info::display(), tmp.data(), false );
	Window root = screen_P == -1 ? QX11Info::appRootWindow() : QX11Info::appRootWindow( screen_P );
	return send_message_internal( root, message_P, BROADCAST_MASK, QX11Info::display(), a1, a2, handle->winId());
}

bool XMessages::sendMessage( WId w_P, const char* msg_type_P, const QString& message_P ) {
        QByteArray tmp ( msg_type_P );
	tmp = tmp + "_BEGIN";

	Atom a2 = XInternAtom( QX11Info::display(), msg_type_P, false );
	Atom a1 = XInternAtom( QX11Info::display(), tmp.data(), false );
	return send_message_internal( w_P, message_P, 0, QX11Info::display(), a1, a2, handle->winId());
}

// for intercepting X Server error codes from XSendEvent
static XErrorHandler old_handler = 0;
static int xerror = 0;

extern "C" int xmerrhandler(Display* dpy, XErrorEvent* err)
{
	Q_UNUSED(dpy);
	xerror = err->error_code;
	return 0; // ignore the error
}

static void trap_errors()
{
	xerror = 0;
	old_handler = XSetErrorHandler(xmerrhandler);
}

static int untrap_errors()
{
	XSetErrorHandler(old_handler);
	return (xerror != BadValue) && (xerror != BadWindow);
}

bool XMessages::send_message_internal( WId w_P, const QString& msg_P, long mask_P, Display* disp, Atom atom1_P, Atom atom2_P, Window handle_P )
{
	unsigned int pos = 0;
	QByteArray msg = msg_P.toUtf8();
	unsigned int len = msg.size(); // originally strlen ( QCString(msg_P) ), is it different ? --- J. Verner
	XEvent e;

	e.xclient.type = ClientMessage;
	e.xclient.message_type = atom1_P; // leading message
	e.xclient.display = disp;
	e.xclient.window = handle_P;
	e.xclient.format = 8;

	trap_errors();
	do
	{
		unsigned int i;
		for( i = 0; i < 20 && i + pos <= len; ++i )
			e.xclient.data.b[ i ] = msg[ i + pos ];

		XSendEvent( disp, w_P, False, mask_P, &e );

		e.xclient.message_type = atom2_P; // following messages
		pos += i;
	} while( pos <= len );

	XSync( disp, False );
	return untrap_errors();
}

/*static*/
void XMessages::processXMessages(XEvent *ev)
{
  for( int i=0; i<xm_instances.size(); ++i )
    if ( xm_instances[i]->x11Event(ev) ) break;
      /* */
	/*QListIterator<XMessages> it(xm_instances);*/
	/*while(it.current())*/
	/*{*/
		/*if(it.current()->x11Event(ev))*/
			/*break;*/
		/*++it;*/
	/*}*/
}

bool XMessages::x11Event( XEvent* ev_P )
{
	if( ev_P->type != ClientMessage || ev_P->xclient.format != 8 )
		return QWidget::x11Event( ev_P );

	if( ev_P->xclient.message_type != accept_atom1 && ev_P->xclient.message_type != accept_atom2 )
		return QWidget::x11Event( ev_P );

	char buf[ 21 ]; // can't be longer
	int i;
	for( i = 0; i < 20 && ev_P->xclient.data.b[ i ] != '\0'; ++i )
		buf[ i ] = ev_P->xclient.data.b[ i ];

	buf[ i ] = '\0';

	if( incoming_messages.contains( ev_P->xclient.window ))
	{
		if( ev_P->xclient.message_type == accept_atom1 && accept_atom1 != accept_atom2 )
			// two different messages on the same window at the same time shouldn't happen anyway
			incoming_messages[ ev_P->xclient.window ] = QString();

		incoming_messages[ ev_P->xclient.window ] += buf;
	}
	else
	{
		if( ev_P->xclient.message_type == accept_atom2 && accept_atom1 != accept_atom2 )
			return false; // middle of message, but we don't have the beginning

		incoming_messages[ ev_P->xclient.window ] = buf;
	}

	if( i < 20 ) // last message fragment
	{
		emit gotMessage( ev_P->xclient.window, incoming_messages[ ev_P->xclient.window ] );
		incoming_messages.remove( ev_P->xclient.window );
	}

	return false; // let other XMessages instances get the event too
}

// #include "xmessages.moc"

#endif /* Q_WS_X11 */
