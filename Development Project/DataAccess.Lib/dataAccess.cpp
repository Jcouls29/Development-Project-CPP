#include <iostream>
#include <optional>
#include <algorithm>
#include "dataAccess.h"

dataAccess::dataAccess() {

}

bool dataAccess::connect(std::string& connectionString) {
	bool connected = false;
	SQLCHAR retConString[SQL_RETURN_CODE_LEN]{};

	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &envHandle)) {
		disconnect();
	}
	else if (SQL_SUCCESS != SQLSetEnvAttr(envHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) {
		disconnect();
	}
	else if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, envHandle, &connHandle)) {
		disconnect();
	}
	else {
		// attempt to connect to SQL data source
		auto result = SQLDriverConnectA(
			connHandle, nullptr, (SQLCHAR *)connectionString.c_str(), SQL_NTS,
			retConString, 1024, nullptr, SQL_DRIVER_NOPROMPT);

		if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
			connected = true;
		} else {
			// free any resources allocated during connect attemp
			disconnect();
		}
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

bool dataAccess::execQuery(std::string &query, std::optional<recordSet *> results) {
	bool success {false};

    if (SQLAllocHandle(SQL_HANDLE_STMT, connHandle, &stmtHandle) == SQL_SUCCESS) {
		auto ret = SQLExecDirectA(stmtHandle, (SQLCHAR *)query.c_str(), SQL_NTS);
		success = ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO;
	}

	// if the user wants query results, they can provide an optional recordSet object to be populated
	if (results.has_value()) {
		getQueryResult(results.value());
	}

	return success;
}

// TODO: populate column names, types, and records
bool dataAccess::getQueryResult(recordSet *results) {
	bool success {false};

	SQLSMALLINT columns{}; // number of columns in query result
	SQLNumResultCols(stmtHandle, &columns);

	// get column names
	SQLCHAR col_name[40];
	SQLSMALLINT len{};
	SQLSMALLINT type{};
	SQLULEN size{};
	SQLSMALLINT scale;
	SQLSMALLINT nullable;
	std::vector<std::shared_ptr<SQLCHAR[]>> col_bufs;
	std::vector<SQLLEN> lens;
	SQLINTEGER len_or_ind{};

	for (int i = 1; i <= columns; ++i) {
		SQLDescribeColA(stmtHandle, i, col_name, 40, &len, &type, &size, &scale, &nullable);
		std::string cname{ reinterpret_cast<char*>(col_name) };
		results->columnNames.push_back(cname);
		results->columnMap[cname] = i-1;

		size = std::min<SQLULEN>(size, 1024);
		std::shared_ptr<SQLCHAR[]> col_buf(new SQLCHAR[size+1]);
		SQLBindCol(stmtHandle, i, SQL_C_CHAR, col_buf.get(), size, &len_or_ind);
		lens.push_back(len_or_ind);
		col_bufs.push_back(col_buf);
	}

	record rec;
	while (SQLFetch(stmtHandle) != SQL_NO_DATA) {
		for (int i = 0; i < columns; ++i) {

			rec.push_back(lens[i] == SQL_NULL_DATA ? "NULL" : (char *) col_bufs[i].get());

			// debug
			std::cout << (lens[i] == SQL_NULL_DATA ? "NULL" : (char *) col_bufs[i].get()) << " ";
		}
		// debug
		std::cout << std::endl;

		results->records.push_back(rec);
		rec.clear();
	}

	SQLCloseCursor(stmtHandle);

	// unbind columns
	for (int i = 0; i < columns; ++i) {
		SQLBindCol(stmtHandle, i, 0, nullptr, 0, nullptr);
	}

	return success;
}
