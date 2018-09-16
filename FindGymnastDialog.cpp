#include "FindGymnastDialog.h"
#include "ui_FindGymnastDialog.h"

FindGymnastDialog::FindGymnastDialog(const IFindGymnastItemModelPtr &find_gymnast_model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindGymnastDialog),
    m_find_gymnast_model(find_gymnast_model)
{
    ui->setupUi(this);
    ui->findGymnastTableView->setModel(m_find_gymnast_model->get_qt_model());

    connect(ui->gymnastNameLineEdit, SIGNAL(textChanged(QString)), SLOT(search_key_changed(QString)));
}

FindGymnastDialog::~FindGymnastDialog()
{
    delete ui;
}

void FindGymnastDialog::search_key_changed(const QString & search_key)
{
    m_find_gymnast_model->set_search_name(search_key);
    ui->findGymnastTableView->resizeColumnsToContents();
    if (m_find_gymnast_model->get_qt_model()->rowCount() > 0)
    {
        ui->findGymnastTableView->selectRow(0);
    }
}
