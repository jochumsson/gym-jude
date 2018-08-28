#pragma once

#include "IGymnastModel.h"
#include <QString>
#include <QSqlDatabase>
#include <boost/optional.hpp>

class GymnastSqlModel : public IGymnastModel
{
public:
    GymnastSqlModel(QSqlDatabase & db);

    bool set_competition(const QString & competition_name) final;
    bool set_gymnast(const QString & gymnast_id, QString & error_str) final;
    bool get_gymnast_info(GymnastInfo & gymnast_info) const final;
    bool update_gymnast(const GymnastInfo & gymnast_info, QString & error_str) final;

private:
    QSqlDatabase & m_db;
    boost::optional<QString> m_competition_name;
    boost::optional<GymnastInfo> m_gymnast_info;

};
