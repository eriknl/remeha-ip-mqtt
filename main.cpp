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

#include <unistd.h>
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <QDebug>

#include "remehaip.h"
#include "qpaho.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream qout(stdout);
    if (argc < 6) {
        qout << "5 parameters required: " << argv[0] << " <Remeha host> <Remeha port> <MQTT broker host> <MQTT clientid> <MQTT topic>\n";
        qout.flush();
        return 1;
    }

    QString message;
    QString topic = argv[5];
    float flowTemp, returnTemp, dhwTemp, roomTemp, roomSetpoint, boilerTemp;

    RemehaIP remehaConnection(argv[1], atoi(argv[2]), 10);
    QPaho mqttConnection(argv[3], argv[4]);

    while(1) {
        if (remehaConnection.getCurrentReturn(&flowTemp, &returnTemp, &dhwTemp, &roomTemp, &roomSetpoint, &boilerTemp)) {
            message = "{";
            message.append("\"flowTemp\": \"" + QString::number(flowTemp) + "\",\n");
            message.append("\"returnTemp\": \"" + QString::number(returnTemp) + "\",\n");
            message.append("\"dhwTemp\": \"" + QString::number(dhwTemp) + "\",\n");
            message.append("\"roomTemp\": \"" + QString::number(roomTemp) + "\",\n");
            message.append("\"roomSetpoint\": \"" + QString::number(roomSetpoint) + "\",\n");
            message.append("\"boilerTemp\": \"" + QString::number(boilerTemp) + "\"\n");
            message.append("}");
            qDebug() << message;
            mqttConnection.publishMessage(topic + "/currentReturn", message);
            QThread::sleep(60);
        } else {
            qDebug() << "Failed to read Remeha";
            QThread::sleep(5);
        }
    }

    return a.exec();
}
