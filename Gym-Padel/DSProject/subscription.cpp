#include "subscription.h"
#include "padel.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

Subscription::Subscription(int memberId, int classId,
    const std::string& startDate,
    const std::string& endDate,
    float paymentAmount) :
    memberId(memberId),
    classId(classId),
    startDate(startDate),
    endDate(endDate),
    paymentAmount(paymentAmount),
    active(true)
{
    // Validate input
    if (memberId <= 0 || classId <= 0) {
        throw std::invalid_argument("Invalid ID values");
    }

    if (!validateDate(startDate)) {
        throw std::invalid_argument("Invalid start date format (expected YYYY-MM-DD)");
    }

    if (!validateDate(endDate)) {
        throw std::invalid_argument("Invalid end date format (expected YYYY-MM-DD)");
    }

    if (startDate > endDate) {
        throw std::invalid_argument("End date cannot be before start date");
    }

    if (paymentAmount <= 0) {
        throw std::invalid_argument("Payment amount must be positive");
    }
}

// Getters
int Subscription::getMemberId() const { return memberId; }
int Subscription::getClassId() const { return classId; }
std::string Subscription::getStartDate() const { return startDate; }
std::string Subscription::getEndDate() const { return endDate; }
float Subscription::getPaymentAmount() const { return paymentAmount; }

bool Subscription::isActive() const {
    if (!active) return false;

    // Get current date as tm struct
    time_t now = time(0);
    tm currentTime;
    localtime_s(&currentTime, &now);

    // Parse endDate string into tm struct
    std::tm endTime = {};
    std::istringstream endStream(endDate);
    endStream >> std::get_time(&endTime, "%Y-%m-%d");

    // Compare current date with end date
    return std::mktime(&currentTime) <= std::mktime(&endTime);
}


std::string Subscription::getStatus() const {
    if (!active) return "Cancelled";
    if (!isActive()) return "Expired";
    return "Active";
}

// Operations
void Subscription::renew(int months, float additionalPayment) {
    if (months <= 0) {
        throw std::invalid_argument("Months must be positive");
    }

    if (additionalPayment <= 0) {
        throw std::invalid_argument("Additional payment must be positive");
    }

    endDate = addMonthsToDate(endDate, months);
    paymentAmount += additionalPayment;
    active = true;
}

void Subscription::cancel() {
    active = false;

    // Calculate prorated refund
    int totalDays = daysBetweenDates(startDate, endDate);
    int usedDays = daysBetweenDates(startDate, Padel::getCurrentDate());
    float refund = paymentAmount * (1.0f - static_cast<float>(usedDays) / totalDays);

    // In a real system, you would process the refund here
}

void Subscription::applyDiscount(float percentage) {
    if (percentage < 0 || percentage > 100) {
        throw invalid_argument("Discount percentage must be between 0-100");
    }
    paymentAmount *= (1 - percentage / 100.0f);
}

// Helper methods
std::string Subscription::addMonthsToDate(const std::string& date, int months) {
    tm tm = {};
    istringstream ss(date);
    ss >> get_time(&tm, "%Y-%m-%d");

    tm.tm_mon += months;
    std::mktime(&tm); // Normalize the time

    std::ostringstream result;
    result << std::put_time(&tm, "%Y-%m-%d");
    return result.str();
}

int Subscription::daysBetweenDates(const string& date1, const string& date2) {
    tm tm1 = {};
    tm tm2 = {};

    std::istringstream ss1(date1), ss2(date2);
    ss1 >> std::get_time(&tm1, "%Y-%m-%d");
    ss2 >> std::get_time(&tm2, "%Y-%m-%d");

    if (ss1.fail() || ss2.fail()) return -1;

    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);

    if (time1 == -1 || time2 == -1) return -1;

    return static_cast<int>(std::difftime(time2, time1) / (60 * 60 * 24));
}

bool Subscription::validateDate(const std::string& date) {
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return !ss.fail();
}

bool Subscription::isValid() const {
    return memberId > 0 &&
        classId > 0 &&
        validateDate(startDate) &&
        validateDate(endDate) &&
        paymentAmount > 0;
}


