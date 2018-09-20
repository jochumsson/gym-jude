#pragma once

#include "IScoreTableModel.h"
#include "ScoreFocusNavigator.h"
#include <QStyledItemDelegate>
#include <QTableView>
#include <QStyledItemDelegate>

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
    mutable ScoreFocusNavigator m_navigator;

};
