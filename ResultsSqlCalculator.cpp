#include "ResultsSqlCalculator.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>
#include <boost/optional.hpp>
#include <set>
#include <unordered_map>

IResultsCalculator::ResultsMap ResultsSqlCalculator::calculate_results(
        const CompetitionInfo & competition_info,
        const boost::optional<int> & level,
        const ResultTypeInfo result_type)
{
    if (result_type.all_around_result)
    {
        return calculate_all_around_results(competition_info, level);
    }
    else
    {
        return calculate_apparatus_results(competition_info, level, result_type);
    }
}


ResultsSqlCalculator::TeamResultsMap
ResultsSqlCalculator::calculate_team_results(const CompetitionInfo & competition_info)
{
    // add all indevidual results to correct team results
    std::multimap< QString, TeamResults > team_results;
    const auto & competition_level_results = calculate_all_around_results(competition_info, boost::none);
    for(auto indevidual_results: competition_level_results)
    {
        if (indevidual_results.second.gymnast_team.isEmpty())
        {
            qDebug() << "Gymnast "
                     << indevidual_results.second.gymnast_name
                     << " is not included in any team in the team competition "
                     << competition_info.name;
            continue;
        }

        // gymnast team
        const QString gymnast_team = indevidual_results.second.gymnast_team.trimmed();

        // find team
        auto it = team_results.find(gymnast_team);

        //if not found create
        if (it == team_results.end())
        {
            TeamResults team_result;
            team_result.team_name = gymnast_team;
            team_result.team_club = indevidual_results.second.gymnast_club.trimmed();
            it = team_results.insert(std::make_pair(gymnast_team, team_result));
        }

        //add indevidual results to team results
        it->second.gymnast_results.push_back(indevidual_results.second);
    }

    // update the team results final score
    for (auto & team_it: team_results)
    {
        using ScoreSet = std::multiset<double, std::greater<double>>;
        using ApparatusScoresMap = std::map<QString, ScoreSet>;
        ApparatusScoresMap scores;

        for (auto & gymnast_it: team_it.second.gymnast_results)
        {
            for (auto & score_it: gymnast_it.apparatus_score)
            {
                scores[score_it.first].insert(score_it.second.final_score);
            }
        }

        for (auto & apparatus_scores: scores)
        {
            double apparatus_score = 0;
            int score_count = 0;
            for (auto score_it = apparatus_scores.second.begin();
                 score_it != apparatus_scores.second.end() && score_count < 3;
                 ++score_it, ++score_count)
            {
                apparatus_score += *score_it;
            }

            team_it.second.apparatus_score.insert(std::make_pair(apparatus_scores.first, apparatus_score));
            team_it.second.final_score += apparatus_score;
        }
    }

    TeamResultsMap current_results;
    for (auto & team_results_it : team_results)
    {
        current_results.insert(std::make_pair(team_results_it.second.final_score, team_results_it.second));
    }

    return current_results;
}


ResultsSqlCalculator::ResultsMap ResultsSqlCalculator::calculate_all_around_results(
        const CompetitionInfo & competition_info,
        const boost::optional<int> & level)
{
    auto results = m_result_type_model->get_all_result_types();

    // get all apparatus results that should be part of the all around results
    std::map<QString, GymnastResults> all_results;
    for (auto apparatus_result_it: results)
    {
        if (apparatus_result_it.include_in_all_around)
        {
            auto apparatus_results = calculate_apparatus_results(competition_info, level, apparatus_result_it);
            for(auto apparatus_result_it: apparatus_results)
            {
                auto & all_results_item = all_results[apparatus_result_it.second.gymnast_id];
                if (all_results_item.gymnast_id != apparatus_result_it.second.gymnast_id)
                {
                    all_results_item.gymnast_name = apparatus_result_it.second.gymnast_name;
                    all_results_item.gymnast_id = apparatus_result_it.second.gymnast_id;
                    all_results_item.gymnast_club = apparatus_result_it.second.gymnast_club;
                    all_results_item.gymnast_team = apparatus_result_it.second.gymnast_team;
                }

                all_results_item.apparatus_score.insert(
                            apparatus_result_it.second.apparatus_score.begin(),
                            apparatus_result_it.second.apparatus_score.end());
                all_results_item.final_results += apparatus_result_it.second.final_results;
            }
        }
    }

    // calculate results for all apparatus results
    ResultsMap result_map;
    for (auto all_results_it: all_results)
    {
        result_map.insert(std::make_pair(all_results_it.second.final_results, all_results_it.second));
    }
    return result_map;
}

