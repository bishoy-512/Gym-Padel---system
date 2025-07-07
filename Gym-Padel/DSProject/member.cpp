#include "member.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "classes.h"
#include <fstream>
#include "FileHandler.h"
using json = nlohmann::json;

// Initialize static members
int member::member_count = 0;
std::map<int, member> member::all_members;
vector<int> expired_classes;  // Store classes that need to be cancelled (new for subscription logic)
// Helper Functions
std::string member::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);

    struct tm tm;
    localtime_s(&tm, &currentTime);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

    return std::string(buffer);
}

std::string member::addMonthsToDate(const std::string& currentDate, int monthsToAdd) {
    struct tm tm = {};
    sscanf_s(currentDate.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    tm.tm_mon += monthsToAdd;
    mktime(&tm);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

    return std::string(buffer);
}

int member::daysBetweenDates(const std::string& date1, const std::string& date2) {
    tm tm1 = {}, tm2 = {};
    std::istringstream ss1(date1), ss2(date2);
    ss1 >> std::get_time(&tm1, "%Y-%m-%d");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d");

    if (ss1.fail() || ss2.fail()) return -1;

    tm1.tm_hour = tm2.tm_hour = 0;
    tm1.tm_min = tm2.tm_min = 0;
    tm1.tm_sec = tm2.tm_sec = 0;
    tm1.tm_isdst = tm2.tm_isdst = -1;

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == -1 || time2 == -1) return -1;

    double difference = difftime(time2, time1) / (60 * 60 * 24);
    return static_cast<int>(difference);
}

// Constructors
member::member() : id(0), is_VIP(false) {}

member::member(int id, std::string name, std::string date_of_birth,
    bool is_VIP, std::string password) :
    id(id), name(name), date_of_birth(date_of_birth),
    is_VIP(is_VIP), password(password) {
    member_count++;
}

// Static Member Management
int member::getMemberCount() { return member_count; }

void member::decrementMemberCount() { member_count--; }

member member::get_member_by_id(int id) {
    auto it = all_members.find(id);
    if (it != all_members.end()) {
        return it->second;
    }
    std::cout << "No member exists with ID: " << id << "\n";
    return member();
}

bool member::is_member_exist(int id) {
    return all_members.find(id) != all_members.end();
}

void member::display_all_members() {
    if (all_members.empty()) {
        std::cout << "No members exist yet\n";
        return;
    }

    for (const auto& pair : all_members) {
        pair.second.display();
    }
}

void member::add_member() {
    std::string name, date_of_birth, password;
    bool is_VIP;
    int choice;

    std::cout << "Enter name: ";
    std::getline(std::cin >> std::ws, name);
    std::cout << "Enter date of birth (YYYY-MM-DD): ";
    std::cin >> date_of_birth;
    std::cout << "Enter password: ";
    std::cin >> password;
    std::cout << "Is VIP? (1 for yes, 0 for no): ";
    std::cin >> choice;
    is_VIP = (choice == 1);

    int new_id = member_count ;
    all_members[new_id] = member(new_id, name, date_of_birth, is_VIP, password);
    cout << "Member added successfully with ID: " << new_id << "\n";
}

void member::remove_member() {
    if (all_members.empty()) {
        std::cout << "No members to remove\n";
        return;
    }

    display_all_members();
    std::cout << "Enter member ID to remove: ";
    int id;
    std::cin >> id;

    if (all_members.erase(id)) {
        decrementMemberCount();
        std::cout << "Member removed successfully\n";
    }
    else {
        std::cout << "No member exists with ID: " << id << "\n";
    }
}

// Member Operations
void member::display() const {
    std::cout << "-------------------\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Date of Birth: " << date_of_birth << "\n";
    std::cout << "VIP Status: " << (is_VIP ? "Yes" : "No") << "\n";
    std::cout << "-------------------\n";
}

void member::display_classes() const {
    if (classes_id.empty()) {
        std::cout << "No classes joined yet\n";
        return;
    }

    for (const auto& pair : classes_id) {
        std::cout << "-------------------\n";
        std::cout << "Class ID: " << pair.first << "\n";
        std::cout << "Class Name: " << pair.second.first.first << "\n";
        std::cout << "Join Date: " << pair.second.first.second << "\n";
        std::cout << "Expiry Date: " << pair.second.second << "\n";
    }
    std::cout << "-------------------\n";
}

