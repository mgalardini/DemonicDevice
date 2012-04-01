#include "settingsloader.h"

SettingsLoader::SettingsLoader()
{
    m_Quest.clear();
    m_Msg.clear();
}

bool SettingsLoader::init()
{
    QFile qfQuest(QUEST_FILE);
    if(!qfQuest.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&qfQuest);
    while (!in.atEnd())
    {
         QString line = in.readLine();
         parseQuest(line);
    }
    qfQuest.close();

    //

    QFile qfMsg(MSG_FILE);
    if(!qfMsg.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in1(&qfMsg);
    while (!in1.atEnd())
    {
         QString line = in1.readLine();
         parseMsg(line, Msg);
    }
    qfMsg.close();

    //

    QFile qfMsg1(GOODSPELL_FILE);
    if(!qfMsg1.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in2(&qfMsg1);
    while (!in2.atEnd())
    {
         QString line = in2.readLine();
         parseMsg(line, GoodSpell);
    }
    qfMsg1.close();

    //

    QFile qfMsg2(BADSPELL_FILE);
    if(!qfMsg2.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in3(&qfMsg2);
    while (!in3.atEnd())
    {
         QString line = in3.readLine();
         parseMsg(line, BadSpell);
    }
    qfMsg2.close();

    //

    QFile qfMsg3(HCKR_FILE);
    if(!qfMsg3.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in4(&qfMsg3);
    while (!in4.atEnd())
    {
         QString line = in4.readLine();
         parseMsg(line, Hckr);
    }
    qfMsg3.close();

    return true;
}

void SettingsLoader::parseQuest(QString qsQuest)
{
    // Quest must be in the form: NUMBER;KEY;SECONDS;REPETITION
    // Double slash is a comment (just as in c++)
    if(qsQuest.contains("//", Qt::CaseInsensitive) || !qsQuest.contains(";", Qt::CaseInsensitive))
        return;

    QStringList list1 = qsQuest.split(SEPARATOR, QString::KeepEmptyParts, Qt::CaseInsensitive);
    if (list1.size() != 4)
        return;

    // Horrible code section :P
    DdQuest myQuest;
    myQuest.qsNumQuest   = list1.at(0);
    myQuest.qsKey       = list1.at(1);
    myQuest.qsSeconds    = list1.at(2);
    myQuest.qsRepetition = list1.at(3);

    if(m_Quest.contains(myQuest.qsNumQuest.toInt()))
        return;
    else
        m_Quest[myQuest.qsNumQuest.toInt()] = myQuest;
}

// Messages are in the form: NAME;
void SettingsLoader::parseMsg(QString qsMsg, MsgType Kind)
{
    // Double slash is a comment (just as in c++)
    if(qsMsg.contains("//", Qt::CaseInsensitive))
        return;

    QStringList list1 = qsMsg.split(SEPARATOR, QString::KeepEmptyParts, Qt::CaseInsensitive);
    if (list1.size() != 2)
        return;

    // Horrible code section :P
    switch(Kind)
    {
    case Msg:
        m_Msg[list1.at(0)] = list1.at(1);
        break;
    case BadSpell:
        m_BadSpell[list1.at(0)] = list1.at(1);
        break;
    case GoodSpell:
        m_GoodSpell[list1.at(0)] = list1.at(1);
        break;
    case Hckr:
        m_Hckr[list1.at(0)] = list1.at(1);
        break;
    default:
        break;
    }
}

QString SettingsLoader::getMsg(QString qsKey)
{
    // Be sure that the message is there!!
    if(!m_Msg.contains(qsKey))
        return "NO MESSAGE";
    else
        return m_Msg[qsKey];
}

bool SettingsLoader::getQuest(int iNumQuest, DdQuest &Quest)
{
    // Make sure that this quest is present!
    if(!m_Quest.contains(iNumQuest))
    {   // Dummy quest
        Quest.qsNumQuest    = "0";
        Quest.qsKey         = "Dummy";
        Quest.qsSeconds     = "60";
        Quest.qsRepetition  = "2";
        return false;
    }
    else
        Quest = m_Quest[iNumQuest];
    return true;
}

QString SettingsLoader::getGoodSpell()
{
    if(m_GoodSpell.keys().size() == 0)
        return "NONE";
    // Horrible code section :)
    return m_GoodSpell.values()[getRandomInt(0, m_GoodSpell.keys().size())];
}

QString SettingsLoader::getBadSpell()
{
    if(m_BadSpell.keys().size() == 0)
        return "NONE";
    // Horrible code section :)
    return m_BadSpell.values()[getRandomInt(0, m_BadSpell.keys().size())];
}

QString SettingsLoader::getHckr()
{
    if(m_Hckr.keys().size() == 0)
        return "";
    // Horrible code section :)
    return m_Hckr.values()[getRandomInt(0, m_Hckr.keys().size())];
}
