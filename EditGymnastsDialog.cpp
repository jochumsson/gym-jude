#include "EditGymnastsDialog.h"
#include "ui_EditGymnastsDialog.h"

#include <QMessageBox>
#include <QUuid>
#include <QKeyEvent>
#include <QDebug>

EditGymnastsDialog::EditGymnastsDialog(
        const ICompetitionModelPtr & competition_model,
        const IGymnastModelPtr & gymnast_model,
        const IGymnastTableModelPtr & gymnast_table_model,
        const ILevelTableModelPtr & level_table_model,
        QWidget * parent) :
    QDialog(parent),
    ui(new Ui::EditGymnastsDialog),
    m_competition_model(competition_model),
    m_gymnast_model(gymnast_model),
    m_gymnast_table_model(gymnast_table_model),
    m_level_table_model(level_table_model)
{
    ui->setupUi(this);
    ui->gymnastListView->setModel(m_gymnast_table_model->get_qt_model());
    ui->levelComboBox->setModel(m_level_table_model->get_qt_model());

    // refresh actions
    connect(ui->levelComboBox, SIGNAL(currentIndexChanged(int)), SLOT(level_selection_changed()));
    connect(ui->filterByLevelCheckBox, SIGNAL(clicked(bool)), SLOT(init_level_combo_box(bool)));
    connect(ui->filterByLevelCheckBox, SIGNAL(clicked(bool)), SLOT(init_gymnast_table_model(bool)));
    connect(ui->gymnastListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            SLOT(current_gymnast_changed(const QModelIndex&, const QModelIndex&)));

    // button actions
    connect(ui->applyChangesPushButton, SIGNAL(clicked(bool)), SLOT(apply_changes()));
    connect(ui->deleteGymnastPushButton, SIGNAL(clicked(bool)), SLOT(delete_gymnast()));
    connect(ui->addGymnastPushButton, SIGNAL(clicked(bool)), SLOT(add_gymnast()));
    connect(ui->closePushButton, SIGNAL(clicked(bool)), SLOT(accept()));
}

EditGymnastsDialog::~EditGymnastsDialog()
{
    delete ui;
}

void EditGymnastsDialog::showEvent(QShowEvent *e)
{
    reinitialize();
    super::showEvent(e);
}

void EditGymnastsDialog::keyPressEvent(QKeyEvent *key_event)
{
    if (key_event->key() == Qt::Key_Return)
    {
        // if the return button is pressed then move the focus to the next sibling
        focusNextChild();
    }
}

void EditGymnastsDialog::init_level_combo_box(bool enabled)
{
    QString current_selection_text = ui->levelComboBox->currentText();

    m_level_table_model->refresh();
    ui->levelComboBox->setEnabled(enabled);

    if (not current_selection_text.isEmpty())
    {
        ui->levelComboBox->setCurrentText(current_selection_text);
    }
    else
    {
        ui->levelComboBox->setCurrentIndex(0);
    }
}

void EditGymnastsDialog::init_gymnast_table_model(bool filter_by_level)
{
    m_gymnast_table_model->set_filter_by_level(filter_by_level);
    m_gymnast_table_model->set_level(ui->levelComboBox->currentText().toInt());
    m_gymnast_table_model->refresh();    
}

void EditGymnastsDialog::level_selection_changed()
{
    init_gymnast_table_model();
}

void EditGymnastsDialog::current_gymnast_changed(const QModelIndex & current_index, const QModelIndex & previous_index)
{
    Q_UNUSED(previous_index);
    set_gymnast_selection(current_index);
}

void EditGymnastsDialog::apply_changes()
{
    GymnastInfo gymnast_update_info = {
        ui->gymnastIdLineEdit->text(),
        ui->gymnastNameLineEdit->text(),
        ui->gymnastClubNameLineEdit->text(),
        ui->gymnastLevelSpinBox->value(),
        ui->gymnastStartNumberSpinBox->value(),
        ui->gymnastTeamNameLineEdit->text()
    };

    QString error_str;
    bool update_ok = m_gymnast_model->update_gymnast(gymnast_update_info, error_str);
    if(update_ok)
    {
        // reinitialize since we might need to add / remove level
        init_level_combo_box();

        // move to right level selection
        ui->levelComboBox->setCurrentText(QString::number(gymnast_update_info.level));
    }
    else
    {
        QString warning_str = "Update failed with error: " + error_str;
        QMessageBox::warning(this, "Update Gymnast Error", warning_str);
    }
}

