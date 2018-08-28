#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <QDate>
#include <memory>


class ICompetitionTableModel
{
public:
    virtual ~ICompetitionTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;
    virtual void include_closed_competitions(bool include_closed_competitions) = 0;

    virtual QString get_competition_name(int index) const = 0;
    virtual bool add_competition(const CompetitionInfo & competition_info, QString & error_str) = 0;
    virtual bool delete_competition(const QString & competition_name, QString & error_str) = 0;

};

using ICompetitionTableModelPtr = std::shared_ptr<ICompetitionTableModel>;
