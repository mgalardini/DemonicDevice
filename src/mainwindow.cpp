#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "msgdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent,  Qt::CustomizeWindowHint), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);

    m_pRes = new SettingsLoader();
    m_iNextQuest = 0;
    m_bStarted = false;
    m_bTimeStopped = false;
    m_bNextQuest = false;
    m_bEnd = false;
    m_bQuickShow=false;
    m_bLastMinutes=false;
    m_qpEspeak = new QProcess(this);

    connect(this, SIGNAL(sig_questTimeout()), this, SLOT(questTimeout()));

    m_tSecs = new QTimer(this);
    m_tSpecial = new QTimer(this);
    m_tHideSpecial = new QTimer(this);
    m_tHacker = new QTimer(this);

    // Set the background
    setStyleSheet("MainWindow {background-image: url(:/img/res/dd.jpg);}");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pRes;
    delete m_qpEspeak;
    delete m_tSecs;
    delete m_tSpecial;
    delete m_tHideSpecial;
    delete m_tHacker;
}

bool MainWindow::resInit()
{
    if(!m_pRes->init())
        // File not found or other errors
        return false;
    return true;
}

void MainWindow::initObjects()
{
    // The special button is hidden for now
    ui->SpecialButton->hide();

    // Control Labels
    ui->QuestNum->setText("0");
    ui->RipetionNum->setText("0");

    // The first message in the EditBar
    ui->QuestSolution->setText(m_pRes->getMsg("start"));

    // The focus is already on the EditBar
    ui->QuestSolution->setFocus(Qt::OtherFocusReason);

    // The seconds clock
    connect(m_tSecs, SIGNAL(timeout()), this, SLOT(updateClock()));
    m_tSecs->start(1000);
    // Special Button clock
    connect(m_tSpecial, SIGNAL(timeout()), this, SLOT(checkAndShowSpecial()));
    m_tSpecial->start(getRandomInt(30000, 50000));
    // Special Button hiding
    connect(m_tHideSpecial, SIGNAL(timeout()), this, SLOT(hideSpecial()));
    m_tHideSpecial->start(30000);
    // Hacker Messages clock
    connect(m_tHacker, SIGNAL(timeout()), this, SLOT(checkAndVoiceHckr()));
    m_tHacker->start(getRandomInt(65000, 120000));
}

void MainWindow::loadQuest(int iQuest, bool bRepeat)
{
    if(m_bEnd)
        return;

    DdQuest myQuest;
    if(!m_pRes->getQuest(iQuest, myQuest))
    {   // No more quests - exit
        m_bEnd = true;
        pauseClock(endQuest);
        return;
    }

    // Control Labels
    ui->QuestNum->setText(myQuest.qsNumQuest);
    if(!bRepeat)
        ui->RipetionNum->setText(myQuest.qsRepetition);
    else
    {
        QString a;
        ui->RipetionNum->setText(a.setNum(m_iRepetition));
    }

    // EditBar
    ui->QuestSolution->setText("");

    if(bRepeat)
    {
        m_iSeconds = m_iSeconds / 2;
        setClock((m_iSeconds / 60), (m_iSeconds % 60));
    }
    else
    {
        m_iSeconds = myQuest.qsSeconds.toInt();
        setClock((myQuest.qsSeconds.toInt() / 60), (myQuest.qsSeconds.toInt() % 60));
    }

    m_CurQuest = myQuest;
    if(!bRepeat)
        m_iRepetition = myQuest.qsRepetition.toInt();

    if(!bRepeat)
    {
        m_bLastMinutes = false;
        m_iNextQuest++;
    }

    ui->QuestSolution->hide();
}

void MainWindow::setClock(int iMin, int iSec)
{
    QString qsMinutes, qsSeconds;
    // Minutes
    ui->Minutes->setText(qsMinutes.setNum(iMin));
    // Seconds
    ui->Seconds->setText(qsSeconds.setNum(iSec));
}

void MainWindow::getClock(int &iMin, int &iSec)
{
    iMin = ui->Minutes->text().toInt();
    iSec = ui->Seconds->text().toInt();
}

void MainWindow::updateClock()
{
    if(!m_bStarted || m_bTimeStopped || m_bNextQuest)
        return;

    if(m_bQuickShow)
    {
        m_bQuickShow = false;
        hideQuest();
    }

    int iS, iM;
    getClock(iM, iS);

    // Quest has the priority
    if(iS == 0 && iM == 0)
    {
        emit sig_questTimeout();
        return;
    }
    else if(iS == 0)
    {
        iS = 59;
        iM--;
        // Show the linedit
        if((iM % 2) == 0 || iM < 4)
        {
            if(iM < 4)
                m_bLastMinutes = true;
            showQuest();
        }
    }
    else if(iS == 30 && !m_bLastMinutes)
    {
        iS--;
        hideQuest();
    }
    else
        iS--;

    setClock(iM, iS);
}

