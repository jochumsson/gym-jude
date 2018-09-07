#pragma once

#include "IResultsCalculator.h"
#include <QSqlDatabase>

class ResultsSqlCalculator : public IResultsCalculator
{
public:
    explicit ResultsSqlCalculator(QSqlDatabase & db, const IResultTypeModelPtr & result_type_model) :
        m_db(db),
        m_result_type_model(result_type_model)
    {
    }

    ResultsMap calculate_results(const CompetitionInfo & competition_info,
            const boost::optional<int> & level,
            const ResultType result_type) final;

    TeamResultsMap calculate_team_results(
            const CompetitionInfo & competition_info,
            const ResultType result_type) final;

private:
    QSqlDatabase & m_db;
    IResultTypeModelPtr m_result_type_model;
};
