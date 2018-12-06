#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <QString>
#include <memory>
#include <vector>

struct ResultTypeInfo
{
    QString result_type_string;

    bool apparatus_result = false;
    bool all_around_result = false;
    bool include_in_all_around = false;
};

class IResultTypeModel
{
public:
    virtual ~IResultTypeModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;
    virtual void set_competition(const CompetitionInfo & competition_info)  = 0;

    virtual ResultTypeInfo get_result_type(unsigned int visibility_index) const = 0;
    virtual ResultTypeInfo get_result_type_info(const QString & result_type) const = 0;
    virtual std::vector<ResultTypeInfo> get_visible_result_types() const = 0;
    virtual std::vector<ResultTypeInfo> get_all_result_types() const = 0;

};

using IResultTypeModelPtr = std::shared_ptr<IResultTypeModel>;
