#include "ResultSqlItemModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>
#include <math.h>

bool is_equal(double dX, double dY)
{
    const double dEpsilon = 0.000001; // or some other small number
    return fabs(dX - dY) <= dEpsilon * fabs(dX);
}

ResultSqlItemModel::ResultSqlItemModel(
        QSqlDatabase & db,
        const IResultsCalculatorPtr & results_calculator):
    m_db(db),
    m_results_calculator(results_calculator)
{
}

void ResultSqlItemModel::refresh()
{
    if (m_gui_state == GuiState::Initialized)
    {
        update_results();
    }
}

void ResultSqlItemModel::set_competition(const CompetitionInfo & competition_info)
{
    m_current_competition = competition_info;
}

void ResultSqlItemModel::set_level(boost::optional<int> level)
{
    m_current_level = level;
}

void ResultSqlItemModel::set_result_type(const ResultTypeInfo & result_type)
{
    m_result_type_info = result_type;
}

void ResultSqlItemModel::set_show_score_details(bool show_score_details)
{
    m_show_score_details = show_score_details;
}

const IResultItemModel::ResultsMap & ResultSqlItemModel::get_current_results() const
{
    return m_current_results;
}

void ResultSqlItemModel::publish_results() const
{
    if (not m_current_competition ||
            m_result_type_info.result_type_string == QString())
        return;

    QString query_str;
    QTextStream stream(&query_str);

    if (not m_current_results.empty())
    {
        stream << "REPLACE INTO competition_result VALUES";
        int pos = 0;
        double previous_results = 0.0;
        for (const auto & gym_result_it: m_current_results)
        {
            if (pos != 0)
            {
                // first item does not need separator
                stream << ",";
            }

            if (pos == 0 ||
                    not is_equal(gym_result_it.second.final_results, previous_results))
            {
                ++pos;
                previous_results = gym_result_it.second.final_results;
            }

            stream << "("
                   << "\"" << (*m_current_competition).name << "\","
                   << "\"" << m_result_type_info.result_type_string << "\","
                   << get_sql_level_value() << ","
                   << pos << ","
                   << "\"" << gym_result_it.second.gymnast_id << "\","
                   << gym_result_it.second.final_results
                   << ")";
        }

        QSqlQuery publish_query(query_str, m_db);
        if (not publish_query.exec())
        {
            qWarning() << "Failed to publish results with error " << publish_query.lastError().text();
        }
    }
}

void ResultSqlItemModel::remove_publication() const
{
    if (not m_current_competition ||
            m_result_type_info.result_type_string == QString())
        return;

    QSqlQuery query(m_db);
    if ((*m_current_competition).competition_type.has_level)
    {
        query.prepare("DELETE FROM competition_result "
                      "WHERE competition_name=:competition_name_bind_value "
                      "AND result_type=:result_type_bind_value "
                      "AND level=:level_bind_value");
        query.bindValue(":competition_name_bind_value", (*m_current_competition).name);
        query.bindValue(":result_type_bind_value", m_result_type_info.result_type_string);
        query.bindValue(":level_bind_value", get_sql_level_value());
    }
    else
    {
        query.prepare("DELETE FROM competition_result "
                      "WHERE competition_name=:competition_name_bind_value "
                      "AND result_type=:result_type_bind_value");
        query.bindValue(":competition_name_bind_value", (*m_current_competition).name);
        query.bindValue(":result_type_bind_value", m_result_type_info.result_type_string);
    }

    if (not query.exec())
    {
        qWarning() << "Failed to remove publication with error: " << query.lastError().text();
    }
}

bool ResultSqlItemModel::is_results_published() const
{
    if (m_current_results.empty() ||
            m_gui_state == GuiState::Uninitialized) return false;

    QSqlQuery sql_query(m_db);
    sql_query.prepare("SELECT gymnast_id FROM competition_result "
                      "WHERE competition_name=:competition_name_bind_value "
                      "AND result_type=:result_type_bind_value "
                      "AND level=:level_bind_value");
    sql_query.bindValue(":competition_name_bind_value", (*m_current_competition).name);
    sql_query.bindValue(":result_type_bind_value", m_result_type_info.result_type_string);
    sql_query.bindValue(":level_bind_value", get_sql_level_value());

    if (not sql_query.exec())
    {
        qWarning() << "Failed to execute publication check sql query with error: "
                   << sql_query.lastError().text();
        return false;
    }

    return (sql_query.size() > 0);
}

bool ResultSqlItemModel::is_published_results_up_to_date() const
{
    if (m_current_results.empty() ||
            m_gui_state == GuiState::Uninitialized) return false;

    QSqlQuery sql_query(m_db);
    sql_query.prepare("SELECT gymnast_id, final_score FROM competition_result "
                      "WHERE competition_name=:competition_name_bind_value "
                      "AND result_type=:result_type_bind_value "
                      "AND level=:level_bind_value "
                      "ORDER BY final_score");
    sql_query.bindValue(":competition_name_bind_value", (*m_current_competition).name);
    sql_query.bindValue(":result_type_bind_value", m_result_type_info.result_type_string);
    sql_query.bindValue(":level_bind_value", get_sql_level_value());

    if (not sql_query.exec())
    {
        qWarning() << "Failed to execute publication check sql query with error: "
                   << sql_query.lastError().text();
        return false;
    }

    if (static_cast<unsigned int>(sql_query.size()) != m_current_results.size())
    {
        return false;
    }

    while (sql_query.next())
    {
        const auto & record = sql_query.record();

        const QString & gymnast_id = record.field(0).value().toString();
        auto it = std::find_if(
                    m_current_results.begin(),
                    m_current_results.end(),
                    [gymnast_id](auto res_it){return res_it.second.gymnast_id == gymnast_id;});

        if (it == m_current_results.end())
        {
            return false;
        }

        const double & final_score = record.field(1).value().toDouble();
        const bool is_final_score_equal = is_equal(final_score, it->second.final_results);
        if (not is_final_score_equal)
        {
            return false;
        }
    }

    return true;
}

