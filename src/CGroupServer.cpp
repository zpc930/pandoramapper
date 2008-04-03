#include <QHostAddress>

#include "utils.h"
#include "CGroupServer.h"

CGroupServer::CGroupServer(int localPort, QObject *parent) : 
	QTcpServer(parent)
{
	printf("Creating the GroupManager Server.\r\n");
	if (listen(QHostAddress::Any, localPort) != true) {
		print_debug(DEBUG_GROUP, "Failed to start a group Manager server!");
		emit failedToStart();
	} else {
		print_debug(DEBUG_GROUP, "Listening on port %i!", localPort);
	}
}

CGroupServer::~CGroupServer()
{
	
}

void CGroupServer::incomingConnection(int socketDescriptor)
{
	print_debug(DEBUG_GROUP, "Incoming connection");
	// connect the client straight to the Communicator, as he handles all the state changes 
	// data transfers and similar.
	CGroupClient *client = new CGroupClient(socketDescriptor, parent());
	addClient(client);
}

void CGroupServer::addClient(CGroupClient *client)
{
	print_debug(DEBUG_GROUP, "Adding a client to the connections list.");
	connections.append(client);
}

void CGroupServer::connectionClosed(CGroupClient *connection)
{
	print_debug(DEBUG_GROUP, "Removing and deleting the connection completely.");
	connections.removeAll(connection);
	delete connection;
}