#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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
    int n = num;
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
            printf("%s", syms[i]);
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

		if (day > kalendae && day < nonae)
			roman_numeral(sink, nonae+1-day);
		else if (day > kalendae && day < idus)
			roman_numeral(sink, idus+1-day);
		else if (day > kalendae)
			roman_numeral(sink, mlength+2-day);
    } else if (day == mlength || day == nonae-1 || day == idus-1) {
		fprintf(sink, "Prid.");
    }
	
	if (day == kalendae) {
		fprintf(sink, " Kal. ");
		month_name(sink, month);
    } else if (day <= nonae) {
		fprintf(sink, " Nōn. ");
		month_name(sink, month);
	} else if (day <= idus) {
		fprintf(sink, " Eid. ");
		month_name(sink, month);
	} else {
		fprintf(sink, " Kal. ");
		month_name(sink, (month)%12 + 1);
    }
}

void year_name(FILE* sink, int year) {
    roman_numeral(sink, year + 753);
    fprintf(sink, " AVC");
}

void hour_name(FILE* sink, int hour, int minute) {
	if (hour >= 6 && hour < 18) {
		switch(hour - 6) {
			case 0:
				fprintf(sink, "prīma");
                break;
			case 1:
				fprintf(sink, "secunda");
                break;
			case 2:
				fprintf(sink, "tertia");
                break;
			case 3:
				fprintf(sink, "quārta");
                break;
			case 4:
				fprintf(sink, "quīnta");
                break;
			case 5:
				fprintf(sink, "sexta");
                break;
			case 6:
				fprintf(sink, "septima");
                break;
			case 7:
				fprintf(sink, "octāva");
                break;
			case 8:
				fprintf(sink, "nōna");
                break;
			case 9:
				fprintf(sink, "decima");
                break;
			case 10:
				fprintf(sink, "ūndecima");
                break;
			case 11:
				fprintf(sink, "duodecima");
                break;
            default:
                assert(0 && "unreacheable");
        }
		fprintf(sink, " diēī hōra");
    } else {
		switch(((hour + 6) % 24) / 4) {
			case 0:
				fprintf(sink, "prīma");
                break;
			case 1:
				fprintf(sink, "secunda");
                break;
			case 2:
				fprintf(sink, "tertia");
                break;
			case 3:
				fprintf(sink, "quārta");
                break;
            default:
                assert(0 && "unreacheable");
        }
		fprintf(sink, " noctis vigilia");
    }
}

int main(int argc, const char** argv) {
    int watch = argc > 1 && strcmp(argv[1], "-w") == 0;

    do {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);

        fprintf(stdout, "\r[ ");
        hour_name(stdout, tm->tm_hour, tm->tm_min);
        fprintf(stdout, " | ");
        day_name(stdout, tm->tm_mday, tm->tm_mon+1, tm->tm_year);
        fprintf(stdout, " | ");
        year_name(stdout, tm->tm_year);
        fprintf(stdout, " ]");
        usleep(10*1000);
        fflush(stdout);
    } while (watch);
    fprintf(stdout, "\n");

    return 0;
}