#include "FileHandler.h"
#include <ctime> // For getCurrentDate()

// ======================= Helper Methods =======================
std::string FileHandler::getCurrentDate() {
    time_t now = time(0);
    struct tm tmStruct;
    localtime_s(&tmStruct, &now);  // Safe version
    return std::to_string(1900 + tmStruct.tm_year) + "-" +
        std::to_string(1 + tmStruct.tm_mon) + "-" +
        std::to_string(tmStruct.tm_mday);
}

// ======================= Padel Operations =======================
void FileHandler::savePadelCourts(const std::vector<Padel>& padelCourts, const std::string& filename) {
    json j;
    for (const auto& court : padelCourts) {
        json courtJson;
        courtJson["courtID"] = court.getCourtID();
        courtJson["location"] = court.getLocation();
        courtJson["StartTime"] = court.getStartTime();
        courtJson["EndTime"] = court.getEndTime();

        // Save bookings (VS-compatible)
        json bookingsJson;
        for (const auto& dateSlot : court.Courts) {
            const std::string& date = dateSlot.first;
            const auto& timeSlots = dateSlot.second;

            for (const auto& timeSlot : timeSlots) {
                int time = timeSlot.first;
                const auto& booking = timeSlot.second;

                if (booking.first) {
                    bookingsJson[date][std::to_string(time)] = {
                        booking.first,
                        booking.second
                    };
                }
            }
        }
        courtJson["Bookings"] = bookingsJson;
        j["padel_courts"].push_back(courtJson);
    }

    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
    }
    else {
        std::cerr << "Error saving padel courts to file: " << filename << "\n";
    }
}

std::vector<Padel> FileHandler::loadPadelCourts(const std::string& filename) {
    std::vector<Padel> courts;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Note: No Padel data found. Loading templates.\n";
        return loadPadelTemplates();
    }

    try {
        json j;
        inFile >> j;
        inFile.close();

        std::string currentDate = getCurrentDate();

        if (j.contains("padel_courts")) {
            for (const auto& courtJson : j["padel_courts"]) {
                Padel court(
                    courtJson["courtID"],
                    courtJson["location"],
                    currentDate,
                    courtJson["StartTime"],
                    courtJson["EndTime"]
                );

                if (courtJson.contains("Bookings")) {
                    for (auto& dateBooking : courtJson["Bookings"].items()) {
                        string date = dateBooking.key();
                        for (auto& timeBooking : dateBooking.value().items()) {
                            int time = stoi(timeBooking.key());
                            bool isBooked = timeBooking.value()[0];
                            string bookedBy = timeBooking.value()[1];

                            if (isBooked) {
                                court.setBooked(date, time, true, bookedBy);
                            }
                        }
                    }
                }
                courts.push_back(court);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading Padel courts: " << e.what() << "\nLoading templates instead.\n";
        return loadPadelTemplates();
    }
    return courts;
}

std::vector<Padel> FileHandler::loadPadelTemplates() {
    std::vector<Padel> courts;
    courts.emplace_back(1, "Downtown Sports Club", getCurrentDate(), 9, 22);
    courts.emplace_back(2, "Riverside Padel Center", getCurrentDate(), 9, 22);
    courts.emplace_back(3, "Greenfield Athletic Club", getCurrentDate(), 10, 21);
    courts.emplace_back(4, "Westside Padel Arena", getCurrentDate(), 8, 20);
    courts.emplace_back(5, "Central City Sports Complex", getCurrentDate(), 9, 22);
    courts.emplace_back(6, "Eastside Padel Club", getCurrentDate(), 10, 20);
    return courts;
}

// ======================= Classes Operations =======================
void FileHandler::saveClasses(const std::map<int, classes>& classesMap, const std::string& filename) {
    json j;
    for (std::map<int, classes>::const_iterator it = classesMap.begin(); it != classesMap.end(); ++it) {
        const classes& cls = it->second;
        json classJson;
        classJson["id"] = cls.getId();
        classJson["coach_id"] = cls.getCoachID();
        classJson["title"] = cls.getTitle();
        classJson["coachName"] = cls.getCoachName();
        classJson["dateAndTime"] = cls.getDateAndTime();
        classJson["capacity"] = cls.getCapacity();
        classJson["size"] = cls.getSize();

        // Save VIP waitlist
        json waitlistVipJson = json::array();
        std::queue<int> temp_vip = cls.wait_list_vip; // Make a copy
        while (!temp_vip.empty()) {
            waitlistVipJson.push_back(temp_vip.front());
            temp_vip.pop();
        }
        classJson["wait_list_vip"] = waitlistVipJson;

        // Save regular waitlist
        json waitlistJson = json::array();
        std::queue<int> temp_wait = cls.wait_list; // Make a copy
        while (!temp_wait.empty()) {
            waitlistJson.push_back(temp_wait.front());
            temp_wait.pop();
        }
        classJson["wait_list"] = waitlistJson;

        // Save list of members
        json membersJson;
        for (std::map<int, std::string>::const_iterator memberIt = cls.listOfMembers.begin();
            memberIt != cls.listOfMembers.end(); ++memberIt) {
            membersJson[std::to_string(memberIt->first)] = memberIt->second;
        }
        classJson["listOfMembers"] = membersJson;

        j["classes"].push_back(classJson);
    }

    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
    }
    else {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
    }
}

