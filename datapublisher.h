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

#ifndef DATAPUBLISHER_H
#define DATAPUBLISHER_H

#include <QObject>

class DataPublisher : public QObject
{
	Q_OBJECT
public:
	explicit DataPublisher(QString root, QObject *parent = 0);
	~DataPublisher();
signals:
	void publisherChanged(QString path, QString value);
protected:
	QString root;
};

#endif // DATAPUBLISHER_H
