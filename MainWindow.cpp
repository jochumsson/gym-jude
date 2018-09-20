#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScoreItemDelegate.h"
#include "ScoreFocusNavigator.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQueryModel>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QItemDelegate>
#include <stdexcept>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QErrorMessage>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QUuid>
#include <QModelIndex>
#include <QDebug>
#include <map>
#include <functional>

MainWindow::MainWindow(
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
        QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_competition_model(competition_model),
    m_competition_table_model(competition_table_model),
    m_gymnast_model(gymnast_model),
    m_gymnast_table_model(gymnast_table_model),
    m_level_table_model(level_table_model),
    m_apparatus_table_model(apparatus_table_model),
    m_judge_table_model(judge_table_model),
    m_judgement_model(judgement_model),
    m_score_table_model(score_table_model),
    m_result_type_model(result_type_mode),
    m_result_item_model(result_table_model),
    m_team_result_item_model(team_result_item_model),
    m_raw_data_model(raw_data_model)
{
    ui->setupUi(this);
    ui->score_splitter->setSizes({20, 400});
    ui->results_splitter->setSizes({20, 600});
    ui->publish_results_status_label->setAutoFillBackground(true);

    //connect modules to views
    ui->competition_combo_box->setModel(m_competition_table_model->get_qt_model());
    ui->level_combo_box->setModel(m_level_table_model->get_qt_model());
    ui->results_level_combo_box->setModel(m_level_table_model->get_qt_model());
    ui->apparatus_combo_box->setModel(m_apparatus_table_model->get_qt_model());
    ui->judge_1_combo_box->setModel(m_judge_table_model->get_qt_model());
    ui->judge_2_combo_box->setModel(m_judge_table_model->get_qt_model());
    ui->judge_3_combo_box->setModel(m_judge_table_model->get_qt_model());
    ui->gymnast_table_view->setModel(m_score_table_model->get_qt_model());
    ui->results_type_comboBox->setModel(m_result_type_model->get_qt_model());
    ui->results_table_view->setModel(m_result_item_model->get_qt_model());
    ui->team_results_table_view->setModel(m_team_result_item_model->get_qt_model());
    ui->raw_data_table_view->setModel(m_raw_data_model->get_qt_model());

    //update gui on user input
    connect(ui->tab_widget, SIGNAL(currentChanged(int)), this, SLOT(current_tab_changed(int)));
    connect(ui->three_judges_check_box, SIGNAL(toggled(bool)), this, SLOT(three_judges_changed()));
    connect(ui->judge_1_combo_box, SIGNAL(currentIndexChanged(int)), SLOT(judge_1_selection_changed()));
    connect(ui->judge_2_combo_box, SIGNAL(currentIndexChanged(int)), SLOT(judge_2_selection_changed()));
    connect(ui->judge_3_combo_box, SIGNAL(currentIndexChanged(int)), SLOT(judge_3_selection_changed()));
    connect(ui->showClosedCompetitionsCheckBox, SIGNAL(clicked(bool)), SLOT(show_closed_competitions_changed()));
    connect(ui->competition_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(competition_changed()));
    connect(ui->apparatus_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(apparatus_changed()));
    connect(ui->level_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(score_level_changed()));
    connect(ui->results_level_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(results_level_changed()));
    connect(ui->score_details_check_box, SIGNAL(stateChanged(int)), this, SLOT(show_score_details_changed()));
    connect(ui->results_type_comboBox, SIGNAL(currentIndexChanged(int)), SLOT(result_type_changed()));
    connect(ui->publish_results_pushButton, SIGNAL(clicked(bool)), SLOT(publish_results()));
    connect(ui->remove_publication_pushButton, SIGNAL(clicked(bool)), SLOT(remove_publication()));

    //menu actions
    connect(ui->actionEditCompetitions, SIGNAL(triggered(bool)), this, SIGNAL(open_edit_competitions_dialog()));
    connect(ui->actionImport, SIGNAL(triggered(bool)), this, SLOT(import_gymnast_list()));
    connect(ui->actionEditCompetitionGymnasts, SIGNAL(triggered(bool)), SIGNAL(open_edit_competition_gymnasts_dialog()));
    connect(ui->actionFindGymnast, SIGNAL(triggered(bool)), SIGNAL(open_find_gymnast_dialog()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));

    //results menu
    ui->results_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->results_table_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(results_menu_requested(QPoint)));
    connect(m_action_print_results, SIGNAL(triggered(bool)), SLOT(print_results()));
    connect(m_action_export_results, SIGNAL(triggered(bool)), SLOT(export_results()));

    //team results menu
    ui->team_results_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->team_results_table_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(team_results_menu_requested(QPoint)));
    connect(m_action_print_team_results, SIGNAL(triggered(bool)), SLOT(print_team_results()));
    connect(m_action_export_team_results, SIGNAL(triggered(bool)), SLOT(export_team_results()));

    //score item delegate
    ui->gymnast_table_view->setItemDelegate(new ScoreItemDelegate(score_table_model, ui->gymnast_table_view));
    ui->gymnast_table_view->installEventFilter(new ScoreFocusNavigator(score_table_model, ui->gymnast_table_view));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_selection(
        const QString & competition_name,
        const QString & gymnast_id,
        const QString & apparatus,
        boost::optional<int> level)
{
    ui->competition_combo_box->setCurrentText(competition_name);

    CompetitionInfo selected_competition_info;
    if (m_competition_model->get_competition_info(selected_competition_info))
    {
        ui->tab_widget->setCurrentIndex(
                    TabInfo::get_tab_index(TabInfo::GymTab::JudgeTabIndex,
                                           selected_competition_info.team_competition));
    }

    const int & apparatus_index = m_apparatus_table_model->get_text_index(apparatus);
    if (apparatus_index > -1)
    {
        ui->apparatus_combo_box->setCurrentIndex(apparatus_index);
    }
    if (level)
    {
        ui->level_combo_box->setCurrentText(QString::number(*level));
    }
    const int & gymanst_index = m_score_table_model->get_gymnast_index(gymnast_id);
    if (gymanst_index > -1)
    {
        ui->gymnast_table_view->setFocus();
        ui->gymnast_table_view->selectRow(gymanst_index);
    }
}


