#include "FindGymnastSqlItemModel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QTextStream>
#include <QDebug>

FindGymnastSqlItemModel::FindGymnastSqlItemModel(QSqlDatabase & db)
    : m_query(db)
{
}

QAbstractItemModel * FindGymnastSqlItemModel::get_qt_model()
{
    return this;
}

void FindGymnastSqlItemModel::set_search_name(const QString & name)
{
    QString sql_query_str;
    QTextStream sql_stream(&sql_query_str);
    sql_stream << "SELECT distinct gymnast_name as Name, apparatus as Apparatus, competition_name as Competition, gymnast_club as Club "
              << "FROM competition_score_cop_view where gymnast_name LIKE "
              <<"\'%" << name << "%\'"
             << " AND competition_name IN(SELECT competition_name FROM competition WHERE closed=false) "
             << "ORDER BY gymnast_name, competition_name;";

    beginResetModel();
    if (not m_query.exec(sql_query_str))
    {
        qWarning() << "Find gymnast query failed with sql error " << m_query.lastError().text();
        qWarning() << "Query: " << sql_query_str;
    }
    endResetModel();
}

int FindGymnastSqlItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_query.size();
}

int FindGymnastSqlItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_query.first())
    {
        return m_query.record().count();
    }

    return 0;
}

QVariant FindGymnastSqlItemModel::data(const QModelIndex &index, int role) const
{
    QVariant data;

    if (role == Qt::DisplayRole)
    {
        if (m_query.seek(index.row()))
        {
            const QSqlRecord & record = m_query.record();
            const QSqlField & field = record.field(index.column());
            if (not field.isNull() && field.isValid())
            {
                data = m_translator.translate(field.value().toString());
            }
            else
            {
                qWarning() << "FindGymansSqlItemModel error: invalid field: " << index.column();
            }
        }
        else
        {
            qWarning() << "FinGymnastSqlItemModel error, invalid index :" << index.row();
        }
    }

    return data;
}

QVariant FindGymnastSqlItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return QVariant();
        }
        else
        {
            const QSqlRecord & record = m_query.record();
            if (section < record.count())
            {
                return m_translator.translate(record.field(section).name());
            }
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);

}
