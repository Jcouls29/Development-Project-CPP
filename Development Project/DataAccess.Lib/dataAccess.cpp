#include <optional>
#include "dataAccess.h"

dataAccess::dataAccess() {

}

bool dataAccess::connect(std::string& connectionString) {
	bool connected = false;
	SQLWCHAR retConString[SQL_RETURN_CODE_LEN]{};

	// attempt to connect to SQL data source
	auto result = SQLDriverConnect(
	    connHandle, nullptr, (SQLWCHAR *)connectionString.c_str(), SQL_NTS,
	    retConString, 1024, nullptr, SQL_DRIVER_NOPROMPT);

	if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
		connected = true;
	} else {
		// free any resources allocated during connect attemp
		disconnect();
	}

	return connected;
}

void dataAccess::disconnect() {
	SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
	SQLDisconnect(connHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, connHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, envHandle);
	stmtHandle = nullptr;
	connHandle = nullptr;
	envHandle = nullptr;
}

bool dataAccess::execQuery(std::string &query, std::optional<recordSet> results) {
	bool success {false};

    if (SQLAllocHandle(SQL_HANDLE_STMT, connHandle, &stmtHandle) == SQL_SUCCESS) {
		success = SQLExecDirect(stmtHandle, (SQLWCHAR *)query.c_str(), SQL_NTS) == SQL_SUCCESS;
	}

	// if the user wants query results, they can provide an optional recordSet object to be populated
	if (results.has_value()) {
		getQueryResult(results.value());
	}

	return success;
}

// TODO: populate column names, types, and records
bool dataAccess::getQueryResult(recordSet &results) {
	bool success {false};

	SQLSMALLINT columns{}; // number of columns in query result
	SQLNumResultCols(stmtHandle, &columns);

	// TODO: get column names and types using SQLDescribeCol
	// TODO: bind columns and fetch results using SQLBindCol and SQLFetch

	return success;
}