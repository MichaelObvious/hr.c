#include <assert.h>
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "coords.h"

#define JAN 1
#define FEB 2
#define MAR 3
#define APR 4
#define MAY 5
#define JUN 6
#define JUL 7
#define AUG 8
#define SEP 9
#define OCT 10
#define NOV 11
#define DEC 12

int is_leap_year(int year) {
	return year % 4 == 0
           && !(year % 100 == 0 && !(year % 400 == 0));
}

int month_length(int month, int year) {
	assert(month <= 12 && month >= 1);
	switch(month) {
		case JAN:
        case MAR:
        case MAY:
        case JUL:
        case AUG:
        case OCT:
        case DEC:
			return 31;
        case APR:
        case JUN:
        case SEP:
        case NOV:
			return 30;
		case FEB: {
			if (is_leap_year(year)) {
				return 29;
            } else {
				return 28;
            }
        }
        default:
            assert(0 && "unreacheable");
    }
}

#define ARRAY_LEN(xs) sizeof(xs) / sizeof((xs)[0])

void roman_numeral(FILE* sink, int num) {
	int vals[] = {
		1000, 900, 500, 400,
		100, 90, 50, 40,
		10, 9, 5, 4,
		1
    };
	char* syms[] = {
		"M", "CM", "D", "CD",
		"C", "XC", "L", "XL",
		"X", "IX", "V", "IV",
		"I"
    };
    int i = 0;
    while (num > 0) {
        while (num >= vals[i]) {
            fprintf(sink, "%s", syms[i]);
            num -= vals[i];
        }
        i++;
    }
}

void month_name(FILE* sink, int month) {
	assert(month <= 12 && month >= 1);
	switch (month) {
		case JAN:
			fprintf(sink, "Iānuāriās");
            break;
		case FEB:
			fprintf(sink, "Februāriās");
            break;
		case MAR:
			fprintf(sink, "Martiās");
            break;
		case APR:
			fprintf(sink, "Aprīlis");
            break;
		case MAY:
			fprintf(sink, "Maiās");
            break;
		case JUN:
			fprintf(sink, "Iūniās");
            break;
		case JUL:
			fprintf(sink, "Iūliās");
            break;
		case AUG:
			fprintf(sink, "Augustī");
            break;
		case SEP:
			fprintf(sink, "Septembris");
            break;
		case OCT:
			fprintf(sink, "Octōbris");
            break;
		case NOV:
			fprintf(sink, "Novembris");
            break;
		case DEC:
			fprintf(sink, "Decembris");
            break;
        default:
            assert(0 && "unreacheable");
    }
}

void day_name(FILE* sink, int day, int month, int year) {
	int kalendae = 1;
	int idus = 0;
	int nonae = 0;
	int mlength = month_length(month, year);
	switch (month) {
		case MAR:
        case MAY:
        case JUL:
        case OCT:
			idus = 15;
            break;
		case JAN:
        case AUG:
        case DEC:
        case APR:
        case JUN:
        case SEP:
        case NOV:
        case FEB:
			idus = 13;
            break;
        default:
            assert(0 && "unreacheable");
    }
	nonae = idus - 8;
	
	if (day != kalendae && day != mlength
        && day != nonae && day != nonae-1
        && day != idus && day != idus-1) {
		fprintf(sink, "a. d. ");

		if (day > kalendae && day < nonae) {
			roman_numeral(sink, nonae+1-day);
			fprintf(sink, " ");
		} else if (day > kalendae && day < idus) {
			roman_numeral(sink, idus+1-day);
			fprintf(sink, " ");
		} else if (day > kalendae) {
			roman_numeral(sink, mlength+2-day);
			fprintf(sink, " ");
		}
    } else if (day == mlength || day == nonae-1 || day == idus-1) {
		fprintf(sink, "Prid. ");
    }
	
	if (day == kalendae) {
		fprintf(sink, "Kal. ");
		month_name(sink, month);
    } else if (day <= nonae) {
		fprintf(sink, "Nōn. ");
		month_name(sink, month);
	} else if (day <= idus) {
		fprintf(sink, "Eid. ");
		month_name(sink, month);
	} else {
		fprintf(sink, "Kal. ");
		month_name(sink, (month)%12 + 1);
    }
}

void year_name(FILE* sink, int year) {
    roman_numeral(sink, year + 753);
    fprintf(sink, " AVC");
}

