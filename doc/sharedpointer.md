Nut can compile in *shared pointer* mode or *regular* mode

In *shared pointer* mode results of queries is QList<QSharedPointer<T>> and in *regular* mode results are QList<T*>

Almost in every case shared pointer mode is better, But nut support regular mode for backward comptability. 

To compiling in *shared pointer* define **NUT_SHARED_POINTER** macro

Nut has template alias 

```cpp
#ifdef NUT_SHARED_POINTER
    template <typename T>
    using RowList = QList<QSharedPointer<T>>;

    template <typename T>
    using Row = QSharedPointer<T>;
#else
    template <typename T>
    using RowList = QList<T*>;

    template <typename T>
    using Row = T*;
#endif
```

In other words these types are defined by this table:

| Mode  | Nut::Row               | Nut::RowList                   |
|------ |-----              |---------                  |
|Regular|T*                 | QList\<T\*\>                 |
|Shared pointer|QSharedPointer\<T\>  | QList\<QSharedPointer\<T\>\>  |

For the integration of your source, you can use these aliases.

Ans also Nut::create<T>() method are defined for two mode 

```cpp
#ifdef NUT_SHARED_POINTER
    template<class T>
    inline Row<T> create(QObject *parent) {
        return QSharedPointer<T>(new T(parent));
    }
#else
    template<class T>
    inline Row<T> create() {
        return new T;
    }
#endif
```

So you can use the Nut::create function without considering in what way the library is being compiled. Example:
```cpp
auto post = Nut::create<Post>();
```

In above example if *NUT_SHARED_POINTER* is defined *post* is *QSharedPointer<Post>* else is *Post\**

I recommand use *NUT_SHARED_POINTER* always!
