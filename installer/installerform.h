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

    void setProgressValue(int);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::InstallerForm *ui;

};

#endif // INSTALLERFORM_H
