#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button pins
const int button1 = 18;  // GPIO for cycling
const int button2 = 19;  // GPIO for confirming

// Your WiFi credentials
const char* ssid = "TripoleWifi";
const char* password = "!@#Tripolewifi2024";

// API details for SMS
const String apiUrl = "https://sms.iprogtech.com/api/v1/sms_messages";
const String apiToken = "75f55c9307f2494e02e68fa8a8b49c13c639bbcc"; // Replace with your API token
const String phoneNumber = "09055501894"; // Andrea's Phone Number is 09614628598
const String message = "Happy Monthsary my dear! 💖"; // The message to be sent

// Month list
const char* months[] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};
int monthIndex = 0;      // Current month index
int dayIndex = 1;        // Current day (1-based)
bool lastButton1State = HIGH; // Debounce state for Button 1
bool lastButton2State = HIGH; // Debounce state for Button 2
bool selectingDay = false;    // Tracks if selecting days
bool selectionConfirmed = false; // Tracks if final selection is confirmed

// Custom heart characters (used with LCD custom char function)
byte heart1[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

byte heart2[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};


void setup() {
  // Initialize LCD
  lcd.begin();
  lcd.backlight();

  // Create custom characters
  lcd.createChar(0, heart1);
  lcd.createChar(1, heart2);

  // Display the initial message
  lcd.setCursor(0, 0);
  lcd.print("Pick Month:");

  // Set up button pins as inputs with pull-up resistors
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

   // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Display the first month
  lcd.setCursor(0, 1);
  lcd.print(months[monthIndex]);
}

void loop() {
  // Read button states
  bool currentButton1State = digitalRead(button1);
  bool currentButton2State = digitalRead(button2);

  // Handle Button 1 (Cycling)
  if (currentButton1State == LOW && lastButton1State == HIGH) {
    if (!selectingDay && !selectionConfirmed) {
      // Cycle through months
      monthIndex = (monthIndex + 1) % 12;
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear the line
      lcd.setCursor(0, 1);
      lcd.print(months[monthIndex]);
    } else if (selectingDay && !selectionConfirmed) {
      // Cycle through days (1 to 31)
      dayIndex = (dayIndex % 31) + 1;
      lcd.setCursor(0, 1);
      lcd.print("Day: ");
      lcd.print(dayIndex);
      lcd.print("  "); // Clear leftover characters
    }
  }

  // Handle Button 2 (Confirming)
  if (currentButton2State == LOW && lastButton2State == HIGH) {
    if (!selectingDay && !selectionConfirmed) {
      // Confirm month and move to day selection
      selectingDay = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pick Day:");
      lcd.setCursor(0, 1);
      lcd.print("Day: ");
      lcd.print(dayIndex);
    } else if (selectingDay && !selectionConfirmed) {
      // Confirm day selection
      selectionConfirmed = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected Date:");
      lcd.setCursor(0, 1);
      lcd.print(months[monthIndex]);
      lcd.print(" ");
      lcd.print(dayIndex);

      // Check if the selected date is November 16
      if (monthIndex == 10 && dayIndex == 16) {  // November (index 10) and day 16
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Happy Monthsary");
        lcd.setCursor(0, 1);
        lcd.print("my dear! ");
        lcd.write(byte(0));  // Display first heart
        sendSMS();
      }
    }
  }

  // Update button states for debounce
  lastButton1State = currentButton1State;
  lastButton2State = currentButton2State;

  delay(100);  // Debounce delay
}

void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Define the API URL
    String apiUrl = "https://app.philsms.com/api/v3/sms/send";
    
    // Specify the content type and authorization header
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer 1179|EkuFgMmv7JYBgb0qwf5ZD6kZWKrvuqFyGHGb9g04"); // Replace with your actual token
    
    // Create the JSON payload
    String jsonData = "{\"sender_id\":\"PhilSMS\",\"recipient\":\"+639055501894,+639308024299\",\"message\":\"" + message + "\"}";

    // Send POST request
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      // If the HTTP request was successful
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error sending SMS: " + String(httpResponseCode));
    }

    // End the HTTP connection
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

