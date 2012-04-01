#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
using namespace std;

#include <QtCore>
#include <QtGui>

#define QUEST_FILE "quest.conf"
#define MSG_FILE "msg.conf"
#define GOODSPELL_FILE "goodspell.conf"
#define BADSPELL_FILE "badspell.conf"
#define HCKR_FILE "hckr.conf"

#define SEPARATOR ";"

enum MsgType
{
    Msg,
    BadSpell,
    GoodSpell,
    Hckr,
};

typedef struct Quest
{
    QString qsNumQuest;
    QString qsKey;
    QString qsSeconds;
    QString qsRepetition;
} DdQuest;

#endif // INCLUDES_H
