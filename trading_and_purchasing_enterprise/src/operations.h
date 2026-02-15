#pragma once
#include <string>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#ifndef operations_H 
#define operations_H

using namespace std;

sql::Connection* getConnection();

class Product {
private:
	string name;
	string category;
	int price;
	int count;
	int supplierId;
public:
	Product(const string& hname, const string& hcategory, int hprice, int hcount, int hsupplierId);

	void createNewProduct(sql::Connection* con);

	static void deleteByName(sql::Connection* con, const string& name);

	static void findByName(sql::Connection* con, const string& name);

	static void updateByName(sql::Connection* con, const string& name);

	static void showAll(sql::Connection* con);

	static void sellProduct(sql::Connection* con, int id, int count);
};

class Counterparty {
protected:
	string name;
	string phone;
	string email;
public:
	Counterparty() = default;
	Counterparty(const string& name, const string& phone, const string& email);
};

class Supplier : public Counterparty {
public:
	Supplier(const string& name, const string& phone, const string& email);

	void addSupplier(sql::Connection* con);

	static void editSupplier(sql::Connection* con, int supplierID);

	static void deleteSupplier(sql::Connection* con, int supplierId);

	static void showAllSuppliers(sql::Connection* con);

	static void showSupplier(sql::Connection* con, int supplierId);

	static void showProductsBySupplier(sql::Connection* con, int supplierId);

	static bool supplierExists(sql::Connection* con, int supplierId);
};

class Shop : public Counterparty {
public:
	Shop(const string& name, const string& phone, const string& email);

	void addShop(sql::Connection* con);
	
	static void deleteShop(sql::Connection* con, int shopId);

	static void editShop(sql::Connection* con, int ShopId);

	static void showAllShops(sql::Connection* con);

	static void showShop(sql::Connection* con, int shopId);

	static void makeOrder(sql::Connection* con, int shopId);

	static void showShopStatistics(sql::Connection* con); 
};

#endif