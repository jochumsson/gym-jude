#pragma once

#include "ILevelTableModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>

class LevelSqlTableModel :
        public QAbstractTableModel,
        public ILevelTableModel
{
public:
    LevelSqlTableModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void set_competition(const CompetitionInfo & competition_info) final;

    void refresh() final;

    int get_level(const QString & level_string) const final;

    int rowCount(const QModelIndex &) const final;

    int columnCount(const QModelIndex &) const final;

    QVariant data(const QModelIndex &index, const int role) const final;

private:
    static QString translate_to_star_level(const QString & level);
    static int translate_from_star_level(const QString & level);

    QSqlDatabase & m_db;
    mutable QSqlQuery m_data_query;
    CompetitionInfo m_competition_info;

};