// source: https://en.wikipedia.org/wiki/Sunrise_equation#Example_of_implementation_in_Python
void day_calc(int day, int month, int year, double f, double l_w, double elevation, double* sunrise, double* sunset) {
	struct tm date;
    date.tm_year = year-1900;
    date.tm_mon = month-1;
    date.tm_mday = day;
    date.tm_hour = 6;
    date.tm_min = 0;
    date.tm_sec = 0;

    time_t ts = mktime(&date);
    double J_date = ts / 86400.0 + 2440587.5;

	double n = ceil(J_date - (2451545.0 + 0.0009) + 69.184 / 86400.0);
	double J_ = n + 0.0009 - l_w / 360.0;
	double M_degrees = fmod(357.5291 + 0.98560028 * J_, 360);
    double M_radians = M_degrees * M_PI / 180.0;
	double C_degrees = 1.9148 * sin(M_radians) + 0.02 * sin(2 * M_radians) + 0.0003 * sin(3 * M_radians);
	double L_degrees = fmod(M_degrees + C_degrees + 180.0 + 102.9372, 360);
	double Lambda_radians = L_degrees * M_PI / 180.0;
	double J_transit = 2451545.0 + J_ + 0.0053 * sin(M_radians) - 0.0069 * sin(2 * Lambda_radians);
	double sin_d = sin(Lambda_radians) * sin((23.4397) * M_PI / 180.0);
	double cos_d = cos(asin(sin_d));
	double some_cos = (sin((-0.833 - 2.076 * sqrt(elevation) / 60.0) * M_PI / 180.0) - sin(f * M_PI / 180.0) * sin_d) / (cos((f) * M_PI / 180.0) * cos_d);
	if (some_cos < -1.0 || some_cos > 1.0) {
		*sunrise = 0.0;
		*sunset = 0.0;
		return;
	}
	double w0_radians = acos(some_cos);
	double w0_degrees = w0_radians * 180.0 / M_PI;
	double j_rise = J_transit - w0_degrees / 360;
    double j_set = J_transit + w0_degrees / 360;

	*sunrise = (j_rise - 2440587.5) * 86400;
	*sunset = (j_set - 2440587.5) * 86400;
}

void hour_name(FILE* sink, int hour, int minute, int sec, int day, int month, int year, double* progress) {
	double yesterday_sunrise, yesterday_sunset;
	double today_sunrise, today_sunset;
	double tomorrow_sunrise, tomorrow_sunset;

	// no problem for month, it automatically fixes itself
	day_calc(day-1, month, year, LAT, LON, ELE, &yesterday_sunrise, &yesterday_sunset);
	day_calc(day, month, year, LAT, LON, ELE, &today_sunrise, &today_sunset);
	day_calc(day+1, month, year, LAT, LON, ELE, &tomorrow_sunrise, &tomorrow_sunset);

	struct tm *tm;
    time_t t = yesterday_sunset;
    tm = localtime(&t);
	int prev_set_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = today_sunrise;
    tm = localtime(&t);
	int rise_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = today_sunset;
    tm = localtime(&t);
	int set_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = tomorrow_sunrise;
    tm = localtime(&t);
	int snd_rise_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

	// printf("%02d:%02d\n", prev_set_time/60, prev_set_time%60);
	// printf("%02d:%02d - %02d:%02d\n", rise_time/60, rise_time%60, set_time/60, set_time%60);
	// printf("%02d:%02d\n", snd_rise_time/60, snd_rise_time%60);

	int daylight_duration = set_time - rise_time;
	int hour_duration = daylight_duration / 12;
	// printf("%02d:%02d\n", hour_duration/60, hour_duration%60);

	int prev_night_duration = 24 * 3600 - prev_set_time + rise_time;
	int prev_vigilia_duration = prev_night_duration / 4;
	int next_night_duration = 24 * 3600 - set_time + snd_rise_time;
	int next_vigilia_duration = next_night_duration / 4;
	// printf("%02d:%02d\n", prev_night_duration/60, prev_night_duration%60);
	// printf("%02d:%02d\n", next_night_duration/60, next_night_duration%60);
	// printf("%02d:%02d\n", prev_vigilia_duration/60, prev_vigilia_duration%60);
	// printf("%02d:%02d\n", next_vigilia_duration/60, next_vigilia_duration%60);

	int current_time = hour * 3600 + minute * 60 + sec;

	if (current_time >= rise_time && current_time < set_time) {
		*progress = (double)((current_time - rise_time) % hour_duration) / (double) hour_duration;
		roman_numeral(sink, ((current_time - rise_time) / hour_duration) + 1);
		// switch((current_time - rise_time) / hour_duration) {
		// 	case 0:
		// 		fprintf(sink, "prīma");
        //         break;
		// 	case 1:
		// 		fprintf(sink, "secunda");
        //         break;
		// 	case 2:
		// 		fprintf(sink, "tertia");
        //         break;
		// 	case 3:
		// 		fprintf(sink, "quārta");
        //         break;
		// 	case 4:
		// 		fprintf(sink, "quīnta");
        //         break;
		// 	case 5:
		// 		fprintf(sink, "sexta");
        //         break;
		// 	case 6:
		// 		fprintf(sink, "septima");
        //         break;
		// 	case 7:
		// 		fprintf(sink, "octāva");
        //         break;
		// 	case 8:
		// 		fprintf(sink, "nōna");
        //         break;
		// 	case 9:
		// 		fprintf(sink, "decima");
        //         break;
		// 	case 10:
		// 		fprintf(sink, "ūndecima");
        //         break;
		// 	case 11:
		// 		fprintf(sink, "duodecima");
        //         break;
        //     default:
        //         assert(0 && "unreacheable");
        // }
		fprintf(sink, " diēī hōra");
    } else {
		int vigilia; 
		if (current_time < rise_time) {
			vigilia = (current_time + 24 * 3600 - prev_set_time) / prev_vigilia_duration;
			*progress = (double)((current_time + 24 * 3600 - prev_set_time) % prev_vigilia_duration) / (double) prev_vigilia_duration;
		} else {
			vigilia = (current_time - set_time) / next_vigilia_duration;
			*progress = (double)((current_time - set_time) % next_vigilia_duration) / (double) next_vigilia_duration;
		}

		roman_numeral(sink, vigilia + 1);
		// switch (vigilia) {
		// 	case 0:
		// 		fprintf(sink, "prīma");
        //         break;
		// 	case 1:
		// 		fprintf(sink, "secunda");
        //         break;
		// 	case 2:
		// 		fprintf(sink, "tertia");
        //         break;
		// 	case 3:
		// 		fprintf(sink, "quārta");
        //         break;
        //     default:
        //         assert(0 && "unreacheable");
        // }
		fprintf(sink, " noctis vigilia");
    }
}

