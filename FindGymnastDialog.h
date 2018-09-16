#pragma once

#include "IFindGymnastItemModel.h"
#include <QDialog>

namespace Ui {
class FindGymnastDialog;
}

class FindGymnastDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindGymnastDialog(const IFindGymnastItemModelPtr & find_gymnast_model, QWidget *parent = 0);
    ~FindGymnastDialog();

private slots:
    void search_key_changed(const QString & search_key);

private:
    Ui::FindGymnastDialog *ui;
    IFindGymnastItemModelPtr m_find_gymnast_model;

};
