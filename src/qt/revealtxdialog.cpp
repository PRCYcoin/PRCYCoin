#include "revealtxdialog.h"
#include "ui_revealtxdialog.h"
#include "bitcoinunits.h"

#include <QClipboard>

RevealTxDialog::RevealTxDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                                  ui(new Ui::RevealTxDialog)
{
    ui->setupUi(this);

    ui->pushButtonCPID->setStyleSheet("background:transparent;");
    ui->pushButtonCPID->setIcon(QIcon(":/icons/editcopy"));
    connect(ui->pushButtonCPID, SIGNAL(clicked()), this, SLOT(copyID()));

    ui->pushButtonCPAddr->setStyleSheet("background:transparent;");
    ui->pushButtonCPAddr->setIcon(QIcon(":/icons/editcopy"));
    connect(ui->pushButtonCPAddr, SIGNAL(clicked()), this, SLOT(copyAddress()));

    ui->pushButtonCPPrivK->setStyleSheet("background:transparent;");
    ui->pushButtonCPPrivK->setIcon(QIcon(":/icons/editcopy"));
    connect(ui->pushButtonCPPrivK, SIGNAL(clicked()), this, SLOT(copyPrivateKey()));

    ui->pushButtonTxFee->setStyleSheet("background:transparent;");
    ui->pushButtonTxFee->setIcon(QIcon(":/icons/editcopy"));
    connect(ui->pushButtonTxFee, SIGNAL(clicked()), this, SLOT(copyTxFee()));
}

RevealTxDialog::~RevealTxDialog()
{
    delete ui;
}

void RevealTxDialog::setTxID(QString strId)
{
    ui->lblTxID->setText(strId);
}

void RevealTxDialog::setTxAddress(QString strAddr)
{
    ui->lblAddress->setText(strAddr);
}

void RevealTxDialog::setTxPrivKey(QString strPrivKey)
{
    ui->lblPrivateKey->setText(strPrivKey);
}

void RevealTxDialog::setTxFee(CAmount fee)
{
    int nDisplayUnit;
    ui->lblTxFee->setText(BitcoinUnits::formatHtmlWithUnit(0, fee, false, BitcoinUnits::separatorAlways));
}

void RevealTxDialog::on_buttonBox_accepted()
{
}

void RevealTxDialog::copyID()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->lblTxID->text());
}

void RevealTxDialog::copyAddress()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->lblAddress->text());
}

void RevealTxDialog::copyPrivateKey()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->lblPrivateKey->text());
}

void RevealTxDialog::copyTxFee()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->lblTxFee->text());
}
