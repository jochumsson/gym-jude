#pragma once

#include "IFindGymnastItemModel.h"
#include "Translator.h"
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QItemSelectionModel>

class FindGymnastSqlItemModel:
        public QAbstractTableModel,
        public IFindGymnastItemModel
{
public:
    FindGymnastSqlItemModel(QSqlDatabase & db);

    QAbstractItemModel * get_qt_model() final;

    void set_search_name(const QString & name) final;

    FindGymnastItem get_selection(int model_index) const final;
    
    int move_selection_up() final;

    int move_selection_down() final;

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const final;

private:
    mutable QSqlQuery m_query;
    Translator m_translator;
    QItemSelectionModel m_selection_model;

};
