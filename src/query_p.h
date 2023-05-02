/**************************************************************************
**
** This file is part of Nut project.
** https://github.com/HamedMasafi/Nut
**
** Nut is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nut is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Nut.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#ifndef QUERY_P_H
#define QUERY_P_H

#include "phrase.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedData>

NUT_BEGIN_NAMESPACE

class Database;
class TableSetBase;
class QueryBase;
struct RelationModel;
class QueryPrivate : public QSharedData {
    QueryBase *q_ptr;
    Q_DECLARE_PUBLIC(QueryBase)

public:
    explicit QueryPrivate(QueryBase *parent);
    ~QueryPrivate();

    QString sql;
    QString className;
    QString tableName;
    QString select;
    Database *database;
    TableSetBase *tableSet;
    QStringList joins;
    QList<RelationModel*> relations;
    int skip;
    int take;
    PhraseList orderPhrase, fieldPhrase, groupByPhrase;
    ConditionalPhrase wherePhrase;
};

NUT_END_NAMESPACE

#endif // QUERY_P_H
