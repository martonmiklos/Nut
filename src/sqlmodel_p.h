#ifndef SQLMODEL_P_H
#define SQLMODEL_P_H

#include <QSharedPointer>
#include <QString>
#include "defines.h"

NUT_BEGIN_NAMESPACE

class SqlModel;
class Table;
class TableModel;
class SqlModelPrivate : public QSharedData {
public:
    explicit SqlModelPrivate(SqlModel *parent);

    QString tableName;

    RowList<Table> rows;
    TableModel *model = nullptr;
};

NUT_END_NAMESPACE

#endif // SQLMODEL_P_H
