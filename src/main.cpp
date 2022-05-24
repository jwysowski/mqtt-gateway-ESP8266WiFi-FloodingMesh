#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <TypeConversionFunctions.h>
#include <FloodingMesh.h>

#include "data.hpp"
#include "handlers.hpp"

// Prototypes
bool received_callback(String &msg, FloodingMesh &mesh_instance);
void mqtt_callback(char *topic, byte *payload, unsigned int length);
IPAddress getlocal_ip();

// A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a default Kok set, but it can be replaced if desired.
// All ESP-NOW keys below must match in an encrypted connection pair for encrypted communication to be possible.
// Note that it is also possible to use Strings as key seeds instead of arrays.
uint8_t espnowEncryptedConnectionKey[16] = { 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44,  // This is the key for encrypting transmissions of encrypted connections.
																						 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x32, 0x11 };
uint8_t espnowHashKey[16] = { 0xEF, 0x44, 0x33, 0x0C, 0x33, 0x44, 0xFE, 0x44,  // This is the secret key used for HMAC during encrypted connection requests.
															0x33, 0x44, 0x33, 0xB0, 0x33, 0x44, 0x32, 0xAD };

FloodingMesh mesh = FloodingMesh(received_callback, FPSTR(MESH_PASSWORD), espnowEncryptedConnectionKey,
								espnowHashKey, FPSTR(MESH_PREFIX), MeshTypeConversionFunctions::uint64ToString(ESP.getChipId()), true);


WiFiClient wifi;
IPAddress my_ip(0, 0, 0, 0);
IPAddress mqtt_broker(192, 168, 4, 2);
PubSubClient mqtt(mqtt_broker, 1883, mqtt_callback, wifi);

extern void (*command_handlers[HANDLERS_SIZE])(data_frame& frame);

void setup() {
	WiFi.persistent(false);
	Serial.begin(BAUDRATE);

	mesh.begin();
	mesh.activateAP();
}

void loop() {
	floodingMeshDelay(1);
	mqtt.loop();

	if (!mqtt.connected())
		if (mqtt.connect("gate")) {
			mqtt.publish(alive, "Ready!");
			mqtt.subscribe(command);
		}

	if (my_ip != getlocal_ip()) {
		my_ip = getlocal_ip();

		if (mqtt.connect("gate")) {
			mqtt.publish(alive, "Ready!");
			mqtt.subscribe(command);
		}
	}
}

bool received_callback(String &msg, FloodingMesh &mesh_instance) {
	data_frame frame;
	const char *message = msg.c_str();

	mqtt.publish(report, message);
	decode_msg(message, frame);
	if (!validate(frame))
		return false;

	if (frame.data_type == TEMPERATURE_TYPE) {
		mqtt.publish(temperature, message);
		Serial.print(message);
	 } else if (frame.data_type == HUMIDITY_TYPE) {
		mqtt.publish(humidity, message);
		Serial.print(message);
	}

	return true;
}

void mqtt_callback(char *topic, uint8_t *payload, unsigned int length) {
	char *clean_payload = (char *)malloc(length + 1);

	memcpy(clean_payload, payload, length);
	clean_payload[length] = '\0';
	String msg = String(clean_payload);
	free(clean_payload);

	data_frame frame;
	decode_msg(msg.c_str(), frame);
	String target = String(topic);
	if (target != command)
		return;

	int index = get_handler_index(frame.data_type);
	command_handlers[index](frame);
}

IPAddress getlocal_ip() {
	return IPAddress(WiFi.localIP());
}
