#include "CompetitionTypeSqlModel.h"
#include <QSqlError>
#include <QDebug>

CompetitionTypeSqlModel::CompetitionTypeSqlModel(QSqlDatabase & db) :
    m_sql_model(nullptr, db)
{
}

QAbstractItemModel * CompetitionTypeSqlModel::get_qt_model()
{
    return &m_sql_model;
}

void CompetitionTypeSqlModel::refresh()
{
    m_sql_model.setTable("competition_type");
    m_sql_model.setHeaderData(0, Qt::Horizontal, QObject::tr("Competition Type"));
    if (not m_sql_model.select())
    {
        qWarning() << "Failed to initialize competition type sql model with error: " << m_sql_model.lastError();
    }
}