void member::display_inbox() const {
    if (inbox.empty()) {
        std::cout << "No messages in inbox\n";
        return;
    }

    std::cout << "Messages:\n";
    for (const auto& message : inbox) {
        std::cout << " - " << message << "\n";
    }
}
//void member::join_class(int class_id) {// updated foe subscription logic
//    // Open the file first to check existing data
//    std::ifstream file("members.json");
//    json memberJson;
//    file >> memberJson;
//    file.close();
//
//    // Check if member exists in file
//    if (!memberJson["members"].contains(std::to_string(id))) {
//        // If member doesn't exist in file, create empty classes object
//        memberJson["members"][std::to_string(id)]["classes"] = json::object();
//    }
//
//    // Check if class already exists in file
//    if (memberJson["members"][std::to_string(id)]["classes"].contains(std::to_string(class_id))) {
//        std::cout << "You are already enrolled in this class.\n";
//        return;
//    }
//
//    // Continue with the rest of the code...
//    std::string start_date = getCurrentDate();
//    std::string end_date = addMonthsToDate(start_date, 1);
//
//    // Add class to member's memory
//    classes_id[class_id] = {
//        {"Class Name", start_date},
//        end_date
//    };
//
//    subscriptions[class_id] = { start_date, end_date };
//    inbox.push_back("You have successfully joined class ID: " + std::to_string(class_id));
//
//    // Update member's data in JSON
//    memberJson["members"][std::to_string(id)]["classes"][std::to_string(class_id)] = {
//        {"class_name", "Class Name"},
//        {"join_date", start_date},
//        {"expiry_date", end_date}
//    };
//
//    // Add message to inbox
//    memberJson["members"][std::to_string(id)]["inbox"].push_back(
//        "You have successfully joined class ID: " + std::to_string(class_id)
//    );
//
//    // Save changes to file
//    std::ofstream out("members.json");
//    out << std::setw(4) << memberJson << std::endl;
//    out.close();
//}
void member::cancel_class() {
    if (classes_id.empty()) {
        std::cout << "Not enrolled in any classes\n";
        return;
    }

    display_classes();
    std::cout << "Enter class ID to cancel: ";
    int class_id;
    std::cin >> class_id;

    if (classes_id.erase(class_id)) {
        subscriptions.erase(class_id);
        inbox.push_back("You have cancelled class ID: " + std::to_string(class_id));
        std::cout << "Class cancelled successfully\n";
    }
    else {
        std::cout << "Not enrolled in this class\n";
    }
}

void member::display_member_by_id(int id) {
    cout << "-------------------\n";
    cout << "Name: " << all_members[id].getName() << endl;
    cout << "Date Of birth: " << all_members[id].getDateOfBirth() << endl;
    cout << "VIP: " << (all_members[id].getIsVIP() ? "Yes" : "No") << endl;
    cout << "ID: " << all_members[id].getId() << endl;
    cout << "password: " << all_members[id].getPassword() << endl;
    cout << "-------------------\n";

}
void member::renew_subscription(int class_id, int months) {// updated (new for subscription logic)
   
    auto class_it = classes_id.find(class_id);
    if (class_it != classes_id.end()) {
        std::string current_expiry = class_it->second.second;
        std::string new_expiry = addMonthsToDate(current_expiry, months);
        class_it->second.second = new_expiry;
        if (subscriptions.find(class_id) != subscriptions.end()) {
            subscriptions[class_id].second = new_expiry;
        }
        inbox.push_back("Subscription renewed for class ID: " + std::to_string(class_id) +
            " until " + new_expiry);//done
    }
    else {
        std::cout << "Not subscribed to this class\n";
    }
}

void member::cancel_subscription(int class_id){ 
    if (subscriptions.erase(class_id)) {//done  (new for subscription logic)
        classes_id.erase(class_id);
        inbox.push_back("Subscription cancelled for class ID: " + std::to_string(class_id));
    }
    else {
        std::cout << "No active subscription for this class\n";
    }
}

// Workout History
void member::add_workout(const std::string& workout) {
    std::string timestamp = getCurrentDate();
    this->workout_history.push(timestamp + ": " + workout);
    FileHandler().saveMembers(member::all_members, "members.json");
}

void member::display_workouts() const {
    if (workout_history.empty()) {
        cout << "No workouts recorded\n";
        return;
    }

    auto temp = workout_history;
    
    cout << "Workout History (Newest First):\n";
    while (!temp.empty()) {
        cout << " - " << temp.top() << "\n";
        temp.pop();
    }
}
/*
void member::display_workouts() const {
    if (workout_history.empty()) {
        std::cout << "No workouts recorded\n";
        return;
    }

    std::stack<std::string> temp = workout_history;
    std::cout << "Workout History:\n";
    while (!temp.empty()) {
        std::cout << " - " << temp.top() << "\n";
        temp.pop();
    }
}
*/


