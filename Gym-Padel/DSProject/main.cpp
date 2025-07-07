#include <iostream>
#include <string>
#include "FileHandler.h"
#include "member.h"
#include "staff.h"
#include "classes.h"
#include "padel.h"
#include "subscription.h"

using namespace std;

const string MEMBERS_FILE = "members_saved.json";
const string STAFF_FILE = "staffs_saved.json";
const string CLASSES_FILE = "classes_saved.json";
const string PADEL_FILE = "padel_courts_saved.json";
int temp_id, temp_class_id;
string temp;
char role;

void managerMenu(FileHandler& fh, vector<Padel>& padelCourts, int staffId);
void coachMenu(FileHandler& fh, int staffId);
void memberMenu(FileHandler& fh, int memberId, vector<Padel>& padelCourts);
void displayMainMenu();

int main() {
    FileHandler fileHandler;

    member::all_members = fileHandler.loadMembers(MEMBERS_FILE); // 
    classes::members = fileHandler.loadMembers(MEMBERS_FILE); // 
    classes::classess = fileHandler.loadClasses(CLASSES_FILE);
    staff::staffs = fileHandler.loadStaff(STAFF_FILE);
    vector<Padel> padelCourts = fileHandler.loadPadelCourts(PADEL_FILE);

    // Check subscription expiry for all members
    string current_date = fileHandler.getCurrentDate();
    bool notifications_added = false;
    for (auto& member_pair : member::all_members) {
        member& current_member = member_pair.second;
        current_member.check_subscription_expiry(current_date);
        if (!current_member.notifications.empty()) {
            notifications_added = true;
        }
    }

    // Save immediately if any notifications were added
    if (notifications_added) {
        fileHandler.saveMembers(member::all_members, MEMBERS_FILE);
    }

    int choice;
    int id;
    string password;

    do {
        displayMainMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            cout << "Login As:\n";
            cout << "1. Member\n";
            cout << "2. Staff\n";
            cout << "3. Return Back\n";
            int loginChoice;
            cin >> loginChoice;
            if (loginChoice == 1) { // Login as Member
                cout << "Enter Member ID: ";
                cin >> id;
                cout << "Enter password: ";
                cin >> password;
                if (member::is_member_exist(id) &&
                    member::get_member_by_id(id).getPassword() == password) {
                    memberMenu(fileHandler, id, padelCourts);
                }
                else {
                    cout << "Invalid member credentials\n";
                }
            }
            else if (loginChoice == 2) { // Login as Staff
                cout << "Enter Staff ID: ";
                cin >> id;
                cout << "Enter password: ";
                cin >> password;

                if (staff::is_staff_exist(id) &&
                    staff::get_staff_pass_by_id(id) == password) {
                    role = staff::staffs[id].getrole();
                    if (role == 'm') {
                        managerMenu(fileHandler, padelCourts, id);
                    }
                    else {
                        coachMenu(fileHandler, id);
                    }
                }
                else {
                    cout << "Invalid staff credentials\n";
                        /*coachMenu(fileHandler, id);*/
                    //managerMenu(fileHandler, padelCourts, id);

                }
                break;
            }
            else if (loginChoice == 3) {
                break;
            }
            break;
        }
        case 2: { // Sign Up - Modified to auto-login
            // Add braces to create a scope for variables
            int newId = member::member_count + 1; // Get the ID that will be assigned
            member::add_member();

            // Auto-login the new member
            cout << "You have been automatically logged in.\n";
            memberMenu(fileHandler, newId, padelCourts);
            break;
        }
        case 3: // Save and exit
            fileHandler.saveMembers(member::all_members, MEMBERS_FILE);
            fileHandler.saveStaff(staff::staffs, STAFF_FILE);
             //fileHandler.saveMembers(classes::members, MEMBERS_FILE); // error
            fileHandler.saveClasses(classes::classess, CLASSES_FILE);
            fileHandler.savePadelCourts(padelCourts, PADEL_FILE);
            cout << "Data saved successfully. Exiting...\n";
            return 0;

        default:
            cout << "Invalid choice\n";
        }
    } while (true);

    return 0;
}

void displayMainMenu() {
    cout << "\n=== Welcome To Padel and Gym Management System ===\n";
    cout << "1. Login\n";
    cout << "2. Sign Up\n";
    cout << "3. Save and Exit\n";
    cout << "============================\n";
}

