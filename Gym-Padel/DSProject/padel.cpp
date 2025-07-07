#include "Padel.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <stack>
#include "json.hpp"
using json = nlohmann::json;

#include <fstream>
#include"FileHandler.h"

using namespace std;
using namespace chrono;

int Padel::padel_count = 0;

Padel::Padel() : courtID(0), isBooked(false), bookedBy("") {}

// making padel court available for 30 days from now and time (9 Am -> 10 Pm)
Padel::Padel(int id, string loc, string date, int St, int Et)
    :courtID(id), location(loc), date(date), StartTime(St), EndTime(Et) {
    //this->timeMap = creatTimemap(St, Et);
    string current = getCurrentDate(); // the local day 
    for (int day = 0; day < 30; day++)
    {
        string date = addDaysToDate(current, day);
        for (int hr = St; hr <= Et; hr++)
        {
            Courts[date][hr].first = false; //unBooked yet 
        }
    }
    padel_count++;
}



void Padel::displayDateAvailability(const string& date, int p_id) {
    if (Courts.find(date) == Courts.end()) {
        cout << "Date not available (beyond 1 month window)\n";
        return;
    }
    cout << "Availability for court " << p_id << " on " << date << ":\n";
    for (const auto& slot : Courts[date]) {
        // slot.second is now a pair<bool, string>
        cout << slot.first << ":00 - "
            << (slot.second.first ? "[BOOKED by " + slot.second.second + "]" : "[AVAILABLE]")
            << "\n";
    }
}

void Padel::book_padel_court(int mem_id, int p_id, int time, string date) {
    if (member::all_members[mem_id].booked_courts.find(p_id) == member::all_members[mem_id].booked_courts.end()) {
        if (Courts.find(date) == Courts.end()) {
            cout << "Date not available (beyond 1 month window)\n";
            return;
        }
        // Check if the time is within operating hours
        if (time < StartTime || time > EndTime) {
            cout << "-------------------\n"
                << "Invalid time. Court operates between " << StartTime << ":00 and " << EndTime << ":00.\n"
                << "-------------------\n";
            return;
        }

        if (!Courts[date][time].first) {
            member::all_members[mem_id].booked_courts[p_id].insert({ date, time });
            Courts[date][time].first = true;
            //Courts[date][time].second = members[mem_id].getName();
            cout << "Booked successfully for time slot " << time << ":00" << endl; // Optionally add user info
        }
        else {
            cout << "-------------------\nThis time slot is already booked\n-------------------\n";
            cout << "-------------------\nThere Is The Avalible Courts\n-------------------\n";
            displayDateAvailability(date, p_id);
        }
    }
    else {
        cout << "-------------------\nYou Already Booked This Court\n-------------------\n";
    }
}

void Padel::display_closest_time_court(string date, int time, int mem_id) {
    stack<int>s;
    int c;
    if (this->location == location && time >= StartTime && time <= EndTime) {
        for (int hr = EndTime; hr > time; hr--)
        {
            if (!Courts[date][hr].first) {
                s.push(hr);
            }
        }
    }

    if (!s.empty()) {
        cout << "The Closest Available Time for court " << courtID << " is " << s.top() << ":00\n";
        cout << "Book It ?\n1 - yes\n2 - no\n";
        cin >> c;
        if (c == 1)
            book_padel_court(mem_id, this->courtID, s.top(), date);
        else
            cout << "-------------------\nNo available courts found\n-------------------\n";
    }
}


void Padel::cancel_booked_courts(string date, string location, int time, int mem_id, int p_id) {
    int current_hr = getCurrentHour();
    string current_date = getCurrentDate();
    if (member::all_members[mem_id].booked_courts.find(p_id) != member::all_members[mem_id].booked_courts.end()) {

        if (date != current_date) {
            Courts[date][time].first = false;
            Courts[date][time].second = "";
            member::all_members[mem_id].booked_courts[p_id].erase({ date, time });
            if (member::all_members[mem_id].booked_courts[p_id].empty()) {
                member::all_members[mem_id].booked_courts.erase(p_id);
            }
            cout << "Cancellation is done :) \n";
        }
        else if (time - current_hr > 3) {
            Courts[date][time].first = false;
            Courts[date][time].second = "";
            member::all_members[mem_id].booked_courts[p_id].erase({ date, time });
            if (member::all_members[mem_id].booked_courts[p_id].empty()) {
                member::all_members[mem_id].booked_courts.erase(p_id);
            }
            cout << "Cancellation is done :) \n";
        }
        else {
            cout << "Sorry, you can't cancel :(\n";
        }
    }
    else {
        cout << "You Don't Booked This Court\n";
    }
}

