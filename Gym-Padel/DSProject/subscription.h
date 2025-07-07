#include <string>
#include <stdexcept>
#include <chrono>
#include <ctime>
#define _CRT_SECURE_NO_WARNINGS

class Subscription {
public:
    Subscription(int memberId, int classId,
        const std::string& startDate,
        const std::string& endDate,
        float paymentAmount);

    // Getters
    int getMemberId() const;
    int getClassId() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    float getPaymentAmount() const;
    bool isActive() const;
    std::string getStatus() const;
    int daysRemaining() const;

    // Operations
    void renew(int months, float additionalPayment);
    void cancel();
    void applyDiscount(float percentage);

    // Validation
    bool isValid() const;

private:
    int memberId;
    int classId;
    std::string startDate;
    std::string endDate;
    float paymentAmount;
    bool active;

    // Helper methods
    static std::string addMonthsToDate(const std::string& date, int months);
    static int daysBetweenDates(const std::string& date1, const std::string& date2);
    static bool validateDate(const std::string& date);
    
};