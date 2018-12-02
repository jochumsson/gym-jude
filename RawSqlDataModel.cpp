#include "RawSqlDataModel.h"
#include <QTextStream>

RawSqlDataModel::RawSqlDataModel(QSqlDatabase & db):
    m_db(db),
    m_sql_model(nullptr, db)
{
}

QAbstractItemModel * RawSqlDataModel::get_qt_model()
{
    return &m_sql_model;
}

void RawSqlDataModel::refresh()
{
    m_sql_model.setTable("competition_score_cop_view");

    if (m_selected_competition.is_initialized())
    {
        QString filter;
        QTextStream filter_stream(&filter);
        filter_stream << "competition_name=\'" << *m_selected_competition << "\'";
        m_sql_model.setFilter(filter);
    }

    m_sql_model.select();
}

void RawSqlDataModel::set_competition(const QString & competition_name)
{
    m_selected_competition = competition_name;
}

