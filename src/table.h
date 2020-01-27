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

#ifndef TABLE_H
#define TABLE_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include <QtCore/QMetaType>
#include <QtCore/QSet>

#include "tablemodel.h"
#include "defines.h"
#include "phrase.h"


#define NUT_FOREIGN_KEY_DECLARE(type, keytype, keyname, keywrite, name, read, write)                     \
    NUT_INFO(__nut_FIELD, keyname, 0)                                             \
    NUT_INFO(__nut_FOREIGN_KEY, keyname :: name, type)                                   \
    Nut::Row<type> m_##name; \
    keytype m_##keyname; \
    Q_PROPERTY(Nut::Row<Table> name READ _##read WRITE _##write)                                \
    Q_PROPERTY(keytype keyname READ keyname WRITE keywrite)                                \
public:                                                                        \
    void write(Nut::Row<Table> name); \
    keytype keyname() const;                                                   \
    static NUT_WRAP_NAMESPACE(FieldPhrase<keytype>)& keyname ## Field(){             \
        static NUT_WRAP_NAMESPACE(FieldPhrase<keytype>) f =                       \
                NUT_WRAP_NAMESPACE(FieldPhrase<keytype>)                          \
                        (staticMetaObject.className(), #keyname);                 \
        return f;                                                              \
    }                                                                          \
    void keywrite(keytype keyname); \
    Nut::Row<type> read() const;                          \
    void write(Nut::Row<type> name); \
    \
private: \
    Nut::Row<Table> _##read() const;                          \
    void _##write(Nut::Row<Table> name);


#define NUT_FOREIGN_KEY_IMPLEMENT(class, type, keytype, keyname, keywrite, name, read, write)                     \
    Nut::Row<type> class::read() const  {                         \
        return m_##name; \
    } \
    \
    void class::write(Nut::Row<type> name) { \
        _##write(name);\
    } \
    Nut::Row<Table> class::_##read() const { return m_##name ; }                           \
    \
    void class::_##write(Nut::Row<Table> name){                                           \
        propertyChanged(QT_STRINGIFY2(keyname));                                                \
        m_##name = qSharedPointerCast< type >( name );\
        m_##keyname = m_##name->primaryValue().value<keytype>(); \
    } \
    \
    keytype class::keyname() const{                                                         \
        if (m_##name) \
            return m_##name->primaryValue().value<keytype>(); \
        return m_##keyname;                                                       \
    }                                                                          \
    \
    void class::keywrite(keytype keyname){                                                     \
        propertyChanged(QT_STRINGIFY2(keyname));                                                \
        m_##keyname = keyname;                                                       \
        m_##name = nullptr; \
        propertyChanged(QT_STRINGIFY2(keyname));                                                \
    }


NUT_BEGIN_NAMESPACE



class Database;
class TableSetBase;
class TableModel;
class TablePrivate;
class NUT_EXPORT Table : public QObject
{
    Q_OBJECT
    QExplicitlySharedDataPointer<TablePrivate> d;

public:
    Table(QObject *parentTableSet = nullptr);
    virtual ~Table();

    enum Status{
        NewCreated,
        FeatchedFromDB,
        Added,
        Modified,
        Deleted
    };

    int save(Database *db);

    virtual QVariant primaryValue() const = 0;
    virtual void setPrimaryValue(const QVariant &value) = 0;

    Status status() const;
    void setStatus(const Status &status);

    TableSetBase *parentTableSet() const;
    void setParentTableSet(TableSetBase *parentTableSet);

    TableSetBase *childTableSet(const QString &name) const;

    QSet<QString> changedProperties() const;

    bool setParentTable(Table *master, TableModel *masterModel, TableModel *model);
signals:

public slots:

protected:
    void propertyChanged(const QString &propName);

private:
    void setModel(TableModel *model);
//    TableModel *myModel;
//    Status _status;
//    QSet<QString> _changedProperties;
    //TODO: is this removable?
//    TableSetBase *_parentTableSet;

//    QSet<TableSetBase*> childTableSets;
    void clear();
    void add(TableSetBase *);

    template<class T>
    friend class Query;

    template<class T>
    friend class TableSet;
    friend class TableSetBase;
};

NUT_END_NAMESPACE

Q_DECLARE_METATYPE(Nut::Row<Nut::Table>)

#endif // TABLE_H
