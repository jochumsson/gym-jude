#include "ScoreSqlTableModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QTextStream>
#include <QDebug>
#include <vector>

std::vector<ColumnInfo> column_info_level_less_than_five = {
    {ScoreField::GymnastStartNumber, "gymnast_start_number", "Start Number", false},
    {ScoreField::GymnastName, "gymnast_name", "Gymnast Name", false},
    {ScoreField::GymnastClub, "gymnast_club", "Gymnast Club", false},
    {ScoreField::GymnastTeam, "gymnast_team", "Gymnast Team", false},
    {ScoreField::FinalScore, "final_score", "Final Score", true},
    {ScoreField::GymnastId, "gymnast_id", "Gymnast ID", false},
};

std::vector<ColumnInfo> column_info_level_five_and_higer = {
    {ScoreField::GymnastStartNumber, "gymnast_start_number", "Start Number", false},
    {ScoreField::GymnastName, "gymnast_name", "Gymnast Name", false},
    {ScoreField::GymnastClub, "gymnast_club", "Gymnast Club", false},
    {ScoreField::GymnastTeam, "gymnast_team", "Gymnast Team", false},
    {ScoreField::FinalScore, "final_score", "Final Score", false},
    {ScoreField::BaseScore, "base_score", "Base", false},
    {ScoreField::DScore, "d_score", "D Score", true},
    {ScoreField::E1Deduction, "e1_deduction", "E1 Deduction", true},
    {ScoreField::E2Deduction, "e2_deduction", "E2 Deduction", true},
    {ScoreField::E3Deduction, "e3_deduction", "E3 Deduction", true},
    {ScoreField::DPenalty, "d_penalty", "D Panelty", true},
    {ScoreField::GymnastId, "gymnast_id", "Gymnast ID", false},
};

ScoreSqlTableModel::ScoreSqlTableModel(QSqlDatabase & db, QObject * parent):
    QAbstractTableModel(parent),
    m_db(db),
    m_data_query(db)
{
}

QAbstractTableModel * ScoreSqlTableModel::get_qt_model()
{
    return this;
}

void ScoreSqlTableModel::refresh()
{
    // do not initialize the sql query if the module has not been correctly set up first
    if ((not m_selected_competition_info.is_initialized()) ||
            m_selected_apparatus.isEmpty() ||
            m_selected_level < 0 ||
            m_column_info.empty())
        return;

    init_score_sql_query();
}

void ScoreSqlTableModel::recalculate_score()
{
    // no need for recalculation for level less than five
    if (m_selected_level < 5)
        return;

    for (int row_index = 0; row_index < m_data_query.size(); ++row_index)
    {
        calculate_score(row_index);
    }
}

void ScoreSqlTableModel::set_competition(const CompetitionInfo & competition_info)
{
    m_selected_competition_info = competition_info;
    set_column_info(); // since the column info is affected whether it is a team compitition or not
}

void ScoreSqlTableModel::set_apparatus(const QString & apparatus)
{
    m_selected_apparatus = apparatus;
}

void ScoreSqlTableModel::set_level(int level)
{
    m_selected_level = level;
    set_column_info();
}

void ScoreSqlTableModel::set_e_number(int number_e_deductions)
{
    if (m_number_e_deductions != number_e_deductions)
    {
        m_number_e_deductions = number_e_deductions;
        set_column_info();
    }
}

int ScoreSqlTableModel::rowCount(const QModelIndex &) const
{
    return m_data_query.size();
}

int ScoreSqlTableModel::columnCount(const QModelIndex &) const
{
    if (m_data_query.first())
    {
        const QSqlRecord & record = m_data_query.record();
        // -1 in order to hide gymnast_id
        return record.count() - 1;
    }

    return 0;
}

