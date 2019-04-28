#pragma once

#include "CompetitionInfo.h"
#include <QAbstractListModel>
#include <QString>
#include <memory>

class ICompetitionModel
{
public:
    virtual ~ICompetitionModel() = default;
    virtual bool set_competition(const QString & competition_name, QString & error_str) = 0;
    virtual bool get_competition_info(CompetitionInfo & competition_info) const = 0;
    virtual bool update_competition(
            const QString & competition_name,
            const QDate & competition_date,
            const QString & competition_type,
            bool team_competition,
            bool closed,
            QString & error_str) = 0;
};

using ICompetitionModelPtr = std::shared_ptr< ICompetitionModel >;
