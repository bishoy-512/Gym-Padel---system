#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include <map>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "classes.h"  
#include "member.h"

using namespace std;

class member {
private:
    int id;
    string name;
    string date_of_birth;
    bool is_VIP;
    string password;
    
    
  
    // Helper methods
    static string getCurrentDate();
    static string addMonthsToDate(const string& currentDate, int monthsToAdd);
    static int daysBetweenDates(const string& date1, const string& date2);

public:
    std::map<int, std::string> subscription_plans = {
        {1, "1 month (Price: 500 EGP)"},
        {3, "3 months (Price: 1200 EGP)"},
        {6, "6 months (Price: 2500 EGP)"},
        {12, "12 months (Price: 5300 EGP)"}
    };
    map<int, set<pair<string, int>>> booked_courts;
    queue<string> notifications;
    stack<string> workout_history;
    string subscription_type, subscription_expire;
    map<int, pair<pair<string, string>, string>> classes_id;
    vector<string> inbox;
    map<int, pair<string, string>> subscriptions;/////////// request
    static map<int, member> all_members;  // Static container for all members
    static int member_count;
    member();
    member(int id, string name, string date_of_birth, bool is_VIP, string password);

    // Static member management
    static int getMemberCount();
    static void decrementMemberCount();
    static member get_member_by_id(int id);
    static bool is_member_exist(int id);
    static void display_all_members();
    static void display_member_by_id(int id);
    static void add_member();
    static void remove_member();

    // Member operations
    void display() const;
    void display_inbox() const;
    void display_classes() const;
    /*void join_class(int class_id);*/
    void cancel_class();
    void renew_subscription(int class_id, int months);
    void cancel_subscription(int class_id);
    static void generateMonthlyReport();//newwwwwwwwww
    // Workout history
    void add_workout(const string& workout);
    void display_workouts() const;

    // Getters and setters
    int getId() const;
    std::string getName() const;
    std::string getDateOfBirth() const;
    bool getIsVIP() const;
    std::string getPassword() const;
    void setId(int id);
    void setName(std::string name);
    void setDateOfBirth(std::string date_of_birth);
    void setIsVIP(bool is_VIP);
    void setPassword(std::string password);

    // Notification system
    void add_notification(const std::string& message);
    std::string get_next_notification();
    bool has_notifications() const;
    void check_subscription_expiry(const std::string& current_date);
    std::string get_subscription_plan(int period);
};