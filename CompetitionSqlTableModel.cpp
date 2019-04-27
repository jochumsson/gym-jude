#include "CompetitionSqlTableModel.h"
#include <QSqlQuery>
#include <QSqlError>

CompetitionSqlTableModel::CompetitionSqlTableModel(QSqlDatabase & db):
    m_db(db)
{
}

CompetitionSqlTableModel::~CompetitionSqlTableModel()
{
}

QAbstractTableModel * CompetitionSqlTableModel::get_qt_model()
{
    return &m_sql_model;
}


void CompetitionSqlTableModel::refresh()
{
    QString query_str = "SELECT competition_name, competition_date FROM competition";
    if (not m_include_closed_competitions)
    {
        query_str += " WHERE closed=false";
    }
    m_sql_model.setQuery(query_str, m_db);
    m_sql_model.setHeaderData(0, Qt::Horizontal, QObject::tr("Competition Name"));
    m_sql_model.setHeaderData(1, Qt::Horizontal, QObject::tr("Competition Date"));

    if (m_sql_model.lastError().isValid())
    {
            qWarning() << "Failed to retrive compitition names with sql error: "
                       << m_sql_model.lastError().text();
    }
}

void CompetitionSqlTableModel::include_closed_competitions(bool include_closed_competitions)
{
    m_include_closed_competitions = include_closed_competitions;
}

QString CompetitionSqlTableModel::get_competition_name(int index) const
{
    QString competition_name;

    QModelIndex model_index = m_sql_model.index(index, 0);
    if (model_index.isValid())
    {
        competition_name = m_sql_model.data(model_index).toString();

    }

    return competition_name;
}

bool CompetitionSqlTableModel::add_competition(const CompetitionInfo & competition_info, QString & sql_error_str)
{
    QSqlQuery query;
    query.prepare("INSERT INTO competition(competition_name, competition_date, competition_type, team_competition, closed) VALUES(:competition_name_bind_value, :competition_date_bind_value, :competition_type_bind_value, :team_competition_bind_value, :closed_bind_value)");
    query.bindValue(":competition_name_bind_value", competition_info.name);
    query.bindValue(":competition_date_bind_value", competition_info.date.toString("yyyy-MM-dd"));
    query.bindValue(":competition_type_bind_value", competition_info.competition_type.name);
    query.bindValue(":team_competition_bind_value", competition_info.team_competition);
    query.bindValue(":closed_bind_value", false);

    bool return_value = query.exec();
    if (return_value)
    {
        refresh();
    }
    else
    {
        sql_error_str = query.lastError().text();
    }

    return return_value;
}

bool CompetitionSqlTableModel::delete_competition(
        const QString & competition_name,
        QString & sql_error_str)
{
    QSqlQuery query;
    query.prepare("DELETE FROM competition WHERE competition_name=:competition_name_bind_value");
    query.bindValue(":competition_name_bind_value", competition_name);

    bool return_value = query.exec();
    if (return_value)
    {
        refresh();
    }
    else
    {
        sql_error_str = query.lastError().text();
    }

    return return_value;
}

