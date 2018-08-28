#pragma once

#include "ITeamResultItemModel.h"
#include "Translator.h"
#include "IResultsCalculator.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <boost/optional.hpp>

class TeamResultSqlItemModel : public ITeamResultItemModel
{
public:
    TeamResultSqlItemModel(const IResultsCalculatorPtr & results_calculator);

    QAbstractItemModel * get_qt_model() final {return &m_model;}

    void refresh() final;

    void set_competition(const CompetitionInfo & competition_info) final;

    const TeamResultsMap & get_current_results() const final;

private:
    void update_results();

    QStandardItemModel m_model;
    Translator m_translator;
    IResultsCalculatorPtr m_results_calculator;
    boost::optional<TeamResultsMap> m_current_results;
    boost::optional<CompetitionInfo> m_current_competition;

};


