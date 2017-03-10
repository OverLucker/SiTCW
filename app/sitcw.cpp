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

	connect(serial, SIGNAL(new_message(Message)), this, SLOT(new_message(Message)));

	connect(ui.messageSendButton, SIGNAL(released()), this, SLOT(add_item()));
	connect(ui.messageList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(select_user(QListWidgetItem *)));
}



void SiTCW::SiTCW::new_message(Message message){

    QVBoxLayout *MessageLayout= new QVBoxLayout();

    QLabel *MessageLabel = new QLabel(
        QString::fromStdString(message.getSender()),
        ui.messageList
    );

    QLabel *MessageText = new QLabel(
        QString::fromStdString(message.getMessage()),
        ui.messageList
    );

    MessageLabel->setStyleSheet("font-weight: bold");
    MessageText->setMaximumWidth(601);
    MessageText->setWordWrap(true);

    MessageLayout->addWidget(MessageLabel);
    MessageLayout->addWidget(MessageText);

    QWidget * MessageWidget = new QWidget();
    MessageWidget->setStyleSheet(" background-color: #fff; border-radius: 8px; margin-bottom: 10px; ");
    MessageWidget->setLayout(MessageLayout);

    QListWidgetItem *ListItem=new QListWidgetItem();
    ListItem->setSizeHint( MessageWidget->sizeHint() );
    ui.messageList->addItem(ListItem);
    ui.messageList->setItemWidget(ListItem, MessageWidget );
    ui.messageList->setSelectionMode(QAbstractItemView::NoSelection);

    // ... insert in database...
}


void SiTCW::SiTCW::add_item(){

    QVBoxLayout *MessageLayout= new QVBoxLayout();

    QLabel *MessageLabel = new QLabel("Вася", ui.messageList);
    QLabel *MessageText = new QLabel(ui.messageTextInput->toPlainText(), ui.messageList);
    MessageLabel->setStyleSheet("font-weight: bold");
    MessageText->setMaximumWidth(601);
    MessageText->setWordWrap(true);

    MessageLayout->addWidget(MessageLabel);
    MessageLayout->addWidget(MessageText);

    QWidget * MessageWidget = new QWidget();
    MessageWidget->setStyleSheet(" background-color: #fff; border-radius: 8px; margin-bottom: 20px; ");
    MessageWidget->setLayout(MessageLayout);

    QListWidgetItem *ListItem=new QListWidgetItem();
    ListItem->setSizeHint( MessageWidget->sizeHint() );
    ui.messageList->addItem(ListItem);
    ui.messageList->setItemWidget(ListItem, MessageWidget );
    ui.messageList->setSelectionMode(QAbstractItemView::NoSelection);
}

void SiTCW::SiTCW::select_user(QListWidgetItem * item){

    const QString text = item->text();
    ui.currentItem->setText(text);

}
