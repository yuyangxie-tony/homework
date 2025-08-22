#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// 日期结构体
struct Date {
    int year;
    int month;
    int day;
    bool hasYear; // 标记是否包含年份信息
    
    Date(int y, int m, int d) : year(y), month(m), day(d), hasYear(true) {}
    Date(int m, int d) : month(m), day(d), hasYear(false) {}
    Date() : year(0), month(0), day(0), hasYear(false) {}
};

// 星座结构体
struct Constellation {
    string name;
    int startMonth;
    int startDay;
    int endMonth;
    int endDay;
    vector<string> fortunes; // 存储不同日期的运势
    
    Constellation(string n, int sm, int sd, int em, int ed) 
        : name(n), startMonth(sm), startDay(sd), endMonth(em), endDay(ed) {}
};

// 函数声明
bool isLeapYear(int year);
int daysInMonth(int year, int month);
int calculateDaysBetween(Date start, Date end);
Date getCurrentDate();
Constellation determineConstellation(Date birthDate, vector<Constellation>& constellations);
void initializeConstellations(vector<Constellation>& constellations);
string getTodaysFortune(const Constellation& c);
void exportToFile(const string& content, const string& filename);
bool validateDate(Date date);
Date inputBirthDate();
void displayMenu();
// 新增函数：计算距离下一个生日的天数
int daysUntilNextBirthday(Date birthDate, Date today);

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
int calculateDaysBetween(Date start, Date end) {
    // 确保开始日期在结束日期之前
    bool swapped = false;
    Date temp = start;
    if (start.year > end.year || 
        (start.year == end.year && start.month > end.month) ||
        (start.year == end.year && start.month == end.month && start.day > end.day)) {
        start = end;
        end = temp;
        swapped = true;
    }
    
    int days = 0;
    
    // 计算完整年份的天数
    for (int y = start.year; y < end.year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    
    // 减去开始日期所在年已经过去的天数
    for (int m = 1; m < start.month; m++) {
        days -= daysInMonth(start.year, m);
    }
    days -= start.day;
    
    // 加上结束日期所在年已经过去的天数
    for (int m = 1; m < end.month; m++) {
        days += daysInMonth(end.year, m);
    }
    days += end.day;
    
    return swapped ? -days : days;
}

// 获取当前日期
Date getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    return Date(1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
}

// 判断星座
Constellation determineConstellation(Date birthDate, vector<Constellation>& constellations) {
    for (const auto& c : constellations) {
        // 处理跨年度的星座（如摩羯座）
        if (c.startMonth > c.endMonth) {
            if ((birthDate.month == c.startMonth && birthDate.day >= c.startDay) ||
                (birthDate.month == c.endMonth && birthDate.day <= c.endDay) ||
                (birthDate.month > c.startMonth) || (birthDate.month < c.endMonth)) {
                return c;
            }
        } else {
            if ((birthDate.month == c.startMonth && birthDate.day >= c.startDay) &&
                (birthDate.month == c.endMonth && birthDate.day <= c.endDay)) {
                return c;
            }
            if (birthDate.month > c.startMonth && birthDate.month < c.endMonth) {
                return c;
            }
        }
    }
    return constellations[0]; // 默认返回第一个星座
}

// 初始化星座数据
void initializeConstellations(vector<Constellation>& constellations) {
    constellations.emplace_back("水瓶座", 1, 20, 2, 18);
    constellations.emplace_back("双鱼座", 2, 19, 3, 20);
    constellations.emplace_back("白羊座", 3, 21, 4, 19);
    constellations.emplace_back("金牛座", 4, 20, 5, 20);
    constellations.emplace_back("双子座", 5, 21, 6, 21);
    constellations.emplace_back("巨蟹座", 6, 22, 7, 22);
    constellations.emplace_back("狮子座", 7, 23, 8, 22);
    constellations.emplace_back("处女座", 8, 23, 9, 22);
    constellations.emplace_back("天秤座", 9, 23, 10, 23);
    constellations.emplace_back("天蝎座", 10, 24, 11, 22);
    constellations.emplace_back("射手座", 11, 23, 12, 21);
    constellations.emplace_back("摩羯座", 12, 22, 1, 19);
    
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
    
    for (auto& c : constellations) {
        for (int i = 0; i < 366; i++) { // 为每个星座添加一年的运势
            int idx = rand() % fortuneTemplates.size();
            c.fortunes.push_back(fortuneTemplates[idx]);
        }
    }
}

// 获取今日运势
string getTodaysFortune(const Constellation& c) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int dayOfYear = ltm->tm_yday; // 一年中的第几天
    
    return c.fortunes[dayOfYear];
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
bool validateDate(Date date) {
    if (date.month < 1 || date.month > 12) return false;
    if (date.day < 1 || date.day > daysInMonth(date.year, date.month)) return false;
    return true;
}

// 输入出生日期
Date inputBirthDate() {
    Date birthDate;
    char choice;
    
    cout << "是否要输入出生年份？(y/n): ";
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        cout << "请输入出生年份: ";
        cin >> birthDate.year;
        birthDate.hasYear = true;
    }
    
    cout << "请输入出生月份(1-12): ";
    cin >> birthDate.month;
    cout << "请输入出生日期(1-31): ";
    cin >> birthDate.day;
    
    // 验证日期有效性
    if (!birthDate.hasYear) {
        Date today = getCurrentDate();
        birthDate.year = today.year; // 临时使用当前年份进行验证
        if (!validateDate(birthDate)) {
            cout << "无效的日期！请重新输入。" << endl;
            return inputBirthDate();
        }
        birthDate.hasYear = false; // 恢复为无年份状态
    } else {
        if (!validateDate(birthDate)) {
            cout << "无效的日期！请重新输入。" << endl;
            return inputBirthDate();
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

// 计算距离下一个生日的天数（全新实现）
int daysUntilNextBirthday(Date birthDate, Date today) {
    // 确定今年的生日日期
    int birthdayYear = today.year;
    int birthdayMonth = birthDate.month;
    int birthdayDay = birthDate.day;
    
    // 特殊处理2月29日
    if (birthdayMonth == 2 && birthdayDay == 29 && !isLeapYear(birthdayYear)) {
        birthdayDay = 28; // 非闰年调整为2月28日
    }
    
    // 检查今年生日是否已过
    bool birthdayPassed = false;
    if (birthdayMonth < today.month) {
        birthdayPassed = true;
    } else if (birthdayMonth == today.month) {
        if (birthdayDay < today.day) {
            birthdayPassed = true;
        }
    }
    
    // 如果生日已过，则计算明年的生日
    if (birthdayPassed) {
        birthdayYear++;
        // 再次处理2月29日的情况
        if (birthDate.month == 2 && birthDate.day == 29) {
            if (isLeapYear(birthdayYear)) {
                birthdayDay = 29; // 闰年恢复为2月29日
            } else {
                birthdayDay = 28; // 非闰年保持2月28日
            }
        }
    }
    
    // 创建生日日期对象
    Date nextBirthday(birthdayYear, birthdayMonth, birthdayDay);
    
    // 计算并返回天数差
    return calculateDaysBetween(today, nextBirthday);
}

int main() {
    cout << "欢迎使用生日与星座运势查询程序！" << endl;
    
    // 初始化星座数据
    vector<Constellation> constellations;
    initializeConstellations(constellations);
    
    // 输入出生日期
    Date birthDate = inputBirthDate();
    Constellation userConstellation = determineConstellation(birthDate, constellations);
    
    int choice;
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                Date today = getCurrentDate();
                int daysToBirthday = daysUntilNextBirthday(birthDate, today);
                cout << "距离下一个生日还有 " << daysToBirthday << " 天。" << endl;
                
                // 如果有出生年份，计算已出生天数
                if (birthDate.hasYear) {
                    Date birthWithYear(birthDate.year, birthDate.month, birthDate.day);
                    int daysAlive = calculateDaysBetween(birthWithYear, today);
                    cout << "你已经出生了 " << daysAlive << " 天。" << endl;
                }
                break;
            }
            case 2: {
                cout << "你的星座是: " << userConstellation.name << endl;
                break;
            }
            case 3: {
                string fortune = getTodaysFortune(userConstellation);
                cout << userConstellation.name << "今日运势: " << fortune << endl;
                
                char exportChoice;
                cout << "是否要将运势导出到文件？(y/n): ";
                cin >> exportChoice;
                
                if (exportChoice == 'y' || exportChoice == 'Y') {
                    string filename;
                    cout << "请输入文件名: ";
                    cin >> filename;
                    
                    time_t now = time(0);
                    stringstream ss;
                    ss << put_time(localtime(&now), "%Y-%m-%d");
                    string dateStr = ss.str();
                    
                    string content = "日期: " + dateStr + "\n";
                    content += "星座: " + userConstellation.name + "\n";
                    content += "运势: " + fortune + "\n";
                    
                    exportToFile(content, filename);
                }
                break;
            }
            case 4: {
                birthDate = inputBirthDate();
                userConstellation = determineConstellation(birthDate, constellations);
                cout << "出生日期已更新。" << endl;
                break;
            }
            case 5: {
                cout << "感谢使用，再见！" << endl;
                break;
            }
            default: {
                cout << "无效的选择，请重新输入。" << endl;
                break;
            }
        }
    } while (choice != 5);
    
    return 0;
}
