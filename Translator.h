#pragma once

#include <QObject>
#include <QString>
#include <map>

class Translator
{
public:
    const QString & translate(const QString & word) const
    {
        auto tranlation_it = m_translation.find(word);
        if (tranlation_it != m_translation.end())
        {
            return (*tranlation_it).second;
        }
        else
        {
            return word;
        }
    }

private:
    const std::map<QString, QString> m_translation =
    {
        {"Balance Beam", QObject::tr("Balance Beam")},
        {"Floor", QObject::tr("Floor")},
        {"Jump", QObject::tr("Jump")},
        {"Jump 1", QObject::tr("Jump 1")},
        {"Jump 2", QObject::tr("Jump 2")},
        {"Uneven Bars", QObject::tr("Uneven Bars")},
        {"WAG All Arround", QObject::tr("WAG All Arround")},
        {"Name", QObject::tr("Name")},
        {"Apparatus", QObject::tr("Apparatus")},
        {"Competition", QObject::tr("Competition")},
        {"Club", QObject::tr("Club")},
    };

};
