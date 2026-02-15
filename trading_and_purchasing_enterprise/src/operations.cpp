#include "operations.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <iostream>

using namespace std;
using namespace sql;

Connection* getConnection() {
	try {
		mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
		Connection* con = driver->connect("tcp://localhost:3306", "YOUR_LOGIN_HERE", "YOUR_PASSWORD_HERE");
		con->setSchema("inventory");
		return con;
	}
	catch (SQLException& e) {
		cerr << "Не удалось подключиться к базе данных: " << e.what() << endl;
		exit(1);
	}
}


// 
// РЕАЛИЗАЦИЯ КЛАССА Product
//

// Реализация конструктора Product
Product::Product(const string& hname, const string& hcategory, int hprice, int hcount, int hsupplierId)
	: name(hname), category(hcategory), price(hprice), count(hcount), supplierId(hsupplierId) {}




void Product::createNewProduct(Connection* con) {
	try {

		// Создание таблицы (если не существует)
		unique_ptr<Statement> stmt(con->createStatement());
		stmt->execute(
			"CREATE TABLE IF NOT EXISTS products ("
			"id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
			"name VARCHAR(255) NOT NULL,"
			"category VARCHAR(255) NOT NULL,"
			"price INT NOT NULL,"
			"count INT NOT NULL,"
			"supplier_id INT,"
			"FOREIGN KEY (supplier_id) REFERENCES suppliers(id) ON DELETE SET NULL"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;");

		// Добавление товара
		unique_ptr<PreparedStatement> prep_stmt(
			con->prepareStatement("INSERT INTO products (name, category, price, count, supplier_id) VALUES (?, ?, ?, ?, ?)")
		);
		prep_stmt->setString(1, name);
		prep_stmt->setString(2, category);
		prep_stmt->setInt(3, price);
		prep_stmt->setInt(4, count);
		if (supplierId == 0)
			prep_stmt->setNull(5, DataType::INTEGER);
		else
			prep_stmt->setInt(5, supplierId);
		prep_stmt->executeUpdate();

		// Вывод последнего добавленного товара
		unique_ptr<PreparedStatement> last_stmt(
			con->prepareStatement("SELECT * FROM products ORDER BY id DESC LIMIT 1")
		);
		unique_ptr<ResultSet> res(last_stmt->executeQuery());

		if (res->next()) {
			cout << "Вы добавили товар:" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество: " << res->getInt("count") << endl;
		}
	}
	catch (SQLException& e) {
		cerr << "Ошибка при добавлении товара: " << e.what() << endl;
	}
}


void Product::deleteByName(Connection* con, const string& name) {
	try {
		unique_ptr<PreparedStatement> prep_stmt(
			con->prepareStatement("SELECT * FROM products WHERE name = ?")
		);
		prep_stmt->setString(1, name);
		unique_ptr<ResultSet> res(prep_stmt->executeQuery());

		int count = 0;
		int foundID;
		while (res->next()) {
			if (count == 0) {
				cout << "Найдены товары с названием \"" << name << "\":" << endl;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество: " << res->getInt("count") << endl;
			foundID = res->getInt("id");
			++count;
		}cout << endl;

		if (count == 0) {
			cout << "Товар с названием \"" << name << "\" не найден." << endl;
			return;
		}
		else if (count > 1) {
			cout << "Введите ID нужного вам товара: ";
			cin >> foundID;
		}
		size_t x;
		cout << "Вы уверены что хотите удалить товар(0-Нет, 1-Да): ";
		cin >> x;
		if (x == 1) {
			prep_stmt = unique_ptr<PreparedStatement>(
				con->prepareStatement("DELETE FROM products WHERE name = ?")
			);
			prep_stmt->setString(1, name);
			prep_stmt->execute();

			cout << "Товары с названием \"" << name << "\" успешно удалены." << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при удалении по имени: " << e.what() << endl;
	}
}

void Product::findByName(Connection* con, const string& name) {
	try {
		unique_ptr<PreparedStatement> prep_stmt(
			con->prepareStatement("SELECT * FROM products WHERE name = ?")
		);
		prep_stmt->setString(1, name);
		unique_ptr<ResultSet> res(prep_stmt->executeQuery());

		bool found = false;
		while (res->next()) {
			if (!found) {
				cout << "Найдены товары с названием \"" << name << "\":" << endl;
				found = true;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество: " << res->getInt("count") << endl;
		}

		if (!found) {
			cout << "Товар с названием \"" << name << "\" не найден." << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске товара: " << e.what() << endl;
	}
}

void Product::updateByName(Connection* con, const string& name) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM products WHERE name = ?")
		);
		pstmt->setString(1, name);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		int count = 0;
		int foundID;
		while (res->next()) {
			if (count == 0) {
				cout << "Найдены товары с названием \"" << name << "\":" << endl;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество: " << res->getInt("count") << endl;
			cout << "ID поставщика: ";
			if (res->isNull("supplier_id")) {
				cout << "отсутствует" << endl;
			}
			else {
				cout << res->getInt("supplier_id") << endl;
			}
			foundID = res->getInt("id");
			++count;
		}cout << endl;

		if (count == 0) {
			cout << "Товар с названием \"" << name << "\" не найден." << endl;
			return;
		}
		else if (count > 1) {
			cout << "Введите ID нужного вам товара: ";
			cin >> foundID;
		}
		size_t x;
		cout << "Что вы хотите изменить(1-Название, 2-Категорию, 3-Цену, 4-Количество, 5-поставщика): ";
		cin >> x;
		switch (x) {
		case 1: {
			string newName;
			cout << "Введите новое название товара: ";
			cin >> newName;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE products SET name = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newName);
			upd_stmt->setInt(2, foundID);
			upd_stmt->executeUpdate();
			cout << "Данные товара изменены";

			break;
		}
		case 2: {
			string newCategory;
			cout << "Введите новую категорию товара: ";
			cin >> newCategory;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE products SET category = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newCategory);
			upd_stmt->setInt(2, foundID);
			upd_stmt->executeUpdate();
			cout << "Данные товара изменены";

			break;
		}
		case 3: {
			int newPrice;
			cout << "Введите новую цену товара: ";
			cin >> newPrice;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE products SET price = ? WHERE id = ?")
			);
			upd_stmt->setInt(1, newPrice);
			upd_stmt->setInt(2, foundID);
			upd_stmt->executeUpdate();
			cout << "Данные товара изменены";

			break;
		}
		case 4: {
			int newCount;
			cout << "Введите новое количество товара: ";
			cin >> newCount;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE products SET count = ? WHERE id = ?")
			);
			upd_stmt->setInt(1, newCount);
			upd_stmt->setInt(2, foundID);
			upd_stmt->executeUpdate();
			cout << "Данные товара изменены";

			break;
		}
		case 5: {
			int newSupplierId;
			cout << "Введите новое ID поставщика(или 0, чтобы удалить привязку к поставщику): ";
			cin >> newSupplierId;

			if (newSupplierId != 0 && !Supplier::supplierExists(con, newSupplierId)) {
				cout << "Поставщик с таким ID не найден. Привязка будет снята." << endl;
				newSupplierId = 0;
			}

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE products SET supplier_id = ? WHERE id = ?")
			);
			if (newSupplierId == 0) {
				upd_stmt->setNull(1, sql::DataType::INTEGER);
			}
			else {
				upd_stmt->setInt(1, newSupplierId);
			}
			upd_stmt->setInt(2, foundID);
			upd_stmt->executeUpdate();
			cout << "Поставщик успешно обновлен." << endl;
			break;
		}
		}cout << endl;
	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске товара: " << e.what() << endl;
	}
}

