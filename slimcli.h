#ifndef SLIMCLI_H
#define SLIMCLI_H

// QT5 Headers
#include <QObject>
#include <QTcpSocket>
#include <QBuffer>
#include <QTimer>
#include <QUrl>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QByteArray>
#include <QPixmap>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

#include "squeezedefines.h"

/*
  * NOTE: This class establishes an object to communicate with a SqueezeCenter Server
  * via the SqueezeCenter command line interface (usually located at port 9090).  You
  * MUST set the IP address of the SqueezeCenter server and the port BEFORE you call init().
  * Otherwise it will default to the localhost and port 9090.
*/

class SlimCLI : public QObject {
    Q_OBJECT

public:
    SlimCLI( QObject *parent=0,
             QString serverAdd= "127.0.0.1", QByteArray mac= QByteArray( "00:00:00:00:00:04" ),
             quint16 cliport = 9090);
    ~SlimCLI();

    void Init( void );
    void Connect( void );

    QString GetServerAddr(void) { QMutexLocker l(&mutex); return SlimServerAddr; }
    quint16 GetServerPort(void) { QMutexLocker l(&mutex); return cliPort; }


    bool SendBlockingCommand( QByteArray cmd );
    bool SendBlockingCommand( QByteArray cmd, QByteArray mac );
    QByteArray GetBlockingCommandResponse( QByteArray cmd );    // send a blocking command and get the response back

    void RemoveNewLineFromResponse( void );

    void    SetMACAddress( QString addr );
    QByteArray GetMACAddress( void ) { return macAddress; }

    QByteArray GetMessage(void) { QMutexLocker l(&mutex); return responseQueue.dequeue(); }
    bool MessageAvailable(void) { QMutexLocker l(&mutex); return !responseQueue.isEmpty(); }

signals:
    void isConnected( void );               // we're connected to the SqueezeCenter server
    void cliError(QString message);
    void MessageReady(void);

//    void DeviceStatusMessage(QByteArray msg);
//    void DevicePlaylistMessage(QByteArray msg);
//    void PlaylistInteractionMessage(QByteArray msg);

public slots:
    void SendStandardCommand(CliCommand cmd);
    void SendCommand( QByteArray cmd );
    void SendCommand( QByteArray cmd, QByteArray mac );

private slots:
    bool msgWaiting( void );          // we have a message waiting from the server
    bool CLIConnectionOpen( void );   // CLI interface successfully established
    void LostConnection( void );      // lost connection (check if we want reconnect)
    void ConnectionError( int err );  // error message sent with connection
    void ConnectionError( QAbstractSocket::SocketError );
    void SentBytes( int b );          // bytes sent to SqueezeCenter

private:
    void ErrorMessageSender(QString s);
    bool SetupLogin( void );
    void ProcessLoginMsg( void );
//    void ProcessControlMsg( void );
//    void processStatusMsg( void );
    bool waitForResponse( void );

private:
    QTcpSocket *slimCliSocket;// socket for CLI interface

    QByteArray command;       // string to build a command (different from "currCommand" below that is used to check what the CLI sends back
    QByteArray response;      // buffer to hold CLI response
    QQueue<QByteArray> responseQueue;     // Queue of responses


    QByteArray macAddress;       // NOTE: this is stored in URL escaped form, since that is how we mostly use it.  If you need it in plain text COPY IT to another string and use QUrl::decode() on that string.
    QString SlimServerAddr;   // server IP address
    quint16 cliPort;          // port to use for cli, usually 9090, but we allow the user to change this
    QByteArray MaxRequestSize;

    QString cliUsername;      // username for cli if needed
    QString cliPassword;      // password for cli if needed **NOTE: DANGER, DANGER this is done in clear text, so don't use a password you care about!!
    bool useAuthentication;   // test for using authentication
    bool isAuthenticated;     // have we been authenticated?
    int iTimeOut;             // number of milliseconds before CLI blocking requests time out

    QMutex mutex;

//    void DeviceMsgProcessing( void ); // messages forwarded to devices
//    void SystemMsgProcessing( void ); // messages forwarded to the system for processing
};


#endif // SLIMCLI_H
