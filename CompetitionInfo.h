#pragma once

#include <QString>
#include <QDate>

struct CompetitionInfo
{
    QString name;
    QDate date;
    bool team_competition;
    bool closed;
};
