#pragma once

#include "GymnastResults.h"
#include "TeamResults.h"
#include "IResultTypeModel.h"
#include "CompetitionInfo.h"
#include <map>
#include <memory>


class IResultsCalculator
{
public:

    virtual ~IResultsCalculator() = default;

    using ResultsMap = std::multimap< double, GymnastResults, std::greater<double> >;

    virtual ResultsMap calculate_results(
            const CompetitionInfo & competition_info,
            int level,
            const ResultType result_type) = 0;

    using TeamResultsMap = std::multimap< double, TeamResults, std::greater<double> >;

    virtual TeamResultsMap calculate_team_results(
            const CompetitionInfo & competition_info,
            const ResultType result_type) = 0;

};

using IResultsCalculatorPtr = std::shared_ptr<IResultsCalculator>;