void Product::showAll(Connection* con) {
	try {
		unique_ptr<PreparedStatement> prep_stmt(
			con->prepareStatement("SELECT * FROM products")
		);
		unique_ptr<ResultSet> res(prep_stmt->executeQuery());

		bool found = false;
		while (res->next()) {
			if (!found) {
				cout << "Список всех товаров на складе:" << endl;
				found = true;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество: " << res->getInt("count") << endl;
			cout << "ID поставщика: ";
			if (res->isNull("supplier_id")) {
				cout << "отсутствует" << endl;
			}
			else {
				cout << res->getInt("supplier_id") << endl;
			}
		}

		if (!found) {
			cout << "Товар не найдены" << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске товаров: " << e.what() << endl;
	}
}

void Product::sellProduct(Connection* con, int id, int count) {
	try {
		unique_ptr<PreparedStatement> pstmt(con->prepareStatement(
			"SELECT id, count, supplier_id FROM products WHERE id = ?")
		);
		pstmt->setInt(1, id);
		unique_ptr<ResultSet> res( pstmt->executeQuery());

		if (res->next()) {
			int currentCount = res->getInt("count");
			bool hasSupplier = !res->isNull("supplier_id");

			if (currentCount < count) {
				cout << "На складе недостаточно товара.\n";
				return;
			}

			int newCount = currentCount - count;

			unique_ptr<PreparedStatement> updateStmt(con->prepareStatement(
				"UPDATE products SET count = ? WHERE id = ?"));
			updateStmt->setInt(1, newCount);
			updateStmt->setInt(2, id);
			updateStmt->executeUpdate();

			cout << "Продажа прошла успешно.\n";

			// Если товара нет и поставщик отсутствует — удалить товар
			if (newCount == 0 && !hasSupplier) {
				unique_ptr<PreparedStatement> deleteStmt(con->prepareStatement(
					"DELETE FROM products WHERE id = ?"));
				deleteStmt->setInt(1, id);
				deleteStmt->executeUpdate();
				cout << "Товар удалён, так как он закончился и не имеет поставщика.\n";
			}
		}
		else {
			cout << "Товар не найден.\n";
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске товаров: " << e.what() << endl;
	}
}



//
// РЕАЛИЗАЦИЯ КЛАССА Counterparty
//

Counterparty::Counterparty(const string& name, const string& phone, const string& email)
	: name(name), phone(phone), email(email) {}



//
// РЕАЛИЗАЦИЯ КЛАССА Supplier
//

Supplier::Supplier(const string& name, const string& phone, const string& email)
	: Counterparty(name, phone, email) {}

void Supplier::addSupplier(Connection* con) {
	try {
		unique_ptr<Statement> stmt(con->createStatement());
		stmt->execute(
			"CREATE TABLE IF NOT EXISTS suppliers ("
			"id INT AUTO_INCREMENT PRIMARY KEY,"
			"name VARCHAR(255) NOT NULL,"
			"phone VARCHAR(15),"
			"email VARCHAR(255)"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
		);


		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("INSERT INTO suppliers (name, phone, email) VALUES (?, ?, ?)")
		);
		pstmt->setString(1, name);
		pstmt->setString(2, phone);
		pstmt->setString(3, email);
		pstmt->executeUpdate();

		// Вывод последнего добавленного товара
		unique_ptr<PreparedStatement> last_stmt(
			con->prepareStatement("SELECT * FROM suppliers ORDER BY id DESC LIMIT 1")
		);
		unique_ptr<ResultSet> res(last_stmt->executeQuery());

		if (res->next()) {
			cout << "Вы добавили поставщика:" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка добавления поставщика: " << e.what() << endl;
	}
}

void Supplier::editSupplier(Connection* con, int supplierId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM suppliers WHERE id = ?")
		);
		pstmt->setInt(1, supplierId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Найден поставщик: " << endl;
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}
		else {
			cout << "Поставщик не найден." << endl;
			return;
		}

		size_t x;
		cout << "Что вы хотите изменить(1-Название, 2-Телефон, 3-E-mail): ";
		cin >> x;
		switch (x) {
		case 1: {
			string newName;
			cout << "Введите новое название поставщика: ";
			cin >> newName;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE suppliers SET name = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newName);
			upd_stmt->setInt(2, supplierId);
			upd_stmt->executeUpdate();
			cout << "Данные поставщика изменены.";
			break;
		}
		case 2: {
			string newPhone;
			cout << "Введите новый номер телефона поставщика: ";
			cin >> newPhone;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE suppliers SET phone = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newPhone);
			upd_stmt->setInt(2, supplierId);
			upd_stmt->executeUpdate();
			cout << "Данные поставщика изменены";

			break;
		}
		case 3: {
			string newEmail;
			cout << "Введите новый E-mail поставщика: ";
			cin >> newEmail;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE suppliers SET email = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newEmail);
			upd_stmt->setInt(2, supplierId);
			upd_stmt->executeUpdate();
			cout << "Данные поставщика изменены";

			break;
		}
		}cout << endl;
	}
	catch (SQLException& e) {
		cerr << "Ошибка при изменении данных поставщика: " << e.what() << endl;
	}
}

