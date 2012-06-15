/*
	Extra useful functions for the NMEA library
*/
#include "NMEA.h"
#include <ctype.h>		// isprint
#include <string.h>		// strlen
#include <stdio.h>		// sprintf
#include <stdlib.h>		// strtoul

unsigned char buf[20];

unsigned long NMEA_get_number(NMEA_msg_t *const msg, const unsigned char index, const unsigned char base)
{
	NMEA_get(msg, index, buf);
	return strtoul(buf, NULL, base);
}

char NMEA_add_number(NMEA_msg_t *const msg, const unsigned long number, unsigned char *const format)
{
	sprintf(buf, format, number);
	return NMEA_add(msg, buf);
}

char NMEA_arg_equal(NMEA_msg_t *const msg, const unsigned long index, unsigned char *const string)
{
	NMEA_get(msg, index, buf);
	return strcmp(buf,string) == 0;
}