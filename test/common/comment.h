#ifndef COMMENT_H
#define COMMENT_H

#include <QtCore/qglobal.h>
#include <QtCore/QDateTime>
#include "table.h"

#ifdef NUT_NAMESPACE
using namespace NUT_NAMESPACE;
#endif

class Post;
class User;
class Comment : public Table
{
    Q_OBJECT

    NUT_PRIMARY_AUTO_INCREMENT(id)
    NUT_DECLARE_FIELD(int, id, id, setId)
    NUT_DECLARE_FIELD(QString, message, message, setMessage)
    NUT_DECLARE_FIELD(QDateTime, saveDate, saveDate, setSaveDate)
    NUT_DECLARE_FIELD(qreal, point, point, setPoint)

    // NUT_FOREIGN_KEY_DECLARE(type, keytype, keyname, keywrite, name, read, write)
    NUT_FOREIGN_KEY_DECLARE(Post, int, postId, setPostId, post, post, setPost)
    NUT_FOREIGN_KEY_DECLARE(User, int, authorId, setAuthorId, author, author, setAuthor)

public:
    Q_INVOKABLE explicit Comment(QObject *parentTableSet = nullptr);
};

Q_DECLARE_METATYPE(Comment*)

#endif // COMMENT_H
