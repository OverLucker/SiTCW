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
	
	// connection events
	connect(serial, &PostSerial::connectionOpen, this, &SiTCW::connectionOpen);
	connect(serial, &PostSerial::errorOccured, this, &SiTCW::netError);

	// address book events
	connect(serial, &DirectedTokenRing::userLoggedIn, this, &SiTCW::addressBookAdd);
	connect(serial, &DirectedTokenRing::userLoggedOut, this, &SiTCW::addressBookRemove);

	connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));
	connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
	connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));
	connect(ui.netBtnConnect, SIGNAL(released()), this, SLOT(netConnect()));
	connect(ui.netBtnDisconnect, SIGNAL(released()), this, SLOT(netDisconnect()));
	connect(ui.pbLogin, SIGNAL(released()), this, SLOT(login()));


	
}

void SiTCW::SiTCW::display_message(Message& message) {
	QGridLayout *MessageLayout = new QGridLayout();

	QLabel *MessageSender = new QLabel(
		QString("%1: %2").arg("FROM", message.getSender()),
		ui.messageList
	);
	MessageSender->setStyleSheet("font-weight: bold");

	QLabel *MessageRecipient = new QLabel(
		QString("%1: %2").arg("TO", message.getRecepient()),
		ui.messageList
	);
	MessageRecipient->setStyleSheet("font-weight: bold; text-align: right;");

	MessageLayout->addWidget(MessageSender, 0, 0);
	MessageLayout->addWidget(MessageRecipient, 0, 1);


	QLabel *MessageText = new QLabel(
		message.getMessage(),
		ui.messageList
	);
	MessageText->setMaximumWidth(601);
	MessageText->setWordWrap(true);

	MessageLayout->addWidget(MessageText, 1, 0);

	QWidget * MessageWidget = new QWidget();
	MessageWidget->setStyleSheet(" background-color: #fff; border-radius: 8px; margin-bottom: 10px; ");
	MessageWidget->setLayout(MessageLayout);

	QListWidgetItem *ListItem = new QListWidgetItem();
	ListItem->setSizeHint(MessageWidget->sizeHint());
	ui.messageList->addItem(ListItem);
	ui.messageList->setItemWidget(ListItem, MessageWidget);
}

void SiTCW::SiTCW::new_message(Message message){
	display_message(message);
    

    // ... insert in database...
}

void SiTCW::SiTCW::add_item(){

	// sending message
	if (ui.contactList->currentItem()) {
		QString to = ui.contactList->currentItem()->text();
		Message mess(serial->get_current_logged_user(), to, ui.messageTextInput->toPlainText());
		serial->send_message(mess);
		display_message(mess);
		// clear input
		ui.messageTextInput->clear();
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

void SiTCW::SiTCW::addressBookAdd(QString username) {
	ui.contactList->addItem(username);
}

void SiTCW::SiTCW::addressBookRemove(QString username) {
	ui.contactList->removeItemWidget(ui.contactList->findItems(username, Qt::MatchExactly)[0]);
}
