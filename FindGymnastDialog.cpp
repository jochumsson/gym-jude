#include "FindGymnastDialog.h"
#include "ui_FindGymnastDialog.h"
#include <QKeyEvent>

FindGymnastDialog::FindGymnastDialog(const IFindGymnastItemModelPtr &find_gymnast_model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindGymnastDialog),
    m_find_gymnast_model(find_gymnast_model)
{
    ui->setupUi(this);
    ui->findGymnastTableView->setModel(m_find_gymnast_model->get_qt_model());
    ui->gymnastNameLineEdit->installEventFilter(this);

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

bool FindGymnastDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->gymnastNameLineEdit &&
            event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Up)
        {
            int current_row = m_find_gymnast_model->move_selection_up();
            ui->findGymnastTableView->selectRow(current_row);
        }
        else if (keyEvent->key() == Qt::Key_Down)
        {
            int current_row = m_find_gymnast_model->move_selection_down();
            ui->findGymnastTableView->selectRow(current_row);
        }
    }

    return false;
}
