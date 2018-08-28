#include "ResultTypeSqlModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

ResultTypeSqlModel::ResultTypeSqlModel(QSqlDatabase & db):
    m_db(db)
{
}

QAbstractTableModel * ResultTypeSqlModel::get_qt_model()
{
    return this;
}

void ResultTypeSqlModel::refresh()
{
    if (m_selected_level < 0) return;

    QSqlQuery sql_query(m_db);
    sql_query.prepare("SELECT DISTINCT result_type FROM result");
    if (not sql_query.exec())
    {
        qWarning() << "ResqulTypeSqlModel failed with error: " << sql_query.lastError().text();
    }

    beginResetModel();
    m_level_result_types.clear();

    while(sql_query.next())
    {
        const QSqlField & field = sql_query.record().field("result_type");
        if (field.isNull() || not field.isValid())
        {
            qWarning() << "Invalid field result_type in result table";
            continue;
        }

        if (m_selected_level > 4 || is_all_arround(field.value().toString()))
        {
            // level < 5 only support all arround competitions
            m_level_result_types.push_back(field.value());
        }
    }

    endResetModel();
}

void ResultTypeSqlModel::set_level(int level)
{
    m_selected_level = level;
}

QVariant ResultTypeSqlModel::get_result_type(int index)
{
    if ((unsigned int)index < m_level_result_types.size())
        return m_level_result_types[index];
    return QString();
}

int ResultTypeSqlModel::rowCount(const QModelIndex &) const
{
    return m_level_result_types.size();
}

int ResultTypeSqlModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ResultTypeSqlModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    if ((unsigned int)index.row() < m_level_result_types.size())
    {
        const QVariant & result_type_variant = m_level_result_types[index.row()];
        return m_translator.translate(result_type_variant.toString());
    }

    return QVariant();
}

bool ResultTypeSqlModel::is_all_arround(const QVariant & result_type) const
{
    QSqlQuery sql_query(m_db);
    sql_query.prepare("SELECT COUNT(result_type) FROM result "
                      "WHERE result_type=:result_type_bind_value");
    sql_query.bindValue(":result_type_bind_value", result_type);
    if (not sql_query.exec())
    {
        qWarning() << "is_all_arround query failed with error: " << sql_query.lastError().text();
    }

    if (sql_query.next())
    {
        const auto & field = sql_query.record().field(0);
        if (not field.isNull() && field.isValid())
        {
            return (field.value().toInt() > 2);
        }
    }

    return false;
}
