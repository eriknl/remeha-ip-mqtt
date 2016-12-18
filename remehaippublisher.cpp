/*
This file is part of remeha-ip-mqtt Copyright (C) 2016 Erik de Jong
remeha-ip-mqtt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
remeha-ip-mqtt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with remeha-ip-mqtt.  If not, see <http://www.gnu.org/licenses/>.
The Remeha name and the Remeha product and services names are trade marks of Remeha B.V.
The author of this software has no affiliation whatsoever with Remeha B.V.
*/

#include <arpa/inet.h>
#include <unistd.h>
#include <QBitArray>
#include <QThread>

#include "remehaippublisher.h"

#define REMEHA_SHORT(address) (ntohs(*(short*)(address)))

RemehaIpPublisher::RemehaIpPublisher(QString root, QString remehaHost, unsigned short remehaPort, unsigned int retries, unsigned int interval) : DataPublisher(root), remehaHost(remehaHost), remehaPort(remehaPort), retries(retries), interval(interval)
{
	socket = new QTcpSocket();
	publishTimer = new QTimer();
	connectToHost();
	publishTimer->setInterval(interval);
	connect(publishTimer, SIGNAL(timeout()), this, SLOT(publishTimerTimeout()));
	publishTimerTimeout();
	publishTimer->setSingleShot(false);
	publishTimer->start();
}

void RemehaIpPublisher::publishTimerTimeout()
{
	QString message;
	float flowTemp, returnTemp, dhwTemp, roomTemp, roomSetpoint, boilerTemp;
	if (getCurrentReturn(&flowTemp, &returnTemp, &dhwTemp, &roomTemp, &roomSetpoint, &boilerTemp)) {
		message = "{";
		message.append("\"flowTemp\": \"" + QString::number(flowTemp) + "\",\n");
		message.append("\"returnTemp\": \"" + QString::number(returnTemp) + "\",\n");
		message.append("\"dhwTemp\": \"" + QString::number(dhwTemp) + "\",\n");
		message.append("\"roomTemp\": \"" + QString::number(roomTemp) + "\",\n");
		message.append("\"roomSetpoint\": \"" + QString::number(roomSetpoint) + "\",\n");
		message.append("\"boilerTemp\": \"" + QString::number(boilerTemp) + "\"\n");
		message.append("}");
		qDebug() << message;
		emit(publisherChanged(root + "/currentReturn", message));
	} else {
		qDebug() << "Failed to read Remeha";
	}
}

bool RemehaIpPublisher::connectToHost()
{
	if (socket->state() != QTcpSocket::ConnectedState) {
		socket->connectToHost(remehaHost, remehaPort);
		return socket->waitForConnected();
	}
	return true;
}

bool RemehaIpPublisher::getCurrentReturn(float *flowTemp, float *returnTemp, float *dhwTemp, float *roomTemp, float *roomSetpoint, float *boilerTemp)
{
	QByteArray reply;
	unsigned short temp;
	if (sendMessageReply(QByteArray::fromHex("0252050602005303"), &reply)) {
		temp = REMEHA_SHORT(reply.data() + 7);
		*flowTemp = (float)temp/100;
		temp = REMEHA_SHORT(reply.data() + 9);
		*returnTemp = (float)temp/100;
		temp = REMEHA_SHORT(reply.data() + 19);
		*boilerTemp = (float)temp/100;
		temp = REMEHA_SHORT(reply.data() + 21);
		*roomTemp = (float)temp/100;
		temp = REMEHA_SHORT(reply.data() + 25);
		*dhwTemp = (float)temp/100;
		temp = REMEHA_SHORT(reply.data() + 27);
		*roomSetpoint = (float)temp/100;
		return true;
	}
	return false;
}

bool RemehaIpPublisher::sendMessageReply(QByteArray msg, QByteArray *reply)
{
	bool success = false;
	int retry = retries;
	reply->clear();
	if (connectToHost()) {
		while (!success && retry-- >= 0) {
			socket->write(msg);
			socket->waitForBytesWritten();
			QThread::msleep(200);
			socket->waitForReadyRead();
			*reply = socket->readAll();
			if (validateCrc(*reply)) {
				success = true;
			} else {
				qDebug() << "CRC failed";
			}
		}
	}
	return success;
}

bool RemehaIpPublisher::validateCrc(QByteArray msg)
{
	quint8 crc = msg.at(msg.length() - 2);
	QBitArray crcTemp(8, 0);
	quint8 calculatedCrc = 0;
	QBitArray DoInvert(1, 0);
	for (int i = 1; i < msg.length() - 2; i++) {
		QBitArray bits(8);
		for (int j = 0; j < 8; j++) {
			bits.setBit(j, msg.at(i) << j  & 0x80);
		}
		for (int j = 0; j < 8; j++) {
			DoInvert.setBit(0, (bits.at(j)) ^ crcTemp.at(7));
			crcTemp.setBit(7, crcTemp.at(6));
			crcTemp.setBit(6, crcTemp.at(5));
			crcTemp.setBit(5, crcTemp.at(4));
			crcTemp.setBit(4, crcTemp.at(3));
			crcTemp.setBit(3, crcTemp.at(2));
			crcTemp.setBit(2, crcTemp.at(1));
			crcTemp.setBit(1, crcTemp.at(0));
			crcTemp.setBit(0, DoInvert.at(0));
		}
	}
	for (int j = 0; j < 8; j++) {
		calculatedCrc += crcTemp.at(j) * (1 << j);
	}
	if (calculatedCrc == crc) {
		return true;
	}
	return false;
}
