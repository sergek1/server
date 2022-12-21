#ifndef SERVER_H
#define SERVER_H
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDebug>
#include <fstream>
#include <vector>
#include <QFileDialog>
#include <thread>
#include <iostream>
#include <QNetworkInterface>
#include <QTimer>

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server();
    void set_file(std::string file_name);
    void StartEncription(int key, int task_type, int machine_type);
    QTcpSocket *socket;

signals:
    void signalSaveImage(/*QVector<*/QString/*>*/ transformed_string);
    void signalClientInfo(QString client_ip_addresses_);
    void signalServerIP(QString server_ip);

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();

private:
    std::vector<std::thread> threads;
    QVector<QTcpSocket*> Sockets;
    /*QVector<*/QString/*>*/ transformed_strings_vector;
    QByteArray Data;
    quint64 nextBlockSize;
    int client_count = 0;
    int count = 0;
    void SendToClient1(/*QVector<*/QString/*>*/ str, int which_client/*, QDataStream &out,QByteArray &Data*/);
    void CalculateOnClients(int key);
    void CalculateOnServer(int key);
    std::string file_name_;
    QVector<QString> storage_;
//    std::vector<std::string> storage_;
//    QStringList tlist;
    QString client_ip_addresses_;
};

#endif // SERVER_H
