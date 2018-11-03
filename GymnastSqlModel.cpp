#include "GymnastSqlModel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

GymnastSqlModel::GymnastSqlModel(QSqlDatabase & db):
    m_db(db)
{
}


bool GymnastSqlModel::set_competition(const QString & competition_name)
{
    // sql check if competition exists

    m_competition_name = competition_name;
    return true;
}

bool GymnastSqlModel::set_gymnast(const QString & gymnast_id, QString & error_str)
{
    if (not m_competition_name.is_initialized())
    {
        error_str = "no competition selected";
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT gymnast_id, gymnast_name, gymnast_club, level, gymnast_start_number, gymnast_team FROM competition_gymnast "
                  "WHERE competition_name=:competition_name_bind_value AND gymnast_id=:gymnast_id_bind_value");
    query.bindValue(":competition_name_bind_value", *m_competition_name);
    query.bindValue(":gymnast_id_bind_value", gymnast_id);

    if (not query.exec())
    {
        error_str = query.lastError().text();
        return false;
    }

    if (not query.next())
    {
        error_str = "no gymnast with id \'";
        error_str += gymnast_id;
        error_str += "\'";
        return false;
    }

    auto gymnast_sql_record = query.record();
    m_gymnast_info = GymnastInfo {
        gymnast_id,
        gymnast_sql_record.field(1).value().toString(),
        gymnast_sql_record.field(2).value().toString(),
        gymnast_sql_record.field(3).value().toInt(),
        gymnast_sql_record.field(4).value().toInt(),
        gymnast_sql_record.field(5).value().toString(),
    };

    return true;
}

bool GymnastSqlModel::get_gymnast_info(GymnastInfo & gymnast_info) const
{
    gymnast_info = *m_gymnast_info;
    return m_gymnast_info.is_initialized();
}

bool GymnastSqlModel::update_gymnast(const GymnastInfo & gymnast_info, QString & error_str)
{
    if (not m_competition_name.is_initialized())
    {
        error_str = "no competition selected";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE competition_gymnast SET gymnast_name=:gymnast_name_bind_value, gymnast_club=:gymnast_club_bind, gymnast_team=:gymnast_team_bind_value, level=:level_bind_value, gymnast_start_number=:gymnast_start_number_bind_value "
                  "WHERE gymnast_id=:gymnast_id_bind_value and competition_name=:competition_name_bind_value");
    query.bindValue(":gymnast_id_bind_value", gymnast_info.id);
    query.bindValue(":gymnast_name_bind_value", gymnast_info.name);
    query.bindValue(":gymnast_club_bind", gymnast_info.club);
    query.bindValue(":gymnast_team_bind_value", gymnast_info.team);
    query.bindValue(":level_bind_value", gymnast_info.level);
    query.bindValue(":gymnast_start_number_bind_value", gymnast_info.start_nr);
    query.bindValue(":competition_name_bind_value", *m_competition_name);
    if(not query.exec())
    {
        error_str = query.lastError().databaseText();
        return false;
    }

    return true;
}
