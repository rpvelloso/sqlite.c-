#include <cstdio>
#include <iostream>

#include "sqlite.c++.h"

struct Record {
	int id;
	sqlite3_int64 price;
	std::string desc;
	std::vector<char> data;
};

int main () {
	
	// DB
	std::string dbName = "tmp.db";
	if (dbName != ":memory:")
		remove(dbName.c_str());
	SQLiteDB db(dbName);
	
	// DDL
	db.createQuery(
		"create table test ( "
			"id number, "
			"price bigint, "
			"desc varchar, "
			"data blob );").execute();
	
	// Insert
	Record r = {1, 998798, "item 1 description", {1,2,3,4,5}};
	auto insert = db.createQuery(
			"insert into test values (?, ?, ?, ?);",
			r.id, r.price, r.desc, r.data); // binded values
	insert.execute();
	std::cout << "Row ID: " << db.lastInsertRowID() << std::endl;
	
	insert.reset(); // bind one value at a time
	insert.bind(1, 2);
	insert.bind(2, 203948394);
	insert.bind(3, "item 2 description");
	insert.bind(4, std::vector<char>{6, 7, 8, 9});
	insert.execute();
	std::cout << "Row ID: " << db.lastInsertRowID() << std::endl;
	
	insert.reset();
	insert.bindValues(
		3,
		10293812938,
		"item 3 description",
		std::vector<char>{11, 12, 13, 14, 15}); // bind all values
	insert.execute();
	std::cout << "Row ID: " << db.lastInsertRowID() << std::endl;
	
	// Query
	auto result =
			db.
			createQuery("select id, price, desc, data from test where id <> ?;", 2).
			getResult();

	while (result.next()) {
		Record rec;
		
		result.fetch(rec.id, rec.price, rec.desc, rec.data);
		std::cout << rec.id << ", " << rec.price << ", " << rec.desc << ", data: ";
		for (auto &c:rec.data)
			std::cout << (int)c << " ";
		std::cout << std::endl;
	}
}
