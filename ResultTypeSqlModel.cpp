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
    QSqlQuery sql_query(m_db);
    sql_query.prepare("SELECT result_type, all_around_result, apparatus_result, include_in_all_around FROM competition_type_result WHERE competition_type=:competition_type_bind_value");
    sql_query.bindValue(":competition_type_bind_value", m_selected_competition_info.competition_type.name);

    if (not sql_query.exec())
    {
        qWarning() << "ResqulTypeSqlModel failed with error: " << sql_query.lastError().text();
    }

    beginResetModel();
    m_visible_result_types.clear();
    m_all_result_types.clear();

    while(sql_query.next())
    {
        const QSqlField & result_type_field = sql_query.record().field("result_type");
        const QSqlField & all_around_result_field = sql_query.record().field("all_around_result");
        const QSqlField & apparatus_result_field = sql_query.record().field("apparatus_result");
        const QSqlField & include_in_all_around_field = sql_query.record().field("include_in_all_around");
        if (not result_type_field.isValid() ||
                not all_around_result_field.isValid() ||
                not apparatus_result_field.isValid() ||
                not include_in_all_around_field.isValid())
        {
            qWarning() << "Invalid result type record";
            continue;
        }

        const QString result_type_string = result_type_field.value().toString();
        const bool all_around_result = all_around_result_field.value().toBool();
        const bool apparatus_result = apparatus_result_field.value().toBool();
        const bool include_in_all_around = include_in_all_around_field.value().toBool();
        const ResultTypeInfo result_type_info = {result_type_string, apparatus_result, all_around_result, include_in_all_around};
        m_all_result_types.push_back(result_type_info);
        if (all_around_result || apparatus_result)
        {
            m_visible_result_types.push_back(result_type_info);
        }
    }

    endResetModel();
}

void ResultTypeSqlModel::set_competition(const CompetitionInfo & competition_info)
{
    m_selected_competition_info = competition_info;
}

ResultTypeInfo ResultTypeSqlModel::get_result_type(unsigned int visibility_index) const
{
    if (visibility_index < m_visible_result_types.size())
        return m_visible_result_types[visibility_index];
    return {};
}

ResultTypeInfo ResultTypeSqlModel::get_result_type_info(const QString & result_type) const
{
    for (auto result_type_it: m_all_result_types)
    {
        if (result_type_it.result_type_string == result_type)
            return result_type_it;
    }

    return {};
}

std::vector<ResultTypeInfo> ResultTypeSqlModel::get_visible_result_types() const
{
    return m_visible_result_types;
}

std::vector<ResultTypeInfo> ResultTypeSqlModel::get_all_result_types() const
{
    return m_all_result_types;
}

int ResultTypeSqlModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_visible_result_types.size());
}

int ResultTypeSqlModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ResultTypeSqlModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    if (static_cast<unsigned int>(index.row()) < m_visible_result_types.size())
    {
        const auto & result_type_str =
                m_visible_result_types[static_cast<unsigned int>(index.row())].result_type_string;
        return m_translator.translate(result_type_str);
    }

    return QVariant();
}
