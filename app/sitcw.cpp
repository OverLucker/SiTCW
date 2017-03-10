#include "sitcw.h"
#include "data_link_layer.h"
#include <QTextCodec>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QHBoxLayout>
#include "message.h"

// VERY IMPORTANT THING!!!
#pragma execution_character_set("utf-8")


SiTCW::SiTCW(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
    serial = new PostSerial(this);


	// running tests
	QVector<quint8> tmp = { quint8(1), quint8(2), quint8(3) };

  connect(ui.netBtnConnect, SIGNAL(released()), this, SLOT(openSerialPort()));
  connect(ui.netBtnDisconnect, SIGNAL(released()), this, SLOT(closeSerialPort()));

  connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));

<<<<<<< HEAD
    connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));
    connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(send_message()));
    connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));
=======
  connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
  connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));

/*
	for(auto it : QSerialPortInfo::availablePorts())
		ui.textBrowser->append(it.portName());*/
>>>>>>> 7640863a3a731e54788944cd573cf613ae831d3c
}


void SiTCW::SiTCW::render_message(Message message){
    QVBoxLayout *MessageLayout= new QVBoxLayout();

    QLabel *MessageLabel = new QLabel(
        QString::fromStdString(message.getSender()),
        ui.messageList
    );

    QLabel *MessageText = new QLabel(
        QString::fromStdString(message.getMessage()),
        ui.messageList
    );

    MessageLabel->setStyleSheet("font-weight: bold");
    MessageText->setMaximumWidth(601);
    MessageText->setWordWrap(true);

    MessageLayout->addWidget(MessageLabel);
    MessageLayout->addWidget(MessageText);

    QWidget * MessageWidget = new QWidget();
    MessageWidget->setStyleSheet(" background-color: #fff; border-radius: 8px; margin-bottom: 10px; ");
    MessageWidget->setLayout(MessageLayout);

    QListWidgetItem *ListItem=new QListWidgetItem();
    ListItem->setSizeHint( MessageWidget->sizeHint() );
    ui.messageList->addItem(ListItem);
    ui.messageList->setItemWidget(ListItem, MessageWidget );
    ui.messageList->setSelectionMode(QAbstractItemView::NoSelection);
}

void SiTCW::SiTCW::new_message(Message message){
    this->render_message(message);
    // ... insert in database...
}




void SiTCW::SiTCW::send_message(){
    Message message = Message(
        "ОтправительИмя",
        "ПолучательИмя",
        ui.messageTextInput->toPlainText().toStdString()
    );
    this->render_message(message);
    serial->send_message(message);

    // ...insert in database ...
}












void SiTCW::SiTCW::openSerialPort()
{
//	serial->setPortName(ui.tePortName->toPlainText());

//	if (serial->open(QIODevice::ReadWrite)) {
//		QMessageBox::about(this, "Success", "Connected");
//	}
//	else {
//		QMessageBox::critical(this, tr("Error"), serial->errorString());
//	}
}

// SiTCW::SiTCW::create_message_widget

void SiTCW::SiTCW::add_item(){

    QVBoxLayout *MessageLayout= new QVBoxLayout();

    QLabel *MessageLabel = new QLabel("Вася", ui.messageList);
    QLabel *MessageText = new QLabel(ui.messageTextInput->toPlainText(), ui.messageList);
    MessageLabel->setStyleSheet("font-weight: bold");
    MessageText->setMaximumWidth(601);
    MessageText->setWordWrap(true);

    MessageLayout->addWidget(MessageLabel);
    MessageLayout->addWidget(MessageText);

    QWidget * MessageWidget = new QWidget();
    MessageWidget->setStyleSheet(" background-color: #fff; border-radius: 8px; margin-bottom: 20px; ");
    MessageWidget->setLayout(MessageLayout);

    QListWidgetItem *ListItem=new QListWidgetItem();
    ListItem->setSizeHint( MessageWidget->sizeHint() );
    ui.messageList->addItem(ListItem);
    ui.messageList->setItemWidget(ListItem, MessageWidget );
    ui.messageList->setSelectionMode(QAbstractItemView::NoSelection);
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
}

void SiTCW::SiTCW::writeData()
{
}

void SiTCW::SiTCW::IncMessage(Message mess) {
	ui.textBrowser->append(QString("%1 send to %2 message %3").arg(mess.getSender().c_str(), mess.getRecepient().c_str(), mess.getMessage().c_str()));
}