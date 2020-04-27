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

#include <QDateTime>
#include <QPoint>
#ifdef QT_GUI_LIB
#include <QPolygon>
#include <QPolygonF>
#endif
#include <QVariant>
#include <QJsonDocument>

#include "postgresqlgenerator.h"
#include "../table.h"
#include "../tablemodel.h"
#include "sqlserializer.h"

NUT_BEGIN_NAMESPACE

bool PostgreSqlGenerator::readInsideParentese(QString &text, QString &out)
{
    int start = -1;
    int end = -1;
    int pc = 0;
    for (int i = 0; i < text.length(); ++i) {
        QChar ch = text.at(i);

        if (ch == '(') {
            if (start == -1)
                start = i;
            pc++;
        }
        if (ch == ')') {
            pc--;

            if (!pc && end == -1)
                end = i;
        }
        if (start != -1 && end != -1){
            out = text.mid(start + 1, end - start - 1);
            text = text.mid(end + 1);
            return true;
        }
    }
    return false;
}

bool PostgreSqlGenerator::isPostGisType(const QVariant::Type &t) const
{
    return t == QVariant::Point
            || t == QVariant::PointF
            || t == QVariant::Rect
            || t == QVariant::RectF
            || t == QVariant::Polygon
            || t == QVariant::PolygonF;
}

PostgreSqlGenerator::PostgreSqlGenerator(Database *parent) : SqlGeneratorBase (parent)
{

}

QString PostgreSqlGenerator::fieldType(FieldModel *field)
{
    QString dbType;

    switch (field->type) {
    case QMetaType::Bool:
        dbType = "BOOLEAN";
        break;

    case QMetaType::QBitArray:
    case QMetaType::QByteArray:
        dbType = "BYTEA";
        break;
    case QMetaType::QDate:
        dbType = "DATE";
        break;
    case QMetaType::QDateTime:
        dbType = "TIMESTAMP";
        break;
    case QMetaType::QTime:
        dbType = "TIME";
        break;

    case QMetaType::SChar:
    case QMetaType::UChar:
    case QMetaType::Short:
    case QMetaType::UShort:
        dbType = "SMALLINT";
        break;

    case QMetaType::Float:
        dbType = "FLOAT";
        break;

    case QMetaType::Double:
        dbType = "REAL";
        break;

    case QMetaType::Int:
    case QMetaType::UInt:
        if(field->isAutoIncrement)
            dbType = "SERIAL";
        else
            dbType = "INTEGER";
        break;

    case QMetaType::Long:
    case QMetaType::ULong:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        if(field->isAutoIncrement)
            dbType = "BIGSERIAL";
        else
            dbType = "BIGINT";
        break;

    case QMetaType::Char:
    case QMetaType::QChar:
        return "CHAR(1)";

    case QMetaType::QString:
        if(field->length)
            dbType = QString("VARCHAR(%1)").arg(field->length);
        else
            dbType = "TEXT";
        break;

    case QMetaType::QPoint:
    case QMetaType::QPointF:
        dbType="POINT";
        break;

    case QMetaType::QUuid:
        dbType = "UUID";
        break;

    case QMetaType::QPolygon:
    case QMetaType::QPolygonF:
        dbType = "POLYGON";
        break;

    case QMetaType::QLine:
    case QMetaType::QLineF:
        return "LINE";

    case QMetaType::QRect:
    case QMetaType::QRectF:
        return "BOX";

    case QMetaType::QJsonArray:
    case QMetaType::QJsonValue:
    case QMetaType::QJsonObject:
    case QMetaType::QJsonDocument:
        return "JSONB";

    case QMetaType::QStringList:
        return "TEXT[]";

    case QMetaType::QSize:
    case QMetaType::QSizeF:
    case QMetaType::QUrl:
    case QMetaType::QColor:
        return "TEXT";

    default:
        dbType = QString();
    }

    return dbType;
}