void Supplier::deleteSupplier(Connection* con, int supplierId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM suppliers WHERE id = ?")
		);
		pstmt->setInt(1, supplierId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Найден поставщик: " << endl;
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}
		else{
			cout << "Поставщик не найден." << endl;
			return;
		}

		size_t x;
		cout << "Вы уверены что хотите удалить поставщика(0-Нет, 1-Да): ";
		cin >> x;
		if (x == 1) {
			pstmt = unique_ptr<PreparedStatement>(
				con->prepareStatement("DELETE FROM suppliers WHERE id = ?")
			);
			pstmt->setInt(1, supplierId);
			pstmt->executeUpdate();


			cout << "Поставщик успешно удален." << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при удалении поставщика: " << e.what() << endl;
	}
}

void Supplier::showAllSuppliers(Connection* con) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM suppliers")
		);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		bool found = false;
		while (res->next()) {
			if (!found) {
				cout << "Список всех поставщиков:" << endl;
				found = true;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
		}

		if (!found) {
			cout << "Поставщики не найдены" << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске поставщика: " << e.what() << endl;
	}
}

void Supplier::showSupplier(Connection* con, int supplierId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM suppliers WHERE id = ?")
		);
		pstmt->setInt(1, supplierId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Поставщик:" << endl;\
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}else {
			cout << "Поставщик не найден" << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске поставщика: " << e.what() << endl;
	}
}


