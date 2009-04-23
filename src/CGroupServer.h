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

#ifndef CGROUPSERVER_H_
#define CGROUPSERVER_H_

#include <QList>
#include <QTcpServer>

#include "CGroupClient.h"
class CGroupCommunicator;

class CGroupServer: public QTcpServer
{
	Q_OBJECT
	
	QList<CGroupClient *> connections;
public:
	CGroupServer(int localPort, QObject *parent);
	virtual ~CGroupServer();
	void addClient(CGroupClient *client);
	void sendToAll(QByteArray);
	void sendToAllExceptOne(CGroupClient *conn, QByteArray);
	void closeAll();
	
protected:
    void incomingConnection(int socketDescriptor);
    
public slots:
	void connectionClosed(CGroupClient *connection);

signals:
	void failedToStart();


};

#endif /*CGROUPSERVER_H_*/
