#include <QHostAddress>

#include "CGroupClient.h"
#include "CGroupCommunicator.h"
#include "utils.h"

void CGroupClient::linkSignals()
{
	connect(this, SIGNAL(disconnected()), this, SLOT(lostConnection() ) );
	connect(this, SIGNAL(connected()), this, SLOT(connectionEstablished() ) );
		
	connect(this, SIGNAL(error(QAbstractSocket::SocketError )), this, SLOT(errorHandler(QAbstractSocket::SocketError) ) );
	connect(this, SIGNAL(readyRead()), this, SLOT( dataIncoming() )   );
	
	buffer = "";
	currentMessageLen = 0;
}

CGroupClient::CGroupClient(QByteArray host, int remotePort, QObject *parent) :
	QTcpSocket(parent)
{
	print_debug(DEBUG_GROUP, "Connecting to remote host...");
	setConnectionState(Connecting);
	print_debug(DEBUG_GROUP, "NOW issuing the connect command...");
	connectToHost(host, remotePort);
	protocolState = AwaitingLogin;

	linkSignals();
}

CGroupClient::CGroupClient(QObject *parent) :
	QTcpSocket(parent)
{
	connectionState = Closed;
	
	linkSignals();
	protocolState = Idle;
}


void CGroupClient::setSocket(int socketDescriptor)
{
	if (setSocketDescriptor(socketDescriptor) == false) {
		// failure ... what to do?
		print_debug(DEBUG_GROUP, "Connection failed. Native socket not recognized by CGroupClient.");
	} 

	setConnectionState(Connected);
}

void CGroupClient::setProtocolState(int val)
{
	print_debug(DEBUG_GROUP, "Protocol state: %i", val);
	protocolState = val;
}

void CGroupClient::setConnectionState(int val) 
{
	print_debug(DEBUG_GROUP, "Connection state: %i", val);
	connectionState = val;
	getParent()->connectionStateChanged(this);
}
	

CGroupClient::~CGroupClient()
{
	printf("in CGroupClient destructor!\r\n");
}

void CGroupClient::lostConnection()
{
	setConnectionState(Closed);
}

void CGroupClient::connectionEstablished()
{
	setConnectionState(Connected);
}

void CGroupClient::errorHandler ( QAbstractSocket::SocketError socketError )
{
	CGroupCommunicator *comm = (CGroupCommunicator *)parent();
	comm->errorInConnection(this);
}

void CGroupClient::dataIncoming()
{
	QByteArray message;
	QByteArray rest;
	
//	print_debug(DEBUG_GROUP, "Incoming Data [conn %i, IP: %s]", socketDescriptor(),
//			(const char *) peerAddress().toString().toAscii() );

	QByteArray tmp = readAll();
	
	
	buffer += tmp;
	
//	print_debug(DEBUG_GROUP, "RAW data buffer: %s", (const char *) buffer);

	while ( currentMessageLen < buffer.size()) {
//		print_debug(DEBUG_GROUP, "in data-receiving cycle, buffer %s", (const char *) buffer);
		cutMessageFromBuffer();
	}
}

void CGroupClient::cutMessageFromBuffer()
{
	QByteArray rest;
	
	if (currentMessageLen == 0) {
		int index = buffer.indexOf(' ');
		
		QString len = buffer.left(index + 1);
		currentMessageLen = len.toInt();
//		print_debug(DEBUG_GROUP, "Incoming buffer length: %i, incoming message length %i",
//				buffer.size(), currentMessageLen);
		
		rest = buffer.right( buffer.size() - index - 1);
		buffer = rest;
		
		if (buffer.size() == currentMessageLen)
			cutMessageFromBuffer();

		printf("returning from cutMessageFromBuffer\r\n");
		return;
	}

//	print_debug(DEBUG_GROUP, "cutting off one message case");
	getParent()->incomingData(this, buffer.left(currentMessageLen));
	rest = buffer.right( buffer.size() - currentMessageLen);
	buffer = rest;
	currentMessageLen = 0;
}


void CGroupClient::sendData(QByteArray data)
{
	QByteArray buff;
	QString len;
	
	len = QString("%1 ").arg(data.size());
	
//	char len[10];
	
//	sprintf(len, "%i ", data.size());
	
	buff = len.toAscii();
	buff += data;
	
	write(buff);
}


