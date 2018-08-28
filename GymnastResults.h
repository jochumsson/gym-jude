#pragma once

#include <QString>
#include <map>

struct GymnastResults
{
    QString gymnast_name;
    QString gymnast_id;
    QString gymnast_club;
    QString gymnast_team;

    struct Score
    {
        double final_score = 0.0;
        bool has_cop_score = false;
        double d_score = 0.0;
        double d_penalty = 0.0;
        double e_score = 0.0;
    };

    using ApparatusName = QString;
    using ApparatusScore = std::map<ApparatusName, Score>;
    ApparatusScore apparatus_score;
    double final_results = 0.0;
};
