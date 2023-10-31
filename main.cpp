#include <QApplication>

#include "../TimeIsMoney-TimeIsMoneyDB_CMake/include/taskdb.hpp"

#include "externalapi.hpp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    auto db = TaskDB();
    
    auto dbptr = db.getDBPtr();
    ExternalApi w {dbptr};
    w.show();
    return a.exec();
}
