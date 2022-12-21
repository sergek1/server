#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <fstream>
#include <QMessageBox>
#include "server.h"
#include <iostream>
#include <QNetworkInterface>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void probable_letter_slot(QString transformed_string);
    void client_info_slot(QString client_info);
    void on_open_file_clicked();
    void on_start_clicked();

private:
    Ui::MainWindow *ui;
    Server *server;
    bool is_file_set = false;
   void set_server_ip();
};
#endif // MAINWINDOW_H
