#pragma once

#include <QAbstractTableModel>
#include <memory>

class IJudgeTableModel
{
public:
    virtual ~IJudgeTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;
};

using IJudgeTableModelPtr = std::shared_ptr<IJudgeTableModel>;
