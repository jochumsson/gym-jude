#pragma once

#include "IGymnastTableModel.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <boost/optional.hpp>
#include <vector>

class GymnastSqlTableModel:public IGymnastTableModel
{
public:
    GymnastSqlTableModel(QSqlDatabase & db);

    QAbstractTableModel * get_qt_model() final;

    void set_competition(const QString & competition_name) final;
    void set_filter_by_level(bool filter_by_level) final;
    void set_level(int level) final;
    void refresh() final;

    QString get_gymnast_id(int index) final;
    int get_gymnast_index(const QString & gymnast_id) final;

    bool add_gymnast(const GymnastInfo & gymnast_info, QString & error_log) final;
    bool delete_gymnast(const QString & gymnast_id, QString & error_log) final;

private:
    bool add_gymnast(
            const QString & apparatus,
            const GymnastInfo & gymnast_info,
            QString & error_log);
    std::vector<QString> get_apparatus_list();

    QSqlDatabase & m_db;
    QSqlQueryModel m_sql_model;
    boost::optional<QString> m_competition_name;
    bool m_filter_by_level = false;
    int m_level = 0;

};

