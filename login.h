#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QWidget>
#include <QMessageBox>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    qint16 getChatPort();
    qint16 getFilePort();
    void login(qint16 chatPort, qint16 filePort);

private slots:
    void on_loginPushButton_clicked();

    void on_closePushButton_clicked();

private:
    Ui::Login *ui;

    qint16 chatPort;
    qint16 filePort;
};

#endif // LOGIN_H
