/* HX711 configuration:

LOADCELL_DOUT_PIN           defines the pin used for DOUT (default GPIO33)
LOADCELL_SCK_PIN            defines the pin used for SCK (default GPIO32)

CALIBRATION_FACTOR          defines the calibration factor for the scale below the litter box,
                            use Adafruit HX711 calibration Arduino example to calibrate. */

#define LOADCELL_DOUT_PIN  33
#define LOADCELL_SCK_PIN  32
#define CALIBRATION_FACTOR -20000.0

/* Cat configuration:

Use these settings to eliminate false positives when the litter box is for example moved. All
measurements outside of the range defined here are ignored.

CAT_MIN_WEIGHT              minimum weight of cat to expect
CAT_MAX_WEIGHT              maximum weight of cat to expect */

#define CAT_MIN_WEIGHT 3.0
#define CAT_MAX_WEIGHT 6.0

/* Network settings 

WIFI_SSID                   SSID of network to connect to
WIFI_PSK                    passphrase of network to connect to */

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PSK "WIFI_PSK"

/* MQTT settings

MQTT_SERVER                 IP address of MQTT server
MQTT_CLIENT_ID              client ID to use for MQTT
MQTT_OUTPUT_TOPIC           MQTT topic to output to
MQTT_INPUT_TOPIC            MQTT topic to listen to */

#define MQTT_SERVER "MQTT_SERVER"
#define MQTT_CLIENT_ID "litterbox"
#define MQTT_OUTPUT_TOPIC "litterbox/output"
#define MQTT_INPUT_TOPIC "litterbox/input"
