#include "Threads.h"

void cliThread::run(void)
{
    m_cli->Init();
    exec(); // start the event loop
}

void playerInfoThread::run(void)
{
    m_playerInfo->Init();
    exec(); // start the event loop
}

