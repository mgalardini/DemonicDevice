#ifndef SETTINGSLOADER_H
#define SETTINGSLOADER_H

#include "includes.h"

class SettingsLoader
{
public:
    SettingsLoader();

    bool init();
    QString getMsg(QString qsKey);
    int getQuestSize() {return m_Quest.size();}
    bool getQuest(int iNumQuest, DdQuest &Quest);
    QString getGoodSpell();
    QString getBadSpell();
    QString getHckr();

private:
    QMap<int, DdQuest> m_Quest;
    QMap<QString, QString> m_Msg;
    QMap<QString, QString> m_BadSpell;
    QMap<QString, QString> m_GoodSpell;
    QMap<QString, QString> m_Hckr;

    void parseQuest(QString qsQuest);
    void parseMsg(QString qsMsg, MsgType Kind);
    int getRandomInt(int iMin, int iMax)
    {
        return iMin + qrand() % iMax;
    }

};

#endif // SETTINGSLOADER_H
