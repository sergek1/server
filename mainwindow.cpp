#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), server(new  Server) {

    ui->setupUi(this);
    connect(server, SIGNAL(signalSaveImage(/*QVector<*/QString/*>*/)), this, SLOT(probable_letter_slot(/*QVector<*/QString/*>*/)));
    connect(server, SIGNAL(signalClientInfo(QString)), this, SLOT(client_info_slot(QString)));
    set_server_ip();
}

void MainWindow::client_info_slot(QString client_info){
    ui->client_info->setText(client_info);
}

void MainWindow::probable_letter_slot(QString transformed_string) {
    std::cout<<"пришли сохранять"<<std::endl;
    QString qsave_output = QFileDialog::getSaveFileName(this, "Сохранить в файл ...", QDir::homePath() + "/" );
    std::ofstream out;
    out.open(qsave_output.toStdString());
    if (out.is_open()) {
//        for (int i = 0; i < transformed_string.size(); ++i) {
            out << transformed_string.toStdString();
//        }
    }
    out.close();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_open_file_clicked() {
    try {
        QString qfile_for_input = QFileDialog::getOpenFileName(this, "Выбрать файл");
        std::string file_for_input = qfile_for_input.toStdString();
        std::ifstream in(file_for_input);
        if (!in) throw std::invalid_argument("Файл для чтения не выбран");
        if (file_for_input.substr(file_for_input.size() - 4, 4) != ".txt")
            throw std::invalid_argument("Неверное расширение файла. Допустимое расширение: .txt");
        server->set_file(file_for_input);
        in.close();
        is_file_set = true;
        size_t index = file_for_input.find_last_of('/');
        ui->file_name->setText(QString::fromStdString(file_for_input.substr(index+1)));
    } catch (const std::exception &ex) {
        is_file_set = false;
        QMessageBox::warning(this, "Werror", ex.what());
    }
}


void MainWindow::on_start_clicked() {
    if (is_file_set){
        int key = ui->key->value(), task_type = ui->task_type->currentIndex(), machine_type = ui->machine_type->currentIndex();
        server->StartEncription(key, task_type, machine_type);
    } else {
        QMessageBox::warning(this, "Werror", "Файл не выбран");
    }
}

void MainWindow::set_server_ip() {
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost){
            ui->server_ip->setText(address.toString());
        }
    }
}

