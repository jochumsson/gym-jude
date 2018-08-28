#pragma once

#include <QAbstractTableModel>
#include <memory>

class ILevelTableModel
{
public:
    virtual ~ILevelTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void set_competition(const QString & competition_name) = 0;
    
    virtual void refresh() = 0;

};

using ILevelTableModelPtr = std::shared_ptr<ILevelTableModel>;