void Supplier::showProductsBySupplier(Connection* con, int supplierId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT id, name, category, price, count FROM products WHERE supplier_id = ?")
		);
		pstmt->setInt(1, supplierId);

		unique_ptr<ResultSet> res(pstmt->executeQuery());

		bool hasProducts = false;
		while (res->next()) {
			if (!hasProducts) {
				cout << "Товары поставщика: " << endl << endl;
				hasProducts = true;
			}
			cout << "ID товара: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Категория: " << res->getString("category") << endl;
			cout << "Цена: " << res->getInt("price") << endl;
			cout << "Количество на складе: " << res->getInt("count") << endl;
			cout << "-----------------------------" << endl;
		}

		if (!hasProducts) {
			cout << "У этого поставщика нет привязанных товаров." << endl;
		}
	}
	catch (SQLException& e) {
		cerr << "Ошибка при выводе товаров поставщика: " << e.what() << endl;
	}
}


bool Supplier::supplierExists(Connection* con, int supplierId) {
	unique_ptr<PreparedStatement> pstmt(
		con->prepareStatement("SELECT id FROM suppliers WHERE id = ?")
	);
	pstmt->setInt(1, supplierId);
	unique_ptr<ResultSet> res(pstmt->executeQuery());

	bool exists = res->next();


	return exists;
}



//
// РЕАЛИЗАЦИЯ КЛАССА Shop
//

Shop::Shop(const string& name, const string& phone, const string& email)
	: Counterparty(name, phone, email) {}


