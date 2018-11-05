#-------------------------------------------------
#
# Project created by QtCreator 2016-11-23T15:27:20
#
#-------------------------------------------------

QT       += core gui sql printsupport
QTPLUGIN += qsqlmysql
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gymnastic-judge
TEMPLATE = app

TRANSLATIONS = gym_judge_sw.ts
CODECFORTR = UTF-8

SOURCES += main.cpp\
        MainWindow.cpp \
    LoginDialog.cpp \
    CompetitionSqlModel.cpp \
    CompetitionSqlTableModel.cpp \
    EditGymnastsDialog.cpp \
    EditCompetitionsDialog.cpp \
    LevelSqlTableModel.cpp \
    GymnastSqlTableModel.cpp \
    GymnastSqlModel.cpp \
    ApparatusSqlTableModel.cpp \
    JudgeSqlTableModel.cpp \
    RawSqlDataModel.cpp \
    ScoreSqlTableModel.cpp \
    ResultsSqlCalculator.cpp \
    ResultSqlItemModel.cpp \
    TeamResultSqlItemModel.cpp \
    JudgementSqlModel.cpp \
    ScoreItemDelegate.cpp \
    ResultTypeSqlModel.cpp \
    FindGymnastDialog.cpp \
    FindGymnastSqlItemModel.cpp \
    ScoreFocusNavigator.cpp \
    ImportGymnastsDialog.cpp

HEADERS  += MainWindow.h \
    LoginDialog.h \
    CompetitionSqlModel.h \
    ICompetitionModel.h \
    CompetitionInfo.h \
    GymnastInfo.h \
    IGymnastModel.h \
    IGymnastTableModel.h \
    CompetitionSqlTableModel.h \
    ICompetitionTableModel.h \
    EditGymnastsDialog.h \
    EditCompetitionsDialog.h \
    ILevelTableModel.h \
    LevelSqlTableModel.h \
    GymnastSqlTableModel.h \
    GymnastSqlModel.h \
    IApparatusTableModel.h \
    ApparatusSqlTableModel.h \
    JudgeSqlTableModel.h \
    IJudgeTableModel.h \
    IRawDataModel.h \
    RawSqlDataModel.h \
    IScoreTableModel.h \
    ScoreSqlTableModel.h \
    GymnastResults.h \
    IResultsCalculator.h \
    ResultsSqlCalculator.h \
    IResultItemModel.h \
    ITeamResultItemModel.h \
    ResultSqlItemModel.h \
    TeamResultSqlItemModel.h \
    TeamResults.h \
    IJudgementModel.h \
    JudgementSqlModel.h \
    GymJudgeException.h \
    ScoreItemDelegate.h \
    Translator.h \
    IResultTypeModel.h \
    ResultTypeSqlModel.h \
    FindGymnastDialog.h \
    IFindGymnastItemModel.h \
    FindGymnastSqlItemModel.h \
    ScoreFocusNavigator.h \
    ImportGymnastsDialog.h

FORMS    += MainWindow.ui \
    LoginDialog.ui \
    EditCompetitionsDialog.ui \
    EditGymnastsDialog.ui \
    FindGymnastDialog.ui \
    ImportGymnastsDialog.ui

RC_FILE = images/gym_judge.rc

LINUX-g++:LIBS += -lmysqlclient -L/usr/lib/i386-linux-gnu/libmysqlclient.a


