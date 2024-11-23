


#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define RST_PIN  4   // Reset pin for RC522
#define SS_PIN   5   // Slave Select pin for RC522

const char* ssid = "WLAN_2818";       // Your Wi-Fi SSID
const char* password = "erfan.askari021"; // Your Wi-Fi password
const String serverURL = "http://192.168.1.5/myprojects/test/reader_data.php";  // URL of your PHP server

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  connectWiFi();
}

void loop() {
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.println("UID: " + uid);

  // Get user details
  String firstName = getInput("Enter first name: ");
  Serial.println("f: " + firstName);
  String lastName = getInput("Enter last name: ");
  Serial.println("l: " + lastName);
  String studentID = getInput("Enter student ID: ");
  Serial.println("#: " + studentID);

  // Send data to PHP server
  sendToServer(uid, firstName, lastName, studentID);

  delay(2000); // Wait before scanning the next card
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

String getInput(String prompt) {
  String input = "";
  Serial.println(prompt);
  while (input.length() == 0) {
    if (Serial.available() > 0) {
      input = Serial.readString();
      input.trim(); // Remove extra spaces
    }
  }
  return input;
}

void sendToServer(String uid, String firstName, String lastName, String studentID) {
  HTTPClient http;
  String url = serverURL + "?uid=" + uid + "&firstName=" + firstName + "&lastName=" + lastName + "&studentID=" + studentID;
  http.begin(url);  // Start HTTP request

  int httpResponseCode = http.GET();  // Send GET request to the server

  if (httpResponseCode > 0) {
    String response = http.getString();  // Get the response from the server
    Serial.println("Response from server: " + response);  // Print the response from the server
  } else {
    Serial.println("Error sending data: " + String(httpResponseCode));
  }

  http.end();  // End the HTTP connection
}






***********************************************************


<?php
$servername = "localhost";  // MySQL server address
$username = "your_username"; // MySQL username
$password = "your_password"; // MySQL password
$dbname = "your_database"; // MySQL database name

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Get data from the GET request
$uid = isset($_GET['uid']) ? $_GET['uid'] : '';
$firstName = isset($_GET['firstName']) ? $_GET['firstName'] : '';
$lastName = isset($_GET['lastName']) ? $_GET['lastName'] : '';
$studentID = isset($_GET['studentID']) ? $_GET['studentID'] : '';

// Check if all required data is provided
if ($uid && $firstName && $lastName && $studentID) {
    // Prepare the SQL statement to insert data
    $sql = "INSERT INTO students (uid, first_name, last_name, student_id) 
            VALUES ('$uid', '$firstName', '$lastName', '$studentID')";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";  // Success message
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;  // Error message if insertion fails
    }
} else {
    echo "Error: Missing data fields";  // Error message if any field is missing
}

$conn->close();
?>




*******************************************************************************



    #include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <urlencode.h>  // This is a helper library for URL encoding, you can use the Arduino `urlencode()` method.

void sendToServer(String uid, String firstName, String lastName, String studentID) {
  HTTPClient http;

  // URL encode the parameters
  String encodedFirstName = urlencode(firstName);  // Ensure first name is URL encoded
  String encodedLastName = urlencode(lastName);    // Ensure last name is URL encoded
  String encodedStudentID = urlencode(studentID);  // Ensure student ID is URL encoded

  String url = "http://192.168.x.x/insert_data.php?uid=" + uid + "&firstName=" + encodedFirstName + "&lastName=" + encodedLastName + "&studentID=" + encodedStudentID;

  http.begin(url);  // Start HTTP request

  int httpResponseCode = http.GET();  // Send GET request to the server

  if (httpResponseCode > 0) {
    String response = http.getString();  // Get the response from the server
    Serial.println("Response from server: " + response);  // Print the response from the server
  } else {
    Serial.println("Error sending data: " + String(httpResponseCode));
  }

  http.end();  // End the HTTP connection
}
