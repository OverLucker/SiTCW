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
	
    serial = new PostSerial(this);
	HammingCodec<7> codec;
	serial->setCodec(new HammingCodec<7>());
	for (auto item : QSerialPortInfo::availablePorts()) {
		ui.netInputPortIn->insertItem(1000, item.portName(), 0);
		ui.netInputPortOut->insertItem(1000, item.portName(), 0);
	}
	
	// ToDo: do it dynamically
	ui.contactList->addItems(serial->get_contacts());
	
	connect(serial, &PostSerial::connectionOpen, this, &SiTCW::connectionOpen);
	connect(serial, &PostSerial::errorOccured, this, &SiTCW::netError);
	connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));
	connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
	connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));
	connect(ui.netBtnConnect, SIGNAL(released()), this, SLOT(netConnect()));
	connect(ui.netBtnDisconnect, SIGNAL(released()), this, SLOT(netDisconnect()));
	connect(ui.pbLogin, SIGNAL(released()), this, SLOT(login()));
	
}



void SiTCW::SiTCW::new_message(Message message){

    QVBoxLayout *MessageLayout= new QVBoxLayout();

    QLabel *MessageLabel = new QLabel(
        message.getSender(),
        ui.messageList
    );

    QLabel *MessageText = new QLabel(
        message.getMessage(),
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

    // ... insert in database...
}


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


	// sending message
	if (ui.contactList->currentItem()) {
		QString to;
		to = ui.contactList->currentItem()->text();
		Message mess(to, ui.messageTextInput->toPlainText());
		serial->send_message(mess);
	}
}

void SiTCW::SiTCW::select_user(QListWidgetItem * item){

    const QString text = item->text();
    ui.currentItem->setText(text);

}

void SiTCW::SiTCW::netConnect() {
	int res = serial->network_connect(ui.netInputPortIn->currentText(), ui.netInputPortOut->currentText());
	if (res) {
		
	}
	else
		QMessageBox::information(this, "Connection status", "An error occured");
}

void SiTCW::SiTCW::netDisconnect() {
	serial->network_disconnect();
	QMessageBox::information(this, "Connection status", "Disconnected");
}

void SiTCW::SiTCW::connectionOpen() {
	QMessageBox::information(this, "Connection status", "Success");
}

void SiTCW::SiTCW::netError(LowLevelClient::LowLevelClientError error) {
	QMessageBox::information(this, "Connection status", "Network error occured");
}

void SiTCW::SiTCW::login() {
	if (!serial->login(ui.leLogin->text(), ui.lePass->text()))
		QMessageBox::information(this, "Login failed", "Login failed!!!");
	else
		QMessageBox::information(this, "Login success", "Login success");
}