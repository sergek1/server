#include "server.h"
#include <iostream>
#include <QNetworkInterface>
#include <QFile>
#include <limits>
Server::Server() {
    if (this->listen(QHostAddress::Any, 2323)){
        qDebug()<< "start";
    } else {
        qDebug()<< "error";
    }
    qDebug()<<"ip port = "<<this->serverPort();
    nextBlockSize = 0;
}

void Server::set_file(std::string file_name) {
    file_name_ = file_name;
}

void Server::StartEncription(int key, int task_type, int machine_type) {
    storage_.clear();
    storage_.reserve(10000);
    client_count = 0;
    Data.clear();
    transformed_strings_vector.clear();
    if (task_type == 1) {
        key*=(-1);
    }
    if(machine_type == 1) {
        CalculateOnClients(key);
    } else if (machine_type == 2) {
        CalculateOnServer(key);
    } else {
        CalculateOnClients(key);
        CalculateOnServer(key);
    }
}

void Server::SendToClient1(/*QVector<*/QString/*>*/ str, int which_client){
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out <<quint64(0)<<str;
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    std::cout<<""<<std::endl;
    Sockets[which_client]->write(Data);
}

void Server::CalculateOnClients(int key) {

//    QFile inputFile(file_name_);
//    if (inputFile.open(QIODevice::ReadOnly))
//    {
//      QTextStream in(&inputFile);
//      QString all = in.readAll();
//      ui -> plainTextEdit->setPlainText(all);
//    }

    std::ifstream in(file_name_);
    std::string key_string = std::to_string(key);
    QString qkey = QString::fromStdString(key_string);
    while (in) {
        std::string line;
        getline(in, line);
        QString qline =  QString::fromStdString(line);
        qline+="\n";
        storage_.push_back(qline);
    }
    in.close();
    int load_on_clients = storage_.size() / Sockets.size();
    qDebug()<<"количество клиентов = "<<Sockets.size();
    int count_ = 0;
    // разбиваем хранилище на несколько частей и раздаем клиентам
    for (int i = 0; i < Sockets.size(); ++i){
        /*QVector<*/QString/*>*/ vector_for_client{};
        vector_for_client+=qkey;
        for (int j = 0; j < load_on_clients; ++j) {
            vector_for_client+=/*.push_back*/storage_[count_];
            ++count_;
        }
        SendToClient1(vector_for_client, i);
    }
}

void Server::CalculateOnServer(int key) {
    std::ifstream in(file_name_);
    QVector<QString> strings;
    std::string common_string;
    while (in) {
        std::string line;
        getline(in, line);
        line+="\n";
        for(size_t i = 0; i < line.size(); ++i){
            int temp = (line[i] + key);
            if (temp > 127) {
                temp -= 127;
            }
            line[i] = (char)temp;
        }
        strings.push_back(QString::fromStdString(line));
    }
    in.close();
//    emit signalSaveImage(strings);
}




void Server::incomingConnection(qintptr socketDescriptor) {
    count++;
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    // для каждого клиента создаем сокет и сохраняем в вектор
    Sockets.push_back(socket);
    std::string client = socket->peerAddress().toString().toStdString();
    std::string substr = client.substr(client.find_last_of(':')+1);

    client_ip_addresses_+=QString::number(count);
    client_ip_addresses_+=") ip адрес: ";
    client_ip_addresses_+="<font color='red'>"+QString::fromStdString(substr) +"</font>" + " id: ";
    client_ip_addresses_+="<font color='blue'>"+QString::number(socketDescriptor)+"</font> \n";
    client_ip_addresses_+="<br></br>";
    emit signalClientInfo(client_ip_addresses_);
    qDebug() << "ipaddress = "<<client_ip_addresses_;
    qDebug() << "client connеcted"<< socketDescriptor;

}

void Server::slotReadyRead(){
    socket = (QTcpSocket* ) sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_4);
    if (in.status() == QDataStream::Ok) {
        qDebug()<<"read...";
        for(;;) {
            if (nextBlockSize == 0) {
                qDebug() << "nextBlockSize = 0";
                if(socket->bytesAvailable()<2){
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize){
                qDebug() << "Data not full, break";
                break;
            }
            QString transformed_string;
            in >> transformed_string;
            nextBlockSize=0;
            transformed_strings_vector += transformed_string;
            std::cout<<"сайз получили обратную связь "<<transformed_string.size()<<std::endl;
            client_count++;
            //            emit signalSaveImage(transformed_string);
//            nextBlockSize = 0;
//            //    for (size_t i = 0; i < threads.size(); ++i) {
//            //        threads[i].join();
//            //    }
            break;
        }
//        qDebug()<<"Sockets.size() = "<<Sockets.size()<<" client_count = "<<client_count;
        if (client_count == Sockets.size() ){
            emit signalSaveImage(transformed_strings_vector);
        }

    } else {
        qDebug()<<"DataStream error";
    }

}



