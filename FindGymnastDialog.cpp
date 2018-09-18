#include "FindGymnastDialog.h"
#include "ui_FindGymnastDialog.h"
#include <QKeyEvent>
#include <QDebug>

FindGymnastDialog::FindGymnastDialog(const IFindGymnastItemModelPtr & find_gymnast_model, MainWindow & main_window) :
    QDialog(&main_window),
    ui(new Ui::FindGymnastDialog),
    m_find_gymnast_model(find_gymnast_model),
    m_main_window(main_window)
{
    ui->setupUi(this);
    ui->findGymnastTableView->setModel(m_find_gymnast_model->get_qt_model());
    ui->gymnastNameLineEdit->installEventFilter(this);

    connect(ui->gymnastNameLineEdit, SIGNAL(textChanged(QString)), SLOT(search_key_changed(QString)));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(selection_accept()));
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

void FindGymnastDialog::selection_accept()
{
    const QModelIndex & current_index = ui->findGymnastTableView->currentIndex();
    FindGymnastItem gymnast_item = m_find_gymnast_model->get_selection(current_index.row());
    qDebug() << "Select gymanst: "
             << gymnast_item.gymanst_name << ", "
             << gymnast_item.competition_name << ", "
             << gymnast_item.apparatus << ", "
             << ((gymnast_item.level) ? *gymnast_item.level : -1);

    m_main_window.set_selection(
                gymnast_item.competition_name,
                gymnast_item.gymnast_id,
                gymnast_item.apparatus,
                gymnast_item.level);
}
