#pragma once

#include "CompetitionInfo.h"
#include <QAbstractTableModel>
#include <vector>
#include <memory>

enum class ScoreField
{
    GymnastId,
    GymnastName,
    GymnastStartNumber,
    GymnastClub,
    GymnastTeam,
    FinalScore,
    DScore,
    BaseScore,
    E1Deduction,
    E2Deduction,
    E3Deduction,
    DPenalty,
    None,
};

struct ColumnInfo
{
    ScoreField field = ScoreField::None;
    QString data_field_name = "empty data field";
    QString header_data = "empty header data";
    bool is_editable = false;
};


class IScoreTableModel
{
public:
    virtual ~IScoreTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void refresh() = 0;

    virtual void recalculate_score() = 0;

    virtual void set_competition(const CompetitionInfo & competition_info) = 0;

    virtual void set_apparatus(const QString & apparatus) = 0;

    virtual void set_level(int level) = 0;

    virtual void set_e_number(int number_e_deductions) = 0;

    virtual int get_gymnast_index(const QString & gymnast_id) const = 0;

    virtual ColumnInfo get_column_info(int column_index) const = 0;

};

using IScoreTableModelPtr = std::shared_ptr<IScoreTableModel>;
