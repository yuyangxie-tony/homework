#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// 日期结构体（修改类名，避免可能的冲突）
struct DateTime {
    int year;
    int month;
    int day;
    bool hasYear; // 标记是否包含年份信息
    
    DateTime(int y, int m, int d) : year(y), month(m), day(d), hasYear(true) {}
    DateTime(int m, int d) : month(m), day(d), hasYear(false) {}
    DateTime() : year(0), month(0), day(0), hasYear(false) {}
};

// 星座结构体（修改类名，避免可能的冲突）
struct StarSign {
    string name;
    int startMonth;
    int startDay;
    int endMonth;
    int endDay;
    vector<string> fortunes; // 存储不同日期的运势
    
    StarSign(string n, int sm, int sd, int em, int ed) 
        : name(n), startMonth(sm), startDay(sd), endMonth(em), endDay(ed) {}
};

// 函数声明
bool isLeapYear(int year);
int daysInMonth(int year, int month);
int calculateDaysBetween(DateTime start, DateTime end);
DateTime getCurrentDateTime();
StarSign determineStarSign(DateTime birthDate, vector<StarSign>& starSigns);
void initializeStarSigns(vector<StarSign>& starSigns);
string getTodaysFortune(const StarSign& s);
void exportToFile(const string& content, const string& filename);
bool validateDate(DateTime date);
DateTime inputBirthDateTime();
void displayMenu();
int daysUntilNextBirthday(DateTime birthDate, DateTime today);
bool getYesNoInput(const string& prompt);

// 判断闰年
bool isLeapYear(int year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    if (year % 400 == 0) return true;
    return false;
}

// 获取月份天数
int daysInMonth(int year, int month) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 0;
    }
}

// 计算两个日期之间的天数
int calculateDaysBetween(DateTime start, DateTime end) {
    bool swapped = false;
    DateTime temp = start;
    if (start.year > end.year || 
        (start.year == end.year && start.month > end.month) ||
        (start.year == end.year && start.month == end.month && start.day > end.day)) {
        start = end;
        end = temp;
        swapped = true;
    }
    
    int days = 0;
    
    for (int y = start.year; y < end.year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    
    for (int m = 1; m < start.month; m++) {
        days -= daysInMonth(start.year, m);
    }
    days -= start.day;
    
    for (int m = 1; m < end.month; m++) {
        days += daysInMonth(end.year, m);
    }
    days += end.day;
    
    return swapped ? -days : days;
}

// 获取当前日期
DateTime getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    return DateTime(1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
}

// 星座判断逻辑
StarSign determineStarSign(DateTime birthDate, vector<StarSign>& starSigns) {
    // 将日期转换为一年中的第几天，便于比较
    int dayOfYear = 0;
    for (int m = 1; m < birthDate.month; m++) {
        dayOfYear += daysInMonth(2000, m); // 使用2000年（闰年）确保2月有29天
    }
    dayOfYear += birthDate.day;
    
    for (const auto& s : starSigns) {
        // 计算星座起始日期和结束日期在一年中的天数
        int startDayOfYear = 0;
        for (int m = 1; m < s.startMonth; m++) {
            startDayOfYear += daysInMonth(2000, m);
        }
        startDayOfYear += s.startDay;
        
        int endDayOfYear = 0;
        for (int m = 1; m < s.endMonth; m++) {
            endDayOfYear += daysInMonth(2000, m);
        }
        endDayOfYear += s.endDay;
        
        // 判断日期是否在星座范围内
        if (s.startMonth <= s.endMonth) {
            // 不跨年度的星座（如白羊座3/21-4/19）
            if (dayOfYear >= startDayOfYear && dayOfYear <= endDayOfYear) {
                return s;
            }
        } else {
            // 跨年度的星座（如摩羯座12/22-1/19）
            if (dayOfYear >= startDayOfYear || dayOfYear <= endDayOfYear) {
                return s;
            }
        }
    }
    return starSigns[0]; // 默认返回第一个星座
}

// 初始化星座数据
void initializeStarSigns(vector<StarSign>& starSigns) {
    starSigns.clear();
    // 标准星座日期范围
    starSigns.emplace_back("白羊座", 3, 21, 4, 19);
    starSigns.emplace_back("金牛座", 4, 20, 5, 20);
    starSigns.emplace_back("双子座", 5, 21, 6, 21);
    starSigns.emplace_back("巨蟹座", 6, 22, 7, 22);
    starSigns.emplace_back("狮子座", 7, 23, 8, 22);
    starSigns.emplace_back("处女座", 8, 23, 9, 22);
    starSigns.emplace_back("天秤座", 9, 23, 10, 23);
    starSigns.emplace_back("天蝎座", 10, 24, 11, 22);
    starSigns.emplace_back("射手座", 11, 23, 12, 21);
    starSigns.emplace_back("摩羯座", 12, 22, 1, 19);
    starSigns.emplace_back("水瓶座", 1, 20, 2, 18);
    starSigns.emplace_back("双鱼座", 2, 19, 3, 20);
    
    srand(time(0));
    vector<string> fortuneTemplates = {
        "今天是充满活力的一天，适合尝试新事物。",
        "你的努力将在今天得到回报，坚持下去！",
        "人际关系方面会有好的发展，多与他人交流。",
        "注意休息，保持良好的精神状态最重要。",
        "财运不错，可能会有意外收获。",
        "今天适合学习新知识，思维会很敏捷。",
        "感情方面有好的进展，大胆表达你的想法。",
        "健康状况良好，适当运动更有益。"
    };
    
    for (auto& s : starSigns) {
        for (int i = 0; i < 366; i++) {
            int idx = rand() % fortuneTemplates.size();
            s.fortunes.push_back(fortuneTemplates[idx]);
        }
    }
}

// 获取今日运势
string getTodaysFortune(const StarSign& s) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int dayOfYear = ltm->tm_yday;
    
    return s.fortunes[dayOfYear];
}

