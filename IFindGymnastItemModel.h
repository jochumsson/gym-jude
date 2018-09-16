#pragma once

#include <QAbstractItemView>
#include <QString>
#include <vector>
#include <memory>

struct FindGymnastItem
{
    QString competition_name;
    std::string gymnast_id;
    QString gymanst_name;
    int level;
    QString apparatus;
};

class IFindGymnastItemModel
{
public:
    virtual ~IFindGymnastItemModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void set_search_name(const QString & name) = 0;

    virtual FindGymnastItem get_selection(int model_index) const = 0;

};

using IFindGymnastItemModelPtr = std::shared_ptr< IFindGymnastItemModel >;
