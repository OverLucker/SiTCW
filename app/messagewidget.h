#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <qlistwidget.h>
#include "message.h"

namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = 0);
	void display_message(Message);
	void setItem(QListWidgetItem *);
    ~MessageWidget();

private:
    Ui::MessageWidget *ui;
	Message message;
	QListWidgetItem * it;

protected:
	void mousePressEvent(QMouseEvent*);

signals:
	void messageRead(Message);

public slots:
	void updateMessage(Message);
};

#endif // MESSAGEWIDGET_H
