#ifndef MSGDIALOG_H
#define MSGDIALOG_H

#include <QtGui/QDialog>

class MainWindow;

namespace Ui {
    class MsgDialog;
}

class MsgDialog : public QDialog {
    Q_OBJECT
public:
    MsgDialog(MainWindow *parent = 0);
    ~MsgDialog();

    // Overloaded
    void exec(bool bPlayerExit);

    void setTarget(bool bIsTarget) {m_bisTargetNeeded = bIsTarget;}

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MsgDialog *m_ui;

    bool m_bisTargetNeeded;
    bool m_bPlayerExited;
    MainWindow* m_pParent;

private slots:
    void checkAndClose();
};

#endif // MSGDIALOG_H
