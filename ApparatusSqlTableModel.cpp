#include "ApparatusSqlTableModel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

ApparatusSqlTableModel::ApparatusSqlTableModel(QSqlDatabase &db):
    m_sql_model_data(db)
{
}

QAbstractTableModel * ApparatusSqlTableModel::get_qt_model()
{
    return this;
}

void ApparatusSqlTableModel::refresh()
{
    if (m_competition_info.type == CompetitionType::Unknown)
    {
        // the model has not been initialized with a selected competition
        return;
    }

    beginResetModel();

    m_sql_model_data.prepare("SELECT DISTINCT apparatus_name FROM competition_type_result_apparatus WHERE competition_type=:competition_type_bind_value");
    m_sql_model_data.bindValue(":competition_type_bind_value", competitionTypeToStr(m_competition_info.type));

    if (not m_sql_model_data.exec())
    {
        qWarning() << "Failed to retrive apparatus names with sql error: "
                   << m_sql_model_data.lastError().text();
        return;
    }

    endResetModel();
}

void ApparatusSqlTableModel::set_competition(const CompetitionInfo & competition_info)
{
    m_competition_info = competition_info;
}

QString ApparatusSqlTableModel::get_apparatus_id(int index) const
{
    QString apparatus_id;

    if (m_sql_model_data.seek(index))
    {
        const QSqlRecord & record = m_sql_model_data.record();
        const QSqlField & field = record.field(0);
        if (not field.isNull() && field.isValid())
        {
            apparatus_id = field.value().toString();
        }
    }

    return apparatus_id;
}

int ApparatusSqlTableModel::get_text_index(const QString & text) const
{
    int index = -1;

    if (m_sql_model_data.first())
    {
        do
        {
            ++index;
            const QSqlRecord & record = m_sql_model_data.record();
            const QSqlField & field = record.field(0);
            if (field.isValid() &&
                    field.value().toString() == text)
            {
                break;
            }
        }
        while(m_sql_model_data.next());
    }

    return index;
}

int ApparatusSqlTableModel::rowCount(const QModelIndex &) const
{
    return m_sql_model_data.size();
}

int ApparatusSqlTableModel::columnCount(const QModelIndex &) const
{
    if (m_sql_model_data.first())
    {
        return m_sql_model_data.record().count();
    }

    return 0;
}

QVariant ApparatusSqlTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    const QString & apparatus_id = get_apparatus_id(index.row());
    if (not apparatus_id.isEmpty())
    {
        return m_translator.translate(apparatus_id);
    }

    return QVariant();
}
