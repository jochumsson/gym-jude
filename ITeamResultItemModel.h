#pragma once

#include "CompetitionInfo.h"
#include "IResultsCalculator.h"
#include <QAbstractItemModel>
#include <map>
#include <memory>
#include <stdexcept>

class IncompleteTeamResults : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;

};

class ITeamResultItemModel
{
public:
    using TeamResultsMap = IResultsCalculator::TeamResultsMap;

    virtual ~ITeamResultItemModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual void set_competition(const CompetitionInfo & competition_info) = 0;

    /**
     * @brief get_current_results returns the module current results
     * @throws IncompleteTeamResutls
     * @return current results
     */
    virtual const TeamResultsMap & get_current_results() const = 0;

};

using ITeamResultItemModelPtr = std::shared_ptr<ITeamResultItemModel>;
