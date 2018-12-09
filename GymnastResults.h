#pragma once

#include "ResultTypeInfo.h"
#include <QString>
#include <vector>

struct Results
{
    struct ApparatusScore
    {
        QString apparatus_name;

        double final_score = 0.0;
        bool has_cop_score = false;
        double d_score = 0.0;
        double d_penalty = 0.0;
        double e_score = 0.0;
    };

    ResultTypeInfo result_info;
    std::vector<ApparatusScore> apparatus_score;
    double final_results = 0.0;
};

struct GymnastResults
{
    QString gymnast_name;
    QString gymnast_id;
    QString gymnast_club;
    QString gymnast_team;

    // Gymnast results can consist of one or many results (e.g. All Around results),
    // each result can consist of one or many apparatus score (e.g. Jump)
    ResultTypeInfo result_info;
    double final_results = 0.0;
    std::vector<Results> results;
};