void MainWindow::initialize()
{
    init_competition_selection();
    init_score_tab();
    init_results_tab();
    m_raw_data_model->refresh();

    showMaximized();
}

void MainWindow::update_score_tab()
{
    update_judge_gui();

    try {
        const JudgementInfo & judgement = m_judgement_model->get_current_judgement_info();
        const bool three_judges = (judgement.number_of_judges > 2);
        ui->three_judges_check_box->setChecked(three_judges);
        ui->judge_1_combo_box->setCurrentText(judgement.judges[0]);
        ui->judge_2_combo_box->setCurrentText(judgement.judges[1]);
        ui->judge_3_combo_box->setCurrentText(judgement.judges[2]);
    }
    catch(std::runtime_error & e)
    {
        qWarning() << "Failed to update judgement tab with error: " << e.what();
    }

    m_score_table_model->refresh();
}

void MainWindow::update_results_tab()
{    
    if (ui->results_level_combo_box->currentIndex() < 0)
        ui->results_level_combo_box->setCurrentIndex(0);
    if (ui->results_type_comboBox->currentIndex() < 0)
        ui->results_type_comboBox->setCurrentIndex(0);

    m_result_item_model->refresh();
    update_results_publish_gui();
}

void MainWindow::update_team_results_tab()
{
    m_team_result_item_model->refresh();
}

void MainWindow::update_raw_data_tab()
{
    m_raw_data_model->refresh();
}

void MainWindow::results_menu_requested(QPoint pos)
{
    QMenu menu(ui->results_table_view);
    menu.addAction(m_action_print_results);
    menu.addAction(m_action_export_results);
    menu.exec(ui->results_table_view->mapToGlobal(pos));
}

