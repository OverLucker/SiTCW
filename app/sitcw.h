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
	void writeData();
	void readData();
<<<<<<< HEAD
    void add_item();
    void select_user(QListWidgetItem * item);
    void new_message(Message message);
    void send_message();
=======

  void add_item();
  void select_user(QListWidgetItem * item);

  void new_message(Message message);
>>>>>>> 7640863a3a731e54788944cd573cf613ae831d3c


private:
	Ui::SiTCWClass ui;
    PostSerial * serial;

    void render_message(Message message);
};