std::map<int, classes> FileHandler::loadClasses(const std::string& filename) {
    std::map<int, classes> loadedClasses;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Note: No existing class data found. Loading templates.\n";
        return loadClassTemplates();
    }

    try {
        json j;
        inFile >> j;
        inFile.close();

        if (j.find("classes") != j.end()) {
            for (json::const_iterator classIt = j["classes"].begin(); classIt != j["classes"].end(); ++classIt) {
                const json& classJson = *classIt;

                // Create class with basic info
                classes cls(
                    classJson["id"].get<int>(),
                    classJson["coach_id"].get<int>(),
                    classJson["title"].get<std::string>(),
                    classJson["coachName"].get<std::string>(),
                    classJson["dateAndTime"].get<std::string>(),
                    classJson["capacity"].get<int>()
                );
                cls.setSize(classJson["size"].get<int>());

                // Load VIP waitlist
                if (classJson.find("wait_list_vip") != classJson.end()) {
                    const json& vipList = classJson["wait_list_vip"];
                    for (json::const_iterator vipIt = vipList.begin(); vipIt != vipList.end(); ++vipIt) {
                        cls.wait_list_vip.push(vipIt->get<int>());
                    }
                }

                // Load regular waitlist
                if (classJson.find("wait_list") != classJson.end()) {
                    const json& waitList = classJson["wait_list"];
                    for (json::const_iterator waitIt = waitList.begin(); waitIt != waitList.end(); ++waitIt) {
                        cls.wait_list.push(waitIt->get<int>());
                    }
                }

                // Load list of members
                if (classJson.find("listOfMembers") != classJson.end()) {
                    const json& members = classJson["listOfMembers"];
                    for (json::const_iterator memIt = members.begin(); memIt != members.end(); ++memIt) {
                        cls.listOfMembers[std::stoi(memIt.key())] = memIt.value().get<std::string>();
                    }
                }

                loadedClasses[cls.getId()] = cls;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading classes: " << e.what() << "\nLoading templates instead.\n";
        return loadClassTemplates();
    }

    return loadedClasses;
}

std::map<int, classes> FileHandler::loadClassTemplates() {
    std::map<int, classes> templates;
    templates[1] = classes(1, 101, "Yoga", "Coach John", "2023-12-01 10:00", 20);
    templates[2] = classes(2, 102, "Pilates", "Coach Alice", "2023-12-02 11:00", 15);
    templates[1].setSize(0);
    templates[2].setSize(0);
    return templates;
}

// ======================= Member Operations =======================
void FileHandler::saveMembers(const std::map<int, member>& members, const std::string& filename) {
    json j;

    for (auto member_it = members.begin(); member_it != members.end(); ++member_it) {
        const auto& id = member_it->first;
        const auto& mem = member_it->second;
        json memberJson;

        // Basic info
        memberJson["id"] = mem.getId();
        memberJson["name"] = mem.getName();
        memberJson["date_of_birth"] = mem.getDateOfBirth();
        memberJson["is_VIP"] = mem.getIsVIP();
        memberJson["password"] = mem.getPassword();

        // Classes and subscriptions
        json classesJson;
        for (auto class_it = mem.classes_id.begin(); class_it != mem.classes_id.end(); ++class_it) {
            const auto& class_id = class_it->first;
            const auto& class_info = class_it->second;
            classesJson[std::to_string(class_id)] = {///////////////////////////
                {"class_name", class_info.first.first},
                {"join_date", class_info.first.second},
                {"expiry_date", class_info.second}
            };
        }
        memberJson["classes"] = classesJson;

        // Workout history - properly handle stack order
        json workoutsJson = json::array();
        std::vector<std::string> workouts;
        auto temp_stack = mem.workout_history;
        
        while (!temp_stack.empty()) {
            workouts.push_back(temp_stack.top());
            temp_stack.pop();
        }
        
        // Store in chronological order (oldest first)
        for (auto it = workouts.rbegin(); it != workouts.rend(); ++it) {
            workoutsJson.push_back(*it);
        }
        memberJson["workouts"] = workoutsJson;

        // Notifications
        json notificationsJson = json::array();
        auto temp_notifs = mem.notifications;
        while (!temp_notifs.empty()) {
            notificationsJson.push_back(temp_notifs.front());
            temp_notifs.pop();
        }
        memberJson["notifications"] = notificationsJson;

        // Inbox
        memberJson["inbox"] = mem.inbox;

        // Booked courts
        json courtsJson;
        for (auto court_it = mem.booked_courts.begin(); court_it != mem.booked_courts.end(); ++court_it) {
            const auto& court_id = court_it->first;
            const auto& court_info = court_it->second;
            for (auto booking_it = court_info.begin(); booking_it != court_info.end(); ++booking_it) {
                courtsJson[std::to_string(court_id)].push_back({
                    {"date", booking_it->first},
                    {"duration", booking_it->second}
                });
            }
        }
        memberJson["booked_courts"] = courtsJson;

        j["members"].push_back(memberJson);
    }

    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << j.dump(4);
        outFile.close();
    }
    else {
        throw std::runtime_error("Failed to save members to " + filename);
    }
}
std::map<int, member> FileHandler::loadMembers(const std::string& filename) {
    std::map<int, member> loadedMembers;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Creating new member data file\n";
        return loadMemberTemplates();
    }

    try {
        json j;
        inFile >> j;
        inFile.close();

        if (j.count("members") > 0) {
            for (const auto& memberJson : j["members"]) {
                member mem(
                    memberJson["id"].get<int>(),
                    memberJson["name"].get<std::string>(),
                    memberJson["date_of_birth"].get<std::string>(),
                    memberJson["is_VIP"].get<bool>(),
                    memberJson["password"].get<std::string>()
                );

                // Load classes
                if (memberJson.count("classes") > 0) {
                    for (auto classItem = memberJson["classes"].begin(); classItem != memberJson["classes"].end(); ++classItem) {
                        int classId = std::stoi(classItem.key());
                        const auto& classInfo = classItem.value();

                        mem.classes_id[classId] = {
                            {classInfo["class_name"], classInfo["join_date"]},
                            classInfo["expiry_date"]
                        };
                    }
                }

                // Load workout history (restore stack order)
                if (memberJson.count("workouts") > 0) {
                    std::stack<std::string> history;
                    // Push in reverse order to maintain chronology
                    for (auto it = memberJson["workouts"].rbegin(); it != memberJson["workouts"].rend(); ++it) {
                        history.push(it->get<std::string>());
                    }
                    mem.workout_history = history;
                }

                // Load notifications
                if (memberJson.count("notifications") > 0) {
                    for (const auto& notification : memberJson["notifications"]) {
                        mem.notifications.push(notification.get<std::string>());
                    }
                }

                // Load inbox
                if (memberJson.count("inbox") > 0) {
                    mem.inbox = memberJson["inbox"].get<std::vector<std::string>>();
                }

                // Load booked courts
                if (memberJson.count("booked_courts") > 0) {
                    for (auto courtItem = memberJson["booked_courts"].begin(); courtItem != memberJson["booked_courts"].end(); ++courtItem) {
                        int courtId = std::stoi(courtItem.key());
                        for (const auto& booking : courtItem.value()) {
                            mem.booked_courts[courtId].insert({
                                booking["date"].get<std::string>(),
                                booking["duration"].get<int>()
                            });
                        }
                    }
                }

                loadedMembers[mem.getId()] = mem;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading members: " << e.what() << "\n";
        throw;
    }

    return loadedMembers;
}

std::map<int, member> FileHandler::loadMemberTemplates() {
    std::map<int, member> templates;
    templates[1] = member(1, "Admin", "2000-01-01", true, "admin123");
    templates[2] = member(2, "John Doe", "1990-05-15", false, "member123");
    return templates;
}

// ======================= Staff Operations =======================
void FileHandler::saveStaff(const std::map<int, staff>& staffs, const std::string& filename) {
    json root;
    root["staff_count"] = staff::getstaffcount(); // Save current count
    root["staffs"] = json::array();

    for (const auto& pair : staffs) {
        const auto& s = pair.second;
        root["staffs"].push_back(s.to_json());
    }

    ofstream out(filename);
    out << root.dump(4);
    out.close();
}


std::map<int, staff> FileHandler::loadStaff(const std::string& filename) {
    std::map<int, staff> loadedStaff;
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Note: No staff data found.\n";
        return {};
    }

    try {
        json j;
        inFile >> j;
        inFile.close();

        if (j.contains("staffs")) {
            for (const auto& staffJson : j["staffs"]) {
                staff s;
                s.staff_count = j["staff_count"];
                s.from_json(staffJson);
                loadedStaff[s.getid()] = s;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading staff: " << e.what() << "\n";
    }

    return loadedStaff;
}