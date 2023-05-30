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

#include "phrasedata.h"

NUT_BEGIN_NAMESPACE

PhraseData::PhraseData() :
    className(""), fieldName(""),
    type(Field), operatorCond(NotAssign)
{ }

PhraseData::PhraseData(const char *className, const char *fieldName) :
    className(className), fieldName(fieldName),
    type(Field), operatorCond(NotAssign),
    left(nullptr), right(nullptr)
{ }

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o)
    : type(WithoutOperand), operatorCond(o), left(l)
{
    l->parents++;
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o,
                       PhraseData *r)
    : type(WithOther), operatorCond(o), left(l), right(r)
{
    l->parents++;
    r->parents++;
}

PhraseData::PhraseData(PhraseData *l, PhraseData::Condition o, QVariant r)
    : type(WithVariant), operatorCond(o), left(l), operand(r)
{ }

PhraseData *PhraseData::operator =(PhraseData *other)
{
    other->parents++;
    return other;
}

PhraseData &PhraseData::operator =(PhraseData &other)
{
    other.parents++;
    return other;
}

QString PhraseData::toString() const
{
    return QString("[%1].%2").arg(className, fieldName);
}

void PhraseData::cleanUp()
{
}

void PhraseData::cleanUp(PhraseData *d)
{
    if (d->left)
        cleanUp(d->left);
    if (d->right)
        cleanUp(d->right);
}

NUT_END_NAMESPACE
