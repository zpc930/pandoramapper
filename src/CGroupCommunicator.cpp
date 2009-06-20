/*
 *  Pandora MUME mapper
 *
 *  Copyright (C) 2000-2009  Azazello
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QDataStream>
#include <QDomNode>
#include <QTextStream>

#include "CGroupCommunicator.h"


#include "defines.h"

#include "CConfigurator.h"
#include "utils.h"
#include "CGroup.h"
#include "CGroupServer.h"
#include "CGroupClient.h"


CGroupCommunicator::CGroupCommunicator(int _type, QObject *parent):
	QObject(parent)
{
	type = Off;
	peer = new CGroupDraftConnection;
	changeType(_type);
}

CGroupCommunicator::~CGroupCommunicator()
{

}


void CGroupCommunicator::connectionStateChanged(CGroupClient *connection)
{
//	Closed, Connecting, Connected, Logged, Quiting
	print_debug(DEBUG_GROUP, "CGroupMananger: connection state changed. type: %i", type);
	switch (connection->getConnectionState()) {
		case CGroupClient::Connecting :
			print_debug(DEBUG_GROUP, "Connecting to the remote host.");
			connecting(connection);
			break;
		case CGroupClient::Connected :
			print_debug(DEBUG_GROUP, "Connection established.");
			connectionEstablished(connection);
			break;
		case CGroupClient::Closed :
			print_debug(DEBUG_GROUP, "Connection closed.");
			connectionClosed(connection);
			break;
		case CGroupClient::Quiting :
			print_debug(DEBUG_GROUP, "Closing the socket. Quiting");
			//connectionClosed(connection);
			break;
		default:
			printf("Some state change...\r\n");
			break;
	}
}




void CGroupCommunicator::connecting(CGroupClient *connection)
{
	print_debug(DEBUG_GROUP, "CONNECTING. Type %i", type);
	if (type == Client) {

	} else if (type == Server) {

	}
}

void CGroupCommunicator::connectionEstablished(CGroupClient *connection)
{
	if (type == Client) {
		connection->setProtocolState(CGroupClient::AwaitingLogin);
	}
	if (type == Server) {
		connection->setProtocolState(CGroupClient::AwaitingLogin);
		sendMessage(connection, REQ_LOGIN, "test");
	}
}

void CGroupCommunicator::connectionClosed(CGroupClient *connection)
{
	if (type == Off) {
		printf("Error ... Wtf?\r\n");
	} else 	if (type == Client) {
		getGroup()->connectionError("Remote host closed the connection");
		changeType(Off);
	} else if (type == Server) {
		int sock = connection->socketDescriptor();

		QByteArray name = clientsList.key( sock );
		if (name != "") {
			sendRemoveUserNotification(connection, name);
			clientsList.remove(name);
			getGroup()->removeChar(name);
			getGroup()->connectionClosed(QString("Client %1 quited.").arg(QString(name)) );
		}
		CGroupServer *server = (CGroupServer *) peer;
		server->connectionClosed(connection);
	}


}

void CGroupCommunicator::errorInConnection(CGroupClient *connection)
{
	QString str;

	switch(connection->error()) {
		case QAbstractSocket::ConnectionRefusedError:
			str = QString("Tried to connect to %1 on port %2")
					.arg(connection->peerName())
					.arg(conf->getGroupManagerRemotePort()) ;
			getGroup()->connectionRefused( str.toAscii() );
			changeType(Off);
			break;
		case QAbstractSocket::RemoteHostClosedError:
			if (type == Server)
				connectionClosed(connection);
			break;
		case QAbstractSocket::HostNotFoundError:
			str = QString("Host %1 not found ").arg(connection->peerName());
			getGroup()->connectionRefused( str.toAscii() );
			changeType(Off);
			break;
		case QAbstractSocket::SocketAccessError:
			getGroup()->connectionError("Socket Access Error");
			break;
		case QAbstractSocket::SocketResourceError:
			getGroup()->connectionError("Socket Resource Error");
			break;
		case QAbstractSocket::SocketTimeoutError:
			getGroup()->connectionError("Socket Timeout Error");
			break;
		case QAbstractSocket::DatagramTooLargeError:
			getGroup()->connectionError("Diagram Too Large Error");
			break;
		case QAbstractSocket::NetworkError:
			getGroup()->connectionError("Network Error");
			break;
		case QAbstractSocket::AddressInUseError:
		case QAbstractSocket::SocketAddressNotAvailableError:
			getGroup()->connectionRefused("Network Error");
			break;
		case QAbstractSocket::UnsupportedSocketOperationError:
		case QAbstractSocket::ProxyAuthenticationRequiredError:
		case QAbstractSocket::UnknownSocketError:
		case QAbstractSocket::UnfinishedSocketOperationError:
			getGroup()->connectionError("Network Error ");
			break;
	}

}

void CGroupCommunicator::serverStartupFailed()
{
	CGroupServer *server = (CGroupServer *) peer;
	getGroup()->serverStartupFailed(server->errorString());
	changeType(Off);
}

//
// Communication protocol switches and logic
//

//
// Low level. Message forming and messaging
//
QByteArray CGroupCommunicator::formMessageBlock(int message, QDomNode data)
{
	QByteArray block;

	QDomDocument doc("datagram");
	QDomElement root = doc.createElement("datagram");
	root.setAttribute("message", message );
	doc.appendChild(root);


	QDomElement dataElem = doc.createElement("data");
	dataElem.appendChild( doc.importNode(data, true) );
	root.appendChild(dataElem);

	block = doc.toString().toAscii();
	print_debug(DEBUG_GROUP, "Message: %s", (const char *) block);

	return block;
}

void CGroupCommunicator::sendMessage(CGroupClient *connection, int message, QByteArray blob)
{

	QDomDocument doc("datagram");
	QDomElement root = doc.createElement("text");

	QDomText t = doc.createTextNode("dataText");
	t.setNodeValue(blob);
	root.appendChild(t);

	sendMessage(connection, message, root);
}

void CGroupCommunicator::sendMessage(CGroupClient *connection, int message, QDomNode node)
{
	connection->sendData( formMessageBlock(message, node) );
}

// the core of the protocol
void CGroupCommunicator::incomingData(CGroupClient *conn, QByteArray buff)
{
	QString data;

	data = buff;

	print_debug(DEBUG_GROUP, "Raw data received: %s", (const char *) data.toAscii());

	QString error;
	int errLine, errColumn;
	QDomDocument doc("datagram");
	if (!doc.setContent(data, &error, &errLine, &errColumn)) {
		print_debug(DEBUG_GROUP, "Failed to parse the datagram. Error in line %i, column %i: %s",
				errLine, errColumn, (const char *) error.toAscii());
		return;
	}

	QDomNode n = doc.documentElement();
	while(!n.isNull()) {
		QDomElement e = n.toElement();

	    print_debug(DEBUG_GROUP, "Cycle Element name: %s", (const char *) e.nodeName().toAscii());

	    if (e.nodeName() == "datagram") {
	    	int message;


	    	QDomElement dataElement = e.firstChildElement();


	    	message = e.attribute("message").toInt();


	    	// converting a given node to the text form.
//	    	QString blob;
//	    	QTextStream stream(&blob);
//	    	stream << dataElement;
//	    	print_debug(DEBUG_GROUP, "Datagram arrived. Message : %i, Blob: %s", message, (const char *) blob.toAscii());

	    	if (type == Client)
				retrieveDataClient(conn, message, dataElement);
			if (type == Server)
				retrieveDataServer(conn, message, dataElement);

	    }


	    n = n.nextSibling();
	}

}

//
// ******************** C L I E N T   S I D E ******************
//
// Client side of the communication protocol
void CGroupCommunicator::retrieveDataClient(CGroupClient *conn, int message, QDomNode data)
{
	switch (conn->getConnectionState()) {
		//Closed, Connecting, Connected, Quiting
		case CGroupClient::Connected:
			// AwaitingLogin, AwaitingInfo, Logged

			if (conn->getProtocolState() == CGroupClient::AwaitingLogin) {
				// Login state. either REQ_LOGIN or ACK should come
				if (message == REQ_LOGIN) {
					sendLoginInformation(conn);
				} else if (message == ACK) {
					// aha! logged on!
					sendMessage(conn, REQ_INFO);
					conn->setProtocolState(CGroupClient::AwaitingInfo);
				} else if (message == STATE_KICKED) {
					// woops
					getGroup()->gotKicked( data );
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingLogin) Unexpected message marker. Trying to ignore.");
				}

			} else if (conn->getProtocolState() == CGroupClient::AwaitingInfo) {
				// almost connected. awaiting full information about the connection
				if (message == UPDATE_CHAR) {
					printf("Adding char ....\r\n");
					getGroup()->addChar(data.firstChildElement());
					printf("Added char ....\r\n");
				} else if (message == STATE_LOGGED) {
					conn->setProtocolState(CGroupClient::Logged);
				} else if (message == REQ_ACK) {
					sendMessage(conn, ACK);
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingInfo) Unexpected message marker. Trying to ignore.");
				}

			} else if (conn->getProtocolState() == CGroupClient::Logged) {
				if (message == ADD_CHAR) {
					getGroup()->addChar(data.firstChildElement());
				} else if (message == REMOVE_CHAR) {
					getGroup()->removeChar(data.firstChildElement());
				} else if (message == UPDATE_CHAR) {
					getGroup()->updateChar(data.firstChildElement());
				} else if (message == RENAME_CHAR) {
					getGroup()->renameChar(data);
				} else if (message == GTELL) {
					getGroup()->gTellArrived(data);
				} else if (message == REQ_ACK) {
					sendMessage(conn, ACK);
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(Logged) Unexpected message marker. Trying to ignore.");
				}

			}

			break;
		case CGroupClient::Closed:
			print_debug(DEBUG_GROUP, "(Closed) Data arrival during wrong connection state.");
			break;
		case CGroupClient::Connecting:
			print_debug(DEBUG_GROUP, "(Connecting) Data arrival during wrong connection state.");
			break;
		case CGroupClient::Quiting:
			print_debug(DEBUG_GROUP, "(Quiting) Data arrival during wrong connection state.");
			break;
	}
}

//
// ******************** S E R V E R   S I D E ******************
//
// Server side of the communication protocol
void CGroupCommunicator::retrieveDataServer(CGroupClient *conn, int message, QDomNode data)
{

	switch (conn->getConnectionState()) {
		//Closed, Connecting, Connected, Quiting
		case CGroupClient::Connected:
			// AwaitingLogin, AwaitingInfo, Logged

			// ---------------------- AwaitingLogin  --------------------
			if (conn->getProtocolState() == CGroupClient::AwaitingLogin) {
				// Login state. either REQ_LOGIN or ACK should come
				if (message == UPDATE_CHAR) {
					// aha! parse the data
					conn->setProtocolState(CGroupClient::AwaitingInfo);
					parseLoginInformation(conn, data);
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingLogin) Unexpected message marker. Trying to ignore.");
				}
				// ---------------------- AwaitingInfo  --------------------
			} else if (conn->getProtocolState() == CGroupClient::AwaitingInfo) {
				// almost connected. awaiting full information about the connection
				if (message == REQ_INFO) {
					sendGroupInformation(conn);
					sendMessage(conn, REQ_ACK);
				} else if (message == ACK) {
					conn->setProtocolState(CGroupClient::Logged);
					sendMessage(conn, STATE_LOGGED);
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingInfo) Unexpected message marker. Trying to ignore.");
				}

				// ---------------------- LOGGED --------------------
			} else if (conn->getProtocolState() == CGroupClient::Logged) {
				// usual update situation. receive update, unpack, apply.
				// usual update situation. receive update, unpack, apply.
				if (message == UPDATE_CHAR) {
					getGroup()->updateChar(data.firstChildElement());
					relayMessage(conn, UPDATE_CHAR, data.firstChildElement());
				} else if (message == GTELL) {
					getGroup()->gTellArrived(data);
					relayMessage(conn, GTELL, data.firstChildElement());
				} else if (message == REQ_ACK) {
					sendMessage(conn, ACK);
				} else if (message == RENAME_CHAR) {
					getGroup()->renameChar(data);
					relayMessage(conn, RENAME_CHAR, data.firstChildElement());
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(Logged) Unexpected message marker. Trying to ignore.");
				}
			}

			break;

		case CGroupClient::Closed:


		case CGroupClient::Connecting:
		case CGroupClient::Quiting:
			print_debug(DEBUG_GROUP, "Data arrival during wrong connection state.");
			break;
	}
}

//
// user functions
//
void CGroupCommunicator::userLoggedOn(CGroupClient *conn)
{
	// TODO: call group-function.
	// TODO: message all other peers on server side
}

void CGroupCommunicator::userLoggedOff(CGroupClient *conn)
{
	// TODO: call group-function.
	// TODO: message all other peers on server side
}


//
// Parsers and Senders of information and signals to upper and lower objects
//
void CGroupCommunicator::sendLoginInformation(CGroupClient *conn)
{
	QDomDocument doc("datagraminfo");

	QDomElement root = doc.createElement("loginData");
	root.setAttribute("protocolVersion", protocolVersion );
	doc.appendChild(root);

	root.appendChild( doc.importNode(getGroup()->getLocalCharData(), true) );

	sendMessage(conn, UPDATE_CHAR, root);
}

void CGroupCommunicator::parseLoginInformation(CGroupClient *conn, QDomNode data)
{

	if (data.nodeName() != "data") {
    	print_debug(DEBUG_GROUP, "Called parseLoginInformation with wrong node. No data node.");
		return;
	}

	QDomElement node = data.firstChildElement().toElement();

	if (node.nodeName() != "loginData") {
    	print_debug(DEBUG_GROUP, "Called parseLoginInformation with wrong node. No loginData node.");
		return;
	}


   	int ver  = node.attribute("protocolVersion").toInt();
	print_debug(DEBUG_GROUP, "Client's protocol version : %i", ver);


	QDomNode charNode = node.firstChildElement();

	QByteArray kickMessage = "";
	if (ver < protocolVersion)
		kickMessage = "Server uses newer version of the protocol. Please update.";

	if (ver > protocolVersion)
		kickMessage = "Server uses older version of the protocol.";

	if (getGroup()->addChar(charNode) == false) {
		kickMessage = "The name you picked is already present!";
	} else {
		QByteArray name = getGroup()->getNameFromBlob(charNode);
		clientsList.insert( name, conn->socketDescriptor() );

		relayMessage(conn, ADD_CHAR, getGroup()->getCharByName(name)->toXML() );
	}

	if (kickMessage != "") {
		// kicked
		sendMessage(conn, STATE_KICKED, kickMessage);
		conn->close();	// got to make sure this causes the connection closed signal ...
	} else {
		sendMessage(conn, ACK);
	}

}



void CGroupCommunicator::sendGroupInformation(CGroupClient *conn)
{
	QByteArray info;

	getGroup()->sendAllCharsData(conn);
}

void CGroupCommunicator::parseGroupInformation(CGroupClient *conn, QDomNode data)
{
	// temporary
	/*
	print_debug(DEBUG_GROUP, "Group Information arrived %s", (const char *) data);
	getGroup()->addChar(data);
	*/
}


