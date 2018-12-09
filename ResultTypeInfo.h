#pragma once

#include <QString>

struct ResultTypeInfo
{
    QString result_type_string;

    bool apparatus_result = false;
    bool all_around_result = false;
    bool include_in_all_around = false;
};