// from hour_name
void print_stats(FILE* sink) {
	struct tm *tm;
	time_t t = time(NULL);
	tm = localtime(&t);

	int day = tm->tm_mday;
	int month = tm->tm_mon + 1;
	int year = tm->tm_year + 1900;
	int hour = tm->tm_hour;
	int minute = tm->tm_min;
	// int sec = tm->tm_sec;

	double yesterday_sunrise, yesterday_sunset;
	double today_sunrise, today_sunset;
	double tomorrow_sunrise, tomorrow_sunset;

	// no problem for month, it automatically fixes itself
	day_calc(day-1, month, year, LAT, LON, ELE, &yesterday_sunrise, &yesterday_sunset);
	day_calc(day, month, year, LAT, LON, ELE, &today_sunrise, &today_sunset);
	day_calc(day+1, month, year, LAT, LON, ELE, &tomorrow_sunrise, &tomorrow_sunset);

    t = yesterday_sunset;
    tm = localtime(&t);
	int prev_set_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = today_sunrise;
    tm = localtime(&t);
	int rise_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = today_sunset;
    tm = localtime(&t);
	int set_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    t = tomorrow_sunrise;
    tm = localtime(&t);
	int snd_rise_time = tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

	int daylight_duration = set_time - rise_time;
	int hour_duration = daylight_duration / 12;

	int prev_night_duration = 24 * 3600 - prev_set_time + rise_time;
	int prev_vigilia_duration = prev_night_duration / 4;
	int next_night_duration = 24 * 3600 - set_time + snd_rise_time;
	int next_vigilia_duration = next_night_duration / 4;
	// int avg_night_duration = (prev_night_duration + next_night_duration) / 2;
	int avg_vigilia_duration = (prev_vigilia_duration + next_vigilia_duration) / 2;

	int midday = rise_time + 6 * hour_duration;
	int midnight = ((prev_set_time + set_time) / 2 + 2 * avg_vigilia_duration) % (24 * 3600);

	fprintf(sink, "Current time:          %02d:%02d\n", hour, minute);
	fprintf(sink, "Times:\n");
	fprintf(sink, "  - Sunrise:           %02d:%02d\n", rise_time/3600, (rise_time%3600)/60);
	fprintf(sink, "  - Midday:            %02d:%02d\n", midday/3600, (midday%3600)/60);
	fprintf(sink, "  - Sunset:            %02d:%02d\n", set_time/3600, (set_time%3600)/60);
	fprintf(sink, "  - Midnight:          %02d:%02d\n", midnight/3600, (midnight%3600)/60);
	fprintf(sink, "Durations:\n");
	fprintf(sink, "  - Hour duration:     %02dh %02dm\n", hour_duration/3600, (hour_duration%3600)/60);
	fprintf(sink, "  - Vigil duration:    %02dh %02dm\n", avg_vigilia_duration/3600, (avg_vigilia_duration%3600)/60);
#if 0
	// int current_time = hour * 3600 + minute * 60 + sec;
	double progress = 0;
	if (current_time >= rise_time && current_time < set_time) {
		progress = (double)((current_time - rise_time) % hour_duration) / (double) hour_duration;
    } else {
		int vigilia; 
		if (current_time < rise_time) {
			vigilia = (current_time + 24 * 60 - prev_set_time) / prev_vigilia_duration;
			progress = (double)((current_time + 24 * 60 - prev_set_time) % prev_vigilia_duration) / (double) prev_vigilia_duration;
		} else {
			vigilia = (current_time - set_time) / next_vigilia_duration;
			progress = (double)((current_time - set_time) % next_vigilia_duration) / (double) next_vigilia_duration;
		}
    }
	fprintf(sink, "Current unit progress: %.2f%\n", progress*100);
#endif
}