void CGroupCommunicator::sendCharUpdate(CGroupClient *conn, QDomNode blob)
{
	sendMessage(conn, UPDATE_CHAR, blob);
}


void CGroupCommunicator::sendRemoveUserNotification(CGroupClient *conn, QByteArray name)
{
   	if (type == Client)
   		return;

	if (type == Server) {
   		printf("[Server] Sending remove user notification!\r\n");
		QByteArray message = formMessageBlock(REMOVE_CHAR, getGroup()->getCharByName(name)->toXML());
		CGroupServer *serv = (CGroupServer *) peer;
		serv->sendToAllExceptOne(conn, message);
	}

}

// this function is for sending gtell from a local user
void CGroupCommunicator::sendGTell(QByteArray tell)
{
	// form the gtell QDomNode first.
	QDomDocument doc("datagram");
	QDomElement root = doc.createElement("gtell");
	root.setAttribute("from", QString(getGroup()->getName()) );

	QDomText t = doc.createTextNode("dataText");
	t.setNodeValue(tell);
	root.appendChild(t);


	// depending on the type of this communicator either send to
	// server or send to everyone
   	if (type == Client) {
   		printf("[Client] Sending gtell!\r\n");
   		sendMessage((CGroupClient *)peer, GTELL, root);
   	}
	if (type == Server) {
   		printf("[Server] Sending gtell!\r\n");
		QByteArray message = formMessageBlock(GTELL, root);
		CGroupServer *serv = (CGroupServer *) peer;
		serv->sendToAll(message);
	}
}


