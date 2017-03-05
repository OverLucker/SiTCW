#include "sitcw.h"
#include "data_link_layer.h"
#include <QTextCodec>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>

// VERY IMPORTANT THING!!!
#pragma execution_character_set("utf-8")


SiTCW::SiTCW(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	serial = new PostSerial(this);

	// running tests
	QVector<quint8> tmp = { quint8(1), quint8(2), quint8(3) };

	connect(ui.btnConnect, SIGNAL(released()), this, SLOT(openSerialPort()));
	connect(ui.btnDisconnect, SIGNAL(released()), this, SLOT(closeSerialPort()));
	connect(serial, &QSerialPort::readyRead, this, &SiTCW::readData);
	connect(ui.btnSend, SIGNAL(released()), this, SLOT(writeData()));
/*
	for(auto it : QSerialPortInfo::availablePorts())
		ui.textBrowser->append(it.portName());*/
}


void SiTCW::SiTCW::openSerialPort()
{
	serial->setPortName(ui.tePortName->toPlainText());

	if (serial->open(QIODevice::ReadWrite)) {
		QMessageBox::about(this, "Success", "Connected");
	}
	else {
		QMessageBox::critical(this, tr("Error"), serial->errorString());
	}
}


void SiTCW::SiTCW::closeSerialPort()
{
	if (serial->isOpen())
		serial->close();
		QMessageBox::about(this, "Success", "Disconnected");
}


void SiTCW::SiTCW::readData()
{
	QByteArray data = serial->readAll();
	ui.textBrowser->append(QString("Received data '%1' sended by %2").arg(QString(data), sender()->objectName()));
}

void SiTCW::SiTCW::writeData()
{
	serial->write(QByteArray(ui.teMess->toPlainText().toStdString().c_str()));
}
