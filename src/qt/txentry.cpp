// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2020 The DAPS Project developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "txentry.h"
#include "ui_txentry.h"

#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>

TxEntry::TxEntry(QWidget* parent) : QWidget(parent),
                                    ui(new Ui::TxEntry),
                                    model(0)
{
    ui->setupUi(this);

    init();
}

TxEntry::~TxEntry()
{
    delete ui;
}
void TxEntry::init()
{
    ui->tableWidgetDetails->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
    ui->tableWidgetDetails->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
    this->setObjectName("TxEntry");

    expand(false);
}

void TxEntry::resizeEvent(QResizeEvent* event)
{
    this->resize(this->parentWidget()->width(), this->parentWidget()->height() * .175);
    ui->tableWidgetDetails->setColumnWidth(0, this->width() * .35);
    ui->tableWidgetDetails->setColumnWidth(1, this->width() * .35);
    this->setMaximumHeight(ui->tableWidgetDetails->isVisible() ?
                               this->parentWidget()->height() * .2 :
                               this->parentWidget()->height() * .15);
}


void TxEntry::setData(int64_t Date, QString Address, QString Amount, QString ID, QString Type)
{
    QDateTime dateTime = QDateTime::fromTime_t((qint32)Date);

    ui->labelTxAmount->setText(Amount);
    ui->labelDate->setText(dateTime.date().toString("MMMM dd yyyy") + QString("\n") + dateTime.toString("hh:mm:ss"));
    ui->labelDate->setAlignment(Qt::AlignRight | Qt::AlignTop);
    ui->tableWidgetDetails->insertRow(0);
    ui->tableWidgetDetails->setItem(0, 0, new QTableWidgetItem(ID));
    ui->tableWidgetDetails->setItem(0, 1, new QTableWidgetItem(Address));
    ui->tableWidgetDetails->setItem(0, 2, new QTableWidgetItem(dateTime.time().toString("hh:mm:ss")));
    ui->tableWidgetDetails->item(0, 2)->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidgetDetails->setHorizontalHeaderItem(2, new QTableWidgetItem(dateTime.date().toString("MM/dd/yyyy")));
    ui->tableWidgetDetails->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    if (Type == "Sent")
        ui->labelTypeIcon->setPixmap(QIcon(":icons/sent").pixmap(QSize(24, 24)));
    else if (Type == "Received")
        ui->labelTypeIcon->setPixmap(QIcon(":icons/received").pixmap(QSize(24, 24)));
    else
        ui->labelTypeIcon->setPixmap(QIcon(":icons/reward").pixmap(QSize(24, 24)));
}
void TxEntry::on_pushButtonExpand_clicked()
{
    expand(!ui->tableWidgetDetails->isVisible());
}

void TxEntry::expand(bool isExpanding)
{
    ui->tableWidgetDetails->setVisible(isExpanding);
    QString newImage = (isExpanding ?
                            ":icons/collapse" :
                            ":icons/expand");
    ui->pushButtonExpand->setIcon(QIcon(newImage));
    this->setMinimumHeight(isExpanding ? 250 : 0);
    ui->labelTxAmount->setAlignment(isExpanding ?
                                        (Qt::AlignLeft | Qt::AlignVCenter) :
                                        (Qt::AlignLeft | Qt::AlignVCenter));
}
