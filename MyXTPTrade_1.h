#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MyXTPTrade.h"

class MyXTPTrade : public QMainWindow
{
    Q_OBJECT

public:
    MyXTPTrade(QWidget *parent = nullptr);
    ~MyXTPTrade();

private:
    Ui::MyXTPTradeClass ui;
};
