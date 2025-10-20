// SPDX-FileCopyrightText: 2020 Carter Nelson for Adafruit Industries
//
// SPDX-License-Identifier: MIT
//
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LSM6DS3TRC.h>
#include <Adafruit_Sensor.h>

#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

Adafruit_LIS3MDL lis3mdl;   // magnetometer
Adafruit_LSM6DS3TRC lsm6ds3trc; // accelerometer, gyroscope
Adafruit_LSM6DS33 lsm6ds33;

long int accel_array[6];
long int check_array[6]={0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

bool new_rev = true;

#define record_time 5 //in seconds
#define sample_rate 100 //x times a second(Hz)
#define batch_size record_time*sample_rate
String sensor_readings[batch_size];

enum Menu {pre_record, recording, post_record, end};
bool sendMessage = true; 

Menu menu = pre_record;

void record(String date) {

  for(int index = 0; index < batch_size; index++){
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    lsm6ds3trc.getEvent(&accel, &gyro, &temp);
    lis3mdl.read();
    float mag_x = lis3mdl.x;
    float mag_y = lis3mdl.y;
    float mag_z = lis3mdl.z;
    float acc_x = accel.acceleration.x;
    float acc_y = accel.acceleration.y;
    float acc_z = accel.acceleration.z;
    float gyro_x = gyro.gyro.x;
    float gyro_y = gyro.gyro.y;
    float gyro_z = gyro.gyro.z;
    unsigned long t = micros();

    String csv = "";
    csv += date;
    csv += ",";
    csv += t;
    csv +=",";
    csv +=mag_x;
    csv +=",";
    csv +=mag_y;
    csv +=",";
    csv +=mag_z;
    csv +=",";
    csv +=acc_x;
    csv +=",";
    csv +=acc_y;
    csv +=",";
    csv +=acc_z;
    csv +=",";
    csv +=gyro_x;
    csv +=",";
    csv +=gyro_y; 
    csv +=",";
    csv +=gyro_z;
    csv +="\n";

    sensor_readings[index] = csv;

    delay(sample_rate);
  }
}

void print_data(bool isOllie){
  bleuart.println("___RECORD START____");
  for(int i = 0; i < batch_size; i++){
    String csv = isOllie + "," + sensor_readings[i]; 
    bleuart.print(sensor_readings[i]);
  }
  bleuart.println("____RECORD END_____");
}

void navigator(){
  
}

void rx_callback(uint16_t conn_handle) {
  String packet = bleuart.readStringUntill('\n');
  packet.trim();
  packet = std:tolower(packet);

  if (packet == "r"){
    sendMessage = true;
  }

  if (menu == pre_record) {
    if (packet == "1"){
      menu = recording; 

    } else if (packet == "2"){
      menu = end;
    }


  } else if (menu == Menu.post_record) {
    if (packet == "1"){
      print_data(isOllie=true);
    } else if (packet == "2"){
      print_data(isOllie=false);
    } else if (packet == "3"){
      menu = pre_record;
      send_message = true;
    }
  }


}

void setup(void) {
  Serial.begin(115200);
  // while (!Serial) delay(10);

  // initialize the sensors
  lis3mdl.begin_I2C();
  lsm6ds33.begin_I2C();
  // check for readings from LSM6DS33
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  accel_array[0] = accel.acceleration.x;
  accel_array[1] = accel.acceleration.y;
  accel_array[2] = accel.acceleration.z;
  accel_array[3] = gyro.gyro.x;
  accel_array[4] = gyro.gyro.y;
  accel_array[5] = gyro.gyro.z;
  // if all readings are empty, then new rev
  for (int i =0; i < 5; i++) {
    if (accel_array[i] != check_array[i]) {
      new_rev = false;
      break;
    }
  }
  // and we need to instantiate the LSM6DS3TRC
  if (new_rev) {
    lsm6ds3trc.begin_I2C();
  }

  #if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();
  bleuart.setRxCallback(rx_callback);

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();


}

void loop(void) {
  if (menu = pre_record) {
    if(sendMessage == true){
      bleuart.println("____SKATEBOARD TRICK RECORDER 3000____");
      bleuart.println("Enter:(1)Record | (2)Exit");
    }
  }else if (menu = post_record) {
    if(sendMessage == true){
      bleuart.println("Recording complete!")
      bleuart.println("Enter:(1)Save as Ollie | (2)Save as 'not ollie' | (3)Delete")
    }
  }





   
}





void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}


