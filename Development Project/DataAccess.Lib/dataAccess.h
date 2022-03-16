#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <string>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include <optional>
#include "recordSet.h"

class recordSet;

class dataAccess {
public:
	dataAccess();

    bool connect(std::string& connectionString);
	void disconnect();
	bool execQuery(std::string &query, std::optional<recordSet> results);

private:
	static const int SQL_RETURN_CODE_LEN {1024+1};

    SQLHANDLE connHandle = nullptr;
    SQLHANDLE stmtHandle = nullptr;
    SQLHANDLE envHandle  = nullptr;

    bool getQueryResult(recordSet& results);
};

