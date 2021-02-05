#ifndef MAINTEST_H
#define MAINTEST_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

#include "weblogdatabase.h"
class Post;
class User;
class BasicTest : public QObject
{
    Q_OBJECT
    WeblogDatabase db;
    int postId;
    Nut::Row<Post> post;
    Nut::Row<User> user;

public:
    explicit BasicTest(QObject *parent = nullptr);

signals:

private slots:
    void initTestCase();

    void dataScheema();
    void createUser();
    void createPost();
    void createPost2();
    void selectPublicts();
    void join();
    void selectPosts();
    void selectScoreAverage();
    void selectScoreSum();
    void selectScoreCount();
    void selectFirst();
    void selectPostsWithoutTitle();
    void selectPostIds();
    void updatePostOnTheFly();
    void testDate();
    void testLimitedQuery();
    void selectWithInvalidRelation();
    void modifyPost();
    void emptyDatabase();

    void cleanupTestCase();
};

#endif // MAINTEST_H
