#include <QString>
#include <QtTest>

class PostClientTest : public QObject
{
    Q_OBJECT

public:
    PostClientTest();

private Q_SLOTS:
    void testHammingCodec_data();
    void testHammingCodec();
};

PostClientTest::PostClientTest()
{
}

void PostClientTest::testHammingCodec_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void PostClientTest::testHammingCodec()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(PostClientTest)

#include "tst_postclienttest.moc"
