#pragma once

#include "GymnastResults.h"
#include <QString>
#include <list>
#include <map>

struct TeamResults
{
    QString team_name;
    QString team_club;

    std::list< GymnastResults > gymnast_results;
    std::map< QString, double > apparatus_score;
    double final_score = 0.0;
};
