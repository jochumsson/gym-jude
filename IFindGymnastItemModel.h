#pragma once

#include <QAbstractItemView>
#include <QString>
#include <boost/optional.hpp>
#include <vector>
#include <memory>

struct FindGymnastItem
{
    QString competition_name;
    QString gymanst_name;
    QString gymnast_id;
    QString apparatus;
    boost::optional<int> level;
};

class IFindGymnastItemModel
{
public:
    virtual ~IFindGymnastItemModel() = default;

    virtual QAbstractItemModel * get_qt_model() = 0;

    virtual void set_search_name(const QString & name) = 0;

    virtual FindGymnastItem get_selection(int model_index) const = 0;

    virtual int move_selection_up() = 0;

    virtual int move_selection_down() = 0;


};

using IFindGymnastItemModelPtr = std::shared_ptr< IFindGymnastItemModel >;