void MainWindow::team_results_menu_requested(QPoint pos)
{
    QMenu menu(ui->team_results_table_view);
    menu.addAction(m_action_print_team_results);
    menu.addAction(m_action_export_team_results);
    menu.exec(ui->team_results_table_view->mapToGlobal(pos));
}

void MainWindow::import_gymnast_list()
{
    QFileDialog dialog(this);

    QStringList filters;
    filters << "Participient files (*.csv)";
    dialog.setNameFilters(filters);

    dialog.setFileMode(QFileDialog::ExistingFile);

    QString error_log;
    if(dialog.exec()) {
        QString file_name = dialog.selectedFiles()[0];
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream file_stream(&file);
        int line_nr = 0;
        while (!file_stream.atEnd())
        {
            ++line_nr;

            QString line_str = file_stream.readLine();
            QStringList line_split = line_str.split(QRegExp("[,;-]"));
            if (line_split.size() != 4 && line_split.size() != 5)
            {
                QString error_string;
                QTextStream error_stream(&error_string);
                error_stream << "Invalid participant csv file, error: line "
                             << line_nr
                             << " does not contain 8 or 9 fields as expected";
                QMessageBox msg_box;
                msg_box.setText(error_string);
                msg_box.setStandardButtons(QMessageBox::Ok);
                msg_box.exec();
                return;
            }
            else if(line_split[0].toLower() == "name" ||
                    line_split[0].toLower() == "gymnast")
            {
                //ignore header
                continue;
            }
            else
            {
                GymnastInfo gymnast_info = {
                    QUuid::createUuid().toString(),
                    line_split[0],
                    line_split[1],
                    line_split[2].toInt(),
                    line_split[3].toInt(),
                    (line_split.size() > 4) ? line_split[4].trimmed() : QString(),
                };
                m_gymnast_table_model->add_gymnast(gymnast_info, error_log);
            }
        }

        QMessageBox msg_box;
        if (error_log.isEmpty())
        {
            msg_box.setText("Data imported without errors");
        }
        else
        {
            msg_box.setText(error_log);
        }
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.exec();
     }

    // reinitialize all views after import
    initialize();
}

void MainWindow::print_results()
{
    print_table_model(ui->results_table_view->model());
}

void MainWindow::print_team_results()
{
    print_table_model(ui->team_results_table_view->model());
}

void MainWindow::export_results()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Participient files (*.csv)"));
    dialog.setDirectory(QDir::home());
    const bool file_dialog_open = dialog.exec();

    if (!file_dialog_open) {
        return;
    }

    QString file_name = dialog.selectedFiles()[0];
    QFile file(file_name);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox msg_box;
        msg_box.setText("Failed to open file for writing");
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.exec();
        return;
    }

    QTextStream stream(&file);
    bool first_line = true;

    IResultsCalculator::ResultsMap current_results;
    try {
        current_results = m_result_item_model->get_current_results();
    }
    catch(IncompleteResults & e) {
        QMessageBox msg_box;
        msg_box.setText(QString("Failed to export results, error: ") + e.what());
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.exec();
        return;
    }

    for(auto result_it = current_results.begin(); result_it != current_results.end(); ++result_it)
    {
        if(first_line)
        {
            stream << "Name," << "Club," << "Results";
            for(auto app_it = result_it->second.apparatus_score.begin();
                app_it != result_it->second.apparatus_score.end();
                ++app_it)
            {
                stream << "," << (*app_it).first;
                if ((*app_it).second.has_cop_score)
                {
                    stream << "," << (*app_it).first + " D Score";
                    stream << "," << (*app_it).first + " D Penalty";
                }
            }
            stream << "\n";
            first_line = false;
        }


        stream << result_it->second.gymnast_name;
        stream << ",";
        stream << result_it->second.gymnast_club;
        stream << ", ";
        stream << result_it->second.final_results;
        for(auto app_it = result_it->second.apparatus_score.begin();
            app_it != result_it->second.apparatus_score.end();
            ++app_it)
        {
            stream <<"," << (*app_it).second.final_score;
            if ((*app_it).second.has_cop_score)
            {
                stream <<"," << (*app_it).second.d_score;
                stream <<"," << (*app_it).second.d_penalty;
            }
        }
        stream << "\n";
    }

    file.close();
}

