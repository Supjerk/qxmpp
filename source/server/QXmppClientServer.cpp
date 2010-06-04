/*
 * Copyright (C) 2010 Sjors Gielen, Manjeet Dahiya
 *
 * Authors:
 *	Manjeet Dahiya
 *  Sjors Gielen
 *
 * Source:
 *	http://code.google.com/p/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */


#include "QXmppClientServer.h"
#include "QXmppLogger.h"
#include "QXmppStream.h"
#include "QXmppMessage.h"
#include "QXmppUtils.h"

#define QXMPPCLIENTSERVER_DEBUG

#define ASSERT_CONNECT(a, b, c, d) \
      { bool check = connect(a, b, c, d); \
      Q_ASSERT( check ); }

/**
 * @brief Creates a QXmppClientServer object.
 *
 * This class is meant to service client connections as a server. This class
 * is only directly used by QXmppServer. If you want to listen to clients, use
 * that class, and not this one.
 *
 * @param socket Server socket (must be nonzero and connected)
 * @param data   Server data to start parsing
 * @param parent QObject parent of this object
 */
QXmppClientServer::QXmppClientServer(QSslSocket *socket,
    const QByteArray &data, QObject *parent)
: QObject(parent)
, m_logger(0)
, m_stream(0)
{
    Q_ASSERT( socket->state() == QAbstractSocket::ConnectedState );

    m_logger = QXmppLogger::getLogger();
    //m_stream = new QXmppStream(this);
    //m_stream->setSocket( socket );

    ASSERT_CONNECT(m_stream, SIGNAL(messageReceived(const QXmppMessage&)),
                       this, SIGNAL(messageReceived(const QXmppMessage&)));

    ASSERT_CONNECT(m_stream, SIGNAL(disconnected()),
                       this, SIGNAL(disconnected()));

    ASSERT_CONNECT(m_stream, SIGNAL(xmppConnected()),
                       this, SIGNAL(connected()));

    ASSERT_CONNECT(m_stream, SIGNAL(error(QXmppClientServer::Error)),
                       this, SIGNAL(error(QXmppClientServer::Error)));
}

/// Destructor, destroys the QXmppClientServer object.
///

QXmppClientServer::~QXmppClientServer()
{
}

/// After successfully connecting to the server use this function to send
/// stanzas to the server. This function can solely be used to send various kind
/// of stanzas to the server. QXmppPacket is a parent class of all the stanzas
/// QXmppMessage, QXmppPresence, QXmppIq, QXmppBind, QXmppRosterIq, QXmppSession
/// and QXmppVCard.
///
/// Following code snippet illustrates how to send a message using this function:
/// \code
/// QXmppMessage message(from, to, message);
/// client.sendPacket(message);
/// \endcode
///
/// \param packet A valid XMPP stanza. It can be an iq, a message or a presence stanza.
///

void QXmppClientServer::sendPacket(const QXmppPacket& packet)
{
    Q_ASSERT( m_stream != 0 );
    m_stream->sendPacket(packet);
}

/// Disconnects the client and the current presence of client changes to
/// QXmppPresence::Unavailable and statatus text changes to "Logged out".
///
/// \note Make sure that the clientPresence is changed to
/// QXmppPresence::Available, if you are again calling connectToServer() after
/// calling the disconnect() function.
///

void QXmppClientServer::disconnect()
{
    Q_ASSERT( m_stream != 0 );
    m_stream->disconnect();
}

/// Returns the socket error if QXmppClientServer::Error is QXmppClientServer::SocketError.
///
/// \return QAbstractSocket::SocketError
///

QAbstractSocket::SocketError QXmppClientServer::getSocketError()
{
    return m_stream->getSocketError();
}

/// Returns the XMPP stream error if QXmppClientServer::Error is QXmppClientServer::XmppStreamError.
///
/// \return QXmppClientServer::Error::Condition
///

QXmppStanza::Error::Condition QXmppClientServer::getXmppStreamError()
{
    return m_stream->getXmppStreamError();
}

/// Return the QXmppLogger associated with the client.

QXmppLogger *QXmppClientServer::logger()
{
    return m_logger;
}

void QXmppClientServer::setLogger(QXmppLogger *logger)
{
    m_logger = logger;
}
