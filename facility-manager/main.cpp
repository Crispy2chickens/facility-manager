#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;

// configure the following
const string server = "tcp://<server-computer-ip-address>:3306";
const string username = "username";
const string password = "password";

void login();
void makeBooking();
void cancelBooking();
void checkBooking();

int userid = -1;

int main()
{
	sql::Driver* driver;
	sql::Connection* con;

	char input;
	int choice;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("facilities_db");
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	while (userid == -1) login();

	cout << "\n\nWelcome to the Facility Booking System" << endl;

	do {
		cout << "\n1. Book Facility \n2. Cancel Booking \n3. Check Booking \n4. Exit" << endl;
		cin >> choice;

		switch (choice) {
			case 1:
				makeBooking();
				break;
			case 2:
				cancelBooking();
				break;
			case 3: 
				checkBooking();
				break;
			case 4:
				system("CLS");
				return 0;
			default:
				cout << "\nWrong input entered. \nTry again." << endl;
				return main();
			}
		cout << "\nDo you wish to continue (Y/N)" << endl;
		cin >> input;
	} while (input == 'Y' || input == 'y');

	
	delete con;
	system("pause");
	return 0;
}


void login() {
	string emailAddress, loginPassword;
	cout << "Login" << endl;
	cout << "Email address: ";
	cin >> emailAddress;
	cout << "Password: ";
	cin >> loginPassword;

	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql::PreparedStatement *pstmt;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("facilities_db");
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	stmt = con->createStatement();
	sql::SQLString query = "SELECT * FROM users WHERE password LIKE ? AND email_address LIKE ?";

	pstmt = con->prepareStatement(query);
	pstmt->setString(1, loginPassword);
	pstmt->setString(2, emailAddress);

	res = pstmt->executeQuery();


	if (res->rowsCount() == 1) {
		while (res->next()) {
			cout << "Name: " << res->getString("first_name") << " " << res->getString("last_name");
			cout << ", Email address: " << res->getString("email_address") << endl;

			userid = res->getInt("idusers");
		}
	}
	else {
		cout << "Error logging in\n" << endl;
	}

	delete res;
	delete pstmt;
	delete stmt;
	delete con;
};

void makeBooking() {
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("facilities_db");
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	stmt = con->createStatement();

	cout << "\nHere is a list of facilities" << endl;

	sql::SQLString query = "SELECT * FROM facilities;";

	sql::ResultSet *res = stmt->executeQuery(query);

	string sptr = "|";

	while (res->next()) {
		cout << "ID" << res->getInt("idfacilities") << sptr << res->getString("name") << sptr << res->getString("code") << sptr
		<< res->getInt("capacity") << " (capacity)" << sptr << res->getString("type") << sptr << res->getString("description") << endl;
	}

	int enteredID;
	string bookingDate, period;

	cout << "\nEnter facility ID: ";
	cin >> enteredID;
	cout << "Enter date you would like to make booking for (yyyy-mm-dd): ";
	cin >> bookingDate;
	cout << "Enter period you would like to book facility for: ";
	cin >> period;

	pstmt = con->prepareStatement("INSERT INTO user_bookings (iduser, idfacilities, booking_date, booking_period) VALUES (?, ? ,?, ?)");

	pstmt->setInt(1, userid);
	pstmt->setInt(2, enteredID);
	pstmt->setString(3, bookingDate);
	pstmt->setString(4, period);

	pstmt->executeUpdate();

	delete pstmt;
	delete stmt;
	delete res;
	delete con;
};

void cancelBooking() {
	sql::Driver* driver;
	sql::Connection* con;
	
	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("facilities_db");
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
	
	int cancelID;

	cout << endl;
	checkBooking();
	cout << "\nPlease insert ID to cancel" << endl;

	cin >> cancelID;

	sql::PreparedStatement* pstmt = con->prepareStatement("DELETE FROM user_bookings WHERE iduser_bookings = ?;");
	pstmt->setInt(1, cancelID);

	pstmt->executeUpdate();

	delete pstmt;
};

void checkBooking() {
	sql::Driver* driver;
	sql::Connection* con;
	sql::ResultSet* res;
	sql::PreparedStatement* pstmt;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("facilities_db");
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	sql::SQLString query = "SELECT * FROM user_bookings WHERE iduser LIKE ?";

	pstmt = con->prepareStatement(query);
	pstmt->setInt(1, userid);

	res = pstmt->executeQuery();
	
	cout << endl;

	while (res->next()) {
		sql::ResultSet* result;

		int facilityid = res->getInt("idfacilities");

		sql::SQLString query = "SELECT * FROM facilities WHERE idfacilities LIKE ?";

		pstmt = con->prepareStatement(query);
		pstmt->setInt(1, facilityid);

		result = pstmt->executeQuery();


		while (result->next()) {
			string facilityName = result->getString("name");

			cout << "ID" << res->getInt("iduser_bookings") << "|" << facilityName << "|" << res->getString("booking_date") 
				<< "|" << res->getString("booking_period") << endl;
		}

		delete result;
	}

	delete res;
	delete pstmt;
	delete con;
};