void CGroupCommunicator::relayMessage(CGroupClient *connection, int message, QDomNode data)
{
	QByteArray buffer = formMessageBlock(message, data);

	printf("Relaying message from %s", (const char *) clientsList.key(connection->socketDescriptor()) );
	CGroupServer *serv = (CGroupServer *) peer;
	serv->sendToAllExceptOne(connection, buffer);
}

void CGroupCommunicator::sendCharUpdate(QDomNode blob)
{
	if (type == Off)
		return;
   	if (type == Client)
   		sendMessage((CGroupClient *)peer, UPDATE_CHAR, blob);
	if (type == Server) {
		QByteArray message = formMessageBlock(UPDATE_CHAR, blob);
		CGroupServer *serv = (CGroupServer *) peer;
		serv->sendToAll(message);
	}
}

void CGroupCommunicator::sendUpdateName(QByteArray oldName, QByteArray newName)
{
	QDomDocument doc("datagram");
	QDomElement root = doc.createElement("rename");
	root.setAttribute("oldname", QString(oldName) );
	root.setAttribute("newname", QString(newName) );

	if (type == Off)
		return;
   	if (type == Client)
   		sendMessage((CGroupClient *)peer, RENAME_CHAR, root);
	if (type == Server) {
		QByteArray message = formMessageBlock(RENAME_CHAR, root);
		CGroupServer *serv = (CGroupServer *) peer;
		serv->sendToAll(message);
	}
}

