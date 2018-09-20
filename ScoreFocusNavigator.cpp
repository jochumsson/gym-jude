#include "ScoreFocusNavigator.h"
#include <QKeyEvent>
#include <QModelIndex>

ScoreFocusNavigator::ScoreFocusNavigator(const IScoreTableModelPtr & score_model, QTableView * score_table_view) :
    m_score_model(score_model),
    m_score_table_view(score_table_view)
{
}

void ScoreFocusNavigator::set_editor(QWidget * editor)
{
    m_editor = editor;
}

bool ScoreFocusNavigator::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const auto & key_even = static_cast< QKeyEvent& >(*event);
        if (key_even.key() == Qt::Key_Return)
        {
            QModelIndex next_index = m_score_table_view->currentIndex();

            if (m_editor)
            {
                (*m_editor)->close();
            }

            // locate next editable index
            while(next_index.isValid())
            {
                const int next_row_index = next_index.row();
                const int next_column_index = next_index.column();
                if (not (next_index = m_score_table_view->model()->index(next_row_index, next_column_index+1)).isValid() &&
                        not (next_index = m_score_table_view->model()->index(next_row_index+1, 0)).isValid())
                {
                    break;
                }

                const auto & column_info = m_score_model->get_column_info(next_index.column());
                if (column_info.field != ScoreField::None &&
                        column_info.is_editable)
                {
                    break;
                }
            }

            m_score_table_view->setCurrentIndex(next_index);
            m_score_table_view->selectionModel()->select(next_index, QItemSelectionModel::Select);
            m_score_table_view->edit(next_index);
            return true;
        }

    }

    return QObject::eventFilter(obj, event);
}

