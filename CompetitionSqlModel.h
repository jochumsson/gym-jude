#pragma once

#include "ICompetitionModel.h"
#include <QSqlDatabase>
#include <boost/optional.hpp>

class CompetitionSqlModel : public ICompetitionModel
{
public:
    CompetitionSqlModel(QSqlDatabase & db);

    bool set_competition(const QString & competition_name, QString & sql_error_str) final;
    bool get_competition_info(CompetitionInfo & competition_info) const final;
    bool update_competition(const CompetitionInfo & competition_info, QString & error_str) final;

private:
    QSqlDatabase & m_db;
    boost::optional< CompetitionInfo > m_current_competition_info;

};
