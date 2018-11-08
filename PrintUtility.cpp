#include "PrintUtility.h"
#include <QPainter>
#include <QPrintDialog>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>

void PrintUtility::print_results(
        QWidget * parent,
        const CompetitionInfo & comptetition_info,
        const boost::optional<QString> & level,
        const QString & results_type,
        QAbstractItemModel * results_item_model)
{
    QPrinter printer;
    QPrintDialog * dialog = new QPrintDialog(&printer, parent);
    QString print_info = QObject::tr("Print Results");
    print_info += ": " + results_type;
    dialog->setWindowTitle(print_info);
    if (dialog->exec() != QDialog::Accepted)
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
    print_header_label->setFixedSize(printer.pageRect().width() - 2 * page_margin, 100);
    print_header_label->setAlignment(Qt::AlignCenter);
    print_header_label->setMargin(page_margin);
    print_header_label->setFont({"Arial", 18, QFont::Bold});
    print_header_label->setStyleSheet("QLabel { background-color : white;}");
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
    QPainter painter(&printer);
    painter.setBackground(QBrush(Qt::white));

    QTableView printTableView;
    printTableView.setModel(results_item_model);
    printTableView.resizeColumnsToContents();

    double width = printTableView.verticalHeader()->width();
    double height = printTableView.horizontalHeader()->height();
    const int columns = results_item_model->columnCount();
    const int rows = results_item_model->rowCount();

    for(int i = 0; i < columns; ++i) {
        width += printTableView.columnWidth(i);
    }

    for(int i = 0; i < rows; ++i) {
        height += printTableView.rowHeight(i);
    }

    printTableView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    printTableView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    printTableView.setFrameShape(QFrame::NoFrame);
    printTableView.setFixedSize(width, height);

    //scale with respect to width
    double scale = 1;
    if (width > printer.pageRect().width())
    {
        scale = double(printer.pageRect().width()) / width;
        painter.scale(scale, scale);
    }

    //print all rows
    int printed_rows = 0;
    double print_from_pos = 0;
    double print_to_pos = printTableView.horizontalHeader()->height();
    do {
        // print the header
        header->render(&painter, {page_margin, page_margin});

        const double page_height = printer.pageRect().height();
        while(printed_rows < rows &&
              (printTableView.rowHeight(printed_rows) + print_to_pos - print_from_pos) * scale < page_height - (page_margin + header->height() + page_margin))
        {
            print_to_pos += printTableView.rowHeight(printed_rows);
            ++printed_rows;
        }

        // print the rows that fit to this page
        printTableView.render(
                    &painter,
                    QPoint(page_margin, page_margin + header->height()),
                    QRegion(0, print_from_pos, width, print_to_pos-print_from_pos));
        print_from_pos += print_to_pos;
    }
    while (printed_rows < rows && printer.newPage());
}
