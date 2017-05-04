#include "sitcw.h"
#include "data_link_layer.h"
#include <QTextCodec>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTimer>
#include "messagewidget.h"
#include <QCloseEvent>

// VERY IMPORTANT THING!!!
#pragma execution_character_set("utf-8")




SiTCW::SiTCW(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
    serial = new PostSerial(this);
	serial->setCodec(new HammingCodec<7>());
	for (auto item : QSerialPortInfo::availablePorts()) {
		ui.netInputPortIn->insertItem(1000, item.portName(), 0);
		ui.netInputPortOut->insertItem(1000, item.portName(), 0);
	}
	
	// prepare first appearance

	connect(this, SIGNAL(close()), this, SLOT(onClose()));

	// connection events

	// physical connection updates
	connect(serial, &PostSerial::connectionOpen, this, &SiTCW::connectionOpen);
	connect(serial, &PostSerial::connectionClosed, this, &SiTCW::connectionClosed);

	// logical connection updates
	connect(serial, &PostSerial::ClientStateChanged, this, &SiTCW::updateClientState);

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

	
}

void SiTCW::SiTCW::display_message(Message& message, bool inc) {
	MessageWidget * wid = new MessageWidget(ui.messageList);
	QListWidgetItem *ListItem = new QListWidgetItem();
	wid->setStyleSheet("{background-color: #fff; }");
	wid->display_message(message, inc);
	wid->setItem(ListItem);
	ListItem->setSizeHint(wid->sizeHint());
	ui.messageList->addItem(ListItem);
	ui.messageList->setItemWidget(ListItem, wid);
	
	connect(wid, &MessageWidget::messageRead, this, &SiTCW::messageRead);
	connect(serial, &PostSerial::message_read, wid, &MessageWidget::updateMessage);
}

void SiTCW::SiTCW::new_message(Message message){
	display_message(message, false);
}

void SiTCW::SiTCW::add_item(){

	// sending message
	if (ui.contactList->currentItem()) {
		QString to = ui.contactList->currentItem()->text();
		Message mess(serial->get_current_logged_user(), to, ui.messageTextInput->toPlainText());
		serial->send_message(mess);
		display_message(mess, true);
		// clear input
		ui.messageTextInput->clear();
	}
}

void SiTCW::SiTCW::select_user(QListWidgetItem * item){

    const QString text = item->text();
	// ToDo: do smthing with new selected user
}

void SiTCW::SiTCW::netConnect() {
	int res = serial->network_connect(ui.netInputPortIn->currentText(), ui.netInputPortOut->currentText());
	ui.statusBar->showMessage(QString("Попытка подсоединиться. Уникальное значение = %1").arg(serial->get_def_num()));
}

void SiTCW::SiTCW::netDisconnect() {
	if (serial->get_current_logged_user() != "") {
		serial->logout(serial->get_current_logged_user());
		QTimer::singleShot(LOG_OUT_DELAY, serial, SLOT(network_disconnect()));
	}
	else {
		serial->network_disconnect();
	}
}


// physical connection
void SiTCW::SiTCW::connectionOpen() {
	ui.statusBar->showMessage("Успешно подключено к портам");
	ui.netBtnDisconnect->setEnabled(true);
	ui.netBtnConnect->setEnabled(false);
	ui.gbPortIn->setEnabled(false);
	ui.gbPortOut->setEnabled(false);
}

void SiTCW::SiTCW::connectionClosed() {
	QMessageBox::information(this, "Статус подключения", "Разъединено");
	ui.statusBar->showMessage("Разъединено");
	ui.netBtnDisconnect->setEnabled(false);
	ui.netBtnConnect->setEnabled(true);
	ui.gbPortIn->setEnabled(true);
	ui.gbPortOut->setEnabled(true);
}
// physical connection

void SiTCW::SiTCW::updateClientState(PostSerial::ClientState state) {
	if (state == PostSerial::ClientState::Online) {
		QMessageBox::information(this, "Статус подключения", "Успех");
		ui.statusBar->showMessage(QString("Успешно подключено."));
		// переключаем на страницу авторизации
		ui.tabWidget->setCurrentIndex(1);
	}

	if (state == PostSerial::ClientState::Offline) {
		QMessageBox::information(this, "Статус подключения", "Оффлайн");
		ui.statusBar->showMessage(QString("Оффлайн"));
	}
	
}

void SiTCW::SiTCW::netError(LowLevelClient::LowLevelClientError error) {
	if (error == LowLevelClient::LowLevelClientError::OpenError) {
		QMessageBox::information(this, "Статус подключения", "Не удалось подключиться к портам");
		ui.statusBar->showMessage("Не удалось подключиться к портам");
	}
	if (error == LowLevelClient::LowLevelClientError::ConnectionClosed) {
		this->connectionClosed();
	}
}

void SiTCW::SiTCW::login() {
	if (!serial->login(ui.leLogin->text(), ui.lePass->text())) {
		QMessageBox::information(this, "Ошибка авторизации", "Ошибка авторизации. Проверьте ваше имя пользователя и пароль.");
		ui.statusBar->showMessage("Ошибка авторизации");
	}
	else {
		ui.statusBar->showMessage(QString("Авторизация прошла успешно. Адрес в сети = %1").arg(serial->get_local_address()));
		ui.leLogin->setEnabled(false);
		ui.lePass->setEnabled(false);
		ui.pbLogin->setEnabled(false);
		ui.pbLogout->setEnabled(true);
		for (auto it : serial->getOutcomingPostbox())
			display_message(it, false);
	}
}

void SiTCW::SiTCW::logout() {
	serial->logout(serial->get_current_logged_user());
	ui.statusBar->showMessage("Выход из сети");
	ui.leLogin->setEnabled(true);
	ui.lePass->setEnabled(true);
	ui.pbLogin->setEnabled(true);
	ui.pbLogout->setEnabled(false);
}

void SiTCW::SiTCW::addressBookAdd(QString username) {
	ui.contactList->addItem(username);
	if (username == serial->get_current_logged_user())
		ui.tabWidget->setCurrentIndex(2);
}

void SiTCW::SiTCW::addressBookRemove(QString username) {
	QList<QListWidgetItem*> items = ui.contactList->findItems(username, Qt::MatchExactly);
	if (items.length() > 0)
		ui.contactList->takeItem(ui.contactList->row(items[0]));
}

void SiTCW::SiTCW::closeEvent(QCloseEvent* ev) {
	serial->network_disconnect();
	ev->accept();
}

void SiTCW::SiTCW::messageRead(Message message) {
	serial->notify_message_read(message.getId());
}
