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

    void set_level(int level) final;

    QVariant get_result_type(int index) final;

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

private:
    bool is_all_arround(const QVariant & result_type) const;

    QSqlDatabase & m_db;
    Translator m_translator;
    std::vector<QVariant> m_level_result_types;
    int m_selected_level = -1;

};
