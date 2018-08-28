#include "LevelSqlTableModel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

LevelSqlTableModel::LevelSqlTableModel(QSqlDatabase & db):
    m_db(db)
{
}

QAbstractTableModel * LevelSqlTableModel::get_qt_model()
{
    return &m_sql_model;
}

void LevelSqlTableModel::set_competition(const QString & competition_name)
{
    m_competition_name = competition_name;
}

void LevelSqlTableModel::refresh()
{
    if (m_competition_name.isEmpty())
    {
        qWarning() << "Refresh LevelSqlTableModel without competition selection";
        return;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT DISTINCT level FROM competition_gymnast WHERE competition_name=:competition_name_bind_value ORDER BY level");
    query.bindValue(":competition_name_bind_value", m_competition_name);


    if(not query.exec())
    {
        qWarning() << "LevelSqlTableModel refresh failed with error: " << query.lastError().text();
    }

    m_sql_model.setQuery(query);    
}
