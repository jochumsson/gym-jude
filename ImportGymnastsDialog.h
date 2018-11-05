#pragma once

#include "GymnastInfo.h"
#include "ICompetitionModel.h"
#include "IGymnastTableModel.h"
#include <QDialog>
#include <QAbstractTableModel>
#include <vector>

namespace Ui {
class ImportGymnastsDialog;
}

class ImportedGymnastTableModel : public QAbstractTableModel
{
public:
    enum ColumnIndex
    {
        Name = 0,
        Club,
        Level,
        StartNumber,
        TeamName,
    };

    int rowCount(const QModelIndex & parent) const final
    {
        if (parent.isValid()) {
            return 0;
        }
        else {
            return m_imported_gymnasts.size();
        }
    }

    int columnCount(const QModelIndex &) const final
    {
        return 5;
    }

    QVariant data(const QModelIndex &index, int role) const final
    {
        if (!index.isValid())
            return QVariant();

        if (static_cast<size_t>(index.row()) >= m_imported_gymnasts.size() ||
                index.row() < 0)
            return QVariant();

        if (role == Qt::DisplayRole) {
            const GymnastInfo & gymnast_info = m_imported_gymnasts.at(index.row());

            switch(index.column()) {
            case ColumnIndex::Name:
                return gymnast_info.name;
            case ColumnIndex::Club:
                return gymnast_info.club;
            case ColumnIndex::Level:
                return gymnast_info.level;
            case ColumnIndex::StartNumber:
                return gymnast_info.start_nr;
            case ColumnIndex::TeamName:
                return gymnast_info.team;
            }
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const final
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal) {
            switch (section) {
            case ColumnIndex::Name:
                return tr("Name");
            case ColumnIndex::Club:
                return tr("Club");
            case ColumnIndex::Level:
                return tr("Level");
            case ColumnIndex::StartNumber:
                return tr("Start Number");
            case ColumnIndex::TeamName:
                return tr("Team Name");
            default:
                return QVariant();
            }
        }
        return QVariant();
    }

    const std::vector< GymnastInfo > & get_gymnasts() const
    {
        return m_imported_gymnasts;
    }

    void set_gymnasts(const std::vector<GymnastInfo> & gymnast_info)
    {
        beginResetModel();
        m_imported_gymnasts = gymnast_info;
        endResetModel();
    }

    void clear()
    {
        beginResetModel();
        m_imported_gymnasts.clear();
        endResetModel();
    }

private:
    std::vector< GymnastInfo > m_imported_gymnasts;

};

class ImportGymnastsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportGymnastsDialog(
            const ICompetitionModelPtr & competition_model,
            const IGymnastTableModelPtr & gymnast_model,
            QWidget *parent = nullptr);
    ~ImportGymnastsDialog();

private slots:
    void readCsvFile();
    void import();
    void clear();

private:
    Ui::ImportGymnastsDialog *ui;
    ICompetitionModelPtr m_competition_model;
    IGymnastTableModelPtr m_gymnast_model;
    ImportedGymnastTableModel m_imported_gymnast_model;
};
