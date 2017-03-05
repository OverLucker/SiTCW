#include "sitcw.h"
#include "data_link_layer.h"
#include <QTextCodec>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QHBoxLayout>

// VERY IMPORTANT THING!!!
#pragma execution_character_set("utf-8")


SiTCW::SiTCW(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	serial = new QSerialPort(this);

	// running tests
	QVector<quint8> tmp = { quint8(1), quint8(2), quint8(3) };

	connect(ui.btnConnect, SIGNAL(released()), this, SLOT(openSerialPort()));
	connect(ui.btnDisconnect, SIGNAL(released()), this, SLOT(closeSerialPort()));
	connect(serial, &QSerialPort::readyRead, this, &SiTCW::readData);
	connect(ui.btnSend, SIGNAL(released()), this, SLOT(writeData()));



    connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
    connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));
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

void SiTCW::SiTCW::add_item(){
//      const QString new_item = ui.itemName->toPlainText();
//      ui.messageList->addItem(new_item);

//        FIRST EXAMPLE
//    QGridLayout *layout = new QGridLayout(this);
//    headerBar *Header = new headerBar(this);
//    layout->addWidget(Header,0,0);


    QVBoxLayout *HLay= new QVBoxLayout();

    QPushButton *b1 = new QPushButton("B1");
    QPushButton *b2 = new QPushButton("B2");

    HLay->addWidget(b1);
    HLay->addWidget(b2);

    QWidget * twoButtonWidget = new QWidget();
    twoButtonWidget->setLayout( HLay );

    QListWidgetItem *ListItem=new QListWidgetItem();

//    b1->setStyleSheet(   "color: blue;"
//                                      "height: 150px;"
////                                      "margin-right: 50px;"
////                                      "margin-bottom: 50px;"
//                                      "background-color: yellow;");

    ui.messageList->addItem(ListItem);
    ui.messageList->setItemWidget(ListItem, twoButtonWidget );

}

void SiTCW::SiTCW::select_user(QListWidgetItem * item){

    const QString text = item->text();
    ui.currentItem->setText(text);

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
	ui.textBrowser->append(data);
}

void SiTCW::SiTCW::writeData()
{
	serial->write(QByteArray(ui.teMess->toPlainText().toStdString().c_str()));
}
