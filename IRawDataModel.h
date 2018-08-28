#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <memory>

class IRawDataModel
{
public:
    virtual ~IRawDataModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual void set_competition(const QString & competition_name) = 0;

};

using IRawDataModelPtr = std::shared_ptr<IRawDataModel>;
