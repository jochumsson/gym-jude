#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <QString>
#include <memory>

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

    virtual ResultTypeInfo get_result_type(int index) const = 0;

    virtual void set_level(int level) = 0;

    virtual ResultTypeInfo get_result_type_info(const ResultType result_type) const = 0;
};

using IResultTypeModelPtr = std::shared_ptr<IResultTypeModel>;