void MainWindow::export_team_results()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Participient files (*.csv)"));
    dialog.setDirectory(QDir::home());
    if (dialog.exec())
    {
        QString file_name = dialog.selectedFiles()[0];
        QFile file(file_name);
        if (!file.open(QIODevice::ReadWrite))
        {
            QMessageBox msg_box;
            msg_box.setText("Failed to open file for writing");
            msg_box.setStandardButtons(QMessageBox::Ok);
            msg_box.exec();
            return;
        }

        ITeamResultItemModel::TeamResultsMap current_team_results;
        try {
            current_team_results = m_team_result_item_model->get_current_results();
        }
        catch (IncompleteTeamResults & e)
        {
            qWarning() << "Failed to export team results with error: " << e.what();
            return;
        }

        QTextStream stream(&file);
        bool first_line = true;
        for(auto result_it = current_team_results.begin(); result_it != current_team_results.end(); ++result_it)
        {
            if(first_line)
            {
                stream << "Team Name," << "Club," << "Results";
                for(auto app_it = result_it->second.apparatus_score.begin();
                    app_it != result_it->second.apparatus_score.end();
                    ++app_it)
                {
                    stream << "," << app_it->first;
                }
                stream << "\n";
                first_line = false;
            }

            stream << result_it->second.team_name.trimmed();
            stream << ",";
            stream << result_it->second.team_club.trimmed();
            stream << ",";
            stream << result_it->second.final_score;
            for(auto app_it = result_it->second.apparatus_score.begin();
                app_it != result_it->second.apparatus_score.end();
                ++app_it)
            {
                stream <<"," << app_it->second;
            }
            stream << "\n";
        }

        file.close();
    }
}

void MainWindow::current_tab_changed(int current_index)
{
    CompetitionInfo current_competition;
    if (not m_competition_model->get_competition_info(current_competition))
    {
        qDebug() << "Failed to update tab with no current competition selection";
        return;
    }

    auto tab_index = TabInfo::get_tab_with_index(current_index, current_competition.team_competition);
    init_current_tab(tab_index);
}

void MainWindow::competition_changed()
{
    // no current selection
    if (ui->competition_combo_box->currentIndex() < 0) return;

    const QString & selected_competition = ui->competition_combo_box->currentText();
    if (selected_competition.isEmpty()) return; // invalid selection

    QString error;
    if (not m_competition_model->set_competition(selected_competition, error))
    {
        qWarning() << "Failed to set current competition ["
                   << selected_competition
                   << "] with error: "
                   << error;
        return;
    }

    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "Failed to get competition info with ["
                   << selected_competition
                   << "]";
        return;
    }

    // show hide team competition tab
    const int current_number_of_tabs = TabInfo::get_number_of_tabs(competition_info.team_competition);
    if (ui->tab_widget->count() < current_number_of_tabs)
    {
        ui->tab_widget->insertTab(
                    TabInfo::get_tab_index(TabInfo::GymTab::TeamResultsTabIndex, true),
                    ui->team_results_tab,
                    "Team Results");
    }
    else if (ui->tab_widget->count() > current_number_of_tabs)
    {
        ui->tab_widget->removeTab(TabInfo::get_tab_index(TabInfo::GymTab::TeamResultsTabIndex, true));
    }

    // show hide level guid
    if (competition_info.type == CompetitionType::SvenskaStegserierna)
    {
        // show level gui
        ui->level_label->show();
        ui->level_combo_box->show();
        ui->results_level_label->show();
        ui->results_level_combo_box->show();
    }
    else
    {
        // hide level gui
        ui->level_label->hide();
        ui->level_combo_box->hide();
        ui->results_level_label->hide();
        ui->results_level_combo_box->hide();
    }

    // update all other models with the new selection
    m_gymnast_model->set_competition(selected_competition);

    m_gymnast_table_model->set_competition(selected_competition);
    m_gymnast_table_model->refresh();

    m_level_table_model->set_competition(selected_competition);
    m_level_table_model->refresh();

    m_judgement_model->set_competition(competition_info);
    m_score_table_model->set_competition(competition_info);
    ui->level_combo_box->setCurrentIndex(0);
    m_score_table_model->refresh();

    m_result_item_model->set_competition(competition_info);
    ui->results_level_combo_box->setCurrentIndex(0);
    m_result_item_model->refresh();

    m_team_result_item_model->set_competition(competition_info);
    m_team_result_item_model->refresh();

    m_raw_data_model->set_competition(selected_competition);
    m_raw_data_model->refresh();

    // update the enabled state when all selections have taken place
    init_enabled_state();

    // refresh the gui of the current tab
    auto tab_index = TabInfo::get_tab_with_index(
                ui->tab_widget->currentIndex(),
                competition_info.team_competition);
    init_current_tab(tab_index);
}

