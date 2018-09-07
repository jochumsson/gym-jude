#include "ResultsSqlCalculator.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include <boost/optional.hpp>
#include <set>

static const QString JUMP_NAME = "Jump";
static const QString JUMP_1_NAME = "Jump 1";
static const QString JUMP_2_NAME = "Jump 2";

IResultsCalculator::ResultsMap ResultsSqlCalculator::calculate_results(
        const CompetitionInfo & competition_info,
        const boost::optional<int> & level,
        const ResultType result_type)
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
    gymnast_query.exec();

    ResultTypeInfo result_type_info = m_result_type_model->get_result_type_info(result_type);
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
                                    "AND apparatus IN(SELECT apparatus FROM result WHERE result_type=:result_type_bind_value)");
        gymnast_score_query.bindValue(":competition_name_bind_value", competition_info.name);
        gymnast_score_query.bindValue(":gymnast_id_bind_value", gymnast_results.gymnast_id);
        gymnast_score_query.bindValue(":result_type_bind_value", result_type_info.result_type_string);
        gymnast_score_query.exec();

        bool add_jump_to_final_score = false;
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

            if (apparatus != JUMP_1_NAME && apparatus != JUMP_2_NAME) // jump is added later
            {
                gymnast_results.final_results += final_score;
            }
            else
            {
                add_jump_to_final_score = true;
            }
        }

        //add final mean value of Jump 1 and Jump 2 to final score
        GymnastResults::ApparatusScore::iterator jump_1_score = gymnast_results.apparatus_score.find(JUMP_1_NAME);
        GymnastResults::ApparatusScore::iterator jump_2_score = gymnast_results.apparatus_score.find(JUMP_2_NAME);

        if (add_jump_to_final_score)
        {
            if(jump_1_score != gymnast_results.apparatus_score.end() &&
                    jump_2_score != gymnast_results.apparatus_score.end())
            {
                const double jump_score =
                        (jump_1_score->second.final_score + jump_2_score->second.final_score) / static_cast<double>(2);
                gymnast_results.apparatus_score[JUMP_NAME] = {jump_score, false, 0.0, 0.0, 0.0};
            }
            else
            {
                gymnast_results.apparatus_score[JUMP_NAME] = {0.0, false, 0.0, 0.0, 0.0};
            }


            if (competition_info.type == CompetitionType::SvenskaPokalenserierna &&
                    result_type_info.result_type == ResultType::AllArround)
            {
                // if pokalen and we are calculating score for multiple apparatus then only the first jump counts
                gymnast_results.final_results += jump_1_score->second.final_score;
            }
            else
            {
                gymnast_results.final_results += gymnast_results.apparatus_score[JUMP_NAME].final_score;
            }
        }

        results_map.insert(std::make_pair(gymnast_results.final_results, gymnast_results));
    }

    return results_map;
}


ResultsSqlCalculator::TeamResultsMap
ResultsSqlCalculator::calculate_team_results(
        const CompetitionInfo & competition_info,
        const ResultType result_type)
{
    // add all indevidual results to correct team results
    std::multimap< QString, TeamResults > team_results;
    const auto & competition_level_results = calculate_results(competition_info, boost::none, result_type);
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

            if (apparatus_scores.first != JUMP_1_NAME &&
                    apparatus_scores.first != JUMP_2_NAME)
            {
                // exclude Jump 1 and Jump 2 from the final score calculation
                team_it.second.final_score += apparatus_score;
            }
        }
    }

    TeamResultsMap current_results;
    for (auto & team_results_it : team_results)
    {
        current_results.insert(std::make_pair(team_results_it.second.final_score, team_results_it.second));
    }

    return current_results;
}