void print_usage(FILE* sink, char* program) {
	fprintf(sink, "Usage: %s [-h] [-w] [-p] [-s]\n", program);
	// fprintf(sink, "\n");
	fprintf(sink, "Options:\n");
	fprintf(sink, "    -h  print help message and exit.\n");
	fprintf(sink, "    -w  watch mode: continue running and updating.\n");
	fprintf(sink, "    -p  show progress bar of the current hour/vigil.\n");
	fprintf(sink, "    -s  print statistics and exit.\n");
}

#define BUF_SIZE 128
char buffer[BUF_SIZE+1] = {0};

int main(int argc, char** argv) {
	int watch = 0;
	int pbar = 0;
	int help = 0;
	int stats = 0;

	for (int i = 1; i < argc; i++) {
		if (*argv[i] != '-') {
			fprintf(stderr, "[ERROR]: Unexpected argument: `%s`.\n\n", argv[i]);
			print_usage(stderr, argv[0]);
			return 1;
		}

		for (char* c = argv[i]+1; *c != 0; c++) {
			switch (*c)
			{
			case 'w':
				watch = 1;
				break;
			case 'p':
				pbar = 1;
				break;
			case 'h':
				help = 1;
				break;
			case 's':
				stats = 1;
				break;
			default:
				fprintf(stderr, "[ERROR]: Unknown option: `%s`.\n\n", argv[i]);
				print_usage(stderr, argv[0]);
				return 1;
			}
		}
	}

	if (help) {
		print_usage(stdout, argv[0]);
		return 0;
	} else if (stats) {
		print_stats(stdout);
		return 0;
	}

	double progress = 0;

    do {
		memset(buffer, 0, BUF_SIZE+1);
		FILE* sink = fmemopen(buffer, sizeof(buffer), "w");

        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        fprintf(stdout, "\e[J\33[2K\r");

		fprintf(sink, "[ ");
        hour_name(sink, tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, &progress);
        fprintf(sink, " | ");

		// the `hour_name` function modifies the struct
        tm = localtime(&t);

        day_name(sink, tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900);
        fprintf(sink, " | ");
        year_name(sink, tm->tm_year+1900);
        fprintf(sink, " ]");

		fclose(sink);
		
		int buf_len = strlen(buffer);
		if (buf_len > BUF_SIZE + 1) {
			buf_len = BUF_SIZE + 1;
		}
		int bar_width = (int) (progress * (double) (buf_len - 2));
		
		// char buf = 0;
		int end_placed = 0;
		for (int i = 0; i < buf_len; i++) {
			if (pbar && i == 1)
				fprintf(stdout, "\033[4m");
			// buf = buffer[i];
			fputc(buffer[i], stdout);
			if (pbar && (buffer[i] != '\xC4' && buffer[i] != '\xC5') && !end_placed && (i >= bar_width || i + 2 == buf_len)) {
				fprintf(stdout, "\033[0m");
				end_placed = 1;
			}
		}

        fflush(stdout);

		if (watch)
        	usleep(500*1000);
		
    } while (watch);

    fprintf(stdout, "\n");

    return 0;
}