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

    ResultTypeInfo get_result_type(int index) const final;

    boost::optional<int> get_selected_level() const final;
    void set_level(boost::optional<int> level) final;

    ResultTypeInfo get_result_type_info(const ResultType result_type) const final;

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

private:
    static ResultType result_type_from_string(const QString & result_type_string);
    static QString string_from_result_type(const ResultType result_type);

    QSqlDatabase & m_db;
    Translator m_translator;
    std::vector<ResultTypeInfo> m_level_result_types;
    boost::optional<int> m_selected_level;

};
