#pragma once

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QSqlDatabase & db, QWidget *parent = nullptr);
    ~LoginDialog();

    bool loggedIn() const
    {
        return m_login_ok;
    }

signals:
    void loggedInOk();
    void cancel();

public slots:
    void login();

private:
    Ui::LoginDialog *ui;
    QSqlDatabase & m_db;
    bool m_login_ok;

};

