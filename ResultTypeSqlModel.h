#pragma once

#include "IResultTypeModel.h"
#include "Translator.h"
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QVariant>
#include <vector>

class ResultTypeSqlModel :
        public IResultTypeModel,
        public QAbstractTableModel
{
public:
    ResultTypeSqlModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void refresh() final;
    void set_competition(const CompetitionInfo & competition_info) final;

    ResultTypeInfo get_result_type(unsigned int visibility_index) const final;
    ResultTypeInfo get_result_type_info(const QString & result_type) const final;
    std::vector<ResultTypeInfo> get_visible_result_types() const final;
    std::vector<ResultTypeInfo> get_all_result_types() const final;

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

private:
    QSqlDatabase & m_db;
    Translator m_translator;
    std::vector<ResultTypeInfo> m_visible_result_types;
    std::vector<ResultTypeInfo> m_all_result_types;
    CompetitionInfo m_selected_competition_info;

};
