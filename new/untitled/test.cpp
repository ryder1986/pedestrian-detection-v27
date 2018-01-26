#include "test.h"
#include "QDataStream"
#include "QFile"
QSampleData::QSampleData()
{
    m_nType = 0;
    m_strName = "";
}
QSampleData::~QSampleData()
{
}
//QSampleData::operator=(const QSampleData& other)
//{
//    m_nType = other.m_nType;
//    m_strName = other.m_strName;
//    return *this;
//}
#ifndef QT_NO_DATASTREAM
QDataStream& operator>>(QDataStream& in, QSampleData& data)
{
    in>>data.m_nType >> data.m_strName;
    return in;
}
QDataStream& operator<<(QDataStream& out, QSampleData& data)
{
    out << data.m_nType << data.m_strName;
    return out;
}
#endif
test::test()
{
    QSampleData d;
    d.m_nType=4;
    d.m_strName="123";

    QFile file("file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << d;
    file.close();
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in >> d;
    file.close();
}

