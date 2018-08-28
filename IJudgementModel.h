#pragma once

#include "CompetitionInfo.h"
#include "GymJudgeException.h"
#include <QString>
#include <vector>
#include <memory>

struct JudgementInfo
{
    int number_of_judges = 2;
    QString judges[4] = { "no selection", "no selection", "no selection", "no selection"};
};

class IJudgementModel
{
public:
    virtual ~IJudgementModel() = default;

    virtual void set_competition(const CompetitionInfo & competition_info) = 0;
    virtual void set_apparatus(const QString & apparatus) = 0;
    virtual void set_level(int level) = 0;

    /**
     * @return JudgementInfo for the current selection
     * @throws IncompleteSelectionException on error
     */
    virtual JudgementInfo get_current_judgement_info() const = 0;


    /**
     * @brief update Update the current selection with the given judgement info
     * @return true on success
     * @throws IncompleteSelectionException on error
     */
    virtual void update(const JudgementInfo & judgement_info) = 0;

};

using IJudgementModelPtr = std::shared_ptr<IJudgementModel>;
