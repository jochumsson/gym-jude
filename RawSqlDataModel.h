#pragma once

#include "IRawDataModel.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <boost/optional.hpp>

class RawSqlDataModel : public IRawDataModel
{
public:
    RawSqlDataModel(QSqlDatabase & db);

    QAbstractItemModel * get_qt_model() final;

    void refresh() final;

    void set_competition(const QString & competition_name);

private:
    QSqlDatabase & m_db;
    QSqlTableModel m_sql_model;
    boost::optional<QString> m_selected_competition;

};

