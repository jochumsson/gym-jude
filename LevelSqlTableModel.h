#pragma once

#include "ILevelTableModel.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

class LevelSqlTableModel : public ILevelTableModel
{
public:
    LevelSqlTableModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void set_competition(const QString & competition_name) final;

    void refresh() final;

private:
    QSqlDatabase & m_db;
    QSqlQueryModel m_sql_model;
    QString m_competition_name;

};

