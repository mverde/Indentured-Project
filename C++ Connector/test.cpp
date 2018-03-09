/* Copyright 2008, 2010, Oracle and/or its affiliates. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <string> 

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

int main(int argc, char *argv[])
{

try {
  /* Initialize Variables */
  //////////////////////////
  sql::Driver *driver;
  sql::Connection *con;
  //sql::Statement *stmt;
  sql::PreparedStatement *prep_stmt;
  sql::ResultSet *res;

  /* Create a connection */
  /////////////////////////
  driver = get_driver_instance();
  //set the variables for connection to the database : endpoint, username, password
  con = driver->connect("escality-db-instance.cykpeyjjej2m.us-west-1.rds.amazonaws.com", "escality_user", "12345678");
  /* Connect to the MySQL test database */
  con->setSchema("escality_location_db");

  /*Use either a static query (comment the variable declaration for prep_stmt and delete prep_stmt, uncomment out stmt and delete stmt)*/
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //stmt = con->createStatement();
  //res = stmt->executeQuery("SELECT * FROM pois");

  
  /*Or a dynamic query (uncomment the variable declaration for prep_stmt and delete prep_stmt, comment out stmt and delete stmt)*/
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //get user input
  cout << "Enter your query" << endl;
  string query;
  getline(cin, query);
  //turn input into a query and execute
  prep_stmt = con->prepareStatement(query);
  res = prep_stmt->executeQuery();


  /*print out the result of your query*/
  while (res->next()) {
    //for each tuple result, we use getString("attribute") to retrieve the data corrosponding to the attribute name
    cout << res->getString("Place") << "," << res->getString("Lat") << "," << res->getString("Lng") << "," << res->getString("Types") << endl;
  }

  /*clean up*/
  delete res;
  //delete stmt;
  delete prep_stmt;
  delete con;

} catch (sql::SQLException &e) {
  cout << "# ERR: SQLException in " << __FILE__;
  cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

cout << endl;

return EXIT_SUCCESS;
}