void manageMember(FileHandler& fh) {
    int choice;
    do {
        cout << "\n=== Manage Members ===\n";
        cout << "1. Add New Member\n";
        cout << "2. Remove Member\n";
        cout << "3. Search Member By ID\n";
        cout << "4. Renew Subscription\n";
        cout << "5. Return Back\n";

        cin >> choice;
        switch (choice) {
        case 1:
            member::add_member();
            break;
        case 2:
            member::remove_member();
            break;
        case 3:
            cout << "Enter ID:\n";
            cin >> temp_id;
            if (member::is_member_exist(temp_id)) {
                member::display_member_by_id(temp_id); // MUST SEARCH BY FILES
            }
            else {
                cout << "-------------------\nNo Member Exists With This ID\n-------------------\n";
            }
            break;
			// done (add supscription logic)
        case 4: {
            cout << "Enter Member ID:\n";
            int member_id;
            cin >> member_id;

            if (!member::is_member_exist(member_id)) {
                cout << "Member not found!\n";
                break;
            }

            //show avaliable classes
            cout << "\nAvailable Classes:\n";
            classes::display_classes();

            cout << "\nEnter Class ID to subscribe to:\n";
            int class_id;
            cin >> class_id;

            if (!classes::is_class_exist(class_id)) {
                cout << "Class not found!\n";
                break;
            }
            

            member& member_obj = member::all_members[member_id];

            // show plans
            cout << "\nAvailable Subscription Plans:\n";
            cout << "1. " << member_obj.get_subscription_plan(1) << endl;
            cout << "3. " << member_obj.get_subscription_plan(3) << endl;
            cout << "6. " << member_obj.get_subscription_plan(6) << endl;
            cout << "12. " << member_obj.get_subscription_plan(12) << endl;

            cout << "\nEnter subscription period (1/3/6/12 months):\n";
            int months;
            cin >> months;
            if (months != 1 && months != 3 && months != 6 && months != 12) {
                cout << "Invalid subscription period!\n";
                break;
            }

            // save in file
            member_obj.renew_subscription(class_id, months);


            fh.saveMembers(member::all_members, MEMBERS_FILE);
            break;
        }
            return;
        case 5:
            return;
        }
    } while (true);
}

void manageClasses(FileHandler& fh, int coachId) {
    int choice;
    do {
        cout << "\n=== Manage Classes ===\n";
        cout << "1. Add Class\n";
        cout << "2. Remove Class\n";
        cout << "3. Show My Classes\n";
        cout << "4. Show All Classes\n";
        cout << "5. Return Back\n";
        cin >> choice;

        switch (choice) {
        case 1:
            classes::add_class();
            break;
        case 2:
            classes::display_classes();
            cout << "Enter Class ID To Remove:\n";
            cin >> temp_class_id;
            if (classes::is_class_exist(temp_class_id)) {
                classes::delete_class_by_id(temp_class_id);
                cout << "Deleted Succsessfully!\n";
            }
            else {
                cout << "No Class Exists With This ID\n";
            }
            break;
        case 3:
            classes::display_coach_classes_by_id(coachId);

            break;
        case 4:
            classes::display_classes();
            break;
        case 5:
            return;
        default:
            cout << "Invalid Choice\n";
        }
    } while (true);
}