QVariant ScoreSqlTableModel::data(const QModelIndex &index, const int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (m_data_query.seek(index.row()))
        {
            const QSqlRecord & record = m_data_query.record();
            const QSqlField & field = record.field(index.column());
            if (not field.isNull() && field.isValid())
            {
                return field.value();
            }
        }
    }

    return QVariant();
}

QVariant ScoreSqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return QVariant();
        }
        else
        {
            if ((unsigned int)section < m_column_info.size())
            {
                return m_column_info[section].header_data;
            }
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags ScoreSqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (m_column_info[index.column()].is_editable)
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool ScoreSqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
    {
        return QAbstractTableModel::setData(index, value, role);
    }

    if (not m_data_query.seek(index.row()))
    {
        qWarning() << "Failed to locate updated row in data model";
        return false;
    }

    if ((unsigned int)index.column() >= m_column_info.size())
    {
        qWarning() << "Invalid column index update";
        return false;
    }

    ColumnInfo update_column = m_column_info[index.column()];

    const QSqlField & gymnast_id_field = m_data_query.record().field("gymnast_id");
    if (not gymnast_id_field.isValid())
    {
        qWarning() << "gymnast_id field could not be located in updated record";
        return false;
    }

    QString value_str = value.toString();

    if (value_str.isEmpty() || value_str.toLower() == "null")
    {
        value_str = "NULL";
    }
    else
    {
        bool is_double = false;
        value.toDouble(&is_double);
        if (not is_double)
        {
            // we can not update the score with other than double value
            return false;
        }
    }

    QString query_str;
    QTextStream query_stream(&query_str);
    query_stream << "UPDATE competition_score_cop_view SET " << update_column.data_field_name << "=" << value_str << ""
                 << " WHERE competition_name=\""<< (*m_selected_competition_info).name << "\""
                 << " AND gymnast_id=\"" << m_data_query.record().field("gymnast_id").value().toString() << "\""
                 << " AND apparatus=\"" << m_selected_apparatus << "\"";
    QSqlQuery update_query(query_str, m_db);
    if (not update_query.exec())
    {
        qWarning() << "Update query failed with error: " << update_query.lastError();
        return false;
    }

    //update the local data query
    m_data_query.prepare(get_score_sql_query((*m_selected_competition_info).type));
    if (not m_data_query.exec())
    {
        qWarning() << "SQL statement failed with error " << m_data_query.lastError();
    }

    if (update_column.field != ScoreField::FinalScore)
    {
        // if final score has not been modified directly it needs to be calculated
        calculate_score(index.row());
    }

    return true;
}

ColumnInfo ScoreSqlTableModel::get_column_info(int column_index) const
{
    if ((unsigned int)column_index < m_column_info.size())
    {
        return m_column_info[column_index];
    }

    return ColumnInfo();
}

void ScoreSqlTableModel::calculate_score(int data_model_index)
{
    const int final_score_index = get_score_field_column_index(ScoreField::FinalScore);

    try
    {
        if (not m_data_query.seek(data_model_index))
        {
            throw std::runtime_error("Invalid index");
        }

        const QSqlRecord & record = m_data_query.record();

        const double base = get_score_from_record(record, ScoreField::BaseScore);
        const double d_value = get_score_from_record(record, ScoreField::DScore);
        EValueSet e_values;
        e_values.insert(get_score_from_record(record, ScoreField::E1Deduction));
        e_values.insert(get_score_from_record(record, ScoreField::E2Deduction));

        if (m_number_e_deductions > 2)
        {
            e_values.insert(get_score_from_record(record, ScoreField::E3Deduction));
        }

        double d_penalty = 0.0;
        try {
            d_penalty = get_score_from_record(record, ScoreField::DPenalty);
        }
        catch(std::runtime_error & e)
        {
            // calculation can be done without d penalty
        }

        const double final_score = calculate_final_score_level_five_and_higher(base, d_value, e_values, d_penalty);
        setData(index(data_model_index, final_score_index), final_score, Qt::EditRole);
    }
    catch (std::runtime_error & e)
    {
        // incomplete data will clear the final_score
        setData(index(data_model_index, final_score_index), "", Qt::EditRole);
    }
}

