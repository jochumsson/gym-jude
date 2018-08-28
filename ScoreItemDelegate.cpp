#include "ScoreItemDelegate.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QTextStream>
#include <QPainter>
#include <QDebug>

ScoreItemNavigator::ScoreItemNavigator(const IScoreTableModelPtr & score_model, QTableView * score_table_view) :
    m_score_model(score_model),
    m_score_table_view(score_table_view)
{
}

void ScoreItemNavigator::set_editor(QWidget * editor)
{
    m_editor = editor;
}

bool ScoreItemNavigator::eventFilter(QObject *obj, QEvent *event)
{
    Q_ASSERT(m_editor != nullptr);

    if (event->type() == QEvent::KeyPress)
    {
        const auto & key_even = static_cast< QKeyEvent& >(*event);
        if (key_even.key() == Qt::Key_Return)
        {
            QModelIndex next_index = m_score_table_view->currentIndex();
            m_editor->close();

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

ScoreItemDelegate::ScoreItemDelegate(const IScoreTableModelPtr & score_model, QTableView *parent):
    QStyledItemDelegate(parent),
    m_score_model(score_model),
    m_navigator(score_model, parent)
{
}

QWidget *ScoreItemDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &,
        const QModelIndex &index) const
{
    Q_UNUSED(index);
    auto * editor = new QLineEdit(parent);

    const QRegExp reg_exp("(?:|(?:\\d{1,2})(?:[.,]\\d{1,2})?)");//(?)");
    Q_ASSERT(reg_exp.isValid());

    auto * validator = new QRegExpValidator(reg_exp, editor);
    editor->setValidator(validator);

    editor->installEventFilter(&m_navigator);
    m_navigator.set_editor(editor);

    editor->setFrame(false);
    return editor;
}

void ScoreItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto * line_edit = static_cast<QLineEdit*>(editor);
    const auto & value = index.model()->data(index, Qt::EditRole);

    if (not value.isNull() && value.isValid())
    {
        QString text;
        QTextStream stream(&text);
        stream << value.toDouble();
        line_edit->setText(text);
    }
}

void ScoreItemDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index) const
{
    auto * line_edit = static_cast<QLineEdit*>(editor);
    const QString & text = line_edit->text();
    model->setData(index, text, Qt::EditRole);
}

void ScoreItemDelegate::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void ScoreItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const auto & column_info = m_score_model->get_column_info(index.column());
    if (column_info.field == ScoreField::None)
    {
        qWarning() << "Invalid score field";
        return Super::paint(painter, option, index);
    }

    QBrush brush = option.backgroundBrush;
    if (not column_info.is_editable)
    {
        // non editable background brush
        const QColor pale_gray(242, 242, 242);
        brush = QBrush(pale_gray);

        if (column_info.field == ScoreField::FinalScore)
        {
            const QVariant & value = index.model()->data(index);
            if (value.isValid())
            {
                // score calculated background color
                const QColor light_green(128, 195, 66);
                brush = QBrush(light_green);
            }
            else
            {
                // score not calculated background color
                brush = QBrush(Qt::red);
            }
        }
    }
    else
    {
        const QVariant & value = index.model()->data(index);
        if (value.isValid())
        {
            // edit ok background color
            const QColor light_green(128, 195, 66);
            brush = QBrush(light_green);
        }
    }

    // fill background
    painter->save();
    painter->fillRect(option.rect, brush);
    painter->restore();

    Super::paint(painter, option, index);
}