int Padel::getCurrentHour() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
    localtime_s(&local_tm, &now_time);
    return local_tm.tm_hour;  // Current hour (0-23)
}


void Padel::Add_new_court(vector<Padel>& padelCourts, const string& filename) {
    int newId, start, end;
    string loc;
    cout << "Enter the details of the new padel court :";
    cout << "Enter the id :";
    cin >> newId;
    for (const auto& court : padelCourts) {
        if (court.getCourtID() == newId) {
            cout << "Error this id already exists :( ";
            return;
        }
    }
    cin.ignore();
    cout << "Enter the Location :";
    getline(cin, loc);
    cout << "Enter the opening time :  (0 - 23)";
    cin >> start;
    while (start < 0 || start >23) {
        cout << "Invalid time Enter another time :";
        cin >> start;
    }
    cout << "Enter the closing time : ";
    cin >> end;
    while (end <= start || end > 23) {
        cout << "Invalid closing time Enter another time :";
        cin >> end;
    }
    string currentDate = getCurrentDate();
    Padel newCourt(newId, loc, currentDate, start, end);
    padelCourts.push_back(newCourt);
    padel_count++;
    FileHandler::savePadelCourts(padelCourts, filename);
    cout << "New padel court added successfully!\n";
    cout << "Total courts now: " << padel_count << "\n";
}

void Padel::Remove_court(vector<Padel>& padelCourts, const string& filename) {
    int courtId;
    cout << "Enter court ID to remove: ";
    cin >> courtId;
    for (auto it = padelCourts.begin(); it != padelCourts.end(); ++it) {
        if (it->getCourtID() == courtId) {
            padelCourts.erase(it);
            padel_count--;
            FileHandler::savePadelCourts(padelCourts, filename);
            cout << "Court removed successfully!"<<endl;
            return;
        }
    }
    for (auto i : member::all_members) {
        i.second.booked_courts.erase(courtId);
    }
    cout << "Court not found!"<<endl;
}


string Padel::getBookedBy(const string& date, int time) const {
    auto itDate = Courts.find(date);
    if (itDate != Courts.end()) {
        auto itTime = itDate->second.find(time);
        if (itTime != itDate->second.end()) {
            return itTime->second.second;
        }
    }
    return "";
}



string Padel::addDaysToDate(const string& dateStr, int days) {
    tm tm = {};
    istringstream ss(dateStr);
    ss >> get_time(&tm, "%Y-%m-%d");
    time_t tt = mktime(&tm);
    tt += 24 * 60 * 60 * days; // Add seconds for days
    tm = {};
    localtime_s(&tm, &tt);
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d");
    return oss.str();
}
string Padel::getCurrentDate() {
    auto now = system_clock::now();
    time_t tt = system_clock::to_time_t(now);
    tm tm = {};
    localtime_s(&tm, &tt);
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d");
    return oss.str();
}


int Padel::getPadelCount() { return padel_count; }
int Padel::getCourtID() const { return courtID; }
string Padel::getLocation() const { return location; }
string Padel::getDate() const { return date; }
bool Padel::getIsBooked() const { return isBooked; }
string Padel::getBookedBy() const { return bookedBy; }
int Padel::getStartTime()const { return StartTime; }
int Padel::getEndTime()const { return EndTime; }

void Padel::decrementPadelCount() {
    padel_count--;
}
void Padel::setPadelCount(int id) { padel_count = id; }
void Padel::setCourtID(int id) { courtID = id; }
void Padel::setLocation(const string& loc) { location = loc; }
void Padel::setDate(const string& d) { date = d; }
void Padel::setIsBooked(bool booked) { isBooked = booked; }
void Padel::setBookedBy(const string& name) { bookedBy = name; }
void Padel::setStartTime(int St) { StartTime = St; }
void Padel::setEndTime(int Et) { EndTime = Et; }

void Padel::setBooked(const string& date, int time, bool booked, const string& name) {
    Courts[date][time] = make_pair(booked, name);
}