void MainWindow::show_closed_competitions_changed()
{
    const bool show_closed_competition = ui->showClosedCompetitionsCheckBox->isChecked();
    m_competition_table_model->include_closed_competitions(show_closed_competition);
    m_competition_table_model->refresh();

    // set initial selection if needed
    if (ui->competition_combo_box->currentIndex() < 0) {
        ui->competition_combo_box->setCurrentIndex(0);
    }
}

void MainWindow::three_judges_changed()
{
    const bool three_judges = ui->three_judges_check_box->isChecked();
    const int number_of_judges = (three_judges ? 3 : 2);

    try {
        JudgementInfo judgement = m_judgement_model->get_current_judgement_info();
        judgement.number_of_judges = number_of_judges;
        m_judgement_model->update(judgement);
    }
    catch(std::runtime_error & e)
    {
        qWarning() << "Failed to update judgement module with number of judges. Error: " << e.what();
    }

    ui->judge_3_combo_box->setEnabled(three_judges);
    m_score_table_model->set_e_number(number_of_judges);
    m_score_table_model->refresh();
    m_score_table_model->recalculate_score();


    update_score_tab();
}

void MainWindow::judge_1_selection_changed()
{
    const QString & current_text = ui->judge_1_combo_box->currentText();
    if (current_text.isEmpty()) return; // invalid selection

    try {
        JudgementInfo judgement = m_judgement_model->get_current_judgement_info();
        judgement.judges[0] = current_text;
        m_judgement_model->update(judgement);
    }
    catch(std::runtime_error & e)
    {
        qWarning() << "Failed to update judgement module with number of judges. Error: " << e.what();
    }
}

void MainWindow::judge_2_selection_changed()
{
    const QString & current_text = ui->judge_2_combo_box->currentText();
    if (current_text.isEmpty()) return; // invalid selection

    try {
        JudgementInfo judgement = m_judgement_model->get_current_judgement_info();
        judgement.judges[1] = current_text;
        m_judgement_model->update(judgement);
    }
    catch(std::runtime_error & e)
    {
        qWarning() << "Failed to update judgement module with number of judges. Error: " << e.what();
    }
}

void MainWindow::judge_3_selection_changed()
{
    const QString & current_text = ui->judge_3_combo_box->currentText();
    if (current_text.isEmpty()) return; // invalid selection

    try {
        JudgementInfo judgement = m_judgement_model->get_current_judgement_info();
        judgement.judges[2] = current_text;
        m_judgement_model->update(judgement);
    }
    catch(std::runtime_error & e)
    {
        qWarning() << "Failed to update judgement module with number of judges. Error: " << e.what();
    }
}

