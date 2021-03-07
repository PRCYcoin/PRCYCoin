#include "2faconfirmdialog.h"
#include "ui_2faconfirmdialog.h"
#include "receiverequestdialog.h"
#include "qgoogleauth.h"
#include "init.h"

#include <QDateTime>

TwoFAConfirmDialog::TwoFAConfirmDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                                          ui(new Ui::TwoFAConfirmDialog)
{
    ui->setupUi(this);

    ui->txtcode_7->setVisible(false);
    ui->txtcode_8->setVisible(false);

    QIntValidator* intVal_1 = new QIntValidator(0, 9, ui->txtcode_1);
    intVal_1->setLocale(QLocale::C);
    ui->txtcode_1->setValidator(intVal_1);
    ui->txtcode_1->setAlignment(Qt::AlignCenter);

    QIntValidator* intVal_2 = new QIntValidator(0, 9, ui->txtcode_2);
    intVal_2->setLocale(QLocale::C);
    ui->txtcode_2->setValidator(intVal_2);
    ui->txtcode_2->setAlignment(Qt::AlignCenter);

    QIntValidator* intVal_3 = new QIntValidator(0, 9, ui->txtcode_3);
    intVal_3->setLocale(QLocale::C);
    ui->txtcode_3->setValidator(intVal_3);
    ui->txtcode_3->setAlignment(Qt::AlignCenter);

    QIntValidator* intVal_4 = new QIntValidator(0, 9, ui->txtcode_4);
    intVal_4->setLocale(QLocale::C);
    ui->txtcode_4->setValidator(intVal_4);
    ui->txtcode_4->setAlignment(Qt::AlignCenter);

    QIntValidator* intVal_5 = new QIntValidator(0, 9, ui->txtcode_5);
    intVal_5->setLocale(QLocale::C);
    ui->txtcode_5->setValidator(intVal_5);
    ui->txtcode_5->setAlignment(Qt::AlignCenter);

    QIntValidator* intVal_6 = new QIntValidator(0, 9, ui->txtcode_6);
    intVal_6->setLocale(QLocale::C);
    ui->txtcode_6->setValidator(intVal_6);
    ui->txtcode_6->setAlignment(Qt::AlignCenter);

    QSettings settings;
    int digits = settings.value("2fadigits").toInt();
    if (digits == 8) {
        ui->label->setText("Please enter an eight digit 2FA code:");
        ui->txtcode_7->setVisible(true);
        ui->txtcode_8->setVisible(true);

        QIntValidator* intVal_7 = new QIntValidator(0, 9, ui->txtcode_7);
        intVal_7->setLocale(QLocale::C);
        ui->txtcode_7->setValidator(intVal_7);
        ui->txtcode_7->setAlignment(Qt::AlignCenter);

        QIntValidator* intVal_8 = new QIntValidator(0, 9, ui->txtcode_8);
        intVal_8->setLocale(QLocale::C);
        ui->txtcode_8->setValidator(intVal_8);
        ui->txtcode_8->setAlignment(Qt::AlignCenter);
    }


    connect(ui->btnOK, SIGNAL(clicked()), this, SLOT(on_acceptCode()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->txtcode_1, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_2, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_3, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_4, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_5, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_6, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_7, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
    connect(ui->txtcode_8, &QLineEdit::textChanged, this, &TwoFAConfirmDialog::codeChanged);
}

TwoFAConfirmDialog::~TwoFAConfirmDialog()
{
    delete ui;
}

void TwoFAConfirmDialog::on_acceptCode()
{
    QString code;
    char code1, code2, code3, code4, code5, code6, code7, code8;
    QString input;
    char* chrlist;
    QRegExp re("\\d*"); // a digit (\d), zero or more times (*)
    input = ui->txtcode_1->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code1 = chrlist[0];

    input = ui->txtcode_2->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code2 = chrlist[0];

    input = ui->txtcode_3->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code3 = chrlist[0];

    input = ui->txtcode_4->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code4 = chrlist[0];

    input = ui->txtcode_5->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code5 = chrlist[0];

    input = ui->txtcode_6->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code6 = chrlist[0];

    input = ui->txtcode_7->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code7 = chrlist[0];

    input = ui->txtcode_8->text();
    if (input.length() > 1)
        return;
    if (!re.exactMatch(input))
        return;
    chrlist = input.toUtf8().data();
    code8 = chrlist[0];

    QSettings settings;
    int digits = settings.value("2fadigits").toInt();
    if (digits == 8) {
        code.sprintf("%c%c%c%c%c%c%c%c", code1, code2, code3, code4, code5, code6, code7, code8);
    } else if (digits == 6) {
        code.sprintf("%c%c%c%c%c%c", code1, code2, code3, code4, code5, code6);
    }

    QString result = "";
    QString secret = QString::fromStdString(pwalletMain->Read2FASecret());
    result = QGoogleAuth::generatePin(secret.toUtf8());

    if (result != code) {
        QMessageBox::critical(this, tr("Wrong 2FA Code"),
            tr("Incorrect 2FA code entered.\nPlease try again."));
        return;
    }

    accept();
}

void TwoFAConfirmDialog::codeChanged(const QString& txt)
{
    this->focusNextChild();
}