void Shop::addShop(Connection* con) {
	try {
		unique_ptr<Statement> stmt(con->createStatement());
		stmt->execute(
			"CREATE TABLE IF NOT EXISTS shops ("
			"id INT AUTO_INCREMENT PRIMARY KEY,"
			"name VARCHAR(255) NOT NULL,"
			"phone VARCHAR(15),"
			"email VARCHAR(255)"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
		);


		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("INSERT INTO shops (name, phone, email) VALUES (?, ?, ?)")
		);
		pstmt->setString(1, name);
		pstmt->setString(2, phone);
		pstmt->setString(3, email);
		pstmt->executeUpdate();

		// Вывод последнего добавленного товара
		unique_ptr<PreparedStatement> last_stmt(
			con->prepareStatement("SELECT * FROM shops ORDER BY id DESC LIMIT 1")
		);
		unique_ptr<ResultSet> res(last_stmt->executeQuery());

		if (res->next()) {
			cout << "Вы добавили магазин:" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка добавления поставщика: " << e.what() << endl;
	}
}

void Shop::deleteShop(Connection* con, int shopId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM shops WHERE id = ?")
		);
		pstmt->setInt(1, shopId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Найден магазин: " << endl;
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}
		else {
			cout << "Магазин не найден." << endl;
			return;
		}

		size_t x;
		cout << "Вы уверены что хотите удалить магазин(0-Нет, 1-Да): ";
		cin >> x;
		if (x == 1) {
			pstmt = unique_ptr<PreparedStatement>(
				con->prepareStatement("DELETE FROM shops WHERE id = ?")
			);
			pstmt->setInt(1, shopId);
			pstmt->executeUpdate();


			cout << "Магазин успешно удален." << endl;
		}
	}
	catch (SQLException& e) {
		cerr << "Ошибка при удалении магазина: " << e.what() << endl;
	}
}

void Shop::editShop(Connection* con, int shopId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM shops WHERE id = ?")
		);
		pstmt->setInt(1, shopId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Найден магазин: " << endl;
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}
		else {
			cout << "Магазин не найден." << endl;
			return;
		}

		size_t x;
		cout << "Что вы хотите изменить(1-Название, 2-Номер телефона, 3-E-mail): ";
		cin >> x;
		switch (x) {
		case 1: {
			cin.ignore();
			string newName;
			cout << "";
			getline(cin, newName);
			cin.ignore();

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE shops SET name = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newName);
			upd_stmt->setInt(2, shopId);
			upd_stmt->executeUpdate();
			cout << "Данные магазина изменены.";
			break;
		}
		case 2: {
			string newPhone;
			cout << "Введите новый номер телефона: ";
			cin >> newPhone;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE shops SET phone = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newPhone);
			upd_stmt->setInt(2, shopId);
			upd_stmt->executeUpdate();
			cout << "Данные изменены.";
			break;
		}
		case 3: {
			string newEmail;
			cout << "Введите новый номер телефона: ";
			cin >> newEmail;

			unique_ptr<PreparedStatement> upd_stmt(
				con->prepareStatement("UPDATE shops SET email = ? WHERE id = ?")
			);
			upd_stmt->setString(1, newEmail);
			upd_stmt->setInt(2, shopId);
			upd_stmt->executeUpdate();
			cout << "Данные изменены.";
			break;
		}
		}cout << endl;

	}
	catch (SQLException& e) {
		cerr << "Ошибка при изменении данных магазина: " << e.what() << endl;
	}
}


void Shop::showAllShops(Connection* con) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM shops")
		);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		bool found = false;
		while (res->next()) {
			if (!found) {
				cout << "Список всех магазинов:" << endl;
				found = true;
			}
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
		}

		if (!found) {
			cout << "Магазины не найдены" << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске магазинов: " << e.what() << endl;
	}
}

void Shop::showShop(Connection* con, int shopId) {
	try {
		unique_ptr<PreparedStatement> pstmt(
			con->prepareStatement("SELECT * FROM shops WHERE id = ?")
		);
		pstmt->setInt(1, shopId);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			cout << "Магазин:" << endl; \
			cout << "------------------------" << endl;
			cout << "ID: " << res->getInt("id") << endl;
			cout << "Название: " << res->getString("name") << endl;
			cout << "Телефон: " << res->getString("phone") << endl;
			cout << "E-mail: " << res->getString("email") << endl;
			cout << "------------------------" << endl;
		}
		else {
			cout << "Магазин не найден" << endl;
		}

	}
	catch (SQLException& e) {
		cerr << "Ошибка при поиске магазина: " << e.what() << endl;
	}
}

