#include <iostream>
#include "class.h"
bool Date::isLeapYear() const{
    if((this->Year%400==0)||(this->Year%4==0&&this->Year%100!=0)){
        return true;
    }
    return false;
}
void Date::setYear(int year){
    if(year<1){
       std::cout<<"Invalid value\n";
       return;
    }
    this->Year=year;
}
void Date::setMonth(int month){
    if(month<1||month>12){
        std::cout<<"Invalid value\n";
        return;
    }
    this->Month=month;
};
void Date::setDay(int day){
    this->Day=day;
}
Date::Date(){
    Year=2026;
    Month=9;
    Day=1;
}
Date::Date(int year,int month,int day){
    this->Year=year;
    this->Month=month;
    this->Day=day;
}

int Date::get_Year() const{
    return this->Year;
}
int Date::get_Month() const{
    return this->Month;
}
int Date::get_Day() const{
    return this->Day;
}