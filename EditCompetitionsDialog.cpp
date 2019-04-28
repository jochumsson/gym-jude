#include "EditCompetitionsDialog.h"
#include "ui_EditCompetitionsDialog.h"
#include <QMessageBox>
#include <QDebug>

EditCompetitionsDialog::EditCompetitionsDialog(
        const ICompetitionModelPtr & competition_model,
        const ICompetitionTableModelPtr & competition_table_model,
        const std::shared_ptr<ICompetitionTypeModel> & competition_type_model,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCompetitionsDialog),
    m_competition_model(competition_model),
    m_competition_table_model(competition_table_model),
    m_competition_type_model(competition_type_model)
{
    ui->setupUi(this);
    ui->competitionsListView->setModel(m_competition_table_model->get_qt_model());
    ui->competitionTypeComboBox->setModel(m_competition_type_model->get_qt_model());

    connect(ui->closePushButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->competitionsListView, SIGNAL(clicked(QModelIndex)), SLOT(competition_selected(QModelIndex)));
    connect(ui->addCompetitionPushButton, SIGNAL(clicked(bool)), SLOT(add_competition()));
    connect(ui->deleteCompetitionPushButton, SIGNAL(clicked(bool)), SLOT(delete_competition()));
    connect(ui->applyPushButton, SIGNAL(clicked(bool)), SLOT(apply_changes()));
}

EditCompetitionsDialog::~EditCompetitionsDialog()
{
    delete ui;
}

void EditCompetitionsDialog::showEvent(QShowEvent *e)
{
    m_competition_table_model->refresh();
    m_competition_type_model->refresh();
    if (ui->competitionTypeComboBox->count() > 0)
    {
        ui->competitionTypeComboBox->setCurrentIndex(0);
    }
    QDialog::showEvent(e);
}

void EditCompetitionsDialog::competition_selected(const QModelIndex & index)
{
    if (not index.isValid()) return;

    QString selected_competition = m_competition_table_model->get_competition_name(index.row());
    QString error_str;
    if (m_competition_model->set_competition(selected_competition, error_str))
    {
        CompetitionInfo selected_competition_info;
        if (m_competition_model->get_competition_info(selected_competition_info))
        {
            set_selection(selected_competition_info);
        }
        else
        {
            qWarning() << "Failed to get competition info for selected competition " << selected_competition;
        }
    }
    else
    {
        qWarning() << "Failed to set current competition ["
                   << selected_competition
                   << "] with error: "
                   << error_str;
    }
}

void EditCompetitionsDialog::set_selection(const CompetitionInfo & competition_data)
{
    ui->competitionNameLineEdit->setText(competition_data.name);
    ui->competitionDateLineEdit->setDate(competition_data.date);
    ui->competitionTypeComboBox->setCurrentText(competition_data.competition_type.name);
    ui->teamCompetitionCheckBox->setChecked(competition_data.team_competition);
    ui->closedCheckBox->setChecked(competition_data.closed);
}

void EditCompetitionsDialog::add_competition()
{
    QString error;
    const bool add_ok =
            m_competition_table_model->add_competition(
    CompetitionInfo{"New Competition Name",
                    QDate::currentDate(),
                    CompetitionType{ui->competitionTypeComboBox->currentText(), false, false},
                    false,
                    false},
                error);
    if (not add_ok)
    {
        qWarning() << "Failed to create new competition, error: " << error;
    }
}

void EditCompetitionsDialog::delete_competition()
{
    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "Failed to delete competition with error: no competition selected";
    }
    else
    {
        QString error;
        if (not m_competition_table_model->delete_competition(competition_info.name, error))
        {
            qWarning() << "Failed to delete competition, error: " << error;
            QMessageBox::warning(
                        this,
                        "Failed to delete competition",
                        "Failed to delete competition.\n"
                        "Notice that all gymnasts need to be deleted from the competition before it can be deleted.\n"
                        "SQL error: " + error);

        }
    }
}

void EditCompetitionsDialog::apply_changes()
{
    const QString & competition_name = ui->competitionNameLineEdit->text();
    const QDate & competition_date = ui->competitionDateLineEdit->date();
    const QString & competition_type = ui->competitionTypeComboBox->currentText();
    const bool team_competition = ui->teamCompetitionCheckBox->isChecked();
    const bool closed = ui->closedCheckBox->isChecked();

    QString error_str;
    if (m_competition_model->update_competition(competition_name, competition_date, competition_type, team_competition, closed, error_str))
    {
        m_competition_table_model->refresh();
    }
    else
    {
        qWarning() << "Failed to update competition, error: " << error_str;
        QMessageBox::warning(
                    this,
                    "Failed to update competition",
                    "Failed to update competition.\n"
                    "sql error: " + error_str);
    }
}
