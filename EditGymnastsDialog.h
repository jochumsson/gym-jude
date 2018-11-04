#pragma once

#include "IGymnastModel.h"
#include "ICompetitionModel.h"
#include "IGymnastTableModel.h"
#include "ILevelTableModel.h"
#include <QDialog>

namespace Ui {
class EditGymnastsDialog;
}

class EditGymnastsDialog : public QDialog
{
    Q_OBJECT
    using super = QDialog;

public:
    explicit EditGymnastsDialog(
            const ICompetitionModelPtr & competition_model,
            const IGymnastModelPtr & gymnast_model,
            const IGymnastTableModelPtr & gymnast_table_model,
            const ILevelTableModelPtr & level_table_model,
            QWidget * parent = nullptr);
    virtual ~EditGymnastsDialog();

protected:
    void showEvent(QShowEvent *e) final;
    void keyPressEvent(QKeyEvent *key_event) final;

private slots:    
    void init_level_combo_box(bool);
    void init_gymnast_table_model(bool filter_by_level);
    void level_selection_changed();
    void current_gymnast_changed(const QModelIndex & current_index, const QModelIndex & previous_index);

    // button slots
    void apply_changes();
    void delete_gymnast();
    void add_gymnast();

private:
    void reinitialize();
    void init_level_combo_box();
    void init_gymnast_table_model();
    void set_gymnast_selection(const QModelIndex & index);
    void clear_gymnast_selection();

    Ui::EditGymnastsDialog * ui;

    ICompetitionModelPtr m_competition_model;
    IGymnastModelPtr m_gymnast_model;
    IGymnastTableModelPtr m_gymnast_table_model;
    ILevelTableModelPtr m_level_table_model;

};

