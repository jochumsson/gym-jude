#include "TeamResultSqlItemModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>

TeamResultSqlItemModel::TeamResultSqlItemModel(
        const IResultsCalculatorPtr & results_calculator):
    m_results_calculator(results_calculator)
{
}

void TeamResultSqlItemModel::refresh()
{
    if (not m_current_competition) {
        qWarning() << "Failed to update team results with incomplete selections";
        return;
    }

    if (not (*m_current_competition).team_competition)
    {
        m_model.clear();
        m_current_results = boost::none;
    }
    else
    {
        // only calculate results for team competitions
        update_results();
    }
}

void TeamResultSqlItemModel::set_competition(const CompetitionInfo & competition_info)
{
    m_current_competition = competition_info;
}

const ITeamResultItemModel::TeamResultsMap &TeamResultSqlItemModel::get_current_results() const
{
    if (not m_current_results) {
        throw IncompleteTeamResults("results have not been calculated");
    }

    return *m_current_results;
}

void TeamResultSqlItemModel::update_results()
{
    m_current_results =
            m_results_calculator->calculate_team_results(
                *m_current_competition,
                ResultType::AllArround);

    // update the model
    m_model.clear();
    for (auto & team_results_it : *m_current_results)
    {
        if (m_model.columnCount() < 2) // create header only once
        {
            int index = 0;
            m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Team Name")));
            m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Team Club")));
            m_model.setHorizontalHeaderItem(index++, new QStandardItem(QObject::tr("Final Score")));
            for (auto & apparatus_score_it: team_results_it.second.apparatus_score)
            {
                const QString & translated_header = m_translator.translate(apparatus_score_it.first);
                m_model.setHorizontalHeaderItem(index++, new QStandardItem(translated_header));
            }
        }

        QList< QStandardItem* > row;

        {
            QStandardItem * name_item = new QStandardItem(team_results_it.second.team_name);
            name_item->setEditable(false);
            row.append(name_item);
        }

        {
            QStandardItem * club_item = new QStandardItem(team_results_it.second.team_club);
            club_item->setEditable(false);
            row.append(club_item);
        }

        {
            QStandardItem * results_item = new QStandardItem;
            results_item->setData(team_results_it.second.final_score, Qt::EditRole);
            results_item->setEditable(false);
            row.append(results_item);
        }

        for (auto & apparatus_score_it: team_results_it.second.apparatus_score)
        {
            QStandardItem * results_item = new QStandardItem;
            results_item->setData(apparatus_score_it.second, Qt::EditRole);
            results_item->setEditable(false);
            row.append(results_item);
        }

        m_model.appendRow(row);
    }
}

