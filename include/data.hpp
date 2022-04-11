#ifndef DATA_HPP
#define DATA_HPP

#include "stdint.h"

#define   MESH_PREFIX     "esp_mesh"
#define   MESH_PASSWORD   "123456789"
#define   MESH_PORT       5555

#define   STATION_SSID     "PraiseTheSun_EXT"
#define   STATION_PASSWORD "jebacjaslo"

#define HOSTNAME "Gateway"

extern const char *alive;
extern const char *command;
extern const char *temperature;
extern const char *humidity;
extern const char *report;

extern const char *get_nodes_command;
extern const char *set_temp_mode;
extern const char *set_hum_mode;

extern const char *temp_msg;
extern const char *hum_msg;

#define	START_SIGN		':'
#define	END_SIGN		'\n'
#define	NODE_ID_SIZE	10
#define	DATA_SIZE		5
#define	CHECKSUM_SIZE	2
#define	MESSAGE_SIZE	21

#define	TEMPERATURE_TYPE			'0'
#define	HUMIDITY_TYPE				'1'
#define	TEMPERATURE_NORM_TYPE		'2'
#define	TEMPERATURE_RISING_TYPE		'3'
#define	TEMPERATURE_FALLING_TYPE	'4'
#define	HUMIDITY_NORM_TYPE			'5'
#define	HUMIDITY_RISING_TYPE		'6'
#define	HUMIDITY_FALLING_TYPE		'7'

#define GET_NODES_COMMAND				'0'
#define SET_TEMP_MODE_COMMAND			'1'
#define SET_HUM_MODE_COMMAND			'2'


#define	CHECKSUM_MOD		0x100
#define BAUDRATE			9600

#define TEMPERATURE		true
#define HUMIDITY		false

struct data_frame {
	char data_type;
	union data {
		char temperature[DATA_SIZE + 1];
		char humidity[DATA_SIZE + 1];
        char target[DATA_SIZE + 1];
	} measurement;
	char node_id[NODE_ID_SIZE + 1];
	char checksum[CHECKSUM_SIZE + 1];
};

void get_message(char *msg, data_frame &frame);
void decode_msg(const char *msg, data_frame &frame);
uint16_t checksum(data_frame &frame);
void decode_msg(const char *msg, data_frame &frame);
bool validate(data_frame &frame);

#endif /* DATA_HPP */