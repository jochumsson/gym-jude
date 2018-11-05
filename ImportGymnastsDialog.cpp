#include "ImportGymnastsDialog.h"
#include "ui_ImportGymnastsDialog.h"

#include <QFileDialog>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QMessageBox>
#include <QUuid>
#include <QDebug>

ImportGymnastsDialog::ImportGymnastsDialog(
        const ICompetitionModelPtr & competition_model,
        const IGymnastTableModelPtr & gymnast_model,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportGymnastsDialog),
    m_competition_model(competition_model),
    m_gymnast_model(gymnast_model)
{
    ui->setupUi(this);
    ui->gymnastTableView->setModel(&m_imported_gymnast_model);

    connect(ui->csfButton, SIGNAL(clicked(bool)), this, SLOT(readCsvFile()));
    connect(ui->importButton, SIGNAL(clicked(bool)), this, SLOT(import()));
    connect(ui->clearButton, SIGNAL(clicked(bool)), this, SLOT(clear()));
    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

ImportGymnastsDialog::~ImportGymnastsDialog()
{
    delete ui;
}

void ImportGymnastsDialog::readCsvFile()
{
    QFileDialog dialog(this);

    QStringList filters;
    filters << "Participient files (*.csv)";
    dialog.setNameFilters(filters);

    dialog.setFileMode(QFileDialog::ExistingFile);

    QString error_log;
    if(dialog.exec()) {
        QString file_name = dialog.selectedFiles()[0];
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        std::vector<GymnastInfo> read_gymnast_info;
        QTextStream file_stream(&file);
        int line_nr = 0;
        while (!file_stream.atEnd())
        {
            ++line_nr;

            QString line_str = file_stream.readLine();
            QStringList line_split = line_str.split(QRegExp("[,;]"));
            if (line_split.size() != 4 &&
                    line_split.size() != 5)
            {
                QString error_string;
                QTextStream error_stream(&error_string);
                error_stream << "Invalid participant csv file, error: line "
                             << line_nr
                             << " does not contain 4 or 5 fields as expected";
                QMessageBox msg_box;
                msg_box.setText(error_string);
                msg_box.setStandardButtons(QMessageBox::Ok);
                msg_box.exec();
                return;
            }
            else if(line_split[0].toLower() == "name" ||
                    line_split[0].toLower() == "gymnast")
            {
                //ignore header
                continue;
            }
            else
            {
                GymnastInfo gymnast_info = {
                    QUuid::createUuid().toString(),
                    line_split[0],
                    line_split[1],
                    line_split[2].toInt(),
                    line_split[3].toInt(),
                    (line_split.size() > 4) ? line_split[4].trimmed() : QString(),
                };
                read_gymnast_info.push_back(gymnast_info);
            }
        }

        m_imported_gymnast_model.set_gymnasts(read_gymnast_info);

        QMessageBox msg_box;
        if (error_log.isEmpty())
        {
            msg_box.setText("CSV file read without errors");
        }
        else
        {
            msg_box.setText(error_log);
        }
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.exec();
     }
}

void ImportGymnastsDialog::import()
{
    CompetitionInfo competition_info;
    if (not m_competition_model->get_competition_info(competition_info))
    {
        qWarning() << "No competition selected";
        return;
    }
    m_gymnast_model->set_competition(competition_info.name);

    const auto & data = m_imported_gymnast_model.get_gymnasts();
    auto gymnast_it = data.begin();
    auto gymnast_end = data.end();
    while (gymnast_it != gymnast_end)
    {
        QString error_string;
        bool ok = m_gymnast_model->add_gymnast(*gymnast_it, error_string);
        if (!ok)
        {
            QString message =
                    "DB import error for gymnast " + (*gymnast_it).name + ": \n" + error_string;
            QMessageBox msg_box;
            msg_box.setText(message);
            msg_box.setStandardButtons(QMessageBox::Ok);
            msg_box.exec();
            break;
        }

        ++gymnast_it;
    }

    if (gymnast_it == gymnast_end)
    {
        QMessageBox msg_box;
        msg_box.setText("Data imported without errors");
        msg_box.setStandardButtons(QMessageBox::Ok);
        msg_box.exec();
        m_imported_gymnast_model.clear();
    }
    else
    {
        m_imported_gymnast_model.set_gymnasts({gymnast_it, gymnast_end});
    }
}

void ImportGymnastsDialog::clear()
{
    m_imported_gymnast_model.clear();
}

