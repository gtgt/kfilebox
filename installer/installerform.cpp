#include "installerform.h"
#include "ui_installerform.h"

InstallerForm::InstallerForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallerForm)
{
    ui->setupUi(this);
}

InstallerForm::~InstallerForm()
{
    delete ui;
}

void InstallerForm::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void InstallerForm::setProgressValue(qint64 bytesReceived, qint64 bytesTotal){
    ui->progressBar->setValue(100*bytesReceived/bytesTotal);
}
