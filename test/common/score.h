#ifndef SCORE_H
#define SCORE_H

#include <QUuid>
#include "table.h"

class User;
class Post;
class Score : public Nut::Table
{
    Q_OBJECT

    NUT_PRIMARY_AUTO_INCREMENT(id)
    NUT_DECLARE_FIELD(int, id, id, setId)

    NUT_DECLARE_FIELD(int, score, score, setScore)

    NUT_FOREIGN_KEY_DECLARE(Post, int, postId, setPostId, post, post, setPost)
    NUT_FOREIGN_KEY_DECLARE(User, QUuid, authorId, setAuthorId, author, author, setAuthor)

public:
    Q_INVOKABLE Score(QObject *parent = Q_NULLPTR);
};

#endif // SCORE_H
