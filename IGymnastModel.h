#pragma once

#include "GymnastInfo.h"
#include <QString>
#include <memory>

class IGymnastModel
{
public:
    virtual ~IGymnastModel() = default;

    virtual bool set_competition(const QString & competition_name) = 0;
    virtual bool set_gymnast(const QString & gymnast_id, QString & error_str) = 0;
    virtual bool get_gymnast_info(GymnastInfo & gymnast_info) const = 0;
    virtual bool update_gymnast(const GymnastInfo & gymnast_info, QString & error_str) = 0;

};

using IGymnastModelPtr = std::shared_ptr<IGymnastModel>;
