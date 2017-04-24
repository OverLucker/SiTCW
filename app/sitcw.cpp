#include "sitcw.h"
#include "data_link_layer.h"
#include <QTextCodec>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QCloseEvent>

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
	
	// prepare first appearance
	//ui.portWidget->setVisible(true);
	// this->setCentralWidget(ui.portWidget);
	//ui.authWidget->setVisible(false);
	//ui.mainWidget->setVisible(false);

	connect(this, SIGNAL(close()), this, SLOT(onClose()));

	// connection events
	// connect(serial, &PostSerial::connectionOpen, this, &SiTCW::connectionOpen);
	connect(serial, &PostSerial::ClientStateChanged, this, &SiTCW::connectionOpen);
	connect(serial, &PostSerial::errorOccured, this, &SiTCW::netError);
	connect(ui.netBtnConnect, SIGNAL(released()), this, SLOT(netConnect()));
	connect(ui.netBtnDisconnect, SIGNAL(released()), this, SLOT(netDisconnect()));

	// address book events
	connect(serial, &PostSerial::userAdded, this, &SiTCW::addressBookAdd);
	connect(serial, &PostSerial::userRemoved, this, &SiTCW::addressBookRemove);

	// user events
	connect(ui.pbLogin, SIGNAL(released()), this, SLOT(login()));
	connect(ui.pbLogout, SIGNAL(released()), this, SLOT(logout()));

	// message events
	connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));
	connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
	connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));

	ui.netBtnDisconnect->setEnabled(false);

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

}

void SiTCW::SiTCW::netConnect() {
	int res = serial->network_connect(ui.netInputPortIn->currentText(), ui.netInputPortOut->currentText());
	ui.statusBar->showMessage(QString("Попытка подсоединиться. Уникальное значение = %1").arg(serial->get_def_num()));
	if (!res) {
		QMessageBox::information(this, "Статус подключения", "Произошла ошибка");
		ui.statusBar->showMessage("Ошибка подключения");
	}
	ui.netBtnDisconnect->setEnabled(true);
	ui.netBtnConnect->setEnabled(false);
	ui.gbPortIn->setEnabled(false);
	ui.gbPortOut->setEnabled(false);
}

void SiTCW::SiTCW::netDisconnect() {
	serial->logout(serial->get_current_logged_user());
	QTimer::singleShot(LOG_OUT_DELAY, serial, SLOT(network_disconnect()));
	QMessageBox::information(this, "Статус подключения", "Разъединено");
	ui.statusBar->showMessage("Разъединено");
}

void SiTCW::SiTCW::connectionOpen(PostSerial::ClientState state) {
	if (state == PostSerial::ClientState::Connected) {
		QMessageBox::information(this, "Статус подключения", "Успех");
		ui.statusBar->showMessage(QString("Успешно подключено. Адрес в сети = %1").arg(serial->get_local_address()));
	}
	
}

void SiTCW::SiTCW::netError(LowLevelClient::LowLevelClientError) {
	QMessageBox::information(this, "Статус подключения", "Произошла ошибка сети");
	ui.statusBar->showMessage("Произошла ошибка сети");
}

void SiTCW::SiTCW::login() {
	if (!serial->login(ui.leLogin->text(), ui.lePass->text())) {
		QMessageBox::information(this, "Ошибка авторизации", "Ошибка авторизации. Проверьте ваше имя пользователя и пароль.");
		ui.statusBar->showMessage("Ошибка авторизации");
	}
	else {
		ui.statusBar->showMessage(QString("Авторизация прошла успешно. Адрес в сети = %1").arg(serial->get_local_address()));
	}
}

void SiTCW::SiTCW::logout() {
	serial->logout(serial->get_current_logged_user());
	ui.statusBar->showMessage("Выход из сети");
}

void SiTCW::SiTCW::addressBookAdd(QString username) {
	ui.contactList->addItem(username);
}

void SiTCW::SiTCW::addressBookRemove(QString username) {
	try {
		if (ui.contactList->findItems(username, Qt::MatchExactly).length() > 0)
			ui.contactList->removeItemWidget(ui.contactList->findItems(username, Qt::MatchExactly)[0]);
	}
	catch (...) {

	}
}

void SiTCW::SiTCW::closeEvent(QCloseEvent*) {
	
	// serial->network_disconnect();
}