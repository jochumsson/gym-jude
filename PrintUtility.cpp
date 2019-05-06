#include "PrintUtility.h"
#include <QPainter>
#include <QPrintDialog>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>

void PrintUtility::print_results(
        const CompetitionInfo & comptetition_info,
        const boost::optional<QString> & level,
        const QString & results_type,
        QAbstractItemModel * results_item_model)
{
    QPrinter printer;
    QPrintDialog dialog(&printer);
    QString print_info = QObject::tr("Print Results");
    print_info += ": " + results_type;
    dialog.setWindowTitle(print_info);
    if (dialog.exec() != QDialog::Accepted)
        return;

    auto header = create_results_header(
                printer,
                comptetition_info.name,
                level,
                results_type);
    print_results_table(printer, std::move(header), results_item_model);
}

std::unique_ptr<QWidget> PrintUtility::create_results_header(
        QPrinter & printer,
        const QString & competition_name,
        const boost::optional<QString> & level,
        const QString & results_type)
{
    auto print_header_label = std::make_unique<QLabel>();
    print_header_label->setFixedSize(printer.pageRect().width() - 2 * page_margin_x, 100);
    print_header_label->setAlignment(Qt::AlignCenter);
    print_header_label->setMargin(page_margin_x);
    print_header_label->setFont({"Arial", 18, QFont::Bold});
    print_header_label->setStyleSheet("QLabel { background-color : white;}");
    print_header_label->setFrameStyle(QFrame::NoFrame);
    print_header_label->setLineWidth(0);
    print_header_label->setMidLineWidth(0);
    QString header_text = competition_name;

    if (level)
    {
        header_text += " - ";
        header_text += QObject::tr("Level");
        header_text += " ";
        header_text += *level;
    }

    header_text += " - ";
    header_text += results_type;
    print_header_label->setText(header_text);

    return std::move(print_header_label);
}

void PrintUtility::print_results_table(
        QPrinter & printer,
        std::unique_ptr<QWidget> header,
        QAbstractItemModel * results_item_model)
{
    QPainter painter;
    painter.begin(&printer);
    painter.setBackground(QBrush(Qt::white));

    QTableView printTableView;
    printTableView.setModel(results_item_model);
    printTableView.resizeColumnsToContents();

    const int columns = results_item_model->columnCount();
    const int rows = results_item_model->rowCount();

    double width = printTableView.verticalHeader()->width();
    for(int i = 0; i < columns; ++i) {
        width += printTableView.columnWidth(i);
    }

    double height = printTableView.horizontalHeader()->height();
    for(int i = 0; i < rows; ++i) {
        height += printTableView.rowHeight(i);
    }

    printTableView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    printTableView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    printTableView.setFrameShape(QFrame::NoFrame);
    printTableView.setFixedSize(width, height);

    //scale with respect to width
    const auto page_rect = printer.pageRect();
    double scale = 1;
    if (width > page_rect.width() - (page_margin_x * 2))
    {
        scale = double(page_rect.width() - (page_margin_x * 2)) / width;
        painter.scale(scale, scale);
    }

    //print all rows
    int printed_rows = 0;
    double print_from_pos = 0;
    double print_to_pos = printTableView.horizontalHeader()->height();
    do {
        // print the header
        header->render(&painter, {page_margin_x, page_margin_y});

        while(printed_rows < rows &&
              (printTableView.rowHeight(printed_rows) + print_to_pos - print_from_pos) * scale < page_rect.height() - (page_margin_y + header->height() + page_margin_y))
        {
            print_to_pos += printTableView.rowHeight(printed_rows);
            ++printed_rows;
        }

        // print the rows that fit to this page
        printTableView.render(
                    &painter,
                    QPoint(page_margin_x, page_margin_y + header->height()),
                    QRegion(0, print_from_pos, width, print_to_pos-print_from_pos));
        print_from_pos = print_to_pos;
    }
    while (printed_rows < rows && printer.newPage());
    painter.end();
}
