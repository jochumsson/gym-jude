#include "CompetitionSqlModel.h"
#include <QSqlQuery>
#include <QTextStream>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

CompetitionSqlModel::CompetitionSqlModel(QSqlDatabase & db):
    m_db(db)
{
}

bool CompetitionSqlModel::set_competition(const QString & competition_name, QString & sql_error_str)
{
    QSqlQuery query;
    query.prepare("SELECT competition_name, competition_date, competition_type, team_competition, closed FROM competition WHERE competition_name=:competition_name_bind_value");
    query.bindValue(":competition_name_bind_value", competition_name);

    if (not query.exec())
    {
        sql_error_str += query.lastError().text();
        m_current_competition_info = boost::none;
        return false;
    }

    if (not query.next())
    {
        sql_error_str = "no such competition";
        return false;
    }

    auto competition_info = query.record();
    m_current_competition_info = CompetitionInfo {
        competition_name,
        competition_info.field(1).value().toDate(),
        strToCompetitionType(competition_info.field(2).value().toString()),
        competition_info.field(3).value().toBool(),
        competition_info.field(4).value().toBool()};

    return true;
}

bool CompetitionSqlModel::get_competition_info(CompetitionInfo & competition_info) const
{
    if (m_current_competition_info.is_initialized())
    {
        competition_info = *m_current_competition_info;
        return true;
    }

    return false;
}

bool CompetitionSqlModel::update_competition(const CompetitionInfo & competition_info, QString & error_str)
{
    if (!m_current_competition_info)
    {
        error_str = "Competition has not been selected";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE competition set competition_name=:competition_name_bind_value, competition_date=:competition_date_bind_value, competition_type=:competition_type_bind_value ,team_competition=:team_competition_bind_value, closed=:closed_bind_value WHERE competition_name=:old_competition_name_bind_value");
    query.bindValue(":competition_name_bind_value", competition_info.name);
    query.bindValue(":competition_date_bind_value", competition_info.date);
    query.bindValue(":competition_type_bind_value", competitionTypeToStr(competition_info.type));
    query.bindValue(":team_competition_bind_value", competition_info.team_competition);
    query.bindValue(":closed_bind_value", competition_info.closed);
    query.bindValue(":old_competition_name_bind_value", (*m_current_competition_info).name);

    if (query.exec())
    {
        m_current_competition_info = m_current_competition_info;
    }
    else
    {
        error_str = query.lastError().text();
        return false;
    }

    return true;
}
