#include <gtest/gtest.h>
#include <QSharedPointer>
#include <QSqlDatabase>
#include "../TimeIsMoney-TimeIsMoneyDB_CMake/include/taskdb.hpp"

#include "../externalapi_db.hpp"


namespace timeismoney
{
namespace externalApi
{
namespace
{
    class DBFixture : public testing::Test
    {
    public:
        static void SetUpTestSuite()
        {
            if (!db)
            {
                auto tmp_db = TaskDB("test_timeismoney_serial");
                db = tmp_db.getDBPtr();
            }
        }
        
        static void TearDownTestSuite()
        {
            db.reset();
        }
        
        static QSharedPointer<QSqlDatabase> db;
    };
    
    QSharedPointer<QSqlDatabase> DBFixture::db = QSharedPointer<QSqlDatabase>(new QSqlDatabase());
    
    TEST_F(DBFixture, insert_new_data_returns_1)
    {
    
    }
}
}
}