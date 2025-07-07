#pragma once
#include"Padel.h"
#include "classes.h"
#include <iostream>
#include "staff.h"
#include "member.h"
#include <chrono>
#include <fstream>
#include <string>
#include<queue>
#include<map>
#include<stack>
using json = nlohmann::json;
using namespace std;

class FileHandler {
public:
    static void savePadelCourts(const std::vector<Padel>& padelCourts, const std::string& filename);
    static std::vector<Padel> loadPadelCourts(const std::string& filename);
    static std::vector<Padel> loadPadelTemplates();

    static void saveClasses(const std::map<int, classes>& classesMap, const std::string& filename);
    static std::map<int, classes> loadClasses(const std::string& filename);
    static std::map<int, classes> loadClassTemplates();

    static void saveMembers(const std::map<int, member>& members, const std::string& filename);
    static std::map<int, member> loadMembers(const std::string& filename);
    static std::map<int, member> loadMemberTemplates();

    static void saveStaff(const std::map<int, staff>& staffs, const std::string& filename);
    static std::map<int, staff> loadStaff(const std::string& filename);

    static string getCurrentDate();

};