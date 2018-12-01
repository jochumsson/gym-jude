#pragma once

#include "IResultItemModel.h"
#include "IGuiStateObserver.h"
#include "Translator.h"
#include "IResultsCalculator.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <boost/optional.hpp>

class ResultSqlItemModel :
        public IResultItemModel,
        public IGuiStateObserver
{
public:
    ResultSqlItemModel(QSqlDatabase & db, const IResultsCalculatorPtr & results_calculator);

    QAbstractItemModel * get_qt_model() final {return &m_model;}

    void refresh() final;

    void set_competition(const CompetitionInfo & competition_info) final;
    void set_level(boost::optional<int> level) final;
    void set_result_type(const ResultTypeInfo & result_type) final;
    void set_show_score_details(bool show_score_details) final;
    const ResultsMap & get_current_results() const final;

    void publish_results() const final;
    void remove_publication() const final;
    bool is_results_published() const final;
    bool is_published_results_up_to_date() const final;

    void on_gui_state_changed(const GuiState new_state) final;

private:
    void update_results();
    int get_sql_level_value() const
    {
        if (m_current_level)
            return *m_current_level;
        else
            return 0;
    }

    QSqlDatabase & m_db;
    QStandardItemModel m_model;
    Translator m_translator;
    IResultsCalculatorPtr m_results_calculator;
    boost::optional<ResultsMap> m_current_results;
    boost::optional<CompetitionInfo> m_current_competition;
    boost::optional<int> m_current_level;
    ResultTypeInfo m_result_type_info;
    bool m_show_score_details = false;
    GuiState m_gui_state = GuiState::Uninitialized;

};

