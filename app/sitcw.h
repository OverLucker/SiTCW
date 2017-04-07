﻿#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sitcw.h"
//#include "app_layer.h"
#include "postserial.h"


class SiTCW : public QMainWindow
{
	Q_OBJECT
public:
	SiTCW(QWidget *parent = Q_NULLPTR);

private slots:

	void add_item();
	void select_user(QListWidgetItem * item);

	void new_message(Message message);
	void netConnect();
	void netDisconnect();
	void connectionOpen();
	void netError(LowLevelClient::LowLevelClientError);
	void login();

private:
	Ui::SiTCWClass ui;
    PostSerial * serial;

};
