#pragma once

#include <QtWidgets/QMainWindow>
#include <QtSerialPort/QSerialPort>
#include "ui_sitcw.h"

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

private:
	Ui::SiTCWClass ui;
	QSerialPort * serial;

};