// Verify the current state first!
void MainWindow::QuestAnswer()
{
    if(ui->SpecialButton->isVisible())
    {
        SpecialAction();
        return;
    }

    // No real answer
    if(ui->QuestSolution->text().isEmpty() || !ui->QuestSolution->isVisible())
        return;

    // Check BackDoor
    if(QString::compare(m_pRes->getMsg("backdoor"), ui->QuestSolution->text(), Qt::CaseInsensitive) == 0)
    {   // Exit of the player
        pauseClock(playerExit);
        return;
    }
    // Check Special Stop Time
    if(QString::compare(m_pRes->getMsg("stoptime"), ui->QuestSolution->text(), Qt::CaseInsensitive) == 0)
    {
        if(m_bTimeStopped)
        {
            m_bTimeStopped = false;
            pauseClock(stopTime);
        }
        else
        {
            m_bTimeStopped = true;
            showQuest();
            pauseClock(stopTime);
        }
        return;
    }

    // First quest, no checks
    if(!m_bStarted)
    {
        m_bStarted = true;
        // Read the text, if any
        voiceString(ui->QuestSolution->text());
        loadQuest(m_iNextQuest);
        return;
    }

    if(m_bNextQuest)
    {
        m_bNextQuest = false;
        loadQuest(m_iNextQuest);
        return;
    }

    // Check the answer
    if(QString::compare(m_CurQuest.qsKey, ui->QuestSolution->text(), Qt::CaseInsensitive) == 0)
    // Correct
        pauseClock(nextQuest);
    else
        pauseClock(wrongAnswer);
}

void MainWindow::questTimeout()
{
    if(m_iRepetition == 0)
    {
        loadQuest(m_iNextQuest);
        pauseClock(timeoutQuest);
        m_bNextQuest = false;
        return;
    }
    m_iRepetition--;
    // Reload
    loadQuest(m_CurQuest.qsNumQuest.toInt(), true);
}

// Special Button pressed!
void MainWindow::SpecialAction()
{
    pauseClock(special);
}

bool MainWindow::voiceString(QString qsToBeRead, bool bRandomize)
{
    if(qsToBeRead.isEmpty())
        return true;

    QStringList qArgs;
    qArgs << "espeak";
    if(!bRandomize)
        qArgs << " -a 200 -s 120 -p 9 ";
    else
    {
        QString s, p;
        s.setNum(getRandomInt(90, 110));
        p.setNum(getRandomInt(0, 50));
        qArgs << " -s " << s << " -p " << p << " ";
    }

    qArgs << "\"" << qsToBeRead << "\"";

    if(m_qpEspeak->state() != QProcess::NotRunning)
        return false;

    m_qpEspeak->start(qArgs.join(""));
    return true;
}

void MainWindow::showQuest()
{
    ui->SpecialButton->hide();
    ui->QuestSolution->show();
    ui->QuestSolution->setFocus();
}

void MainWindow::hideQuest()
{
    ui->QuestSolution->setText("");
    ui->QuestSolution->hide();
}

void MainWindow::checkAndShowSpecial()
{
    // Check if we can show the special button
    if(ui->QuestSolution->isVisible() || ui->SpecialButton->isVisible())
    {
        m_tSpecial->stop();
        m_tSpecial->start(getRandomInt(20000, 45000));
        return;
    }

    showSpecial();
}

void MainWindow::showSpecial()
{
    ui->QuestSolution->hide();
    
    //QPalette pal = ui->SpecialButton->palette();
    //pal.setBrush(/*ui->SpecialButton->backgroundRole()*/QPalette::Button, /*QBrush(QImage("button.jpg"))*/Qt::darkRed);
    //ui->SpecialButton->setPalette(pal);
    
    ui->SpecialButton->show();
    //ui->QuestSolution->setFocus();
}

void MainWindow::hideSpecial()
{
     ui->SpecialButton->hide();
     //m_tSpecial->stop();
     //m_tSpecial->start(getRandomInt(20000, 45000));
}

void MainWindow::checkAndVoiceHckr()
{
    m_tHacker->stop();
    m_tHacker->start(getRandomInt(65000, 120000));

    if(ui->QuestSolution->isVisible() || ui->SpecialButton->isVisible())
        return;

    voiceString(m_pRes->getHckr(), true);
}

void MainWindow::pauseClock(pauseReason reason)
{
    // if needed
    MsgDialog Msg2User(this);

    switch(reason)
    {
    case nextQuest:
        m_bNextQuest = true;
        voiceString(m_pRes->getMsg("correct"));
        ui->QuestSolution->setText(m_pRes->getMsg("next"));
        break;
    case wrongAnswer:
        voiceString(m_pRes->getMsg("incorrect"));
        Msg2User.setTarget(false);
        Msg2User.exec(false);
        break;
    case timeoutQuest:
        if(m_bEnd)
            break;
        m_bNextQuest = true;
        showQuest();
        voiceString(m_pRes->getMsg("timeout"));
        ui->QuestSolution->setText(m_pRes->getMsg("next"));
        Msg2User.setTarget(false);
        Msg2User.exec(false);
    case stopTime:
        ui->QuestSolution->setText("");
        break;
    case playerExit:
        ui->QuestSolution->setText("");
        Msg2User.setTarget(false);
        Msg2User.exec(true);
        break;
    case special:
        voiceString(m_pRes->getMsg("special"), true);
        Msg2User.setTarget(true);
        Msg2User.exec(false);
        // to keep focus
        showQuest();
        m_bQuickShow=true;
        break;
    case endQuest:
        voiceString(m_pRes->getMsg("endquest"));
        showQuest();
        ui->QuestSolution->setText(m_pRes->getMsg("endquest"));
        break;
    }
}
