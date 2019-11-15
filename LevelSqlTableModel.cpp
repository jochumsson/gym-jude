#include "LevelSqlTableModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

LevelSqlTableModel::LevelSqlTableModel(QSqlDatabase & db):
    m_db(db)
{
}

QAbstractTableModel * LevelSqlTableModel::get_qt_model()
{
    return this;
}

void LevelSqlTableModel::set_competition(const CompetitionInfo & competition_info)
{
    m_competition_info = competition_info;
}

void LevelSqlTableModel::refresh()
{
    if (m_competition_info.name.isEmpty())
    {
        qWarning() << "Refresh LevelSqlTableModel without competition selection";
        return;
    }

    m_data_query.prepare("SELECT DISTINCT level FROM competition_gymnast WHERE competition_name=:competition_name_bind_value ORDER BY level");
    m_data_query.bindValue(":competition_name_bind_value", m_competition_info.name);

    beginResetModel();
    if(not m_data_query.exec())
    {
        qWarning() << "LevelSqlTableModel refresh failed with error: " << m_data_query.lastError().text();
    }
    endResetModel();
}

int LevelSqlTableModel::get_level(const QString & level_string) const
{
    if (m_competition_info.competition_type.name != "Stars")
    {
        return level_string.toInt();
    }
    else
    {
        return translate_from_star_level(level_string);
    }
}

int LevelSqlTableModel::rowCount(const QModelIndex &) const
{
    return m_data_query.size();
}

int LevelSqlTableModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant LevelSqlTableModel::data(const QModelIndex &index, const int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (m_data_query.seek(index.row()))
        {
            const QSqlRecord & record = m_data_query.record();
            const QSqlField & field = record.field(index.column());
            if (not field.isNull() && field.isValid())
            {
                if (m_competition_info.competition_type.name != "Stars")
                    return field.value();
                else
                    return translate_to_star_level(field.value().toString());
            }
        }
    }

    return QVariant();
}

QString LevelSqlTableModel::translate_to_star_level(const QString & level)
{
    if (level == "1") return "Förberedande";
    else if (level == "2") return "Järn";
    else if (level == "3") return "Brons";
    else if (level == "4") return "Silver";
    else if (level == "5") return "Guld";
    else return level;
}

int LevelSqlTableModel::translate_from_star_level(const QString & level)
{
    if (level == "Förberedande") return 1;
    else if (level == "Järn") return 2;
    else if (level == "Brons") return 3;
    else if (level == "Silver") return 4;
    else if (level == "Guld") return 5;
    else return 0;
}
