#pragma once

#include "IScoreTableModel.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <set>
#include <vector>
#include <boost/optional.hpp>

class ScoreSqlTableModel :
        public QAbstractTableModel,
        public IScoreTableModel
{
    Q_OBJECT
    using EValueSet = std::multiset< double >;

public:
    ScoreSqlTableModel(QSqlDatabase & db, QObject * parent = nullptr);

    QAbstractTableModel * get_qt_model() final;

    void refresh() final;

    void recalculate_score() final;

    void set_competition(const CompetitionInfo & competition_info) final;

    void set_apparatus(const QString & apparatus) final;

    void set_level(int level) final;

    void set_e_number(int number_e_deductions) final;

    int get_gymnast_index(const QString & gymnast_id) const final;

    int rowCount(const QModelIndex &) const final;

    int columnCount(const QModelIndex &) const final;

    QVariant data(const QModelIndex &index, const int role) const final;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const final;

    Qt::ItemFlags flags(const QModelIndex &index) const final;

    bool setData(const QModelIndex &index, const QVariant &value, int role) final;

    ColumnInfo get_column_info(int column_index) const final;

private slots:
    void calculate_score(int data_model_index);

private:
    static double calculate_final_score_level_five_and_higher(
            const double base,
            const double d_value,
            const EValueSet & e_values,
            const double d_penalty);
    QString get_score_sql_query(const CompetitionType & competition_type);
    void init_score_sql_query();
    void set_column_info();
    double get_score_from_record(const QSqlRecord & record, ScoreField score_field) const;
    int get_score_field_column_index(ScoreField score_field) const;

    QSqlDatabase & m_db;
    mutable QSqlQuery m_data_query;

    boost::optional<CompetitionInfo> m_selected_competition_info;
    QString m_selected_apparatus;
    int m_selected_level = -1;
    int m_number_e_deductions;
    std::vector<ColumnInfo> m_column_info;

};
