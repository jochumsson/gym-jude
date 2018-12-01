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

        const QString result_type_string = field.value().toString();
        const ResultType result_type = result_type_from_string(result_type_string);
        if (not m_selected_level ||
                *m_selected_level > 4 ||
                result_type == ResultType::AllArround)
        {
            // level < 5 only supports all arround competitions
            m_level_result_types.push_back({result_type, result_type_string});
        }
    }

    endResetModel();
}

ResultTypeInfo ResultTypeSqlModel::get_result_type(int index) const
{
    if (static_cast<unsigned int>(index) < m_level_result_types.size())
        return m_level_result_types[static_cast<unsigned int>(index)];
    return {};
}

boost::optional<int> ResultTypeSqlModel::get_selected_level() const
{
    return m_selected_level;
}

void ResultTypeSqlModel::set_level(boost::optional<int> level)
{
    m_selected_level = level;
}

ResultTypeInfo ResultTypeSqlModel::get_result_type_info(const ResultType result_type) const
{
    return {result_type, string_from_result_type(result_type)};
}

int ResultTypeSqlModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_level_result_types.size());
}

int ResultTypeSqlModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ResultTypeSqlModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    if (static_cast<unsigned int>(index.row()) < m_level_result_types.size())
    {
        const auto & result_type_str =
                m_level_result_types[static_cast<unsigned int>(index.row())].result_type_string;
        return m_translator.translate(result_type_str);
    }

    return QVariant();
}

ResultType ResultTypeSqlModel::result_type_from_string(const QString & result_type_string)
{
    if (result_type_string == "Balance Beam")
    {
        return ResultType::BalanceBeam;
    }
    else if (result_type_string == "Floor")
    {
        return ResultType::Floor;
    }
    else if (result_type_string == "Jump")
    {
        return ResultType::Jump;
    }
    else if (result_type_string == "Uneven Bars")
    {
        return ResultType::UnevenBars;
    }
    else if (result_type_string == "WAG All Arround")
    {
        return ResultType::AllArround;
    }
    else
    {
        qWarning() << "Undefined result type: " << result_type_string;
        return ResultType::Unknown;
    }
}

QString ResultTypeSqlModel::string_from_result_type(const ResultType result_type)
{
    switch(result_type)
    {
    case ResultType::BalanceBeam:
        return "Balance Beam";
    case ResultType::Floor:
        return "Floor";
    case ResultType::Jump:
        return "Jump";
    case ResultType::UnevenBars:
        return "Uneven Bars";
    case ResultType::AllArround:
        return "WAG All Arround";
    case ResultType::Unknown:
        return "";
    }

    return "";
}
