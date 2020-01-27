#include "comment.h"
#include "post.h"
#include "user.h"

Comment::Comment(QObject *parent) : Table(parent)
{

}

NUT_FOREIGN_KEY_IMPLEMENT(Comment, Post, int, postId, setPostId, post, post, setPost)
NUT_FOREIGN_KEY_IMPLEMENT(Comment, User, int, authorId, setAuthorId, author, author, setAuthor)
