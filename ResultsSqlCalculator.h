#pragma once

#include "IResultsCalculator.h"
#include <QSqlDatabase>

class ResultsSqlCalculator : public IResultsCalculator
{
public:
    explicit ResultsSqlCalculator(QSqlDatabase & db) :
        m_db(db)
    {
    }

    ResultsMap calculate_results(
            const CompetitionInfo & competition_info,
            int level,
            const QString & result_type) final;

    TeamResultsMap calculate_team_results(const CompetitionInfo & competition_info) final;

private:
    QSqlDatabase & m_db;

};
