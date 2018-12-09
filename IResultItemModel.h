#pragma once

#include "CompetitionInfo.h"
#include "IResultTypeModel.h"
#include "IResultsCalculator.h"
#include <QAbstractItemModel>
#include <memory>
#include <boost/optional.hpp>
#include <stdexcept>

class IResultItemModel
{
public:
    using ResultsMap = IResultsCalculator::ResultsMap;

    virtual ~IResultItemModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual void set_competition(const CompetitionInfo & competition_info) = 0;

    virtual void set_level(boost::optional<int> level) = 0;

    virtual void set_result_type(const ResultTypeInfo & result_type) = 0;

    virtual void set_show_score_details(bool show_score_details) = 0;

    virtual const ResultsMap & get_current_results() const = 0;

    virtual void publish_results() const = 0;

    virtual void remove_publication() const = 0;

    virtual bool is_results_published() const = 0;

    virtual bool is_published_results_up_to_date() const = 0;

};

using IResultItemModelPtr = std::shared_ptr<IResultItemModel>;
