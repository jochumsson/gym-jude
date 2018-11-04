#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include <QDebug>

LoginDialog::LoginDialog(QSqlDatabase & db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    m_db(db),
    m_login_ok(false)
{
    ui->setupUi(this);
    connect(ui->ok_button, SIGNAL(clicked(bool)), this, SLOT(login()));
    connect(ui->cancel_button, SIGNAL(clicked(bool)), this, SIGNAL(cancel()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::login()
{
    m_db.setHostName(ui->host_line_edit->text());
    m_db.setUserName(ui->user_name_line_edit->text());
    m_db.setPassword(ui->password_line_edit->text());
    m_login_ok = m_db.open();

    qDebug() << "Open databalse result: " << (m_login_ok ? "Success" : "Failed");
    qDebug() << "Available drivers: " << QSqlDatabase::drivers();

    if (m_login_ok)
    {
        emit loggedInOk();
        close();
    }
    else
    {
        ui->login_text_label->setText("Invalid Username or password");
    }
}
