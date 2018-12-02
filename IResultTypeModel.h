#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <QString>
#include <memory>
#include <boost/optional.hpp>

enum class ResultType
{
    BalanceBeam,
    Floor,
    Jump,
    UnevenBars,
    AllArround,
    Unknown,
};

struct ResultTypeInfo
{
    ResultType result_type = ResultType::Unknown;
    QString result_type_string = "";
};

class IResultTypeModel
{
public:
    virtual ~IResultTypeModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;
    virtual void set_competition(const CompetitionInfo & competition_info)  = 0;

    virtual ResultTypeInfo get_result_type(int index) const = 0;

    virtual boost::optional<int> get_selected_level() const = 0;
    virtual void set_level(boost::optional<int> level) = 0;

    virtual ResultTypeInfo get_result_type_info(const ResultType result_type) const = 0;
};

using IResultTypeModelPtr = std::shared_ptr<IResultTypeModel>;
