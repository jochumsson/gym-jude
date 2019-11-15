#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <memory>

class ILevelTableModel
{
public:
    virtual ~ILevelTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void set_competition(const CompetitionInfo & competition_info) = 0;
    
    virtual void refresh() = 0;

    virtual int get_level(const QString & level_string) const = 0;
};

using ILevelTableModelPtr = std::shared_ptr<ILevelTableModel>;
