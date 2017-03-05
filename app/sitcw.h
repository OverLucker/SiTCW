#pragma once

#include <QtWidgets/QMainWindow>
#include <QtSerialPort/QSerialPort>
#include "ui_sitcw.h"
//#include "app_layer.h"
#include "postserial.h"


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

private:
	Ui::SiTCWClass ui;
	PostSerial * serial;

};
