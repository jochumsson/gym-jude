#pragma once

#include <GymnastInfo.h>
#include <QAbstractTableModel>
#include <memory>

class IGymnastTableModel
{
public:
    virtual ~IGymnastTableModel() = default;

    virtual QAbstractTableModel * get_qt_model() = 0;

    virtual void set_competition(const QString & competition_name) = 0;
    virtual void set_filter_by_level(bool filter_by_level) = 0;
    virtual void set_level(int level) = 0;
    virtual void refresh() = 0;

    virtual QString get_gymnast_id(int index) = 0;
    virtual int get_gymnast_index(const QString & gymnast_id) = 0;

    virtual bool add_gymnast(const GymnastInfo & gymnast_info, QString & error_log) = 0;
    virtual bool delete_gymnast(const QString & gymnast_id, QString & error_log) = 0;
};

using IGymnastTableModelPtr = std::shared_ptr<IGymnastTableModel>;
