#pragma once

#include <QString>
#include <QDate>
#include <QDebug>

enum class CompetitionType
{
    SvenskaStegserierna,
    SvenskaPokalenserierna,
    Unknown,
};

struct CompetitionInfo
{
    QString name;
    QDate date;
    CompetitionType type;
    bool team_competition;
    bool closed;
};

inline QString competitionTypeToStr(const CompetitionType competition_type)
{
    switch(competition_type)
    {
    case CompetitionType::SvenskaPokalenserierna:
        return "Svenska Pokalenserierna";
    case CompetitionType::SvenskaStegserierna:
        return "Svenska Stegserierna";
    case CompetitionType::Unknown:
        return "Unknown";
    }

    return "";
}

inline CompetitionType strToCompetitionType(const QString & competition_type)
{
    if (competition_type == "Svenska Pokalenserierna")
    {
        return CompetitionType::SvenskaPokalenserierna;
    }
    else if (competition_type == "Svenska Stegserierna")
    {
        return CompetitionType::SvenskaStegserierna;
    }
    else
    {
        qDebug() << "Invalid competition type " << competition_type;
        return CompetitionType::Unknown;
    }
}
