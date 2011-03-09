#ifndef INSTALLERFORM_H
#define INSTALLERFORM_H

#include <QDialog>

namespace Ui {
class InstallerForm;
}

class InstallerForm : public QDialog {
    Q_OBJECT
public:
    InstallerForm(QWidget *parent = 0);
    ~InstallerForm();

public slots:
    void setProgressValue(qint64, qint64);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InstallerForm *ui;

};

#endif // INSTALLERFORM_H
