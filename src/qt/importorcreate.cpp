#include "importorcreate.h"
#include "ui_importorcreate.h"

#include "allocators.h"
#include "guiconstants.h"
#include "guiutil.h"
#include "walletmodel.h"

#include <QDateTime>
#include <QMessageBox>

ImportOrCreate::ImportOrCreate(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                                  ui(new Ui::ImportOrCreate)
{
    ui->setupUi(this);
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(on_next()));
}

ImportOrCreate::~ImportOrCreate()
{
    delete ui;
}

void ImportOrCreate::on_next()
{
    if (ui->rbCreate->isChecked()) {
        accept();
        CHDChain hdChainCurrent;
        if (!pwalletMain->GetDecryptedHDChain(hdChainCurrent))
            return;

        SecureString mnemonic;
        SecureString mnemonicPass;
        if (!hdChainCurrent.GetMnemonic(mnemonic, mnemonicPass))
            return;

        QString mPhrase = std::string(mnemonic.begin(), mnemonic.end()).c_str();
        QMessageBox msgBox;
        QPushButton* copyButton = msgBox.addButton(tr("Copy"), QMessageBox::ActionRole);
        QPushButton* okButton = msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
        copyButton->setStyleSheet("background:transparent;");
        copyButton->setIcon(QIcon(":/icons/editcopy"));
        msgBox.setWindowTitle("Mnemonic Recovery Phrase");
        msgBox.setText("Below is your Mnemonic Recovery Phrase, consisting of 24 seed words. Please copy/write these words down in order. We strongly recommend keeping multiple copies in different locations.");
        msgBox.setInformativeText("\n<b>" + mPhrase + "</b>");
        msgBox.setStyleSheet(GUIUtil::loadStyleSheet());
        msgBox.exec();

        if (msgBox.clickedButton() == copyButton) {
            //Copy Mnemonic Recovery Phrase to clipboard
            GUIUtil::setClipboard(std::string(mnemonic.begin(), mnemonic.end()).c_str());
        }

    } else {
        willRecover = true;
        accept();
    }
}