// Notification System
void member::add_notification(const std::string& message) {
    notifications.push(message);
    inbox.push_back(message);// new for subscription logic
}

std::string member::get_next_notification() {
    if (notifications.empty()) return "";

    std::string msg = notifications.front();
    notifications.pop();
    return msg;
}

bool member::has_notifications() const {
    return !notifications.empty();
}

void member::check_subscription_expiry(const std::string& current_date) {
    for (const auto& class_pair : classes_id) {
        int class_id = class_pair.first;
        string expiry_date = class_pair.second.second;  // Get expiry date from classes_id

        int days_remaining = daysBetweenDates(current_date, expiry_date);

        if (days_remaining <= 0) {
            add_notification("URGENT: Subscription for class ID " +
                to_string(class_id) + " has expired!");
        // add logic to cancel class    (new)   (new for subscription logic)
            expired_classes.push_back(class_id);// done
        }
        else if (days_remaining <= 3) {
            add_notification("Warning: Subscription for class ID " +
                to_string(class_id) + " expires in " +
                to_string(days_remaining) + " days");
        }
        else if (days_remaining <= 7) {
            add_notification("Reminder: Subscription for class ID " +
                to_string(class_id) + " will expire soon");
        }

        // Early renewal offer
        if (days_remaining >= 7 && days_remaining <= 14) {
            add_notification("Special Offer: Renew your subscription for class ID " +
                to_string(class_id) + " now for 10% discount!");
        }
    }
    // Remove expired classes                             (new for subscription logic)
    for (int class_id : expired_classes) {
        if (classes_id.erase(class_id)) {
            // Remove from member's subscriptions
            subscriptions.erase(class_id);

            // Remove member from class's list of members     (new for subscription logic)
            if (classes::classess.find(class_id) != classes::classess.end()) {
                classes::classess[class_id].listOfMembers.erase(this->id);
            }

            inbox.push_back("Class ID: " + to_string(class_id) + " has been automatically cancelled due to expired subscription");
        }
    }

}

// Static helper for subscription plans
std::string member::get_subscription_plan(int period) {
    auto it = subscription_plans.find(period);
    return it != subscription_plans.end() ? it->second : "Invalid subscription period";
}
// Getters and Setters
int member::getId() const { return id; }
std::string member::getName() const { return name; }
std::string member::getDateOfBirth() const { return date_of_birth; }
bool member::getIsVIP() const { return is_VIP; }
std::string member::getPassword() const { return password; }

void member::setId(int id) { this->id = id; }
void member::setName(std::string name) { this->name = name; }
void member::setDateOfBirth(std::string date_of_birth) { this->date_of_birth = date_of_birth; }
void member::setIsVIP(bool is_VIP) { this->is_VIP = is_VIP; }
void member::setPassword(std::string password) { this->password = password; }
// active member(newwwwwwwwww)
// Monthly Activity and Revenue Report
void member::generateMonthlyReport() {
    // 1. Analyze member activity
    map<int, int> memberActivity;
    for (const auto& member : all_members) {
        // Calculate activity score based on classes, padel courts, and workouts
        int activityScore = 0;

        // Count classes
        activityScore += member.second.classes_id.size();

        // Count padel courts
        activityScore += member.second.booked_courts.size();

        // Count workouts
        activityScore += member.second.workout_history.size();

        memberActivity[member.first] = activityScore;
    }

    // Sort members by activity level
    vector<pair<int, int>> sortedActivity(memberActivity.begin(), memberActivity.end());
    sort(sortedActivity.begin(), sortedActivity.end(),
        [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second;
        });

   

    // Display the report
    cout << "\nMonthly Activity Report\n";
    cout << "======================\n\n";

    // Display top 3 most active members
    cout << "Top 3 Most Active Members:\n";
    cout << "------------------------\n";
    int count = 0;
    for (const auto& entry : sortedActivity) {
        if (count >= 3) break;

        // Get member details
        const member& m = all_members[entry.first];
        int classes = m.classes_id.size();
        int padel = m.booked_courts.size();
        int workouts = m.workout_history.size();

        cout << "Name: " << m.getName() << "\n";
        cout << "Classes: " << classes << "\n";
        cout << "Padel Courts: " << padel << "\n";
        cout << "Workouts: " << workouts << "\n";
        cout << "Total Activity: " << entry.second << "\n\n";

        count++;
    }
   
}
