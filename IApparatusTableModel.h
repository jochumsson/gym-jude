#pragma once

#include <QAbstractTableModel>
#include <memory>

class IApparatusTableModel
{
public:
    virtual ~IApparatusTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual QString get_apparatus_id(int index) const = 0;

};

using IApparatusTableModelPtr = std::shared_ptr<IApparatusTableModel>;
