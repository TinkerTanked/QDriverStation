/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "DS_Common.h"
#include "NetConsole.h"

/**
 * \file NetConsole.h
 * \class NetConsole
 *
 * The \c NetConsole allows the client to receive and send broadcasted messages
 * through the network. These messages are mostly robot logs or simple
 * client-to-robot commands for diagnostic purposes.
 */

/**
 * Initializes the NetConsole sockets
 */
NetConsole::NetConsole() {
    m_inputPort = 0;
    m_outputPort = 0;
    m_inputSocket = new ConfigurableSocket (DS::kSocketTypeUDP);
    m_outputSocket = new ConfigurableSocket (DS::kSocketTypeUDP);

    connect (m_inputSocket, SIGNAL (readyRead()),
             this,            SLOT (onReadyRead()));
}

/**
 * Disables the console and deletes sockets
 */
NetConsole::~NetConsole() {
    qDebug() << "Stopping NetConsole...";

    setInputPort (DISABLED_PORT);
    setOutputPort (DISABLED_PORT);

    delete m_inputSocket;
    delete m_outputSocket;

    qDebug() << "NetConsole terminated";
}

/**
 * Returns the port in which we receive broadcasted robot messages
 */
int NetConsole::inputPort() const {
    return m_inputPort;
}

/**
 * Returns the port in which we broadcast commands to the robot
 */
int NetConsole::outputPort() const {
    return m_outputPort;
}

/**
 * Changes the port in which we receive broadcasted robot messages.
 * If the \a port is set to \c 0, then the \c NetConsole will disable the
 * input socket.
 */
void NetConsole::setInputPort (int port) {
    qDebug() << "NetConsole input port set to" << port;

    m_inputPort = port;
    m_inputSocket->socket()->close();

    if (inputPort() > 0) {
        m_inputSocket->bind (QHostAddress::Any, inputPort(),
                             QUdpSocket::ShareAddress |
                             QUdpSocket::ReuseAddressHint);
    }
}

/**
 * Changes the port in which we broadcast commands to the robot.
 * If the \a port is set to \c 0, then the \c NetConsole will disable the
 * output socket.
 */
void NetConsole::setOutputPort (int port) {
    m_outputPort = port;
    qDebug() << "NetConsole output port set to" << port;
}

/**
 * Broadcasts the given \a message to the robot.
 * \note the output port must not be \c 0 in order for this to work
 */
void NetConsole::sendMessage (const QString& message) {
    if (!message.isEmpty() && outputPort() > 0) {
        m_outputSocket->writeDatagram (message.toUtf8(),
                                       QHostAddress::Any,
                                       outputPort());

        qDebug() << "NetConsole: sent" << message;
    }
}

/**
 * Called when the input socket finishes receiving a datagram
 */
void NetConsole::onReadyRead() {
    QString message = QString::fromUtf8 (m_inputSocket->readAll());
    emit newMessage (message);

    qDebug() << "NetConsole: received" << message;
}
