/****************************************************************************
 *
 *   (c) 2009-2018 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "STETrackerSettings.h"

#include <QString>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QHostAddress>
#include <QThread>
#include <QTimer>

// Even though QAbstractSocket::SocketError is used in a signal by Qt, Qt doesn't declare it as a meta type.
// This in turn causes debug output to be kicked out about not being able to queue the signal. We declare it
// as a meta type to silence that.
#include <QMetaType>
#include <QTcpSocket>

class STETCPLink : public QThread
{
    Q_OBJECT

    friend class STETCPLinkTest;
    friend class TCPConfiguration;
    friend class LinkManager;

public:
    STETCPLink(const QString &hostName, quint16 port, int channelIndex);
    ~STETCPLink();

    QTcpSocket* getSocket(void) { return _socket; }

    Q_INVOKABLE void sendFreqChange(int freq) { setFreq(freq); }

signals:
    void pulse(QObject* tcpLink, int channelIndex, float cpuTemp, float pulseValue, int gain);

private slots:
    // From LinkInterface
    void _writeBytes(const QByteArray data);

public slots:
    void waitForBytesWritten(int msecs);
    void waitForReadyRead   (int msecs);
    void setGain            (int gain);
    void setFreq            (int freq);

protected slots:
    void _socketError   (QAbstractSocket::SocketError socketError);
    void _stateChanged  (QAbstractSocket::SocketState socketState);
    void _restart       (void);
    void _connected     (void);
    void _disconnected  (void);

    // From LinkInterface
    virtual void readBytes(void);

protected:
    // From LinkInterface->QThread
    virtual void run(void);

private:
    // From LinkInterface
    virtual bool _connect(void);
    virtual void _disconnect(void);

    bool _hardwareConnect();

    QTcpSocket* _socket;
    QString     _hostName;
    quint16     _port;
    int         _channelIndex;
    bool        _socketIsConnected;
    QList<int>  _rgExpectedIndex;
    QTimer*     _restartTimer;
    STETrackerSettings* _settings;
};

