#pragma once

#include "IJudgementModel.h"
#include <QSqlDatabase>
#include <boost/optional.hpp>

class JudgementSqlModel : public IJudgementModel
{
public:
    explicit JudgementSqlModel(QSqlDatabase & db);

    void set_competition(const CompetitionInfo & competition_info) final;
    void set_level(int level) final;
    void set_apparatus(const QString & apparatus) final;

    JudgementInfo get_current_judgement_info() const final;

    void update(const JudgementInfo & judgement_info) final;

private:
    void update_current_judgement_info();

    QSqlDatabase & m_db;
    boost::optional<CompetitionInfo> m_selected_competition;
    int m_selected_level = -1;
    QString m_selected_apparatus;

    boost::optional<JudgementInfo> m_current_judgement_info;
};
