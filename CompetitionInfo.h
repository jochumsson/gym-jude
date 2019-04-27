#pragma once

#include <QString>
#include <QDate>

struct CompetitionType
{
    QString name;
    bool has_level;
    bool has_all_around;
};

struct CompetitionInfo
{
    QString name;
    QDate date;
    CompetitionType competition_type;
    bool team_competition;
    bool closed;
};
