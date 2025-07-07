#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include<queue>
#include<map>
#include<unordered_set>
#include "staff.h"
#include "member.h"
using namespace std;
class member;
class classes {
private:
    int id;
    int coach_id;
    string coachName;
    string title;
    string dateAndTime;
    int capacity;
    int size;

public:
    queue<int>wait_list;
    queue<int>wait_list_vip;
    map<int, string> listOfMembers;
    classes();
    classes(int id, int coach_id, string title, string coach, string datetime, int cap);
    static int classes_count;
    static map<int, classes> classess;
    static map<int, staff> staffs;
    static map<int, member> members;

    static int getclasscount();
    int getId() const;
    string getTitle() const;
    int getCoachID() const;
    string getCoachName() const;
    string getDateAndTime() const;
    int getCapacity() const;
    int getSize() const;

    string getCurrentDate();
    static string addMonthsToDate(const  string& currentDate, int monthsToAdd);
    static void display_member_classes_by_id(int mem_id);
    static void display_class_by_id(int id);
    static void display_coach_classes_by_id(int id);
    static void add_class();
    bool search_class_by_id(int mem_id, int class_id);
    static bool is_class_exist(int class_id);
    static void delete_class_by_id(int class_id);
    static void display_classes();
    static void join_class(int id);
    static void cancel_class(int id);

    static void decrementClassCount();
    void setId(int _id);
    void setTitle(string title);
    void setCoachName(string coach);
    void setDateAndTime(string datetime);
    void setCapacity(int cap);
    void setSize(int size);
    static void process_waitlist_promotion(classes& cls, int class_id);
    

};