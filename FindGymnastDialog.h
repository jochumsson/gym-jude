#pragma once

#include "IFindGymnastItemModel.h"
#include "MainWindow.h"
#include <QDialog>

namespace Ui {
class FindGymnastDialog;
}

class FindGymnastDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindGymnastDialog(const IFindGymnastItemModelPtr & find_gymnast_model, MainWindow & parent);
    ~FindGymnastDialog();

private slots:
    void search_key_changed(const QString & search_key);
    void selection_accept();

private:
    bool eventFilter(QObject *object, QEvent *event);

    Ui::FindGymnastDialog *ui;
    IFindGymnastItemModelPtr m_find_gymnast_model;
    MainWindow & m_main_window;

};