void MainWindow::apparatus_changed()
{
    const QString & apparatus =
            m_apparatus_table_model->get_apparatus_id(ui->apparatus_combo_box->currentIndex());
    if (apparatus.isEmpty()) return; // invalid selection

    m_judgement_model->set_apparatus(apparatus);
    m_score_table_model->set_apparatus(apparatus);
    update_score_tab();
}

void MainWindow::score_level_changed()
{
    const QString & level_str = ui->level_combo_box->currentText();
    if (level_str.isEmpty()) return; // invalid selection

    bool int_ok = false;
    const int level = level_str.toInt(&int_ok);
    if (not int_ok) return; // invalid data

    m_judgement_model->set_level(level);
    m_score_table_model->set_level(level);
    update_score_tab();
}

void MainWindow::results_level_changed()
{
    const QString & level_str = ui->results_level_combo_box->currentText();
    if (level_str.isEmpty()) return; // invalid selection

    bool int_ok = false;
    const int level = level_str.toInt(&int_ok);
    if (not int_ok) return; // invalid data

    // disable d value selection for level < 5
    ui->score_details_check_box->setDisabled((level<5));

    const int current_result_type_index = ui->results_type_comboBox->currentIndex();
    m_result_type_model->set_level(level);
    m_result_type_model->refresh();
    if (m_result_item_model->get_qt_model()->rowCount() >= current_result_type_index)
    {
        ui->results_type_comboBox->setCurrentIndex(current_result_type_index);
    }

    m_result_item_model->set_level(level);
    m_result_item_model->refresh();

    update_results_tab();
}

void MainWindow::show_score_details_changed()
{
    const bool show_score_details = ui->score_details_check_box->isChecked();
    m_result_item_model->set_show_score_details(show_score_details);
    m_result_item_model->refresh();

    update_results_tab();
}

void MainWindow::result_type_changed()
{
    const auto & result_type =
            m_result_type_model->get_result_type(ui->results_type_comboBox->currentIndex());
    m_result_item_model->set_result_type(result_type);
    m_result_item_model->refresh();

    update_results_tab();
}

void MainWindow::publish_results()
{
    m_result_item_model->publish_results();
    update_results_publish_gui();
}

void MainWindow::remove_publication()
{
    m_result_item_model->remove_publication();
    update_results_publish_gui();
}

void MainWindow::init_competition_selection()
{
    const int current_index = ui->competition_combo_box->currentIndex();

    m_competition_table_model->include_closed_competitions(
                ui->showClosedCompetitionsCheckBox->isChecked());
    m_competition_table_model->refresh();

    // set current index if possible and needed
    if (ui->competition_combo_box->count() > 0 &&
            ui->competition_combo_box->currentIndex() < 0)
    {
        // restore current index
        ui->competition_combo_box->setCurrentIndex(std::max(current_index,0));
    }
}

void MainWindow::init_score_tab()
{
    m_apparatus_table_model->refresh();
    ui->apparatus_combo_box->setCurrentIndex(0);

    m_level_table_model->refresh();
    ui->level_combo_box->setCurrentIndex(0);

    m_judge_table_model->refresh();

    three_judges_changed();
    update_score_tab();
}

void MainWindow::init_results_tab()
{
    // same model as used for score, thus refresh is not needed
    ui->results_level_combo_box->setCurrentIndex(0);

    m_result_type_model->refresh();
    ui->results_type_comboBox->setCurrentIndex(0);

    // trigger intial update
    show_score_details_changed();

    update_results_tab();
}

void MainWindow::init_current_tab(TabInfo::GymTab current_tab)
{
    switch(current_tab)
    {
    case TabInfo::GymTab::JudgeTabIndex:
        update_score_tab();
        return;
    case TabInfo::GymTab::ResultsTabIndex:
        update_results_tab();
        return;
    case TabInfo::GymTab::TeamResultsTabIndex:
        update_team_results_tab();
        return;
    case TabInfo::GymTab::RawDataTabIndex:
        update_raw_data_tab();
        return;
    case TabInfo::GymTab::NumbersOfTabs:
        qWarning() << "Invalid tab index";
        return;
    }
}

