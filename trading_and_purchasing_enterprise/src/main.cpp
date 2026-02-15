#include "operations.h" 
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <iostream>
#include <memory>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <string>

using namespace std;

int main()
{
	unique_ptr<sql::Connection> connection(getConnection());
	if (!connection) {
		cerr << "Ошибка подключения к базе данных. Завершение программы." << endl;
		return 1;
	}

	while (true) {
		size_t choice1;
		cout << "Выберите действие: " << endl
			<< "1. Работа со складом" << endl
			<< "2. Работа с поставщиками" << endl
			<< "3. Работа с магазинами" << endl
			<< "0 - Выход" << endl;
		cin >> choice1;

		if (choice1 == 0) return 0;

		switch (choice1) {
		case 1: { //работа со складом
			int choice2;
			cout << "Выберите действие:" << endl
				<< "1. Добавить товар" << endl
				<< "2. Удалить товар" << endl
				<< "3. Вывести информацию о всех товарах" << endl
				<< "4. Вывести информацию об определенном товаре" << endl
				<< "5. Изменить данные" << endl
				<< "0 - В главное меню" << endl;
			cin >> choice2;

			if (choice2 == 0) break;

			switch (choice2) {
			case 1: { // добавление товара
				cin.ignore();
				string name, category;
				int price, count, supplier_id;
				cout << "Введите название товара: ";
				getline(cin, name);
				cout << "Введите категорию товара: ";
				getline(cin, category);
				cout << "Введите цену товара(руб.): ";
				cin >> price;
				cout << "Введите количество товара(шт.): ";
				cin >> count;
				cout << "Введите ID поставщика: ";
				cin >> supplier_id;

				Product pr(name, category, price, count, supplier_id);
				pr.createNewProduct(connection.get());
				break;
			}
			case 2: { // удаление товара
				string name;
				cout << "Введите название товара, который вы хотите удалить: ";
				cin >> name;
				Product::deleteByName(connection.get(), name);
				break;
			}
			case 3: { // информация о всех товарах
				Product::showAll(connection.get());
				break;
			}
			case 4: { //поиск товара по имени
				string name;
				cout << "Введите название товара: ";
				cin >> name;
				Product::findByName(connection.get(), name);
				break;
			}
			case 5: { // изменение товара
				string name;
				cout << "Введите название товара для изменения: ";
				cin >> name;
				Product::updateByName(connection.get(), name);
				break;
			}
			default:
				cout << "Неверный ввод. Попробуйте снова.\n";
				break;
			}
			break;
		}
		case 2: { // работа с поставщиками
			int choice2;
			cout << "Выберите действие:" << endl
				<< "1. Добавить поставщика" << endl
				<< "2. Удалить поставщика" << endl
				<< "3. Вывести информацию о всех поставщиках" << endl
				<< "4. Вывести информацию об определенном поставщике" << endl
				<< "5. Изменить данные поставщика" << endl
				<< "6. Вывести все товары поставщика поставщика" << endl
				<< "0 - В главное меню" << endl;
			cin >> choice2;

			if (choice2 == 0) break;

			switch (choice2) {
			case 1: { // добавление нового поставщика
				cin.ignore();
				string name, phone, email;
				cout << "Введите название поставщика: ";
				getline(cin, name);
				cout << "Введите телефон поставщика: ";
				cin >> phone;
				cout << "Введите e-mail поставщика: ";
				cin >> email;

				Supplier sup(name, phone, email);
				sup.addSupplier(connection.get());
				break;
			}
			case 2: { //удаление поставщика
				int id;
				cout << "Введите ID поставщика: " << endl;
				cin >> id;
				Supplier::deleteSupplier(connection.get(), id);

				break;
			}
			case 3: { //вывод всех поставщиков
				Supplier::showAllSuppliers(connection.get());
				break;
			}
			case 4: { //информация о конкретном поставщике
				int id;
				cout << "Ввежите ID поставщика: ";
				cin >> id;
				Supplier::showSupplier(connection.get(), id);
				break;
			}
			case 5: {
				int id;
				cout << "Введите ID поставщика для изменения: ";
				cin >> id;
				Supplier::editSupplier(connection.get(), id);
				break;
			}
			case 6: { //все товары поставщика
				int id;
				cout << "Введите ID поставщика: ";
				cin >> id;
				Supplier::showProductsBySupplier(connection.get(), id);
				break;
			}
			}
			break;
		}
		case 3: {// работа с магазинами
			int choice2;
			cout << "Выберите действие:" << endl
				<< "1. Добавить магазин" << endl
				<< "2. Удалить магазин" << endl
				<< "3. Вывести информацию о всех магазинах" << endl
				<< "4. Вывести информацию об определенном магазине" << endl
				<< "5. Изменить данные магазина" << endl
				<< "6. Сделать заказ в магазин" << endl
				<< "7. Вывести статистику магазина" << endl
				<< "0 - В главное меню" << endl;
			cin >> choice2;

			if (choice2 == 0) break;

			switch (choice2) {
			case 1: {//добавление магазина
				cin.ignore();
				string name, phone, email;
				cout << "Введите название магазина: ";
				getline(cin, name);
				cout << "Введите телефон магазина: ";
				cin >> phone;
				cout << "Введите e-mail магазина: ";
				cin >> email;

				Shop sh(name, phone, email);
				sh.addShop(connection.get());
				break;
			}
			case 2: {//удаление магазина
				int id;
				cout << "Введие ID магазина: ";
				cin >> id;
				Shop::deleteShop(connection.get(), id);
				break;
			}
			case 3: {//информация о всех магазинах
				Shop::showAllShops(connection.get());
				break;
			}
			case 4: {//информация о конкретном магазине
				int id;
				cout << "";
				cin >> id;
				Shop::showShop(connection.get(), id);
				break;
			}
			case 5: {//изменение данных
				int id;
				cout << "Введие ID магазина: ";
				cin >> id;
				Shop::editShop(connection.get(), id);
				break;
			}
			case 6: {//заказ товаров
				int id;
				cout << "Введите ID магазина: ";
				cin >> id;
				Shop::makeOrder(connection.get(), id);
				break;
			}
			case 7: {//статистика магазина
				Shop::showShopStatistics(connection.get());
				break;
			}
			}

			break;
		}
		default:
			cout << "Неверный ввод. Попробуйте снова.\n";
			break;
		}

		cout << endl;
	}
}