#pragma once
#include <string>
#include <map>
#include <set>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

class staff {
private:
    int id;
    string name;
    string date_of_birth;
    string password;
    char role;
    map<int, std::set<int>> coach_classes;

public:
    static int staff_count;
    static map<int, staff> staffs;
    staff();
    staff(int id, string name, string date_of_birth, string password, char role);
    //void assign_class(int class_id);
    //void remove_class(int class_id);
    //const std::set<int>& get_assigned_classes() const;

   

    json to_json() const {
        return {
            {"id", id},
            {"name", name},
            {"date_of_birth", date_of_birth},
            {"password", password},
            {"role", string(1, role)}
        };
    }

    void from_json(const json& j) {
        id = j.at("id").get<int>();
        name = j.at("name").get<string>();
        date_of_birth = j.at("date_of_birth").get<string>();
        password = j.at("password").get<string>();
        role = j.at("role").get<string>()[0];
    }


    static int getstaffcount();

    int getid();
    string getname();
    char getrole();
    string getpassword();
    string getbirth();

    static void decrementStaffCount();
    void setid(int id);
    void setname(string name);
    void setrole(char role);
    void setpassword(string pass);
    void setbirth(string birth);

    static staff get_staff_by_id(int id);
    static string get_staff_pass_by_id(int id);
    static string get_staff_name_by_id(int id);
    static bool is_staff_exist(int id);
    static void display_staff_member(int id);
    static void add_staff_member(char role);

    staff get_info();
};
