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

}



void SiTCW::SiTCW::readData()
{
	//QByteArray data = serial->readAll();
	//ui.textBrowser->append(QString("Received data '%1' sended by %2").arg(QString(data), sender()->objectName()));
}

void SiTCW::SiTCW::writeData()
{
	serial->send_message(Message("me", "anton", ui.teMess->toPlainText().toStdString()));
	//serial->write(QByteArray(ui.teMess->toPlainText().toStdString().c_str()));
}

void SiTCW::SiTCW::IncMessage(Message mess) {
	ui.textBrowser->append(QString("%1 send to %2 message %3").arg(mess.getSender().c_str(), mess.getRecepient().c_str(), mess.getMessage().c_str()));
}