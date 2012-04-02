#include "msgdialog.h"
#include "ui_msgdialog.h"

#include "mainwindow.h"

MsgDialog::MsgDialog(MainWindow *parent) :
    QDialog(parent, Qt::CustomizeWindowHint),
    m_ui(new Ui::MsgDialog)
{
    m_ui->setupUi(this);
    // Set the background
    QPalette pal = this->palette();
    pal.setColor(this->backgroundRole(), Qt::darkRed);
    this->setPalette(pal);

    m_pParent = parent;

    m_bisTargetNeeded = true;
    m_bPlayerExited = false;
}

MsgDialog::~MsgDialog()
{
    delete m_ui;
}

void MsgDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MsgDialog::exec(bool bPlayerExit)
{
    if(bPlayerExit)
    {
        QString Msg = "";
        Msg.append(m_pParent->m_pRes->getMsg("playerexit"));
        m_ui->MsgLabel->setText(Msg);
        m_ui->TargetUser->setText("");
        m_bPlayerExited = true;
    }
    else if(m_bisTargetNeeded)
    {
        m_ui->MsgLabel->setText(m_pParent->m_pRes->getMsg("target"));
        m_ui->TargetUser->setText("");
    }
    else
    {
        QString Msg = "YOU\n";
        Msg.append(m_pParent->m_pRes->getBadSpell());
        m_ui->MsgLabel->setText(Msg);
        m_ui->TargetUser->setText(m_pParent->m_pRes->getMsg("return"));
    }

    QDialog::exec();
}

void MsgDialog::checkAndClose()
{
    if(m_bPlayerExited)
    {
        m_pParent->voiceString(m_pParent->m_pRes->getMsg("goodbye"), true);
        close();
    }
    else if(m_bisTargetNeeded)
    {
        QString Msg = m_ui->TargetUser->text();
        if(QString::compare("me", Msg, Qt::CaseInsensitive) == 0)
            Msg = "YOU";
        Msg.append("\n");
        int iGoodBad = m_pParent->getRandomInt(0, 2);
        if(iGoodBad == 0)
            Msg.append(m_pParent->m_pRes->getGoodSpell());
        else
            Msg.append(m_pParent->m_pRes->getBadSpell());
         m_ui->MsgLabel->setText(Msg);
         m_ui->TargetUser->setText(m_pParent->m_pRes->getMsg("returnspecial"));

         m_bisTargetNeeded = false;
    }
    else
        close();
}
