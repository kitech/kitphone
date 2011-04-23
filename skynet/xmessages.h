// xmessages.h --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2010-06-28 22:11:33 +0800
// Version: $Id: xmessages.h 142 2010-06-28 14:42:09Z drswinghead $
// 
/****************************************************************************

 Copyright (C) 2001-2003 Lubos Lunak        <l.lunak@kde.org>

 Modifications copyright (c) 2006 Skype Limited

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

#ifndef XMESSAGES_H
#define XMESSAGES_H


#include <QtCore/QString>
#include <QtGui/QWidget>
#include <QtCore/QMap>

#ifdef Q_WS_X11

#include<X11/X.h>


/**
 * Sending string messages to other applications using the X Client Messages.
 * @author Lubos Lunak <l.lunak@kde.org>
 */
class XMessages : public QWidget
{
    Q_OBJECT
    public:
		/**
		 * Creates an instance which will receive X messages.
		 *
		 * @param accept_broadcast if non-NULL, all broadcast messages with
		 *                         this message type will be received.
		 * @param parent the parent of this widget
		 */
        XMessages( const char* accept_broadcast, QWidget* parent );

        virtual ~XMessages();

		/**
		 * Sends the given message with the given message type only to given
		 * window.
		 *
		 * @param w X11 handle for the destination window
		 * @param msg_type the type of the message
		 * @param message the message itself
		 * @return false when an error occurred, true otherwise
		 */
        bool sendMessage( WId w, const char* msg_type, const QString& message );

		/**
		 * Broadcasts the given message with the given message type.
		 * @param msg_type the type of the message
		 * @param message the message itself
         * @param screen X11 screen to use, -1 for the default
		 * @return false when an error occurred, true otherwise
		 */
        bool broadcastMessage( const char* msg_type, const QString& message, int screen );

		QWidget *handler() { return handle; }

		/**
		 * This little static method is to go through all created XMessages instances and ask them
		 * if they want the x11 event received in Application.
		 * It is called from Application::x11EventFilter() _only_.
		 */
		static void processXMessages(XEvent *ev);

    signals:
		/**
		 * Emitted when a message was received.
		 * @param winId window id of message sender
		 * @param message the message that has been received
		 */
        void gotMessage( int winId, const QString& message );

    protected:
		/**
		 * @internal
		 */
        virtual bool x11Event( XEvent* ev );

    private:
        static bool send_message_internal( WId w_P, const QString& msg_P, long mask_P,
            Display* disp, Atom atom1_P, Atom atom2_P, Window handle_P );
        QWidget* handle;
        Atom accept_atom2;
        Atom accept_atom1;
        QMap< WId, QString > incoming_messages;
    };

#endif /* Q_WS_X11 */
#endif /* XMESSAGES_H */

