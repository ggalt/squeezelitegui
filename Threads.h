#ifndef THREADS_H
#define THREADS_H
#include <QThread>

#include "slimcli.h"
#include "playerinfo.h"

class cliThread : public QThread
{
    Q_OBJECT
public:
    cliThread(SlimCLI *cli, QObject *parent=0) : QThread(parent), m_cli(cli) {}
protected:
    void run(void);
private:
    SlimCLI *m_cli;
};

class playerInfoThread : public QThread
{
    Q_OBJECT
public:
    playerInfoThread(playerInfo *PlayerInfo, QObject *parent=0) : QThread(parent), m_playerInfo(PlayerInfo) {}
protected:
    void run(void);
private:
    playerInfo *m_playerInfo;
};


#endif // THREADS_H
