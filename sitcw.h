#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_sitcw.h"

class SiTCW : public QMainWindow
{
	Q_OBJECT

public:
	SiTCW(QWidget *parent = Q_NULLPTR);

private:
	Ui::SiTCWClass ui;
};
