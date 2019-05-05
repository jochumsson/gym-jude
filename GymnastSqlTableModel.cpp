#include "GymnastSqlTableModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QTextStream>
#include <QDebug>

GymnastSqlTableModel::GymnastSqlTableModel(QSqlDatabase & db):
    m_db(db)
{
}

QAbstractTableModel * GymnastSqlTableModel::get_qt_model()
{
    return &m_sql_model;
}

void GymnastSqlTableModel::set_competition(const QString & competition_name)
{
    m_competition_name = competition_name;
}

void GymnastSqlTableModel::set_filter_by_level(bool filter_by_level)
{
    m_filter_by_level = filter_by_level;
}

void GymnastSqlTableModel::set_level(int level)
{
    m_level = level;
}

void GymnastSqlTableModel::refresh()
{
    if (not m_competition_name)
    {
        // the sql model can not be refreshed before a competition has been selected
        return;
    }

    QSqlQuery query(m_db);
    if (m_filter_by_level)
    {
        m_sql_model.setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
        query.prepare("SELECT gymnast_name, gymnast_id, gymnast_start_number FROM competition_gymnast "
                      "WHERE competition_name=:competition_name_bind_value AND level=:level_bind_value ORDER BY gymnast_start_number");
        query.bindValue(":competition_name_bind_value", *m_competition_name);
        query.bindValue(":level_bind_value", m_level);
    }
    else
    {
        query.prepare("SELECT gymnast_name, gymnast_id FROM competition_gymnast "
                      "WHERE competition_name=:competition_name_bind_value ORDER BY gymnast_name");
        query.bindValue(":competition_name_bind_value", *m_competition_name);
    }
    
    if (not query.exec())
    {
        qWarning() << "GymnastSqlTableModel::refresh failed with error: " << query.lastError().text();
    }

    m_sql_model.setQuery(query);
}

QString GymnastSqlTableModel::get_gymnast_id(int index)
{
    QString index_id;

    QModelIndex model_index = m_sql_model.index(index, 1);
    if (model_index.isValid())
    {
        index_id = m_sql_model.data(model_index).toString();
    }

    return index_id;
}

int GymnastSqlTableModel::get_gymnast_index(const QString & gymnast_id)
{
    int gymnast_index = 0;

    for (int index = 0; index < m_sql_model.rowCount(); ++index)
    {
        QSqlRecord record = m_sql_model.record(index);
        QSqlField field = record.field(1);
        if (field.isValid() &&
                field.value().toString() == gymnast_id)
        {
            gymnast_index = index;
            break;
        }
    }

    return gymnast_index;
}

bool GymnastSqlTableModel::add_gymnast(const GymnastInfo & gymnast_info, QString & error_log)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO competition_gymnast(competition_name, level, gymnast_id, gymnast_name, gymnast_club, gymnast_team, gymnast_start_number) "
                  "VALUES(:competition_name_bind_value, :level_bind_value, :gymnast_id_bind_value, :gymnast_name_bind_value, :gymnast_club_bind_value, :gymnast_team_bind_value, :gymnast_start_number_bind_value)");
    query.bindValue(":competition_name_bind_value", *m_competition_name);
    query.bindValue(":level_bind_value", gymnast_info.level);
    query.bindValue(":gymnast_id_bind_value", gymnast_info.id.trimmed());
    query.bindValue(":gymnast_name_bind_value", gymnast_info.name.trimmed());
    query.bindValue(":gymnast_club_bind_value", gymnast_info.club.trimmed());
    query.bindValue(":gymnast_team_bind_value", gymnast_info.team.trimmed());
    query.bindValue(":gymnast_start_number_bind_value", gymnast_info.start_nr);

    if (not query.exec())
    {
        qWarning() << "Failed to add gymnast to competition_gymnast with error: " << query.lastError();
        return false;
    }

    for (auto apparatus_name: get_apparatus_list())
    {
        if (not add_gymnast(apparatus_name, gymnast_info, error_log))
        {
            qWarning() << "Creating new gymnast failed with error: " << error_log;
            return false;
        }
    }

    return true;
}

bool GymnastSqlTableModel::delete_gymnast(const QString & gymnast_id, QString & error_log)
{
    if (not m_competition_name)
    {
        error_log = "Competition has not been selected";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM competition_score WHERE gymnast_id=:gymnast_id_bind_value and competition_name=:competition_name_bind_value");
    query.bindValue(":gymnast_id_bind_value", gymnast_id);
    query.bindValue(":competition_name_bind_value", *m_competition_name);
    error_log = query.lastError().text();
    const bool delete_from_score_ok = query.exec();

    if (delete_from_score_ok)
    {
        query.prepare("DELETE FROM competition_gymnast WHERE gymnast_id=:gymnast_id_bind_value and competition_name=:competition_name_bind_value");
        query.bindValue(":gymnast_id_bind_value", gymnast_id);
        query.bindValue(":competition_name_bind_value", *m_competition_name);
        error_log = query.lastError().text();
        return query.exec();
    }

    return false;
}

bool GymnastSqlTableModel::add_gymnast(const QString & apparatus,
        const GymnastInfo & gymnast_info,
        QString & error_log)
{
    if (not m_competition_name)
    {
        error_log = "Competition has not been selected";
        return false;
    }

    QString sql_str;
    QTextStream sql_stream(&sql_str);
    sql_stream << "INSERT INTO "
               << "competition_score(competition_name, gymnast_id, apparatus, base_score) "
               << "VALUES("
               << "\'" << *m_competition_name << "\', "
               << "\'" << gymnast_info.id.trimmed() << "\', "
               << "\'" << apparatus.trimmed() << "\', "
               << 10 // default base score = 10
               << ");";

    QSqlQuery insert_query(sql_str);
    if (insert_query.lastError().isValid())
    {
        QTextStream error_log_stream(&error_log);
        error_log_stream << "Insert query failed with error: " << insert_query.lastError().databaseText() << "\n";
        return false;
    }

    return true;
}

std::vector<QString> GymnastSqlTableModel::get_apparatus_list()
{
    // for now we return all apparatus types regardless of competition type

    std::vector<QString> apparatus_list;
    QSqlQuery apparatus_query;
    apparatus_query.prepare("SELECT apparatus_name FROM apparatus");
    if(not apparatus_query.exec())
    {
        qWarning() << "GymnastSqlTable model failed to retrieve apparatus list with error: " << apparatus_query.lastError().text();
    }

    while(apparatus_query.next())
    {
        const QSqlField & apparatus_name_field = apparatus_query.record().field("apparatus_name");
        if (not apparatus_name_field.isValid())
        {
            qWarning() << "Failed to retrieve apparatus_name field from apparatus list";
            continue;
        }
        apparatus_list.push_back(apparatus_name_field.value().toString());
    }

    return apparatus_list;
}
