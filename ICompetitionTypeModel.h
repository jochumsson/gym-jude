#pragma once

#include <QAbstractItemModel>

class ICompetitionTypeModel
{
public:
    virtual ~ICompetitionTypeModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void refresh() = 0;

};
