#pragma once

#include "ICompetitionModel.h"
#include "ICompetitionTableModel.h"
#include <QDialog>

namespace Ui {
class EditCompetitionsDialog;
}

class EditCompetitionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCompetitionsDialog(
            const ICompetitionModelPtr & competition_model,
            const ICompetitionTableModelPtr & competition_table_model,
            QWidget *parent = nullptr);
    ~EditCompetitionsDialog();

protected:
    void showEvent(QShowEvent *e) final;

private slots:
    void competition_selected(const QModelIndex & index);
    void add_competition();
    void delete_competition();
    void apply_changes();

private:
    void set_selection(const CompetitionInfo & competition_data);

    Ui::EditCompetitionsDialog * ui;
    ICompetitionModelPtr m_competition_model;
    ICompetitionTableModelPtr m_competition_table_model;

};
