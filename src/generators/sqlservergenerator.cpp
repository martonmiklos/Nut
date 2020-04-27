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

#include "sqlservergenerator.h"
#include "../table.h"
#include "../tablemodel.h"

#include <QPoint>
#include <QRegularExpression>

NUT_BEGIN_NAMESPACE

SqlServerGenerator::SqlServerGenerator(Database *parent)
    : SqlGeneratorBase(parent)
{
}

QString SqlServerGenerator::masterDatabaseName(QString databaseName)
{
    return databaseName.replace(
        QRegularExpression("DATABASE\\=(\\w+)",
                           QRegularExpression::CaseInsensitiveOption),
        "DATABASE=");
}

QString SqlServerGenerator::fieldType(FieldModel *field)
{
    QString dbType;

    switch (field->type) {
    case QMetaType::Bool:
        dbType = "BIT";
        break;

    case QMetaType::Char:
    case QMetaType::QChar:
        dbType = "CHAR(1)";
        break;

    case QMetaType::SChar:
    case QMetaType::UChar:
        return "tinyint";

    case QMetaType::Short:
    case QMetaType::UShort:
        return "smallint";

    case QMetaType::UInt:
    case QMetaType::Int:
        dbType = "INT";
        if (field->isAutoIncrement)
            dbType += " IDENTITY(1,1)";
        break;

    case QMetaType::Long:
    case QMetaType::ULong:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return "bigint";

    case QMetaType::Float:
        return "FLOAT(24)";

    case QMetaType::Double:
        return "REAL";

    case QMetaType::QBitArray:
    case QMetaType::QByteArray:
        dbType = "VARBINARY";

        if (field->length)
            dbType.append(" (" + QString::number(field->length) + ")");
        else
            dbType.append(" (MAX)");
        break;
    case QMetaType::QDate:
        dbType = "DATE";
        break;
    case QMetaType::QDateTime:
        dbType = "DATETIME";
        break;
    case QMetaType::QTime:
        dbType = "TIME";
        break;

    case QMetaType::QPoint:
    case QMetaType::QPointF:
        dbType = "TEXT";
        break;

    case QMetaType::QString:
        if (field->length)
            dbType = QString("NVARCHAR(%1)").arg(field->length);
        else
            dbType = "NVARCHAR(MAX)";
        break;

    case QMetaType::QUuid:
        dbType = "UNIQUEIDENTIFIER";
        break;

    case QMetaType::QPolygon:
    case QMetaType::QPolygonF:
    case QMetaType::QSize:
    case QMetaType::QSizeF:
    case QMetaType::QRect:
    case QMetaType::QRectF:
    case QMetaType::QLine:
    case QMetaType::QLineF:
    case QMetaType::QColor:
    case QMetaType::QStringList:
    case QMetaType::QJsonArray:
    case QMetaType::QJsonValue:
    case QMetaType::QJsonObject:
    case QMetaType::QJsonDocument:
    case QMetaType::QUrl:
        return "TEXT";

    default:
//        Q_UNREACHABLE();
        dbType = QString();
    }

    return dbType;
}

QString SqlServerGenerator::escapeValue(const QVariant &v) const
{
    switch (v.type()) {
    case QVariant::String:
    case QVariant::Char:
    case QVariant::Polygon:
    case QVariant::PolygonF:
    case QVariant::Size:
    case QVariant::SizeF:
    case QVariant::Rect:
    case QVariant::RectF:
    case QVariant::Line:
    case QVariant::LineF:
    case QVariant::Color:
    case QVariant::StringList:
//    case QVariant::JsonArray:
//    case QVariant::JsonValue:
//    case QVariant::JsonObject:
//    case QVariant::JsonDocument:
    case QVariant::Url:
        return "N" + SqlGeneratorBase::escapeValue(v);

//    case QVariant::Point: {
//        QPoint pt = v.toPoint();
//        return QString("geography::POINT(%1, %2, 4326)").arg(pt.x()).arg(
//                    pt.y());
//    }
//    case QVariant::PointF: {
//        QPointF pt = v.toPointF();
//        return QString("geography::POINT(%1, %2, 4326)").arg(pt.x()).arg(
//                    pt.y());
//    }

    case QVariant::Time:
        return "'" + v.toTime().toString("HH:mm:ss") + "'";

    case QVariant::Date:
        return "'" + v.toDate().toString("yyyy-MM-dd") + "'";

    case QVariant::DateTime:
        return "'" + v.toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "'";

    default:
        break;
    }
    return SqlGeneratorBase::escapeValue(v);
}

QVariant SqlServerGenerator::unescapeValue(const QMetaType::Type &type, const QVariant &dbValue)
{
    if (type == QMetaType::QDateTime)
        return dbValue.toDateTime();

    if (type == QMetaType::QTime)
        return dbValue.toTime();

    if (type == QMetaType::QDate)
        return dbValue.toDate();

    return SqlGeneratorBase::unescapeValue(type, dbValue);
}

void SqlServerGenerator::appendSkipTake(QString &sql, int skip, int take)
{
    if (skip != -1)
        sql.append(QString(" OFFSET %1 ROWS")
                   .arg(skip));
    if (take > 0)
        sql.append(QString(" FETCH %2 %1 ROWS ONLY")
                   .arg(take)
                   .arg(skip > 1 ? "NEXT" : "FIRST"));
}

QString SqlServerGenerator::createConditionalPhrase(const PhraseData *d) const
{
    if (!d)
        return QString();

    PhraseData::Condition op = d->operatorCond;
    //apply not (!)
    if (d->isNot) {
        if (op < 20)
            op = static_cast<PhraseData::Condition>((op + 10) % 20);
    }

    if (d->type == PhraseData::WithVariant) {
        switch (op) {
        case PhraseData::AddYears:
        case PhraseData::AddYearsDateTime:
        case PhraseData::AddMonths:
        case PhraseData::AddMonthsDateTime:
        case PhraseData::AddDays:
        case PhraseData::AddDaysDateTime:
        case PhraseData::AddHours:
        case PhraseData::AddHoursDateTime:
        case PhraseData::AddMinutes:
        case PhraseData::AddMinutesDateTime:
        case PhraseData::AddSeconds:
        case PhraseData::AddSecondsDateTime:
            return QString("DATEADD(%3, %2, %1)")
                    .arg(createConditionalPhrase(d->left),
                         d->operand.toString(),
                         SqlGeneratorBase::dateTimePartName(op));

        default:
            break;
        }
    }

    if (d->type == PhraseData::WithoutOperand) {
        switch (op) {
        case PhraseData::DatePartYear:
        case PhraseData::DatePartMonth:
        case PhraseData::DatePartDay:
        case PhraseData::DatePartHour:
        case PhraseData::DatePartMinute:
        case PhraseData::DatePartSecond:
            return QString("DATEPART(%2, %1)")
                    .arg(createConditionalPhrase(d->left),
                         SqlGeneratorBase::dateTimePartName(op));

        default:
            break;
        }
    }

    return SqlGeneratorBase::createConditionalPhrase(d);
}

NUT_END_NAMESPACE
