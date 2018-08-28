#include "JudgeSqlTableModel.h"

JudgeSqlTableModel::JudgeSqlTableModel(QSqlDatabase & db):
    m_db(db)
{
}

QAbstractTableModel * JudgeSqlTableModel::get_qt_model()
{
    return &m_sql_model;
}

void JudgeSqlTableModel::refresh()
{
    m_sql_model.setQuery("SELECT name FROM judge ORDER BY name", m_db);
    m_sql_model.setHeaderData(0, Qt::Horizontal, QObject::tr("Judge Name"));
}
