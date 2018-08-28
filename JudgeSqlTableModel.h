#pragma once

#include "IJudgeTableModel.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

class JudgeSqlTableModel : public IJudgeTableModel
{
public:
    JudgeSqlTableModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void refresh() final;

private:
    QSqlDatabase & m_db;
    QSqlQueryModel m_sql_model;

};
