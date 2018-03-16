<?php include "../inc/dbinfo.inc"; ?>
<html>
<body>
<h1>POI Table</h1>
<?php

  /* Connect to MySQL and select the database. */
  $connection = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD);

  if (mysqli_connect_errno()) echo "Failed to connect to MySQL: " . mysqli_connect_error();

  $database = mysqli_select_db($connection, DB_DATABASE);

  /* Ensure that the Employees table exists. */
  VerifyPOIsTable($connection, DB_DATABASE); 

  /* If input fields are populated, add a row to the Employees table. */
  $pois_place = htmlentities($_POST['Place']);
  $pois_lat = htmlentities($_POST['Lat']);
  $pois_lng = htmlentities($_POST['Lng']);
  $pois_types = htmlentities($_POST['Types']);

  if (strlen($pois_place) || strlen($pois_lat) || strlen($pois_lng) || strlen($pois_types)) {
    AddPoi($connection, $pois_place, $pois_lat, $pois_lng, $pois_types);
  }
?>

<!-- Display table data. -->
<table border="1" cellpadding="2" cellspacing="2">
  <tr>
    <td>Place</td>
    <td>Lat</td>
    <td>Lng</td>
    <td>Types</td>
  </tr>

<?php

$result = mysqli_query($connection, "SELECT * FROM pois"); 

while($query_data = mysqli_fetch_row($result)) {
  echo "<tr>";
  echo "<td>",$query_data[0], "</td>",
       "<td>",$query_data[1], "</td>",
       "<td>",$query_data[2], "</td>",
       "<td>",$query_data[3], "</td>";
  echo "</tr>";
}
?>

</table>

<!-- Clean up. -->
<?php

  mysqli_free_result($result);
  mysqli_close($connection);

?>

</body>
</html>


<?php

/* Check whether the table exists and, if not, create it. */
function VerifyPOIsTable($connection, $dbName) {
  if(!TableExists("pois", $connection, $dbName)) 
  { 
     $query = "CREATE TABLE `pois` (
         `Place` VARCHAR(70), 
         `Lat` DECIMAL(10, 8) NOT NULL, 
         `Lng` DECIMAL(11, 8) NOT NULL, 
         `Types` TEXT, 
         PRIMARY KEY (place, lat, lng)
       ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1";

     if(!mysqli_query($connection, $query)) echo("<p>Error creating table.</p>");
  }
}

/* Check for the existence of a table. */
function TableExists($tableName, $connection, $dbName) {
  $t = mysqli_real_escape_string($connection, $tableName);
  $d = mysqli_real_escape_string($connection, $dbName);

  $checktable = mysqli_query($connection, 
      "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_NAME = '$t' AND TABLE_SCHEMA = '$d'");

  if(mysqli_num_rows($checktable) > 0) return true;

  return false;
}
?>
