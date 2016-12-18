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

#include "qpaho.h"
#include <QDebug>

QPaho::QPaho(QString brokerHost, QString clientId, QObject *parent) : brokerHost(brokerHost), clientId(clientId), QObject(parent)
{
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_create(&client, brokerHost.toStdString().c_str(), clientId.toStdString().c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 120;
    conn_opts.cleansession = 1;

    // TODO: implement callbacks...
//    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
         qDebug() << "Failed to connect, return code " << rc;
         exit(-1);
    }

}

QPaho::~QPaho()
{
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

void QPaho::publishMessage(QString topic, QString message)
{
    if (!MQTTClient_isConnected(client)) {
        qDebug() << "MQTTClient was disconnected, reconnecting";
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.keepAliveInterval = 120;
        conn_opts.cleansession = 1;
        MQTTClient_connect(client, &conn_opts);
    }
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = (void*)strdup(message.toStdString().c_str());
    pubmsg.payloadlen = message.length();
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, topic.toStdString().c_str(), &pubmsg, &token);
    free(pubmsg.payload);
}
