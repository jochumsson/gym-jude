#pragma once

#include "CompetitionInfo.h"
#include <QPrinter>
#include <QString>
#include <QAbstractItemModel>
#include <QWidget>

#include <boost/optional.hpp>
#include <memory>

class PrintUtility
{
public:
    static void print_results(
            const CompetitionInfo & comptetition_info,
            const boost::optional<QString> & level,
            const QString & results_type,
            QAbstractItemModel * results_item_model);

private:
    static const int page_margin_y = 100;
    static const int page_margin_x = 10;

    static std::unique_ptr<QWidget> create_results_header(
            QPrinter & printer,
            const QString & competition_name,
            const boost::optional<QString> & level,
            const QString & results_type);
    static void print_results_table(QPrinter & printer,
            std::unique_ptr<QWidget> header,
            QAbstractItemModel * results_item_model);

};
