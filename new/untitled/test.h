#ifndef TEST_H
#define TEST_H
#include <QObject>
class QSampleData : public QObject
{
public:
    QSampleData();
    virtual ~QSampleData();
    QSampleData& operator=(const QSampleData &other);

#ifndef QT_NO_DATASTREAM
    friend QDataStream& operator>>(QDataStream&, QSampleData&);
    friend QDataStream& operator<<(QDataStream&, QSampleData&);
#endif
    //定义的数据成员
    int m_nType;
    QString m_strName;
};



class test
{
public:
    test();
};

#endif // TEST_H
