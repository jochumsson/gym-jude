#pragma once

#include "IScoreTableModel.h"
#include <QStyledItemDelegate>
#include <QTableView>
#include <QStyledItemDelegate>

/**
 * @brief The ScoreItemNavigator class Provides navigation between editable table items
 */
class ScoreItemNavigator : public QObject
{
    Q_OBJECT

public:
    ScoreItemNavigator(const IScoreTableModelPtr & score_model, QTableView * score_table_view);
    void set_editor(QWidget * editor);

protected:
    bool eventFilter(QObject *obj, QEvent *event) final;

private:
    IScoreTableModelPtr m_score_model;
    QTableView * m_score_table_view;
    QWidget * m_editor = nullptr;

};

/**
 * @brief The ScoreItemDelegate class implements item deligate accepting double and null value
 */
class ScoreItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    using Super = QStyledItemDelegate;

public:
    explicit ScoreItemDelegate(const IScoreTableModelPtr & score_model, QTableView * parent);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const final;
    void setEditorData(QWidget *editor, const QModelIndex &index) const final;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const final;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const final;

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const final;

private:
    IScoreTableModelPtr m_score_model;
    mutable ScoreItemNavigator m_navigator;

};
