#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sitcw.h"
//#include "app_layer.h"
#include "postserial.h"
#include "message.h"


class SiTCW : public QMainWindow
{
	Q_OBJECT
public:
	SiTCW(QWidget *parent = Q_NULLPTR);

private slots:
	void openSerialPort();
	void closeSerialPort();
	void writeData();
	void readData();
    void add_item();
    void select_user(QListWidgetItem * item);
    void new_message(Message message);
    void send_message();


private:
	Ui::SiTCWClass ui;
    PostSerial * serial;

    void render_message(Message message);
};