double ScoreSqlTableModel::calculate_final_score_level_five_and_higher(
        const double base,
        const double d_value,
        const EValueSet & e_values,
        const double d_penalty)
{
    double e_deduction;
    if (e_values.size() == 2)
    {
        auto deduction_it = e_values.begin();
        e_deduction = *deduction_it;
        ++deduction_it;
        e_deduction += *deduction_it;
        e_deduction = e_deduction / 2;
    }
    else if (e_values.size() == 3)
    {
        auto deduction_it = e_values.begin();
        ++deduction_it;
        e_deduction = *deduction_it;
    }
    else
    {
        throw std::runtime_error("Invalid number of E deductions");
    }

    return base + d_value - e_deduction - d_penalty;
}

QString ScoreSqlTableModel::get_score_sql_query(const CompetitionType competition_type)
{
    QString sql_string;
    QTextStream sql_stream(&sql_string);
    sql_stream << "SELECT ";
    for (unsigned int index = 0; index < m_column_info.size(); ++index)
    {
        if (index > 0) sql_stream << ", ";
        sql_stream << m_column_info[index].data_field_name;
    }
    sql_stream << " FROM competition_score_cop_view"
               << " WHERE competition_name=\"" << (*m_selected_competition_info).name << "\"";
    if (competition_type == CompetitionType::SvenskaStegserierna)
    {
        sql_stream << " AND level=" << m_selected_level;
    }
    sql_stream << " AND apparatus=\"" << m_selected_apparatus << "\""
               << " ORDER BY gymnast_start_number";

    return sql_string;
}

void ScoreSqlTableModel::init_score_sql_query()
{
    if (not m_selected_competition_info)
        return;

    m_data_query.prepare(get_score_sql_query((*m_selected_competition_info).type));

    beginResetModel();
    if (not m_data_query.exec())
    {
        qWarning() << "SQL statement failed with error " << m_data_query.lastError();
    }
    endResetModel();
}

void ScoreSqlTableModel::set_column_info()
{
    if (m_selected_level < 5)
    {
        m_column_info = column_info_level_less_than_five;
    }
    else
    {
        m_column_info = column_info_level_five_and_higer;
    }

    if (m_selected_competition_info &&
            (*m_selected_competition_info).team_competition)
    {
        // remove team information column
        auto team_it = std::find_if(
                    m_column_info.begin(),
                    m_column_info.end(),
                    [](auto column_info){return column_info.field == ScoreField::GymnastTeam;});
        if (team_it != m_column_info.end())
        {
            m_column_info.erase(team_it);
        }
    }

    if (m_number_e_deductions < 3)
    {
        // remove e deduction field 3
        auto e3_deduction_it = std::find_if(
                    m_column_info.begin(),
                    m_column_info.end(),
                    [](auto column_info){return column_info.field == ScoreField::E3Deduction;});
        if (e3_deduction_it != m_column_info.end())
        {
            m_column_info.erase(e3_deduction_it);
        }
    }
}

double ScoreSqlTableModel::get_score_from_record(const QSqlRecord & record, ScoreField score_field) const
{
    const int score_index = get_score_field_column_index(score_field);
    const QSqlField & base_field = record.field(score_index);
    if (not base_field.isValid() || base_field.value().isNull())
    {
        throw std::runtime_error("invalid score");
    }

    bool ok = false;
    double score_value = base_field.value().toDouble(&ok);
    if (not ok)
    {
        throw std::runtime_error("failed to convert score value to double");
    }

    return score_value;
}

int ScoreSqlTableModel::get_score_field_column_index(ScoreField score_field) const
{
    for (unsigned int index = 0; index < m_column_info.size(); ++index)
    {
        if (m_column_info[index].field == score_field)
        {
            return index;
        }
    }

    return -1;
}

