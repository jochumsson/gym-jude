#pragma once

#include "IApparatusTableModel.h"
#include "Translator.h"
#include <QSqlDatabase>
#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QString>

class ApparatusSqlTableModel :
        public IApparatusTableModel,
        public QAbstractTableModel
{
public:
    ApparatusSqlTableModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void refresh() final;
    void set_competition(const CompetitionInfo & competition_info) final;

    QString get_apparatus_id(int index) const final;

    int get_text_index(const QString & text) const final;

    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

private:
    mutable QSqlQuery m_sql_model_data;
    CompetitionInfo m_competition_info;
    Translator m_translator;

};

