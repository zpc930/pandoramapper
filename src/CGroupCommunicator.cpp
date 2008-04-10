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

void CGroupCommunicator::changeType(int newState) {
	if (type == newState)
		return;

	if (type == Client) {
		peer->deleteLater();
	} else 
		delete peer;

	type = newState;

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
		sendMessage(connection, REQ_LOGIN, "TESTING");
	}
}

void CGroupCommunicator::connectionClosed(CGroupClient *connection)
{
	if (type == Client) {
		changeType(Off);
	} else if (type == Server) {
		getGroup()->connectionClosed("");
		
		CGroupServer *server = (CGroupServer *) peer;
		server->connectionClosed(connection);
	}
	
}

void CGroupCommunicator::errorInConnection(CGroupClient *connection)
{
	QString str;

	switch(connection->error()) {
		case QAbstractSocket::ConnectionRefusedError:	
			getGroup()->connectionRefused(connection->peerName() + " port "+ 
					connection->peerPort() );
			break;
		case QAbstractSocket::RemoteHostClosedError:
			//connectionClosed(connection);
			getGroup()->connectionError("Remote host closed the connection");
			break;
		case QAbstractSocket::HostNotFoundError:
			str = "Host not found";
			str += connection->peerName();
			getGroup()->connectionRefused( str );
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
	
	
	
	root.appendChild( doc.importNode(data, true) );
	
	block = doc.toString().toAscii();
	print_debug(DEBUG_GROUP, "Message: %s", (const char *) block);

	return block;
}

void CGroupCommunicator::sendMessage(CGroupClient *connection, int message, QByteArray blob)
{
	
	QDomDocument doc("datagram");
	QDomElement root = doc.createElement("data");

	QDomText t = doc.createTextNode("dataText");
	t.setNodeValue(blob);
	root.appendChild(t);
	
	sendMessage(connection, message, root);
}

void CGroupCommunicator::sendMessage(CGroupClient *connection, int message, QDomNode node)
{
	connection->write( formMessageBlock(message, node) );
}

// the core of the protocol
void CGroupCommunicator::incomingData(CGroupClient *conn)
{
	QString data;
	
	data = conn->readAll();
		
	print_debug(DEBUG_GROUP, "Raw data received: %s", (const char *) data.toAscii());
	
	QDomDocument doc("datagram");
	if (!doc.setContent(data)) {
		print_debug(DEBUG_GROUP, "Failed to set the Contect for the XML Parser");
		return;
	}

	QDomNode n = doc.documentElement();
	while(!n.isNull()) {
		QDomElement e = n.toElement();
		
	    print_debug(DEBUG_GROUP, "Cycle Element name: %s", (const char *) e.nodeName().toAscii());
	    
	    if (e.nodeName() == "datagram") {
	    	int message;
	    	QString blob;
	    	QTextStream stream(&blob);
	    	
	    	
	    	QDomElement dataElement = e.firstChildElement();
	    	stream << dataElement;
	    	//message = e.attributeNode()
	    	
	    	message = e.attribute("message").toInt();
	    	
	    	print_debug(DEBUG_GROUP, "Datagram arrived. Message : %i, Blob: %s", message, (const char *) blob.toAscii());

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
					getGroup()->gotKicked( data.nodeValue() );
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingLogin) Unexpected message marker. Trying to ignore.");
				}
				
			} else if (conn->getProtocolState() == CGroupClient::AwaitingInfo) {
				// almost connected. awaiting full information about the connection
				if (message == UPDATE_CHAR) {
					parseGroupInformation(conn, data);
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
				// usual update situation. receive update, unpack, apply.
				if (message == ADD_CHAR) {

				} else if (message == REMOVE_CHAR) {
					
				} else if (message == UPDATE_CHAR) {
					
				} else if (message == GTELL) {
					
				} else if (message == REQ_ACK) {
					sendMessage(conn, ACK);
				} else {
					// ERROR: unexpected message marker!
					// try to ignore?
					print_debug(DEBUG_GROUP, "(AwaitingInfo) Unexpected message marker. Trying to ignore.");
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
	QByteArray block;

	QDomDocument doc("datagraminfo");

	QDomElement root = doc.createElement("protocol");
	root.setAttribute("version", protocolVersion );
	doc.appendChild(root);
	
	QDomElement dataElem = doc.createElement("data");
	doc.appendChild(dataElem);
	
	QDomText t = doc.createTextNode("dataText");
	t.setNodeValue(getGroup()->getLocalCharData());
	dataElem.appendChild(t);

//	block = doc.toString().toAscii();
//	print_debug(DEBUG_GROUP, "Message: %s", (const char *) block);

	sendMessage(conn, UPDATE_CHAR, root);
}

void CGroupCommunicator::parseLoginInformation(CGroupClient *conn, QDomNode data)
{
	/*
	print_debug(DEBUG_GROUP, "Login Information arrived %s", (const char *) data);

	QByteArray sVer;
	
	int chars = data.size() - data.lastIndexOf(' ') - 1;
	sVer = data.right( chars );
	data.chop( chars + 1 );
	int ver = sVer.toInt();
	
	print_debug(DEBUG_GROUP, "Client's protocol version : %s", (const char *) sVer);
	

	QByteArray kickMessage = "";
	if (ver < protocolVersion) 
		kickMessage = "Server uses newer version of the protocol. Please update.";

	if (ver > protocolVersion) 
		kickMessage = "Server uses older version of the protocol.";

	if (getGroup()->addChar(data) == false)
		kickMessage = "The name you picked is already present!";
	

	if (kickMessage != "") {
		// kicked
		sendMessage(conn, STATE_KICKED, kickMessage);
		conn->close();	// got to make sure this causes the connection closed signal ...
	} else {
		sendMessage(conn, ACK); 
	}
	
	*/
}

void CGroupCommunicator::sendGroupInformation(CGroupClient *conn)
{
	QByteArray info;

	getGroup()->sendAllCharsData(conn);
}

void CGroupCommunicator::sendCharUpdate(CGroupClient *conn, QDomNode blob)
{
	sendMessage(conn, UPDATE_CHAR, blob);
}


void CGroupCommunicator::parseGroupInformation(CGroupClient *conn, QDomNode data)
{
	// temporary
	/*
	print_debug(DEBUG_GROUP, "Group Information arrived %s", (const char *) data);
	getGroup()->addChar(data);
	*/
}


