#include "2faqrdialog.h"
#include "ui_2faqrdialog.h"
#include "receiverequestdialog.h"
#include "guiconstants.h"
#include "key.h"
#include "pubkey.h"
#include <string>
#include <algorithm>

#include <QClipboard>
#include <QDrag>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>

// #define USE_QRCODE

#if defined(HAVE_CONFIG_H)
#include "config/prcycoin-config.h" /* for USE_QRCODE */
#endif

#ifdef USE_QRCODE
#include <qrencode.h>
#endif

TwoFAQRDialog::TwoFAQRDialog(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                                ui(new Ui::TwoFAQRDialog)
{
    ui->setupUi(this);

#ifndef USE_QRCODE
    ui->lblQRCode->setVisible(false);
#endif

    connect(ui->btnCopy, SIGNAL(clicked()), this, SLOT(on_btnCopyURI_clicked()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    ui->label->setVisible(true);
    ui->label_2->setVisible(true);
    update();
}

TwoFAQRDialog::~TwoFAQRDialog()
{
    delete ui;
}

void TwoFAQRDialog::setModel(WalletModel* model)
{
    this->model = model;
}

void TwoFAQRDialog::update()
{
    CKey newKey;
    newKey.MakeNewKey(true);

    CPubKey pubKey;
    pubKey = newKey.GetPubKey();

    QString uri;
    QString infoText;
    CBitcoinAddress address(pubKey.GetID());
    std::string addr = "";
    for (char c : address.ToString()) {
        if (!std::isdigit(c)) addr += c;
    }

    pwalletMain->Write2FASecret(addr);
    QSettings settings;
    int digits = settings.value("2fadigits").toInt();
    if (digits == 8) {
        uri.sprintf("otpauth://totp/PRCY:QT%20Wallet?secret=%s&issuer=prcycoin&algorithm=SHA1&digits=8&period=30", addr.c_str());
    } else if (digits == 6) {
        uri.sprintf("otpauth://totp/PRCY:QT%20Wallet?secret=%s&issuer=prcycoin&algorithm=SHA1&digits=6&period=30", addr.c_str());
    }

    infoText = "Recovery Key: ";
    ui->lblURI->setText(infoText + addr.c_str());

#ifdef USE_QRCODE
    ui->lblQRCode->setText("");
    if (!uri.isEmpty()) {
        // limit URI length
        if (uri.length() > MAX_URI_LENGTH) {
            ui->lblQRCode->setText(tr("Resulting URI too long, try to reduce the text for label / message."));
        } else {
            QRcode* code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
            if (!code) {
                ui->lblQRCode->setText(tr("Error encoding URI into QR Code."));
                return;
            }
            QImage myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
            myImage.fill(0xffffff);
            unsigned char* p = code->data;
            for (int y = 0; y < code->width; y++) {
                for (int x = 0; x < code->width; x++) {
                    myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                    p++;
                }
            }
            QRcode_free(code);

            ui->lblQRCode->setPixmap(QPixmap::fromImage(myImage).scaled(300, 300));
        }
    }
#endif
}

void TwoFAQRDialog::on_btnCopyURI_clicked()
{
    QString secret = QString::fromStdString(pwalletMain->Read2FASecret());
    GUIUtil::setClipboard(secret);
}
