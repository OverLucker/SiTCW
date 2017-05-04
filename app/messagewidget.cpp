#include "messagewidget.h"
#include "ui_messagewidget.h"

// VERY IMPORTANT THING!!!
#pragma execution_character_set("utf-8")


MessageWidget::MessageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWidget)
{
    ui->setupUi(this);
	QPalette p(this->palette());
	p.setColor(QPalette::Background, Qt::red);
	this->setPalette(p);
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::display_message(Message message, bool incoming) {
	this->message = message;
	this->setBackgroundRole(QPalette::ColorRole::Base);
	ui->lbMessage->setText(message.getMessage());
	if (message.getStatus()) {
		ui->lbSTATUS->setText(QString("%1: %2").arg("STATUS", "Прочитано"));
	}
	else {
		ui->lbSTATUS->setText(QString("%1: %2").arg("STATUS", "Не прочитано"));
		if (!incoming)
			ui->lbMessage->hide();
	}
	
	ui->lbFROM->setText(QString("%1: %2").arg("FROM", message.getSender()));
	ui->lbTO->setText(QString("%1: %2").arg("TO", message.getRecepient()));
}

void MessageWidget::mousePressEvent(QMouseEvent* ev) {
	ui->lbMessage->show();
	it->setSizeHint(this->sizeHint());
	ui->lbSTATUS->setText(QString("%1: %2").arg("STATUS", "Прочитано"));
	emit messageRead(this->message);
}

void MessageWidget::setItem(QListWidgetItem * it) {
	this->it = it;
}

void MessageWidget::updateMessage(Message message) {
	if (this->message.getId() == message.getMessage()) {
		ui->lbMessage->show();
		it->setSizeHint(this->sizeHint());
		ui->lbSTATUS->setText(QString("%1: %2").arg("STATUS", "Прочитано"));
	}
}