void Shop::makeOrder(Connection* con, int shopId) {
	try {
		unique_ptr<Statement> stmt(con->createStatement());
		stmt->execute(
			"CREATE TABLE IF NOT EXISTS orders ("
			"id INT AUTO_INCREMENT PRIMARY KEY,"
			"shop_id INT,"
			"product_id INT,"
			"product_name VARCHAR(255),"
			"count INT,"
			"total_amount DECIMAL(10, 2),"
			"order_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
			"FOREIGN KEY(shop_id) REFERENCES shops(id)"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"
		);

		string nameProduct;
		cin.ignore();
		cout << "Введите название товара: ";
		getline(cin, nameProduct);

		Product::findByName(con, nameProduct);

		int idProduct;
		cout << "Введите ID товара: ";
		cin >> idProduct;

		unique_ptr<PreparedStatement> pstmt(con->prepareStatement(
			"SELECT id, count, price, supplier_id FROM products WHERE id = ?")
		);
		pstmt->setInt(1, idProduct);
		unique_ptr<ResultSet> res(pstmt->executeQuery());

		if (res->next()) {
			int currentCount = res->getInt("count");
			bool hasSupplier = !res->isNull("supplier_id");

			int count;
			cout << "Введите необходимое количество товаров: ";
			cin >> count;

			if (currentCount < count) {
				cout << "На складе недостаточно товара.\n";
				return;
			}
			double price = res->getDouble("price");
			double totalAmount = price * count;

			int newCount = currentCount - count;

			unique_ptr<PreparedStatement> updateStmt(con->prepareStatement(
				"UPDATE products SET count = ? WHERE id = ?"));
			updateStmt->setInt(1, newCount);
			updateStmt->setInt(2, idProduct);
			updateStmt->executeUpdate();

			// Если товара нет и поставщик отсутствует — удалить товар
			if (newCount == 0 && !hasSupplier) {
				unique_ptr<PreparedStatement> deleteStmt(con->prepareStatement(
					"DELETE FROM products WHERE id = ?"));
				deleteStmt->setInt(1, idProduct);
				deleteStmt->executeUpdate();
				cout << "Товар удалён, так как он закончился и не имеет поставщика.\n";
			}

			unique_ptr<PreparedStatement> inOrders(
				con->prepareStatement("INSERT INTO orders (shop_id, product_id, product_name, count, total_amount) VALUES (?, ?, ?, ?, ?)")
			);
			inOrders->setInt(1, shopId);
			inOrders->setInt(2, idProduct);
			inOrders->setString(3, nameProduct);
			inOrders->setInt(4, count);
			inOrders->setDouble(5, totalAmount);
			inOrders->executeUpdate();

			cout << "Заказ успешно оформлен" << endl;
		}
		else {
			cout << "Товар не найден.\n";
		}
	}
	catch (SQLException& e) {
		cerr << "Ошибка добавления поставщика: " << e.what() << endl;
	}
}


void Shop::showShopStatistics(Connection* con) {
	try {
		unique_ptr<Statement> stmt(con->createStatement());
		unique_ptr<ResultSet> res(stmt->executeQuery(
			"SELECT s.name AS shop_name, COUNT(o.id) AS total_orders, SUM(o.total_amount) AS total_revenue "
			"FROM shops s "
			"LEFT JOIN orders o ON s.id = o.shop_id "
			"GROUP BY s.id, s.name"
		));

		bool iss = false;
		while (res->next()) {
			if (!iss) {
				cout << "Статистика: " << endl;
				cout << "------------------------" << endl;
				iss = true;
			}
		cout << "Магазин: " << res->getString("shop_name") << endl;
		cout << "Кол-во заказов: " << res->getInt("total_orders") << endl;
		cout << "Общая сумма: " <<(res->isNull("total_revenue") ? 0.0 : res->getDouble("total_revenue")) << endl;
		cout << "------------------------" << endl;
		}
	}
	catch (sql::SQLException& e) {
		cerr << "Ошибка при выводе статистики магазинов: " << e.what() << endl;
	}
}