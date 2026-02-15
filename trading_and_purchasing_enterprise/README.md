# Торгово-закупочное предприятие

Консольная система управления складом, поставщиками и магазинами на C++ с использованием MySQL.

![C++](https://img.shields.io/badge/C%2B%2B-00599C?logo=c%2B%2B&logoColor=white)
![MySQL](https://img.shields.io/badge/MySQL-4479A1?logo=mysql&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual_Studio-5C2D91?logo=visual-studio&logoColor=white)

## Описание

Система автоматизации работы торгово-закупочного предприятия с тремя основными модулями:

- **Склад**: управление товарами (добавление, удаление, поиск, обновление, продажа)
- **Поставщики**: учет поставщиков и их товаров
- **Магазины**: управление сетью магазинов, формирование заказов, статистика продаж

## Структура проекта
```
trade_and_purchasing_enterprise/
├── src/
│ ├── main.cpp # Главное меню приложения
│ ├── operations.h # Объявления классов Product, Supplier, Shop
│ └── operations.cpp # Реализация бизнес-логики и работы с БД
├── .gitignore
└── README.md
```

## Требования

- **ОС**: Windows 10/11
- **Компилятор**: Visual Studio 2019/2022 (C++17)
- **База данных**: MySQL 8.0+
- **Библиотека**: MySQL Connector/C++ 8.0.x (старый API с `cppconn/`)

## Установка и настройка

### 1. Установка зависимостей

1. Установите [MySQL Server](https://dev.mysql.com/downloads/mysql/)
2. Установите [MySQL Connector/C++ 8.0.x](https://dev.mysql.com/downloads/connector/cpp/8.0.html)
   - Скачайте: `mysql-connector-c++-8.0.x-winx64.msi`
   - Установите в папку по умолчанию (`C:\Program Files\MySQL\MySQL Connector C++ 8.0\`)

### 2. Настройка Visual Studio

1. Откройте проект в Visual Studio
2. Кликните правой кнопкой по проекту → **Свойства**
3. Настройте пути:
   - **C/C++ → General → Additional Include Directories**:
     ```
     C:\Program Files\MySQL\MySQL Connector C++ 8.0\include
     ```
   - **Linker → General → Additional Library Directories**:
     ```
     C:\Program Files\MySQL\MySQL Connector C++ 8.0\lib64\vs14
     ```
   - **Linker → Input → Additional Dependencies**:
     ```
     mysqlcppconn8.lib
     ```

### 3. Настройка базы данных

1. Создайте базу данных `inventory`:
   ```sql
   CREATE DATABASE inventory CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
   ```
2. Откройте src/operations.cpp
3. Замените заглушки на ваши реальные данные:
```
// Было:
Connection* con = driver->connect("tcp://localhost:3306", "YOUR_LOGIN_HERE", "YOUR_PASSWORD_HERE");

// Стало:
Connection* con = driver->connect("tcp://localhost:3306", "root", "ваш_пароль");
```
### 4. Сборка и запуск
Соберите проект: Build → Build Solution (F7)
Запустите: Debug → Start Without Debugging (Ctrl+F5)