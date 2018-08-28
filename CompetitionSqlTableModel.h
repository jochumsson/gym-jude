#pragma once

#include "ICompetitionTableModel.h"

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QDebug>

class CompetitionSqlTableModel : public ICompetitionTableModel
{
public:
    CompetitionSqlTableModel(QSqlDatabase & db);
    virtual ~CompetitionSqlTableModel();

    QAbstractTableModel * get_qt_model() final;

    void refresh() final;
    void include_closed_competitions(bool include_closed_competitions);

    QString get_competition_name(int index) const final;
    bool add_competition(const CompetitionInfo & competition_info, QString & sql_error_str) final;
    bool delete_competition(const QString & competition_name, QString & sql_error_str) final;

private:
    QSqlDatabase & m_db;
    QSqlQueryModel m_sql_model;
    bool m_include_closed_competitions = true;

};