void memberMenu(FileHandler& fh, int memberId, vector<Padel>& padelCourts) {
    int choice;
    do {
        cout << "\n=== Member Menu ===\n";
        cout << "1. Join Class\n";
        cout << "2. Cancel Class\n";
        cout << "3. Display My Classes\n";
        cout << "4. Display court availability\n";
        cout << "5. Book Padel Court\n";
        cout << "6. Cancel Padel Court Booking\n";
        cout << "7. Show closest available time\n";
        cout << "8. Display Inbox\n";
        cout << "9. Add Workout\n";
        cout << "10. Display Workout History\n";
        cout << "11. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            classes::join_class(memberId);
            break;
        case 2:
            classes::cancel_class(memberId);
            break;
        case 3:
            classes::display_member_classes_by_id(memberId);
            break;
        case 4: {
            int courtID;
            string date;
            cout << "Enter Court ID: ";
            cin >> courtID;
            cout << "Enter Date (YYYY-MM-DD): ";
            cin >> date;

            if (courtID >= 1 && courtID <= padelCourts.size()) {
                padelCourts[courtID - 1].displayDateAvailability(date, courtID);
            }
            else {
                cout << "Invalid court ID.\n";
            }
            break;
        }
        case 5: {
            int courtId, time;
            string date;
            cout << "Enter Court ID: ";
            cin >> courtId;
            cout << "Enter Date (YYYY-MM-DD): ";
            cin >> date;
            

            for (auto& court : padelCourts) {
                if (court.getCourtID() == courtId) {
                    cout << "Enter Time (hour" << court.StartTime << " - " << court.EndTime << " ):";
                    cin >> time;
                    court.book_padel_court(memberId, courtId, time, date);
                    break;
                }
            }
            break;
        }
        case 6: {
            int courtId, time;
            string date, location;
            cout << "Enter Court ID: ";
            cin >> courtId;
            cout << "Enter Date (YYYY-MM-DD): ";
            cin >> date;

            for (auto& court : padelCourts) {
                if (court.getCourtID() == courtId) {
                    cout << "Enter Time (hour" << court.StartTime << " - " << court.EndTime << " ):";
                    cin >> time;
                    cout << "Enter Location: ";
                    cin.ignore();
                    getline(cin, location);

                    court.cancel_booked_courts(date, location, time, memberId , courtId);
                    break;
                }
            }
            break;
        }
        case 7: {
            int courtId, time;
            string date;
            cout << "Enter Court ID: ";
            cin >> courtId;
            cout << "Enter Date (YYYY-MM-DD): ";
            cin >> date;
            for (auto& court : padelCourts) {
                if (court.getCourtID() == courtId) {
                    cout << "Enter Time (hour" << court.StartTime << " - " << court.EndTime << " ):";
                    cin >> time;
                    court.display_closest_time_court(date, time, memberId);
                    break;
                }
            }
            break;
        }

        case 8:
            member::get_member_by_id(memberId).display_inbox();
            break;
        case 9:{
            string w;
            cout << "Enter Workout:\n";
            cin >> w;
            member::all_members[memberId].add_workout(w);
            break;
        }    
        case 10:
            member::get_member_by_id(memberId).display_workouts();
            break;
        case 11:
            return;
        default:
            cout << "Invalid choice\n";
        }
    } while (true);
}

void coachMenu(FileHandler& fh, int staffId) {
    int choice;
    do {
        cout << "\n=== Coach Menu ===\n";
        cout << "1. Manage Members\n";
        cout << "2. Manage My Classes\n";
        cout << "3. Display My Classes\n";
        cout << "4. Display Profile\n";
        cout << "5. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            manageMember(fh);
            break;
        case 2:
            manageClasses(fh, staffId);
            break;
        case 3:
            classes::display_coach_classes_by_id(staffId);
            break;
        case 4:
            staff::display_staff_member(staffId);
            break;
        case 5:
            return;
        default:
            cout << "Invalid choice\n";
        }
    } while (true);
}

void managerMenu(FileHandler& fh, vector<Padel>& padelCourts, int staffId) {
    int choice;
    do {
        cout << "\n=== Welcome Manager ";
        cout << classes::staffs[staffId].getname() << "===\n";
        cout << "1. Add Member\n";
        cout << "2. Remove Member\n";
        cout << "3. Add Staff\n";
        cout << "4. Add Class\n";
        cout << "5. Delete Class\n";
        cout << "6. Add Padel Court\n";
        cout << "7. Remove Padel Court\n";
        cout << "8. Display All Members\n";
        cout << "9. Display All Classes\n";
        cout << "10.Generate Monthly Report\n";
        cout << "11. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            member::add_member();
            break;
        case 2:
            member::remove_member();
            break;
        case 3: {
            char role;
            cout << "Enter role (c for coach, m for manager, r for receptionist): ";
            cin >> role;
            staff::add_staff_member(role);
            break;
        }
        case 4:
            classes::add_class();
            break;
        case 5: {
            int classId;
            cout << "Enter Class ID to delete: ";
            cin >> classId;
            if (classes::is_class_exist(classId)) {
                classes::delete_class_by_id(classId);
                cout << "Class deleted successfully\n";
            }
            else {
                cout << "Class not found\n";
            }
            break;
        }
        case 6:
            Padel::Add_new_court(padelCourts, PADEL_FILE);
            break;
        case 7:
            Padel::Remove_court(padelCourts, PADEL_FILE);
            break;
        case 8:
            member::display_all_members();
            break;
        case 9:
            classes::display_classes();
            break;
        case 10:
            member::generateMonthlyReport();
            break;
        case 11:
            return;
        default:
            cout << "Invalid choice\n";
        }
    } while (true);
}
