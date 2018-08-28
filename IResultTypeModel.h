#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <memory>

class IResultTypeModel
{
public:
    virtual ~IResultTypeModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual QVariant get_result_type(int index) = 0;

    virtual void set_level(int level) = 0;

};

using IResultTypeModelPtr = std::shared_ptr<IResultTypeModel>;