// 导出内容到文件
void exportToFile(const string& content, const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        cout << "成功导出至 " << filename << endl;
    } else {
        cout << "无法打开文件进行写入！" << endl;
    }
}

// 验证日期有效性
bool validateDate(DateTime date) {
    if (date.month < 1 || date.month > 12) return false;
    if (date.day < 1 || date.day > daysInMonth(date.year, date.month)) return false;
    return true;
}

// 输入出生日期
DateTime inputBirthDateTime() {
    DateTime birthDate;
    // 使用输入验证函数
    bool inputYear = getYesNoInput("是否要输入出生年份？(y/n): ");
    
    if (inputYear) {
        cout << "请输入出生年份: ";
        cin >> birthDate.year;
        birthDate.hasYear = true;
    }
    
    cout << "请输入出生月份(1-12): ";
    cin >> birthDate.month;
    cout << "请输入出生日期(1-31): ";
    cin >> birthDate.day;
    
    if (!birthDate.hasYear) {
        DateTime today = getCurrentDateTime();
        birthDate.year = today.year;
        if (!validateDate(birthDate)) {
            cout << "无效的日期！请重新输入。" << endl;
            return inputBirthDateTime();
        }
        birthDate.hasYear = false;
    } else {
        if (!validateDate(birthDate)) {
            cout << "无效的日期！请重新输入。" << endl;
            return inputBirthDateTime();
        }
    }
    
    return birthDate;
}

// 显示菜单
void displayMenu() {
    cout << "\n===== 生日与星座运势查询 =====" << endl;
    cout << "1. 查询距离下一个生日的天数" << endl;
    cout << "2. 查询星座" << endl;
    cout << "3. 查询今日运势" << endl;
    cout << "4. 重新输入出生日期" << endl;
    cout << "5. 退出程序" << endl;
    cout << "==============================" << endl;
    cout << "请选择操作 (1-5): ";
}

// 计算距离下一个生日的天数
int daysUntilNextBirthday(DateTime birthDate, DateTime today) {
    int birthdayYear = today.year;
    int birthdayMonth = birthDate.month;
    int birthdayDay = birthDate.day;
    
    if (birthdayMonth == 2 && birthdayDay == 29 && !isLeapYear(birthdayYear)) {
        birthdayDay = 28;
    }
    
    bool birthdayPassed = false;
    if (birthdayMonth < today.month) {
        birthdayPassed = true;
    } else if (birthdayMonth == today.month) {
        if (birthdayDay < today.day) {
            birthdayPassed = true;
        }
    }
    
    if (birthdayPassed) {
        birthdayYear++;
        if (birthDate.month == 2 && birthDate.day == 29) {
            if (isLeapYear(birthdayYear)) {
                birthdayDay = 29;
            } else {
                birthdayDay = 28;
            }
        }
    }
    
    DateTime nextBirthday(birthdayYear, birthdayMonth, birthdayDay);
    return calculateDaysBetween(today, nextBirthday);
}

// 获取有效的yes/no输入
bool getYesNoInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        cin >> input;
        
        // 只取第一个字符进行判断，忽略后续字符
        char choice = tolower(input[0]);
        
        if (choice == 'y' || choice == 'n') {
            return choice == 'y';
        } else {
            cout << "输入错误！请输入 'y' (是) 或 'n' (否)。" << endl;
            // 清除输入缓冲区，防止无限循环
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

int main() {
    cout << "欢迎使用生日与星座运势查询程序！" << endl;
    
    vector<StarSign> starSigns;
    initializeStarSigns(starSigns);
    
    DateTime birthDate = inputBirthDateTime();
    StarSign userStarSign = determineStarSign(birthDate, starSigns);
    
    int choice;
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                DateTime today = getCurrentDateTime();
                int daysToBirthday = daysUntilNextBirthday(birthDate, today);
                cout << "距离下一个生日还有 " << daysToBirthday << " 天。" << endl;
                
                if (birthDate.hasYear) {
                    DateTime birthWithYear(birthDate.year, birthDate.month, birthDate.day);
                    int daysAlive = calculateDaysBetween(birthWithYear, today);
                    cout << "你已经出生了 " << daysAlive << " 天。" << endl;
                }
                break;
            }
            case 2: {
                cout << "你的星座是: " << userStarSign.name << endl;
                break;
            }
            case 3: {
                string fortune = getTodaysFortune(userStarSign);
                cout << userStarSign.name << "今日运势: " << fortune << endl;
                
                // 使用输入验证函数
                bool exportChoice = getYesNoInput("是否要将运势导出到文件？(y/n): ");
                
                if (exportChoice) {
                    string filename;
                    cout << "请输入文件名: ";
                    cin >> filename;
                    
                    time_t now = time(0);
                    stringstream ss;
                    ss << put_time(localtime(&now), "%Y-%m-%d");
                    string dateStr = ss.str();
                    
                    string content = "日期: " + dateStr + "\n";
                    content += "星座: " + userStarSign.name + "\n";
                    content += "运势: " + fortune + "\n";
                    
                    exportToFile(content, filename);
                }
                break;
            }
            case 4: {
                birthDate = inputBirthDateTime();
                userStarSign = determineStarSign(birthDate, starSigns);
                cout << "出生日期已更新。" << endl;
                break;
            }
            case 5: {
                cout << "感谢使用，再见！" << endl;
                break;
            }
            default: {
                cout << "无效的选择，请重新输入。" << endl;
                // 清除输入缓冲区
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }
    } while (choice != 5);
    
    return 0;
}