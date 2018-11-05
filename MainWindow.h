#pragma once

#include "ICompetitionModel.h"
#include "ICompetitionTableModel.h"
#include "IGymnastModel.h"
#include "IGymnastTableModel.h"
#include "ILevelTableModel.h"
#include "IApparatusTableModel.h"
#include "IJudgeTableModel.h"
#include "IJudgementModel.h"
#include "IScoreTableModel.h"
#include "IResultTypeModel.h"
#include "IResultItemModel.h"
#include "ITeamResultItemModel.h"
#include "IRawDataModel.h"
#include "GymnastResults.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QAction>
#include <boost/optional.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    struct TabInfo
    {
        enum class GymTab
        {
            JudgeTabIndex = 0,
            ResultsTabIndex,
            TeamResultsTabIndex,
            RawDataTabIndex,
            NumbersOfTabs,
        };

        static int get_tab_index(GymTab tab, bool team_competition)
        {
            switch(tab)
            {
            case GymTab::JudgeTabIndex:
                return 0;
            case GymTab::ResultsTabIndex:
                return 1;
            case GymTab::TeamResultsTabIndex:
                return (team_competition ? 2 : -1);
            case GymTab::RawDataTabIndex:
                return (team_competition ? 3 : 2);
            case GymTab::NumbersOfTabs:
                return (team_competition ? 4 : 3);
            };
            return -1;
        }

        static GymTab get_tab_with_index(int index, bool team_competition)
        {
            switch(index)
            {
            case 0:
                return GymTab::JudgeTabIndex;
            case 1:
                return GymTab::ResultsTabIndex;
            case 2:
                return (team_competition ? GymTab::TeamResultsTabIndex : GymTab::RawDataTabIndex);
            case 3:
                return (team_competition ? GymTab::RawDataTabIndex : GymTab::NumbersOfTabs);
            }
            return GymTab::NumbersOfTabs;
        }

        static int get_number_of_tabs(bool team_competition)
        {
            return get_tab_index(GymTab::NumbersOfTabs, team_competition);
        }
    };

public:
    explicit MainWindow(
            const ICompetitionModelPtr & competition_model,
            const ICompetitionTableModelPtr & competition_table_model,
            const IGymnastModelPtr & gymnast_model,
            const IGymnastTableModelPtr & gymnast_table_model,
            const ILevelTableModelPtr & level_table_model,
            const IApparatusTableModelPtr & apparatus_table_model,
            const IJudgeTableModelPtr & judge_table_model,
            const IJudgementModelPtr & judgement_model,
            const IScoreTableModelPtr & score_table_model,
            const IResultTypeModelPtr & result_type_mode,
            const IResultItemModelPtr & result_table_model,
            const ITeamResultItemModelPtr & team_result_item_model,
            const IRawDataModelPtr & raw_data_model,
            QWidget * parent = nullptr);
    ~MainWindow();

    void set_selection(const QString & competition_name,
            const QString & gymnast_id,
            const QString & apparatus,
            boost::optional<int> level = boost::none);

signals:
    void open_edit_competitions_dialog();
    void open_edit_competition_gymnasts_dialog();
    void open_import_gymnasts_dialog();
    void open_find_gymnast_dialog();

public slots:
    void initialize();

private slots:
    // tab updates
    void update_score_tab();
    void update_results_tab();
    void update_team_results_tab();
    void update_raw_data_tab();

    // slot for showing pop up menus
    void results_menu_requested(QPoint);
    void team_results_menu_requested(QPoint);

    // menu related slots
    void print_results();
    void print_team_results();
    void export_results();
    void export_team_results();

    // user triggered gui activity
    void current_tab_changed(int current_index);
    void competition_changed();
    void show_closed_competitions_changed();
    void three_judges_changed();
    void judge_1_selection_changed();
    void judge_2_selection_changed();
    void judge_3_selection_changed();
    void apparatus_changed();
    void score_level_changed();
    void results_level_changed();
    void show_score_details_changed();
    void result_type_changed();
    void publish_results();
    void remove_publication();

private:
    void init_competition_selection();
    void init_score_tab();
    void init_results_tab();
    void init_current_tab(TabInfo::GymTab current_tab);
    void init_enabled_state();

    //gui update
    void update_judge_gui();
    void update_results_publish_gui();

    void print_table_model(QAbstractItemModel *item_model);

    Ui::MainWindow * ui;

    ICompetitionModelPtr m_competition_model;
    ICompetitionTableModelPtr m_competition_table_model;
    IGymnastModelPtr m_gymnast_model;
    IGymnastTableModelPtr m_gymnast_table_model;
    ILevelTableModelPtr m_level_table_model;
    IApparatusTableModelPtr m_apparatus_table_model;
    IJudgeTableModelPtr m_judge_table_model;
    IJudgementModelPtr m_judgement_model;
    IScoreTableModelPtr m_score_table_model;
    IResultTypeModelPtr m_result_type_model;
    IResultItemModelPtr m_result_item_model;
    ITeamResultItemModelPtr m_team_result_item_model;
    IRawDataModelPtr m_raw_data_model;

    //menu actions
    QAction *m_action_print_results = new QAction("Print Results", this);
    QAction *m_action_export_results = new QAction("Export Results", this);
    QAction *m_action_print_team_results = new QAction("Print Team Results", this);
    QAction *m_action_export_team_results = new QAction("Export Team Results", this);
};

