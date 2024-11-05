To enhance your form with additional fields such as:

1. A **dropdown** for selecting a day of the week (`شنبه - یکشنبه - دوشنبه - سه‌شنبه - چهارشنبه`).
2. **First name** and **Last name** fields.

You can modify the HTML form in your ESP32 web server code. I'll walk you through adding these fields.

### Updated Steps to Modify the Form:

1. **Dropdown for Days**: Add a dropdown menu for selecting a day of the week.
2. **First Name and Last Name**: Add text input fields for the **first name** and **last name**.
3. **AJAX Handling**: Modify the **AJAX** code to send the additional fields to the **PHP server** for processing.

### Step 1: Modify the HTML Form with New Fields

Here’s how you can modify the **HTML** form to include the new fields:

```cpp
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>Enter Student Information</h1>";
    html += "<form id='rfidForm' action='/submit' method='POST'>";
    
    // Day Dropdown (1)
    html += "<label for='day'>Day:</label>";
    html += "<select id='day' name='day'>";
    html += "<option value='شنبه'>شنبه</option>";
    html += "<option value='یکشنبه'>یکشنبه</option>";
    html += "<option value='دوشنبه'>دوشنبه</option>";
    html += "<option value='سه‌شنبه'>سه‌شنبه</option>";
    html += "<option value='چهارشنبه'>چهارشنبه</option>";
    html += "</select><br><br>";

    // First Name (2)
    html += "<label for='first_name'>First Name:</label>";
    html += "<input type='text' id='first_name' name='first_name' required><br><br>";

    // Last Name (3)
    html += "<label for='last_name'>Last Name:</label>";
    html += "<input type='text' id='last_name' name='last_name' required><br><br>";
    
    // Student ID (4)
    html += "<label for='student_id'>Student ID:</label>";
    html += "<input type='text' id='student_id' name='student_id' required><br><br>";
    
    // Hidden RFID UID (5)
    html += "<input type='hidden' id='rfid_uid' name='rfid_uid' value='" + rfid_uid + "'>";
    
    html += "<input type='submit' value='Submit'>";
    html += "</form>";
    
    html += "<script>";
    html += "let rfid_uid = '" + rfid_uid + "';";
    html += "document.getElementById('rfid_uid').value = rfid_uid;";
    html += "document.getElementById('rfidForm').addEventListener('submit', function(e) {";
    html += "    e.preventDefault();";  // Prevent the default form submission
    html += "    let student_id = document.getElementById('student_id').value;";
    html += "    let first_name = document.getElementById('first_name').value;";
    html += "    let last_name = document.getElementById('last_name').value;";
    html += "    let day = document.getElementById('day').value;"; // Get selected day
    
    html += "    let formData = new FormData();";
    html += "    formData.append('student_id', student_id);";
    html += "    formData.append('rfid_uid', rfid_uid);";
    html += "    formData.append('first_name', first_name);";
    html += "    formData.append('last_name', last_name);";
    html += "    formData.append('day', day);";  // Append the selected day
    
    html += "    fetch('/submit', {";
    html += "        method: 'POST',";
    html += "        body: formData";
    html += "    }).then(response => response.text()).then(result => {";
    html += "        if (result == '1') {";
    html += "            alert('Data Submitted Successfully!');";
    html += "        } else {";
    html += "            alert('Submission Failed');";
    html += "        }";
    html += "    });";
    html += "});";
    html += "</script>";
    
    html += "</body></html>";
    request->send(200, "text/html", html);
});
```

### Key Changes:
1. **Day Dropdown**:
   - The `day` dropdown is added with options for each day (`شنبه - یکشنبه - دوشنبه - سه‌شنبه - چهارشنبه`).
   
2. **First Name and Last Name**:
   - **First name** and **Last name** fields are added as required text inputs.
   
3. **AJAX Form Submission**:
   - The `day`, `first_name`, and `last_name` values are collected and sent via **AJAX** to the server along with the **RFID UID** and **student ID**.

### Step 2: Update the ESP32 `/submit` Endpoint to Handle New Fields

Update the **ESP32 `/submit`** handler to send the new fields to your **PHP server**.

Here’s the updated **submit handler** for the ESP32 server:

```cpp
server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request){
    String student_id = request->getParam("student_id", true)->value();
    String first_name = request->getParam("first_name", true)->value();
    String last_name = request->getParam("last_name", true)->value();
    String day = request->getParam("day", true)->value();
    String rfid_uid = request->getParam("rfid_uid", true)->value();
    
    // Send data to PHP server
    HTTPClient http;
    String url = "http://192.168.1.4/myprojects/test/apply_data.php";  // Change to your PHP script URL
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "student_id=" + student_id + 
                      "&first_name=" + first_name + 
                      "&last_name=" + last_name + 
                      "&day=" + day + 
                      "&rfid_uid=" + rfid_uid;
    
    int httpCode = http.POST(postData);
    String result = http.getString();
    
    // Send the result back to the client (AJAX response)
    if (result == "1") {
      request->send(200, "text/plain", "1");  // Success
    } else {
      request->send(200, "text/plain", "0");  // Failure
    }
    http.end();
});
```

### Step 3: Update Your PHP Script to Handle New Fields

Finally, modify your **PHP script** (`apply_data.php`) to handle the new fields and insert them into the database.

Here’s the updated **PHP code**:

```php
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "rfid_system";  // Your MySQL database name

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Retrieve POST data
$student_id = $_POST['student_id'];  // Get student ID from the form
$first_name = $_POST['first_name'];  // Get first name
$last_name = $_POST['last_name'];    // Get last name
$day = $_POST['day'];                // Get selected day
$rfid_uid = $_POST['rfid_uid'];      // Get RFID UID

// Insert data into the database
$sql = "INSERT INTO students (student_id, first_name, last_name, day, rfid_uid) 
        VALUES ('$student_id', '$first_name', '$last_name', '$day', '$rfid_uid')";

// Execute the query and check if successful
if ($conn->query($sql) === TRUE) {
    echo "1";  // Success
} else {
    echo "0";  // Failure
}

$conn->close();
?>
```

### Key Changes in PHP:
- **First Name, Last Name, and Day**: Added the new fields to the SQL query to insert the **first name**, **last name**, and **day** into the `students` table.

### Step 4: Test the System

1. **Scan an RFID Card**: The **RFID UID** will be automatically inserted into the hidden input field.
2. **Fill in the Form**: Enter the **student ID**, **first name**, **last name**, and select the **day**.
3. **Submit the Form**: The data will be sent via **AJAX** to the **PHP server**, and the submission result will be shown on the page.

### Conclusion

With these updates, your form will now include:
1. A dropdown for selecting the **day** (`شنبه - یکشنبه - دوشنبه - سه‌شنبه - چهارشنبه`).
2. **First name** and **last name** input fields.
3. **AJAX** form submission, so the page doesn’t need to refresh after each submission.

This will make the user experience smoother and allow you to capture more detailed information for each RFID scan.