QString PostgreSqlGenerator::escapeValue(const QVariant &v) const
{
    if (v.type() == QVariant::Time)
        return "'" + v.toTime().toString("HH:mm:ss") + "'";

    if (v.type() == QVariant::Date)
        return "'" + v.toDate().toString("yyyy-MM-dd") + "'";

    if (v.type() == QVariant::DateTime)
        return "'" + v.toDateTime().toString("yyyy-MM-dd HH:mm:ss") + "'";

    if (v.type() == QVariant::StringList)
        return "'{" + v.toStringList().join(",") + "}'";

    if (v.type() == QVariant::Point) {
        QPoint pt = v.toPoint();
        return QString("point(%1, %2)").arg(pt.x()).arg(pt.y());
    }
    if (v.type() == QVariant::PointF) {
        QPointF pt = v.toPointF();
        return QString("point(%1, %2)").arg(pt.x()).arg(pt.y());
    }
    if (v.userType() == QMetaType::QJsonDocument) {
        return "'" + QString(v.toJsonDocument().toJson(QJsonDocument::Compact)) + "'";
    }

#ifdef QT_GUI_LIB
    if (v.type() == QVariant::Polygon) {
        QString ret;
        QPoint pt;
        QPolygon pol = v.value<QPolygon>();
        for (int i = 0; i < pol.size(); ++i) {
            pt = pol.at(i);
            if (!ret.isEmpty())
                ret.append("),(");
            ret.append(QString::number(pt.x()) + ", " + QString::number(pt.y()));
        }
        return "'((" + ret + "))'";
    }
    if (v.type() == QVariant::PolygonF) {
        QString ret;
        QPointF pt;
        QPolygonF pol = v.value<QPolygonF>();
        for (int i = 0; i < pol.size(); ++i) {
            pt = pol.at(i);
            if (!ret.isEmpty())
                ret.append("),(");
            ret.append(QString::number(pt.x()) + ", " + QString::number(pt.y()));
        }
        return "'((" + ret + "))'";
    }
#endif

    return SqlGeneratorBase::escapeValue(v);
}

QVariant PostgreSqlGenerator::unescapeValue(const QMetaType::Type &type, const QVariant &dbValue)
{
    if (type == QMetaType::QDateTime)
        return dbValue.toDateTime();

    if (type == QMetaType::QTime)
        return dbValue.toTime();

    if (type == QMetaType::QDate)
        return dbValue.toDate();

    if (type == QMetaType::QPoint)
        return SqlGeneratorBase::unescapeValue(QMetaType::QPoint, dbValue.toString()
                                           .replace("(", "").replace(")", ""));
    if (type == QMetaType::QPointF)
        return SqlGeneratorBase::unescapeValue(QMetaType::QPointF, dbValue.toString()
                                           .replace("(", "").replace(")", ""));
    if (type == QMetaType::QStringList)
        return dbValue.toString().replace("{", "").replace("}", "")
                .split(",");

#ifdef QT_GUI_LIB
    if (type == QMetaType::QPolygon) {
        QString p;
        QString ref = dbValue.toString();
        QPolygon pol;
        if (!readInsideParentese(ref, p))
            return pol;

        ref = p;
        while (readInsideParentese(ref, p)) {
            QList<int> l = _serializer->toListInt(p);
            if (l.count() != 2)
                return QPolygon();
            pol.append(QPoint(l.at(0), l.at(1)));
        }
        return pol;
    }
    if (type == QMetaType::QPolygonF) {
        QString p;
        QString ref = dbValue.toString();
        QPolygonF pol;
        if (!readInsideParentese(ref, p))
            return pol;

        ref = p;
        while (readInsideParentese(ref, p)) {
            QList<qreal> l = _serializer->toListReal(p);
            if (l.count() != 2)
                return QPolygonF();
            pol.append(QPointF(l.at(0), l.at(1)));
        }
        return pol;
    }
#endif
    return SqlGeneratorBase::unescapeValue(type, dbValue);
}

QString PostgreSqlGenerator::createConditionalPhrase(const PhraseData *d) const
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
        if (isPostGisType(d->operand.type()) && d->operatorCond == PhraseData::Equal) {
            return QString("%1 ~= %2")
                    .arg(SqlGeneratorBase::createConditionalPhrase(d->left),
                         escapeValue(d->operand));
        }
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
            return QString("%1 + interval '%2 %3'")
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
            return QString("date_part('%2', %1)")
                    .arg(createConditionalPhrase(d->left),
                         SqlGeneratorBase::dateTimePartName(op));

        default:
            break;
        }
    }

    return SqlGeneratorBase::createConditionalPhrase(d);
}

NUT_END_NAMESPACE