void ResultSqlItemModel::update_results()
{
    if (not m_current_competition ||
            m_result_type_info.result_type_string == QString() ) {
        qDebug() << "Failed to update results with incomplete selections";
        return;
    }

    m_current_results =
            m_results_calculator->calculate_results(
                *m_current_competition,
                m_current_level,
                m_result_type_info);
    m_model.clear();

    auto gymnast_results_it = m_current_results.begin();
    auto gymnast_results_end = m_current_results.end();

    if (gymnast_results_it != gymnast_results_end)
    {
        int index = 0;
        m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Name")));
        m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Club")));
        if ((*m_current_competition).team_competition)
        {
            m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Team")));
        }

        // add column for all around results
        m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Results")));

        const bool include_results_column = (gymnast_results_it->second.results.size() > 1);
        for (auto results_it: gymnast_results_it->second.results)
        {
            if (include_results_column)
            {
                // add results if more then one apparatus is are involved
                const QString & translated_header = m_translator.translate(results_it.result_info.result_type_string);
                m_model.setHorizontalHeaderItem(index++, new QStandardItem(translated_header));
            }

            const bool include_apparatus_column = (results_it.apparatus_score.size() > 1);
            for (auto apparatus_score_it: results_it.apparatus_score)
            {
                if (include_apparatus_column)
                {
                    const QString & translated_header = m_translator.translate(apparatus_score_it.apparatus_name);
                    m_model.setHorizontalHeaderItem(index++, new QStandardItem(translated_header));
                }

                if (m_show_score_details && apparatus_score_it.has_cop_score)
                {
                    m_model.setHorizontalHeaderItem(index++, new QStandardItem("D"));
                    m_model.setHorizontalHeaderItem(index++, new QStandardItem("-"));
                    m_model.setHorizontalHeaderItem(index++, new QStandardItem("E"));
                }
            }
        }
    }

    while (gymnast_results_it != gymnast_results_end)
    {
        QList< QStandardItem* > row;

        {
            QStandardItem * name_item = new QStandardItem(gymnast_results_it->second.gymnast_name);
            name_item->setEditable(false);
            row.append(name_item);
        }

        {
            QStandardItem * club_item = new QStandardItem(gymnast_results_it->second.gymnast_club);
            club_item->setEditable(false);
            row.append(club_item);
        }

        if ((*m_current_competition).team_competition)
        {
            QStandardItem * team_item = new QStandardItem(gymnast_results_it->second.gymnast_team);
            team_item->setEditable(false);
            row.append(team_item);
        }

        {
            QStandardItem * results_item = new QStandardItem;
            results_item->setData(gymnast_results_it->second.final_results, Qt::EditRole);
            results_item->setEditable(false);
            row.append(results_item);
        }

        const bool include_results_column = (gymnast_results_it->second.results.size() > 1);
        for (auto results_it: gymnast_results_it->second.results)
        {
            if (include_results_column)
            {
                QStandardItem * results_item = new QStandardItem;
                results_item->setData(results_it.final_results, Qt::EditRole);
                results_item->setEditable(false);
                row.append(results_item);
            }

            const bool include_apparatus_column = (results_it.apparatus_score.size() > 1);
            for (auto apparatus_score_it: results_it.apparatus_score)
            {
                if (include_apparatus_column)
                {
                    QStandardItem * score_item = new QStandardItem;
                    score_item->setData(apparatus_score_it.final_score, Qt::EditRole);
                    score_item->setEditable(false);
                    row.append(score_item);
                }

                if (m_show_score_details && apparatus_score_it.has_cop_score)
                {
                    {
                        QStandardItem * d_score_item = new QStandardItem;
                        d_score_item->setData(apparatus_score_it.d_score, Qt::EditRole);
                        d_score_item->setEditable(false);
                        row.append(d_score_item);
                    }
                    {
                        QStandardItem * d_penalty_item = new QStandardItem;
                        d_penalty_item->setData(apparatus_score_it.d_penalty, Qt::EditRole);
                        d_penalty_item->setEditable(false);
                        row.append(d_penalty_item);
                    }
                    {
                        QStandardItem * e_score_item = new QStandardItem;
                        e_score_item->setData(apparatus_score_it.e_score, Qt::EditRole);
                        e_score_item->setEditable(false);
                        row.append(e_score_item);
                    }

                }
            }
        }

        m_model.appendRow(row);
        ++gymnast_results_it;
    }
}

void ResultSqlItemModel::on_gui_state_changed(const GuiState new_state)
{
    if (new_state != m_gui_state)
    {
        m_gui_state = new_state;
        if (new_state == GuiState::Initialized)
        {
            update_results();
        }
    }
}