void MainWindow::init_enabled_state()
{
    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "Failed to initi enabled/disable state of components, competition has not been seleted";
        return;
    }

    if (competition_info.closed)
    {
        ui->gymnast_table_view->setEnabled(false);
        ui->raw_data_table_view->setEnabled(false);
        ui->actionImport->setEnabled(false);
        ui->actionEditCompetitionGymnasts->setEnabled(false);
    }
    else
    {
        ui->gymnast_table_view->setEnabled(true);
        ui->raw_data_table_view->setEnabled(true);
        ui->actionImport->setEnabled(true);
        ui->actionEditCompetitionGymnasts->setEnabled(true);
    }
}

void MainWindow::update_judge_gui()
{
    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "Failed to update judge input since competition is incomplete";
        return;
    }

    if (competition_info.type == CompetitionType::SvenskaStegserierna &&
            ui->level_combo_box->currentText().toInt() < 5) // two judges for level 1-4
    {
        ui->three_judges_check_box->hide();
        ui->judge_3_combo_box->hide();
        ui->judge_3_label->hide();
    }
    else
    {
        ui->three_judges_check_box->show();
        ui->judge_3_combo_box->show();
        ui->judge_3_label->show();
    }
}

void MainWindow::update_results_publish_gui()
{
    QColor status_color = Qt::gray;
    if (not m_result_item_model->is_results_published())
    {
        status_color = Qt::red;
        ui->publish_results_status_label->setText("Results have not been published");
    }
    else
    {
        if (not m_result_item_model->is_published_results_up_to_date())
        {
            status_color = Qt::yellow;
            ui->publish_results_status_label->setText("Results are published but not up to date");
        }
        else
        {
            status_color = QColor(128, 195, 66);
            ui->publish_results_status_label->setText("Results are published");
        }
    }

    QPalette palette = ui->publish_results_status_label->palette();
    palette.setBrush(QPalette::Background, QBrush(status_color));
    ui->publish_results_status_label->setPalette(palette);
}


void MainWindow::print_table_model(QAbstractItemModel *table_model)
{
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Results"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    QTableView pTableView;
    pTableView.setModel(table_model);

    double width = pTableView.verticalHeader()->width();
    double height = pTableView.horizontalHeader()->height();
    const int columns = table_model->columnCount();
    const int rows = table_model->rowCount();

    pTableView.resizeColumnsToContents();

    for(int i = 0; i < columns; ++i) {
        width += pTableView.columnWidth(i);
    }

    for(int i = 0; i < rows; ++i) {
        height += pTableView.rowHeight(i);
    }

    pTableView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pTableView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pTableView.setFixedSize(width, height);

    QPainter painter(&printer);
    painter.setBackground(QBrush(Qt::white));

    //scale with respect to width
    double scale = 1;
    if (width > printer.pageRect().width())
    {
        scale = double(printer.pageRect().width()) / width;
        painter.scale(scale,scale);
    }

    double scaled_width = width*scale;
    //double scaled_height = height*scale;

    //print all rows
    int printed_rows = 0;
    double print_from_pos = 0;
    double print_to_pos = pTableView.horizontalHeader()->height() * scale;
    do {
        while(printed_rows < table_model->rowCount() &&
              (print_to_pos - print_from_pos) < printer.pageRect().height())
        {
            print_to_pos += (pTableView.rowHeight(printed_rows) * scale);
            ++printed_rows;
        }

        pTableView.render(&painter, QPoint(0,0), QRegion(0,print_from_pos,scaled_width, print_to_pos+1));
        print_from_pos = print_to_pos;
    }
    while (printed_rows < table_model->rowCount() && printer.newPage());
}