ResultsSqlCalculator::ResultsMap ResultsSqlCalculator::calculate_apparatus_results(
        const CompetitionInfo & competition_info,
        const boost::optional<int> & level,
        const ResultTypeInfo result_type)
{
    ResultsMap results_map;

    QSqlQuery gymnast_query(m_db);
    if (level)
    {
        gymnast_query.prepare("SELECT gymnast_name, gymnast_id, gymnast_club, gymnast_team FROM competition_gymnast "
                              "WHERE competition_name=:competition_name_bind_value AND level=:level_bind_value");
        gymnast_query.bindValue(":competition_name_bind_value", competition_info.name);
        gymnast_query.bindValue(":level_bind_value", *level);
    }
    else
    {
        gymnast_query.prepare("SELECT gymnast_name, gymnast_id, gymnast_club, gymnast_team FROM competition_gymnast "
                              "WHERE competition_name=:competition_name_bind_value");
        gymnast_query.bindValue(":competition_name_bind_value", competition_info.name);
    }

    if (not gymnast_query.exec())
    {
        qWarning() << "SQL score calculation failed with error: " << gymnast_query.lastError();
    }

    while(gymnast_query.next())
    {
        GymnastResults gymnast_results;
        gymnast_results.gymnast_name = gymnast_query.record().field(0).value().toString().trimmed();
        gymnast_results.gymnast_id = gymnast_query.record().field(1).value().toString().trimmed();
        gymnast_results.gymnast_club = gymnast_query.record().field(2).value().toString().trimmed();
        gymnast_results.gymnast_team = gymnast_query.record().field(3).value().toString().trimmed();

        QSqlQuery gymnast_score_query;
        gymnast_score_query.prepare("SELECT apparatus, final_score, d_score, d_penalty, base_score FROM competition_score_cop_view "
                                    "WHERE competition_name=:competition_name_bind_value "
                                    "AND gymnast_id=:gymnast_id_bind_value "
                                    "AND apparatus IN(SELECT apparatus_name FROM result_apparatus WHERE result_type=:result_type_bind_value)");
        gymnast_score_query.bindValue(":competition_name_bind_value", competition_info.name);
        gymnast_score_query.bindValue(":gymnast_id_bind_value", gymnast_results.gymnast_id);
        gymnast_score_query.bindValue(":result_type_bind_value", result_type.result_type_string);
        gymnast_score_query.exec();

        while(gymnast_score_query.next())
        {
            const QString & apparatus = gymnast_score_query.record().field(0).value().toString();
            const double final_score = gymnast_score_query.record().field(1).value().toDouble();

            if (level == boost::none || *level > 4)
            {
                const double d_score = gymnast_score_query.record().field(2).value().toDouble();
                const double d_penalty = gymnast_score_query.record().field(3).value().toDouble();
                const double e_score = final_score - (d_score - d_penalty);
                gymnast_results.apparatus_score[apparatus] = {final_score, true, d_score, d_penalty, e_score};
            }
            else
            {
                gymnast_results.apparatus_score[apparatus] = {final_score, false, 0.0, 0.0, 0.0};
            }

            gymnast_results.final_results += final_score;
        }

        gymnast_results.final_results = gymnast_results.final_results / gymnast_score_query.size();
        results_map.insert(std::make_pair(gymnast_results.final_results, gymnast_results));
    }

    return results_map;
}
