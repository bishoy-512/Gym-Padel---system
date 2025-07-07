#pragma once
#include <iostream>
#include <string>
#include<map>
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

class Padel {
private:
    int courtID;
    string date;

    bool isBooked;
    string bookedBy;



public:
    static int padel_count;
    map<string, map<int, pair<bool, string>>> Courts;  // date -> (time -> (booked, bookedBy))
    int StartTime;
    string location;
    int EndTime;
    map<int, bool>timeMap;
    Padel();
    Padel(int id, string location, string date, int StartTime, int EndTime);

    int getCourtID() const;
    static int getPadelCount();
    string getLocation() const;
    string getDate() const;
    bool getIsBooked() const;
    string getBookedBy() const;
    int getStartTime() const;
    int getEndTime() const;

    map<int, bool> creatTimemap(int St, int Et);
    static void decrementPadelCount();
    void setPadelCount(int id);
    void setCourtID(int id);
    void setLocation(const string& loc);
    void setDate(const string& d);
    void setIsBooked(bool booked);
    void setBookedBy(const string& name);
    void setStartTime(int St);
    void setEndTime(int Et);

    void book_padel_court(int mem_id, int p_id, int time, string date);

    bool isTodayOrFutureDate(string date);
    static string  getCurrentDate();
    static string addDaysToDate(const string& dateStr, int days);
    static vector<Padel> loadFromTemplates();

    void displayDateAvailability(const string& date, int p_id);
    void display_closest_time_court(string date, int time, int mem_id);
    void setBooked(const string& date, int time, bool booked, const string& name);
    string getBookedBy(const string& date, int time) const;
    int getCurrentHour();
    void cancel_booked_courts(string date, string location, int time, int mem_id , int p_id);
    bool is_court_exist(int p_id);
    static void Add_new_court(vector<Padel>& padelCourts, const string& filename);
    static void Remove_court(vector<Padel>& padelCourts, const string& filename);


    //files
    nlohmann::json toJson() const {
        return {
            {"padel_count", padel_count},
            {"courtID", courtID},
            {"location", location},
            {"date", date},
            {"isBooked", isBooked},
            {"bookedBy", bookedBy},
            {"StartTime", StartTime},
            {"EndTime", EndTime},
            {"Courts", Courts}
        };
    }

    void fromJson(const nlohmann::json& j) {
        j.at("padel_count").get_to(padel_count);
        j.at("courtID").get_to(courtID);
        j.at("location").get_to(location);
        j.at("date").get_to(date);
        j.at("isBooked").get_to(isBooked);
        j.at("bookedBy").get_to(bookedBy);
        j.at("StartTime").get_to(StartTime);
        j.at("EndTime").get_to(EndTime);
        j.at("Courts").get_to(Courts);
    }
};