void EditGymnastsDialog::delete_gymnast()
{
    GymnastInfo selected_gymnast_info;
    const bool has_selection = m_gymnast_model->get_gymnast_info(selected_gymnast_info);

    if (not has_selection)
    {
        QMessageBox::warning(this, "Update Gymnast Error", "Delete user failed with error: selection has not been made");
        return;
    }

    QString error_str;
    const bool delete_ok = m_gymnast_table_model->delete_gymnast(selected_gymnast_info.id, error_str);

    if (not delete_ok)
    {
        QString warning_str = "Failed to delete user with error " + error_str;
        QMessageBox::warning(this, "Update Gymnast Error", warning_str);
        return;
    }

    QModelIndex current_index = ui->gymnastListView->selectionModel()->currentIndex();
    clear_gymnast_selection();
    init_level_combo_box();

    // restore selection
    ui->gymnastListView->setFocus();
    QModelIndex next_index = m_gymnast_table_model->get_qt_model()->index(current_index.row(), current_index.column());
    if (next_index.isValid())
    {
        ui->gymnastListView->setCurrentIndex(next_index);
        ui->gymnastListView->selectionModel()->select(next_index, QItemSelectionModel::SelectCurrent);
    }
}

void EditGymnastsDialog::add_gymnast()
{
    const int default_start_number = 0;
    QString error_log;
    const GymnastInfo new_gymnast_info = {
        QUuid::createUuid().toString(),
        "New Gymnast Name",
        "New Gymnast Club",
        ui->levelComboBox->currentText().toInt(),
        default_start_number,
        "New Gymnast Team"
    };
    const bool add_gymnast_ok = m_gymnast_table_model->add_gymnast(new_gymnast_info, error_log);
    if (add_gymnast_ok)
    {
        init_level_combo_box();
        ui->levelComboBox->setCurrentText(QString::number(new_gymnast_info.level));
        int gymnast_model_index = m_gymnast_table_model->get_gymnast_index(new_gymnast_info.id);
        QModelIndex gymnast_qt_model_index = m_gymnast_table_model->get_qt_model()->index(gymnast_model_index, 0);
        ui->gymnastListView->setCurrentIndex(gymnast_qt_model_index);
    }
    else
    {
        QString error_str = "Add gymnast failed with error: " + error_log;
        QMessageBox::warning(this, "Update Gymnast Error", error_str);
    }
}

void EditGymnastsDialog::reinitialize()
{
    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "Initializing EditGymantsDialog without competition selection";
        return;
    }

    m_level_table_model->set_competition(competition_info.name);
    init_level_combo_box();

    m_gymnast_table_model->set_competition(competition_info.name);
    init_gymnast_table_model();

    m_gymnast_model->set_competition(competition_info.name);
    clear_gymnast_selection();

    if (competition_info.competition_type.has_level)
    {
        ui->filterByLevelCheckBox->show();
        ui->levelComboBox->show();
        ui->gymnastLevelLabel->show();
        ui->gymnastLevelSpinBox->show();
    }
    else
    {
        ui->filterByLevelCheckBox->hide();
        ui->levelComboBox->hide();
        ui->gymnastLevelLabel->hide();
        ui->gymnastLevelSpinBox->hide();
    }

}

void EditGymnastsDialog::init_level_combo_box()
{
    init_level_combo_box(ui->filterByLevelCheckBox->isChecked());
}

void EditGymnastsDialog::init_gymnast_table_model()
{
    init_gymnast_table_model(ui->filterByLevelCheckBox->isChecked());
}

void EditGymnastsDialog::set_gymnast_selection(const QModelIndex & index)
{
    QString gymnast_id = m_gymnast_table_model->get_gymnast_id(index.row());
    QString error_str;
    bool gymnast_selection_ok = m_gymnast_model->set_gymnast(gymnast_id, error_str);
    if(not gymnast_selection_ok)
    {
        qWarning() << "Gymnast model failed to select gymnast with id " << gymnast_id;
        return;
    }

    GymnastInfo gymnast_info;

    if (not m_gymnast_model->get_gymnast_info(gymnast_info))
    {
        qWarning() << "Failed to retrieve gymnast info from gymnast model";
        return;
    }

    ui->gymnastIdLineEdit->setText(gymnast_info.id);
    ui->gymnastNameLineEdit->setText(gymnast_info.name);
    ui->gymnastClubNameLineEdit->setText(gymnast_info.club);
    ui->gymnastTeamNameLineEdit->setText(gymnast_info.team);
    ui->gymnastLevelSpinBox->setValue(gymnast_info.level);
    ui->gymnastStartNumberSpinBox->setValue(gymnast_info.start_nr);
}

void EditGymnastsDialog::clear_gymnast_selection()
{
    ui->gymnastIdLineEdit->setText("");
    ui->gymnastNameLineEdit->setText("");
    ui->gymnastClubNameLineEdit->setText("");
    ui->gymnastTeamNameLineEdit->setText("");
    ui->gymnastLevelSpinBox->setValue(0);
    ui->gymnastStartNumberSpinBox->setValue(0);
}

