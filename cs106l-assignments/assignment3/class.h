class Date{
private:
    int Year,Month,Day;
    bool isLeapYear() const;
    
public:
    Date();
    Date(int year,int month,int day);
    
    int get_Year() const;
    int get_Month() const;
    int get_Day() const;
    void setYear(int year);
    void setMonth(int month);
    void setDay(int Day);
};