#include "JudgementSqlModel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

JudgementSqlModel::JudgementSqlModel(QSqlDatabase & db):
    m_db(db)
{
}

void JudgementSqlModel::set_competition(const CompetitionInfo & competition_info)
{
    m_selected_competition = competition_info;
    update_current_judgement_info();
}

void JudgementSqlModel::set_level(int level)
{
    m_selected_level = level;
    update_current_judgement_info();
}

void JudgementSqlModel::set_apparatus(const QString & apparatus)
{
    m_selected_apparatus = apparatus;
    update_current_judgement_info();
}

JudgementInfo JudgementSqlModel::get_current_judgement_info() const
{
    if (not m_current_judgement_info)
        throw IncompleteSelectionError();

    return *m_current_judgement_info;
}

void JudgementSqlModel::update(const JudgementInfo & judgement_info)
{
    if (not m_selected_competition ||
            m_selected_level < 0 ||
            m_selected_apparatus.isEmpty())
    {
        throw IncompleteSelectionError();
    }

    QSqlQuery query(m_db);
    query.prepare("REPLACE INTO competition_judgement VALUES("
                  ":competition_name_bind_value,"
                  ":apparatus_bind_value,"
                  ":level_bind_value,"
                  ":judge_number_bind_value,"
                  ":judge_id_1_bind_value,"
                  ":judge_id_2_bind_value,"
                  ":judge_id_3_bind_value,"
                  ":judge_id_4_bind_value)");
    query.bindValue(":competition_name_bind_value", (*m_selected_competition).name);
    query.bindValue(":apparatus_bind_value", m_selected_apparatus);
    query.bindValue(":level_bind_value", m_selected_level);
    query.bindValue(":judge_number_bind_value", judgement_info.number_of_judges);
    query.bindValue(":judge_id_1_bind_value", judgement_info.judges[0]);
    query.bindValue(":judge_id_2_bind_value", judgement_info.judges[1]);
    query.bindValue(":judge_id_3_bind_value", judgement_info.judges[2]);
    query.bindValue(":judge_id_4_bind_value", judgement_info.judges[3]);

    if (not query.exec())
    {
        throw ModuleUpdateError(query.lastError().text().toLatin1());
    }

    m_current_judgement_info = judgement_info;
}

void JudgementSqlModel::update_current_judgement_info()
{
    if (not m_selected_competition ||
            m_selected_level < 0 ||
            m_selected_apparatus.isEmpty())
    {
        // incomplete selection
        return;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT judge_number, judge_id_1, judge_id_2, judge_id_3, judge_id_4 "
                  "FROM competition_judgement WHERE "
                  "competition_name=:competition_name_bind_value AND "
                  "apparatus=:apparatus_bind_value AND "
                  "level=:level_bind_value");
    query.bindValue(":competition_name_bind_value", (*m_selected_competition).name);
    query.bindValue(":apparatus_bind_value", m_selected_apparatus);
    query.bindValue(":level_bind_value", m_selected_level);

    if (not query.exec())
    {
        qWarning() << query.lastError().text();
        Q_ASSERT(false);
    }

    JudgementInfo judgement_info;
    if(query.next())
    {
        const QSqlRecord & record = query.record();

        const QSqlField & judge_number_field = record.field("judge_number");
        Q_ASSERT(judge_number_field.isValid());

        bool number_ok = false;
        judgement_info.number_of_judges = judge_number_field.value().toInt(&number_ok);
        Q_ASSERT(number_ok);

        const std::vector<QString> judge_fields = {
            "judge_id_1",
            "judge_id_2",
            "judge_id_3",
            "judge_id_4",
        };

        for (int index = 0; index < (int)judge_fields.size(); ++index)
        {
            const QString & value =
                    (index < judgement_info.number_of_judges)
                    ? record.field(judge_fields[index]).value().toString()
                    : "no selection";
            judgement_info.judges[index] = value;
        }
    }

    m_current_judgement_info = judgement_info;
}

