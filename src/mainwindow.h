#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "settingsloader.h"

namespace Ui
{
    class MainWindowClass;
}

class SettingsLoader;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool resInit();
    void initObjects();
    bool voiceString(QString qsToBeRead, bool bRandomize = false);

    int getRandomInt(int iMin, int iMax)
    {
        return iMin + qrand() % iMax;
    }

    SettingsLoader *m_pRes;

private:
    Ui::MainWindowClass *ui;

    // Quest info
    int m_iNextQuest;
    DdQuest m_CurQuest;
    int m_iRepetition;
    int m_iSeconds;
    //
    bool m_bStarted;
    bool m_bTimeStopped;
    bool m_bNextQuest;
    bool m_bEnd;
    bool m_bQuickShow;
    bool m_bLastMinutes;
    QProcess* m_qpEspeak;
    QTimer* m_tSecs;
    QTimer* m_tSpecial;
    QTimer* m_tHideSpecial;
    QTimer* m_tHacker;

    void loadQuest(int iQuest, bool bRepeat = false);
    enum pauseReason
    {
        nextQuest,
        wrongAnswer,
        timeoutQuest,
        stopTime,
        playerExit,
        special,
        endQuest,
    };
    void pauseClock(pauseReason reason);
    void setClock(int iMin, int iSec);
    void getClock(int &iMin, int &iSec);
    void showQuest();
    void showSpecial();

signals:
    void sig_questTimeout();

private slots:
    void QuestAnswer();
    void checkAndShowSpecial();
    void SpecialAction();
    void updateClock();
    void questTimeout();
    void checkAndVoiceHckr();
    void hideSpecial();
    void hideQuest();
};

#endif // MAINWINDOW_H