void CGroupCommunicator::renameConnection(QByteArray oldName, QByteArray newName)
{
	int socket = clientsList.value(oldName);
	clientsList.remove(oldName);
	clientsList.insert(newName, socket);
}


bool CGroupCommunicator::isConnected()
{
   	if (type == Client) {
   		CGroupClient *client = (CGroupClient *) peer;
   		if (client->getConnectionState() == CGroupClient::Connected)
   			return true;
   	}
   	return false;
}

void CGroupCommunicator::reconnect()
{
	print_debug(DEBUG_GROUP, "in Connector::reconnect");

	if (type == Client) {
		peer->deleteLater();
	} else 	if (type == Server) {
	    ((CGroupServer*) peer)->closeAll();
	    delete peer;
	}
	print_debug(DEBUG_GROUP, "reconnect Done");

	getGroup()->resetChars();
	clientsList.clear();
	getGroup()->close();

	if (type == Client) {
	    peer = new CGroupClient(conf->getGroupManagerHost(), conf->getGroupManagerRemotePort(), this);
	} else if (type == Server) {
	    peer = new CGroupServer(conf->getGroupManagerLocalPort(), this);
	}
}

void CGroupCommunicator::changeType(int newState) {
	if (type == newState)
		return;

	if (type == Client)
		peer->deleteLater();
	if (type == Server)  {
		CGroupServer *serv = (CGroupServer *) peer;
		serv->closeAll();
		delete peer;
	}

	type = newState;

	getGroup()->resetChars();
	clientsList.clear();

	print_debug(DEBUG_GROUP, "Changing the Type of the GroupManager: new Type %i.", newState);
	switch (newState) {
		case Server:
			peer = new CGroupServer(conf->getGroupManagerLocalPort(), this);
			break;
		case Client:
			peer = new CGroupClient(conf->getGroupManagerHost(), conf->getGroupManagerRemotePort(), this);
			break;
		default:
			break;
	}
	emit typeChanged(type);
	if (type == Off)
		getGroup()->close();
}
