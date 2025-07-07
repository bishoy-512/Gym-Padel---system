#include "classes.h"
#include <iostream>
#include "staff.h"
#include "member.h"
#include "padel.h"
#include <chrono>
#include <fstream>
# include "FileHandler.h"
using json = nlohmann::json;
using namespace std;

int classes::classes_count = 0;
std::map<int, classes> classes::classess;
std::map<int, staff> classes::staffs;
std::map<int, member> classes::members;

classes::classes() {}

classes::classes(int id, int coach_id, string title, string coach, string datetime, int cap)
{
    this->id = id;
    this->coach_id = coach_id;
    this->title = title;
    this->coachName = coach;
    dateAndTime = datetime;
    capacity = cap;
    setSize(0);
    classes_count++;
}

string classes::getCurrentDate() { // HELPER FUNCTION LEAVE AS IS

    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);

    struct tm tm {};
    localtime_s(&tm, &currentTime);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

    return string(buffer);
}

string classes::addMonthsToDate(const  string& currentDate, int monthsToAdd) {

    struct tm tm {};

    sscanf_s(currentDate.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    tm.tm_mon += monthsToAdd;
    mktime(&tm);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

    return  string(buffer);
}

int classes::getclasscount() {
    return classes_count;
}

int classes::getId() const {
    return id;
}

string classes::getTitle() const {
    return title;
}

string classes::getCoachName() const {
    return coachName;
}

int classes::getCoachID() const {
    return coach_id;
}

string classes::getDateAndTime() const {
    return dateAndTime;
}

int classes::getCapacity() const {
    return capacity;
}

int classes::getSize() const {
    return size;
}

void classes::setId(int _id) {
    id = _id;
}

void classes::setTitle(string title) {
    this->title = title;
}

void classes::setCoachName(string coach) {
    coachName = coach;
}

void classes::setDateAndTime(string datetime) {
    dateAndTime = datetime;
}

void classes::setCapacity(int cap) {
    capacity = cap;
}

void classes::decrementClassCount() {
    classes_count--;
}

void classes::setSize(int size) {
    this->size = size;
}

void classes::display_class_by_id(int id) {
    cout << "-------------------\n";
    cout << "Class ID: " << classess[id].getId() << endl;
    cout << "Class Name: " << classess[id].getTitle() << endl;
    cout << "Coach ID: " << classess[id].getCoachID() << endl;
    cout << "Coach Name: " << classess[id].getCoachName() << endl;
    cout << "Date and Time: " << classess[id].getDateAndTime() << endl;
    cout << "Capacity: " << classess[id].getCapacity() << endl;
    cout << "Current Size: " << classess[id].getSize() << endl;
    cout << "-------------------\n";
}

void classes::display_coach_classes_by_id(int id) {
    bool s = false;
    for (auto i : classess) {
        if (i.second.getCoachID() == id) {
            s = true;
            display_class_by_id(i.first);
        }
    }
    if (!s)
        cout << "-------------------\nYou Don't manage any class yet\n-------------------\n";

}

void classes::add_class() {
    string title, coachName, dateAndTime;
    int coach_id;
    int capacity;
    cout << "Enter Class Name:\n";
    cin >> title;
    cout << "Enter Coach ID:\n";
    cin >> coach_id;
    if (staff::staffs[coach_id].getrole() == 'c') {
        cout << "Enter Date and Time:\n";
        cin >> dateAndTime;
        cout << "Enter Class Capacity:\n";
        cin >> capacity;
        if (capacity > 0) {
            classes new_class(classes::getclasscount() + 1, coach_id, title, staffs[coach_id].getname(), dateAndTime, capacity);
            classess[new_class.getId()] = new_class;
            cout << "-------------------\nClass Added Successfully\n-------------------\n";
        }
        else cout << "-------------------\nThe Capacity Cannot be Zero\n-------------------\n";
    }
    else cout << "-------------------\nNo Coach With This ID\n-------------------\n";
}

bool classes::search_class_by_id(int mem_id, int class_id) {
    if (members.find(mem_id) != members.end()) {
        return members[mem_id].classes_id.find(class_id) != members[mem_id].classes_id.end();
    }
    return false;
}

void classes::display_classes() {
    if (classess.size() > 0) {
        for (auto i : classess) {
            cout << "-------------------\n";
            cout << "Class ID: " << i.first << endl;
            cout << "Class Name: " << i.second.getTitle() << endl;
            cout << "Coach ID: " << i.second.getCoachID() << endl;
            cout << "Coach Name: " << i.second.getCoachName() << endl;
            cout << "Date and Time: " << i.second.getDateAndTime() << endl;
            cout << "Capacity: " << i.second.getCapacity() << endl;
            cout << "Current Size: " << i.second.getSize() << endl;
            cout << "-------------------\n";
        }
    }
    else
        cout << "-------------------\nNo Classes Exist\n-------------------\n";
}
// updated o(n) to  O(log n) 
bool classes::is_class_exist(int class_id) {
    return classess.find(class_id) != classess.end();
}

void classes::delete_class_by_id(int class_id) {
    classess.erase(class_id);
    classes::decrementClassCount();
    for (auto& member : members) {
        member.second.classes_id.erase(class_id);
    }
    for (auto& member : member::all_members) {
        member.second.classes_id.erase(class_id);
    }
    FileHandler::saveMembers(member::all_members, "members.json");
}

//changessssssss ------------------------------------------------------------------------------------

void classes::process_waitlist_promotion(classes& cls, int class_id) {
    int promoted_member_id = -1;
    string member_type;

    // Check VIP waitlist first
    if (!cls.wait_list_vip.empty()) {
        promoted_member_id = cls.wait_list_vip.front();
        cls.wait_list_vip.pop();
        member_type = "VIP";
    }
    // Then check regular waitlist
    else if (!cls.wait_list.empty()) {
        promoted_member_id = cls.wait_list.front();
        cls.wait_list.pop();
        member_type = "regular";
    }

    if (promoted_member_id != -1) {
        // Add promoted member to class
        string start_date = Padel::getCurrentDate();
        string exp_date = Padel::addDaysToDate(start_date, 60);

        cls.listOfMembers[promoted_member_id] = member::all_members[promoted_member_id].getName();
        cls.setSize(cls.getSize() + 1);

        // Update promoted member's records
        member::all_members[promoted_member_id].classes_id[class_id] = {
            {cls.getTitle(), start_date},
            exp_date
        };
        members[promoted_member_id].classes_id[class_id] = {
            {cls.getTitle(), start_date},
            exp_date
        };

        // Send notification
        string msg = "You've been promoted from " + member_type + " waitlist to class: " + cls.getTitle();
        member::all_members[promoted_member_id].add_notification(msg);
        members[promoted_member_id].add_notification(msg);

        cout << member_type << " member " << promoted_member_id << " added from waitlist.\n";
    }
}


void classes::cancel_class(int member_id) {
    // Check if member is enrolled in any classes
    if (member::all_members[member_id].classes_id.empty()) {
        cout << "You haven't joined any classes yet.\n";
        return;
    }

    // Display member's classes and get class to leave
    member::all_members[member_id].display_classes();
    cout << "Enter Class ID to leave: ";
    int class_id;
    cin >> class_id;

    // Validate the member is in this class
    if (!is_class_exist(class_id) ||
        member::all_members[member_id].classes_id.find(class_id) == member::all_members[member_id].classes_id.end()) {
        cout << "You are not enrolled in this class.\n";
        return;
    }

    // Get reference to the class
    classes& cls = classess[class_id];

    // 1. Remove leaving member from class
    cls.listOfMembers.erase(member_id);
    cls.setSize(cls.getSize() - 1);

    // 2. Remove class from leaving member's records
    member::all_members[member_id].classes_id.erase(class_id);
    members[member_id].classes_id.erase(class_id);

    cout << "Successfully left the class.\n";

    // 3. Process waitlist promotions
    process_waitlist_promotion(cls, class_id);

    // 4. Save all changes
    FileHandler().saveMembers(member::all_members, "members.json");
    FileHandler().saveClasses(classess, "classes.json");
}
//--------------------------------------------------------------------------------------------------------
/*
void classes::join_class(int member_id) { // UPDATED JOIN CLASS
    if (classess.empty()) {
        cout << "No classes available.\n";
        return;
    }

    display_classes();
    cout << "Enter Class ID to join:\n";
    int class_id;
    cin >> class_id;

    if (!is_class_exist(class_id)) {
        cout << "Invalid Class ID.\n";
        return;
    }

    classes cls = classess[class_id];
    if (member::all_members[member_id].classes_id.find(class_id) != member::all_members[member_id].classes_id.end()) {
        cout << "You are already enrolled in this class.\n";
        return;
    }

    if (cls.getSize() < cls.getCapacity()) {
        string start_date = Padel::getCurrentDate();
        string exp_date = Padel::addDaysToDate(start_date,60);
        members[member_id].classes_id[class_id] = {
            {cls.getTitle(), start_date},
            addMonthsToDate(start_date, 1)
        };
        member::all_members[member_id].classes_id[class_id] = { {cls.getTitle(), start_date}, exp_date };
        cls.setSize(cls.getSize() + 1);
        cout << "Successfully joined the class!\n";
    }
    else {
        if (member::all_members[member_id].getIsVIP()) {
            cls.wait_list_vip.push(member_id);
            cout << "Class full! Added to VIP waitlist.\n";
        }
        else {
            cls.wait_list.push(member_id);
            cout << "Class full! Added to regular waitlist.\n";
        }
    }

}
*/
void classes::join_class(int member_id) {
    if (classess.empty()) {
        cout << "No classes available.\n";
        return;
    }

    display_classes();
    cout << "Enter Class ID to join:\n";
    int class_id;
    cin >> class_id;

    if (!is_class_exist(class_id)) {
        cout << "Invalid Class ID.\n";
        return;
    }

    // Get reference to the actual class in the map
    classes& cls = classess[class_id];  // Note the & for reference

    if (member::all_members[member_id].classes_id.find(class_id) != member::all_members[member_id].classes_id.end()) {
        cout << "You are already enrolled in this class.\n";
        return;
    }

    if (cls.getSize() < cls.getCapacity()) {
        string start_date = Padel::getCurrentDate();
        string exp_date = Padel::addDaysToDate(start_date, 60);

        // Update member's class list
        member::all_members[member_id].classes_id[class_id] = {
            {cls.getTitle(), start_date},
            exp_date
        };

        // Update class enrollment
        cls.listOfMembers[member_id] = member::all_members[member_id].getName();
        cls.setSize(cls.getSize() + 1);

        cout << "Successfully joined the class!\n";
    }
    else {
        if (member::all_members[member_id].getIsVIP()) {
            cls.wait_list_vip.push(member_id);
            cout << "Class full! Added to VIP waitlist.\n";
        }
        else {
            cls.wait_list.push(member_id);
            cout << "Class full! Added to regular waitlist.\n";
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------------

void classes::display_member_classes_by_id(int id) {
    if (!member::all_members[id].classes_id.empty()) {
        for (const auto& entry : members[id].classes_id) {
            int class_id = entry.first;
            cout << "-------------------\n";
            cout << "Class ID: " << classess[class_id].getId() << endl;
            cout << "Class Name: " << classess[class_id].getTitle() << endl;
            cout << "Join Date: " << entry.second.first.second << endl;
            cout << "Expire Date: " << entry.second.second << endl;
            cout << "-------------------\n";
        }
    }
    else cout << "-------------------\nYou haven't joined any classes yet\n-------------------\n";
}
/*
void classes::cancel_class(int member_id) {
    // CHECK IF MEMBER IS IN ANY CLASSES
    if (member::all_members[member_id].classes_id.size() > 0) {
        display_member_classes_by_id(member_id);
        cout << "Enter Class ID to leave:\n";
        int class_id;
        cin >> class_id;

        if (is_class_exist(class_id) && member::all_members[member_id].classes_id.find(class_id) != member::all_members[member_id].classes_id.end()) {
            classess[class_id].setSize(classess[class_id].getSize() - 1);
            members[member_id].classes_id.erase(class_id);
            member::all_members[member_id].classes_id.erase(class_id);
            cout << "Successfully left the class.\n";

            auto& cls = classess[class_id];
            //CHECK VIP WAITLIST FIRST
            if (!cls.wait_list_vip.empty()) {
                // IF VIP -> POP
                int vip_member = cls.wait_list_vip.front();
                cls.wait_list_vip.pop();

                cls.listOfMembers[vip_member] = members[vip_member].getName();
                cls.setSize(cls.getSize() + 1);
                members[vip_member].inbox.push_back("You ve been added to class " + cls.getTitle());
                // NOTIFICATION
                cout << "VIP member " << vip_member << " added from waitlist.\n";
            }
            // IF NO VIP -> CHECK REGULAR WAITLIST
            else if (!cls.wait_list.empty()) {
                int reg_member = cls.wait_list.front();
                cls.wait_list.pop();
                cls.listOfMembers[reg_member] = members[reg_member].getName();
                cls.setSize(cls.getSize() + 1);
                members[reg_member].inbox.push_back("You ve been added to class " + cls.getTitle());
                cout << "Member " << reg_member << " added from waitlist.\n";
            }
        }
        else {
            cout << "You are not enrolled in this class.\n";
        }
    }
    else {
        cout << "You haven t joined any classes yet.\n";
    }
}
*/


