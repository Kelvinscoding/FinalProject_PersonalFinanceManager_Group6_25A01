#pragma once
#include <iostream>
#include <string>
#include <ctime>

class Date {
private:
	int day;
	int month;
	int year;

public:

	//Helper Functions
	static bool isLeap(int y)  {
		return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
	}

	static int daysInMonth(int m, int y)  {
		if (m < 1 || m > 12) return 0;
		const int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		if (m == 2 && isLeap(y)) return 29;
		return days[m];
	}


	Date(int d = 1, int m = 1, int y = 2025) : day(d), month(m), year(y) {}
	static Date getCurrentDate() {
    time_t now = time(0);
    struct tm* ltm = localtime(&now);
    if (ltm) {
        return Date(ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
    }
    return Date(); 
}


	int getDay() const { return day; }
	int getMonth() const { return month; }
	int getYear() const { return year; }
	//Output
	void outputDate(std::ostream& os = std::cout) const {
		os << day << "-" << month << "-" << year;
	}

    
	//Comparison Operators

	//Less than 
	bool operator<(const Date& other) const {
		if (year != other.year) return year < other.year;
		if (month != other.month) return month < other.month;
		return day < other.day;
	}
	
	//Greater than
	bool operator>(const Date& other) const {
		return other < *this;
	}

	//Equal to
	bool operator==(const Date& other) const {
		return day == other.day && month == other.month && year == other.year;
	}
};