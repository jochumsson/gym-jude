#include "MainWindow.h"

#include "LoginDialog.h"
#include "EditCompetitionsDialog.h"
#include "EditGymnastsDialog.h"
#include "ImportGymnastsDialog.h"
#include "FindGymnastDialog.h"

#include "CompetitionSqlModel.h"
#include "CompetitionSqlTableModel.h"
#include "GymnastSqlModel.h"
#include "GymnastSqlTableModel.h"
#include "LevelSqlTableModel.h"
#include "ApparatusSqlTableModel.h"
#include "JudgeSqlTableModel.h"
#include "JudgementSqlModel.h"
#include "ScoreSqlTableModel.h"
#include "ResultTypeSqlModel.h"
#include "ResultsSqlCalculator.h"
#include "ResultSqlItemModel.h"
#include "TeamResultSqlItemModel.h"
#include "RawSqlDataModel.h"
#include "FindGymnastSqlItemModel.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QTranslator>
#include <QDebug>

void create_edit_competitions_dialog(
        MainWindow * main_window,
        const ICompetitionModelPtr & competition_model,
        const ICompetitionTableModelPtr & compettition_table_model)
{
    // create edit competition dialog
    auto edit_competitions_dialog =
            new EditCompetitionsDialog{competition_model, compettition_table_model, main_window};
    edit_competitions_dialog->setModal(true);
    QObject::connect(main_window, SIGNAL(open_edit_competitions_dialog()),
                     edit_competitions_dialog, SLOT(show()));
    QObject::connect(edit_competitions_dialog, SIGNAL(accepted()), main_window, SLOT(initialize()));
}

void create_edit_gymnasts_dialog(
        MainWindow * main_window,
        const ICompetitionModelPtr & competition_model,
        const IGymnastModelPtr & gymnast_model,
        const IGymnastTableModelPtr & gymnast_table_model,
        const ILevelTableModelPtr & level_table_model)
{
    // create edit gymnasts dialog
    auto edit_gymnasts_dialog = new EditGymnastsDialog{
                competition_model,
                gymnast_model,
                gymnast_table_model,
                level_table_model,
                main_window};
    edit_gymnasts_dialog->setModal(true);
    QObject::connect(main_window, SIGNAL(open_edit_competition_gymnasts_dialog()),
                     edit_gymnasts_dialog, SLOT(show()));
    QObject::connect(edit_gymnasts_dialog, SIGNAL(accepted()), main_window, SLOT(initialize()));
}

void create_import_gymnasts_dialog(
        MainWindow * main_window,
        const ICompetitionModelPtr & competition_model,
        const IGymnastTableModelPtr & gymnast_model)
{
    Q_UNUSED(gymnast_model);
    auto import_gymnast_dialog =
            new ImportGymnastsDialog(competition_model, gymnast_model, main_window);
    QObject::connect(main_window, SIGNAL(open_import_gymnasts_dialog()),
                     import_gymnast_dialog, SLOT(show()));
    QObject::connect(import_gymnast_dialog, SIGNAL(accepted()), main_window, SLOT(initialize()));

}

void create_find_gymnast_dialg(
        MainWindow & main_window,
        const IFindGymnastItemModelPtr & find_gymnast_item_model)
{
    auto find_gymanst_dialg = new FindGymnastDialog(
                find_gymnast_item_model,
                main_window);
    QObject::connect(&main_window, SIGNAL(open_find_gymnast_dialog()),
                     find_gymanst_dialg, SLOT(show()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // provide local translations
    QTranslator translator;
    if (not translator.load("gym_judge_sw"))
    {
        qWarning() << "Failed to load translation gym_judge_sw";
    }
    else
    {
        app.installTranslator(&translator);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("gym_db");
    db.setPort(3306);

    // create main window
    auto competition_model = std::make_shared<CompetitionSqlModel>(db);
    auto score_sql_table_model = std::make_shared<ScoreSqlTableModel>(db);
    auto result_type_sql_model = std::make_shared<ResultTypeSqlModel>(db);
    auto results_calculator = std::make_shared<ResultsSqlCalculator>(db, result_type_sql_model);
    MainWindow main_window(
                competition_model,
                std::make_shared<CompetitionSqlTableModel>(db),
                std::make_shared<GymnastSqlModel>(db),
                std::make_shared<GymnastSqlTableModel>(db),
                std::make_shared<LevelSqlTableModel>(db),
                std::make_shared<ApparatusSqlTableModel>(db),
                std::make_shared<JudgeSqlTableModel>(db),
                std::make_shared<JudgementSqlModel>(db),
                score_sql_table_model,
                result_type_sql_model,
                std::make_shared<ResultSqlItemModel>(db, results_calculator),
                std::make_shared<TeamResultSqlItemModel>(results_calculator),
                std::make_shared<RawSqlDataModel>(db));
    main_window.setWindowIcon(QIcon(":/images/gym_gui.jpg"));

    create_edit_competitions_dialog(
                &main_window,
                std::make_shared<CompetitionSqlModel>(db),
                std::make_shared<CompetitionSqlTableModel>(db));
    create_edit_gymnasts_dialog(
                &main_window,
                competition_model,
                std::make_shared<GymnastSqlModel>(db),
                std::make_shared<GymnastSqlTableModel>(db),
                std::make_shared<LevelSqlTableModel>(db));
    create_import_gymnasts_dialog(
                &main_window,
                competition_model,
                std::make_shared<GymnastSqlTableModel>(db));
    create_find_gymnast_dialg(
                main_window,
                std::make_shared<FindGymnastSqlItemModel>(db));

    // create login dialog
    LoginDialog login_dialog(db);
    QObject::connect(&login_dialog, SIGNAL(loggedInOk()), &main_window, SLOT(initialize()));
    QObject::connect(&login_dialog, SIGNAL(cancel()), &app, SLOT(quit()));
    login_dialog.show();

    return app.exec();
}
