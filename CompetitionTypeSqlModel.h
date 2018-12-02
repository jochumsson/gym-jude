#pragma once

#include <QSqlDatabase>
#include <QSqlTableModel>
#include "ICompetitionTypeModel.h"

class CompetitionTypeSqlModel : public ICompetitionTypeModel
{
public:
    CompetitionTypeSqlModel(QSqlDatabase & db);

    QAbstractItemModel * get_qt_model() final;

    void refresh() final;

private:
    QSqlTableModel m_sql_model;

};

