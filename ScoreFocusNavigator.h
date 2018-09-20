#pragma once

#include "IScoreTableModel.h"
#include <QTableView>
#include <QWidget>
#include <boost/optional.hpp>

/**
 * @brief The ScoreFocusNavigator class Provides navigation between editable table items
 */
class ScoreFocusNavigator : public QObject
{
    Q_OBJECT

public:
    ScoreFocusNavigator(const IScoreTableModelPtr & score_model, QTableView * score_table_view);
    void set_editor(QWidget * editor);

protected:
    bool eventFilter(QObject *obj, QEvent *event) final;

private:
    IScoreTableModelPtr m_score_model;
    QTableView * m_score_table_view;
    boost::optional<QWidget*> m_editor;

};
