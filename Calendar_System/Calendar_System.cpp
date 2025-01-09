#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>  // 用于 _kbhit() 和 _getch()
#include <process.h> // 用于 _beginthread
#include <time.h>
#include <iostream>


#define FILE_NAME "schedules.txt"

#define MAX_SCHEDULES_PER_DAY 10  // 每天最多支持的日程数
#define MAX_SCHEDULE_LENGTH 200    // 每条日程最多200个字符
#define MAX_DATE_LENGTH 20         // 日期格式最大长度
#define TABLE_SIZE 10000 // 表大小


const char* Tiangan[] = { "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸" };
const char* Dizhi[] = { "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥" };
const char* Shengxiao[] = { "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪" };
const char* NongliYue[] = { " 正月 ", " 二月 ", " 三月 ", " 四月 ", " 五月 ", " 六月 ", " 七月 ", " 八月 ", " 九月 ", " 十月 ", "十一月", " 腊月 " };
const char* NongliTian[] = { " 初一 ", " 初二 ", " 初三 ", " 初四 ", " 初五 ", " 初六 ", " 初七 ", " 初八 ", " 初九 ", " 初十 ", 
                             " 十一 ", " 十二 ", " 十三 ", " 十四 ", " 十五 ", " 十六 ", " 十七 ", " 十八 ", " 十九 ", " 二十 ",
                             " 廿一 ", " 廿二 ", " 廿三 ", " 廿四 ", " 廿五 ", " 廿六 ", " 廿七 ", " 廿八 ", " 廿九 ", " 三十 " };
const char* Week[] = { "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六" };
const char* Jieqi[12][2] = { { " 小寒 ", " 大寒 " }, 
                             { " 立春 ", " 雨水 " }, 
                             { " 惊蛰 ", " 春分 " }, 
                             { " 清明 ", " 谷雨 " }, 
                             { " 立夏 ", " 小满 " }, 
                             { " 芒种 ", " 夏至 " }, 
                             { " 小暑 ", " 大暑 " }, 
                             { " 立秋 ", " 处暑 " }, 
                             { " 白露 ", " 秋分 " }, 
                             { " 寒露 ", " 霜降 " }, 
                             { " 立冬 ", " 小雪 " }, 
                             { " 大雪 ", " 冬至 " } };
const char* gregorianFestivals[] = { " 元旦 ", "妇女节", "情人节", "植树节", "劳动节", "青年节", "儿童节", "建党节", "建军节", "国庆节", "教师节", "万圣节", "圣诞节" };
const char* lunarFestivals[] = { " 春节 ", "元宵节", "龙抬头", "端午节", "七夕节", "中元节", "中秋节", "重阳节", "腊八节" };
int gregorianFestivalDates[][2] = { {1, 1}, {3, 8}, {2, 14}, {3, 12}, {5, 1}, {5, 4}, {6, 1}, {7, 1}, {8, 1}, {10, 1}, {9, 10}, {10, 31}, {12, 25} };
int lunarFestivalDates[][2] = { {1, 1}, {1, 15}, {2, 2}, {5, 5}, {7, 7}, {7, 15}, {8, 15}, {9, 9}, {12, 8} };


// 判断某日期是否为节日
int isFestival(int month, int day, int lunarMonth, int lunarDay) {
    // 检查公历节日
    for (int i = 0; i < sizeof(gregorianFestivalDates) / sizeof(gregorianFestivalDates[0]); ++i) {
        if (month == gregorianFestivalDates[i][0] && day == gregorianFestivalDates[i][1]) {
            printf("%s", gregorianFestivals[i]); // 打印节日名称
            return 1; // 是节日
        }
    }

    // 检查农历节日
    for (int i = 0; i < sizeof(lunarFestivalDates) / sizeof(lunarFestivalDates[0]); ++i) {
        if (lunarMonth == lunarFestivalDates[i][0] && lunarDay == lunarFestivalDates[i][1]) {
            printf("%s", lunarFestivals[i]); // 打印节日名称
            return 1; // 是节日
        }
    }

    return 0; // 不是节日
}

// 表节点结构
typedef struct Node {
    int identifier;    // 唯一标识（年月日组合成的数字）
    struct Node* next; // 指向下一个节点（解决冲突）
} Node;

// 全局表
Node* table[TABLE_SIZE];

// 散列函数
unsigned int hashFunc(int identifier) {
    return identifier % TABLE_SIZE;
}

// 插入到表
void insertToTable(int identifier) {
    unsigned int index = hashFunc(identifier);
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("内存分配失败");
        exit(EXIT_FAILURE);
    }
    newNode->identifier = identifier;
    newNode->next = table[index];
    table[index] = newNode;
}

// 查找表
int searchTable(int identifier) {
    unsigned int index = hashFunc(identifier);
    Node* current = table[index];
    while (current) {
        if (current->identifier == identifier) {
            return 1; // 找到
        }
        current = current->next;
    }
    return 0; // 未找到
}

// 初始化表
void initTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }
}

// 清理表
void clearTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
        table[i] = NULL;
    }
}

// 加载文件数据到表
void loadTermsData(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int data[25] = { 0 }; // 12个月的日期（每月两个） + 年份
        int index = 0;

        // 解析一行数据
        char* token = strtok(line, ",");
        while (token != NULL && index < 25) {
            data[index++] = atoi(token);
            token = strtok(NULL, ",");
        }

        int year = data[24];
        for (int month = 1; month <= 12; ++month) {
            int day1 = data[(month - 1) * 2];       // 第一个日期
            int day2 = data[(month - 1) * 2 + 1];   // 第二个日期

            if (day1 > 0) {
                int id1 = year * 10000 + month * 100 + day1; // 生成唯一标识
                insertToTable(id1);
            }
            if (day2 > 0) {
                int id2 = year * 10000 + month * 100 + day2; // 生成唯一标识
                insertToTable(id2);
            }
        }
    }

    fclose(file);
}

// 判断是否为二十四节气日
int isTerm(int year, int month, int day) {
    //初始化表 
    initTable();
    // 加载文件数据到表
    loadTermsData("SolarTermsData.txt");
    int id = year * 10000 + month * 100 + day;
	void clearTable();
    return searchTable(id);
}

//打印节气
void printJieqi(int month, int day) {
    if (day < 10) {
        day = 0;
    }
	else {
		day = 1;
	}
	printf("%s", Jieqi[month - 1][day]);
}

// 定义结构体存储阳历和农历日期
typedef struct {
    char solar[20];
    int lunarYear;
    int lunarMonth;
    int lunarDay;
} Calendar;

// 定义哈希表的节点
typedef struct HashNode {
    char solar[20];
    int lunarYear;
    int lunarMonth;
    int lunarDay;
    struct HashNode* next;
} HashNode;

// 定义哈希表
#define TABLE_SIZE 5000
HashNode* hashTable[TABLE_SIZE];

// 哈希函数
unsigned int hash(char* key) {
    unsigned int hashValue = 0;
    while (*key) {
        hashValue = (hashValue << 5) + *key++;
    }
    return hashValue % TABLE_SIZE;
}

// 插入数据到哈希表
void insert(HashNode* table[], char* solar, int lunarYear, int lunarMonth, int lunarDay) {
    unsigned int index = hash(solar);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    strcpy(newNode->solar, solar);
    newNode->lunarYear = lunarYear;
    newNode->lunarMonth = lunarMonth;
    newNode->lunarDay = lunarDay;
    newNode->next = table[index];
    table[index] = newNode;
}

// 查找数据
int find(HashNode* table[], char* solar, int* lunarYear, int* lunarMonth, int* lunarDay) {
    unsigned int index = hash(solar);
    HashNode* node = table[index];
    while (node) {
        if (strcmp(node->solar, solar) == 0) {
            *lunarYear = node->lunarYear;
            *lunarMonth = node->lunarMonth;
            *lunarDay = node->lunarDay;
            return 1;
        }
        node = node->next;
    }
    return 0;
}

// 判断是否是闰年
int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取某个月的天数
int getDaysInMonth(int year, int month) {
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

// 计算某年某月1日是星期几 (从星期日开始，0为星期日，6为星期六)
int getWeekday(int year, int month) {
    int days = 0;
    for (int y = 1900; y < year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    for (int m = 1; m < month; m++) {
        days += getDaysInMonth(year, m);
    }
    return (days + 1) % 7; // 1900年1月1日是星期一，所以+1
}

// 读取文件内容并填充哈希表
void loadCalendarData(HashNode* table[]) {
    FILE* file = fopen("Calendar Comparison.txt", "r");
    if (!file) {
        printf("无法打开文件 Calendar Comparison.txt。\n");
        return;
    }

    char solar[20];
    int lunarYear, lunarMonth, lunarDay;
    while (fscanf(file, "%[^,],%d/%d/%d\n", solar, &lunarYear, &lunarMonth, &lunarDay) != EOF) {
        insert(table, solar, lunarYear, lunarMonth, lunarDay);
    }

    fclose(file);
}

// 打印月历
void printMonthCalendar(int year, int month) {
    printf("  ================================================================\n");
    printf("  |                          %d年%2d月                          |\n", year, month);
    printf("  |--------------------------------------------------------------|\n");
    printf("  | 星期日 | 星期一 | 星期二 | 星期三 | 星期四 | 星期五 | 星期六 |\n");
    printf("  |--------|--------|--------|--------|--------|--------|--------|\n");
    int firstDay = getWeekday(year, month);
    int daysInMonth = getDaysInMonth(year, month);

    int day = 1;
    int lunarStartDay = 1;

    for (int week = 0; week < 6; week++) {
        int isEmptyWeek = 1;
        printf("  |");
        //阳历
        for (int i = 0; i < 7; i++) {
            if (week == 0 && i < firstDay || day > daysInMonth) {
                printf("        |");
            }
            else {
                printf("   %2d   |", day);
                if (isEmptyWeek) {
                    lunarStartDay = day;
                    isEmptyWeek = 0;
                }
                day++;
            }
        }
        printf("\n");
        printf("  |");

        //农历节日节气
        for (int i = 0; i < 7; i++) {
            printf(" ");
            int lunarDay = lunarStartDay + i - (week == 0 ? firstDay : 0);

            if (week == 0 && i < firstDay || lunarDay > daysInMonth || lunarDay < 1) {
                printf("       |");
            }

            else {
                char inputDate[20];
                sprintf(inputDate, "%d/%d/%d", year, month, lunarDay);

                int lunarYear, lunarMonth, lunarDayInLunar;
                int found = find(hashTable, inputDate, &lunarYear, &lunarMonth, &lunarDayInLunar);
                /*printf(" ");*/
                if (found) {
                    
                    // 先检查是否是节日
                    if (isFestival(month, lunarDay, lunarMonth, lunarDayInLunar)) {
                        printf(" |");
                        continue; // 如果是节日，直接跳过，已经打印
                    }

                    // 其次检查是否是节气
                    if (isTerm(year, month, lunarDay)) {
                        printJieqi(month, lunarDay);
                        printf(" |");
                        continue; // 如果是节气，直接跳过，已经打印
                    }

                    // 如果既不是节日也不是节气，则打印农历
                    if (lunarDayInLunar == 1) {
                        printf("%s", NongliYue[lunarMonth - 1]);
                        printf(" |");
                    }
                    else {
                        printf("%s", NongliTian[lunarDayInLunar - 1]);
                        printf(" |");
                    }
                }
                else {
                    printf("Nfind");
                }
                
            }
        }
        printf("\n  |--------------------------------------------------------------|\n");

        if (day > daysInMonth) break;
    }
    printf("\n");
}

// 计算给定年份对应的天干地支年和生肖
void getTianGanDiZhi_and_Shengxiao(int year, char* result) {
    int baseYear = 1804; // 基准年为1804年，即甲子年
    int diff = year - baseYear;

    int tianGanIndex = (diff % 10 + 10) % 10;
    int diZhiIndex = (diff % 12 + 12) % 12;

    sprintf(result, "%s%s（%s）", Tiangan[tianGanIndex], Dizhi[diZhiIndex], Shengxiao[diZhiIndex]);
}

// 计算星期几（蔡勒公式）
const char* getWeekday(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }

    int K = year % 100;
    int J = year / 100;

    int h = (day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    // 返回对应星期几的中文
    const char* weekdays[] = { "星期六", "星期天", "星期一", "星期二", "星期三", "星期四", "星期五" };
    return weekdays[h];
}

// 功能1：年历查询
void yearCalendarQuery() {
    system("cls");
    int year;

    // 初始化哈希表
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    // 加载农历数据
    loadCalendarData(hashTable);

    // 输入年份
    do {
        printf("请输入年份（1900-2100）：");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("年份超出范围，请重新输入。\n");
        }
    } while (year < 1900 || year > 2100);
    system("cls");
    printf("输入的日期是：%04d\n", year);

    // 循环打印每个月的日历
    for (int month = 1; month <= 12; month++) {
        printMonthCalendar(year, month);
    }
    printf("\n\n按任意键返回菜单");
    _getch();  // 等待用户按任意键
    system("cls");
}

// 功能2：月历查询（简化版）
void monthCalendarQuery() {
    system("cls");
    int year, month;

    // 初始化哈希表
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    // 加载农历数据
    loadCalendarData(hashTable);

    // 输入年份
    do {
        printf("请输入年份（1900-2100）：");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("年份超出范围，请重新输入。\n");
        }
    } while (year < 1900 || year > 2100);

    // 输入月份
    do {
        printf("请输入月份（1-12）：");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("月份无效，请重新输入。\n");
        }
    } while (month < 1 || month > 12);
    system("cls");
    printf("输入的日期是：%04d-%02d\n", year, month);
    printMonthCalendar(year, month); // 打印该月的日历
    printf("\n\n按任意键返回菜单");
    _getch();  // 等待用户按任意键
    system("cls");
}

//检查是否有日程
void getSchedule(int year, int month, int day, int lunarMonth, int lunarDay) {
    // 定义日期字符串和读取行的缓冲区
    char date[20], line[256], * event;
    // 格式化日期字符串
    sprintf(date, "%d-%d-%d", year, month, day);

    // 打开日程文件
    FILE* file = fopen("schedules.txt", "r");
    if (!file) {
        printf("无法打开文件 schedules.txt。\n");
        return;
    }

    int found = 0; // 标记是否找到日程或节日
    // 逐行读取文件内容
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // 去除行末的换行符
        // 检查日期是否匹配
        if (strncmp(line, date, strlen(date)) == 0) {
            found = 1;
            event = strchr(line, ':'); // 查找事件描述的起始位置
            if (event) {
                event++; // 跳过':'
                printf(" %s", event); // 打印事件描述
            }
        }
    }
    fclose(file); // 关闭文件

    // 检查是否有阳历节日
    for (int i = 0; i < sizeof(gregorianFestivalDates) / sizeof(gregorianFestivalDates[0]); i++) {
        if (gregorianFestivalDates[i][0] == month && gregorianFestivalDates[i][1] == day) {
            found = 1;
            printf(" %s", gregorianFestivals[i]); // 打印阳历节日
        }
    }

    // 检查是否有农历节日
    for (int i = 0; i < sizeof(lunarFestivalDates) / sizeof(lunarFestivalDates[0]); i++) {
        if (lunarFestivalDates[i][0] == lunarMonth && lunarFestivalDates[i][1] == lunarDay) {
            found = 1;
            printf(" %s", lunarFestivals[i]); // 打印农历节日
        }
    }

    // 如果没有找到任何日程或节日，打印提示信息
    if (!found) {
        printf("  当天没有安排日程。\n");
    }
    else {
        printf("\n"); // 在所有日程和节日输出完毕后添加换行符
    }
}

void getLunarDate(const char* inputDate, char* lunarDate, int* found) {
    // 打开文件 "Calendar Comparison.txt" 进行读取
    FILE* file = fopen("Calendar Comparison.txt", "r");
    if (!file) {
        // 如果文件无法打开，打印错误信息并设置 found 为 0
        printf("无法打开文件 Calendar Comparison.txt。\n");
        *found = 0;
        return;
    }

    char solar[20], lunar[200];
    // 逐行读取文件内容
    while (fscanf(file, "%[^,],%s\n", solar, lunar) != EOF) {
        char trimmedSolar[20], trimmedLunar[200];
        // 解析读取到的阳历和农历日期
        sscanf(solar, "%s", trimmedSolar);
        sscanf(lunar, "%s", trimmedLunar);

        // 如果找到匹配的阳历日期
        if (strcmp(inputDate, trimmedSolar) == 0) {
            *found = 1;
            // 复制对应的农历日期到 lunarDate
            strcpy(lunarDate, trimmedLunar);
            break;
        }
    }
    // 关闭文件
    fclose(file);
    // 如果未找到匹配的阳历日期，打印提示信息
    if (!*found) {
        printf(" 未找到阳历日期 %s 的对应农历日期。\n", inputDate);
    }
}

//3.日历查询
void dayCalendarQuery() {
    system("cls");
    int year, month, day;
    char inputDate[20], lunarDate[200];
    int found = 0;

    do {
        printf("请输入年份（1900-2100）：");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("年份超出范围，请重新输入。\n");
        }
    } while (year < 1900 || year > 2100);

    do {
        printf("请输入月份（1-12）：");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("月份无效，请重新输入。\n");
        }
    } while (month < 1 || month > 12);

    do {
        printf("请输入日期（1-%d）：", getDaysInMonth(year, month));
        scanf("%d", &day);
        if (day < 1 || day > getDaysInMonth(year, month)) {
            printf("日期无效，请重新输入。\n");
        }
    } while (day < 1 || day > getDaysInMonth(year, month));

    system("cls");

    sprintf(inputDate, "%d/%d/%d", year, month, day);
    getLunarDate(inputDate, lunarDate, &found);

    if (found) {
        int lunarYear, lunarMonth, lunarDay;
        sscanf(lunarDate, "%d/%d/%d", &lunarYear, &lunarMonth, &lunarDay);

        char tianGanDiZhi[20];
        getTianGanDiZhi_and_Shengxiao(lunarYear, tianGanDiZhi);

        printf("  ============================================================================================================\n");
        printf("  | 阳历  |  %d年%2d月%2d日 %s\n", year, month, day, getWeekday(year, month, day));
        printf("  |-----------------------------------------------------------------------------------------------------------\n");
        printf("  | 农历  |  %s年 %s%s\n", tianGanDiZhi, NongliYue[lunarMonth - 1], NongliTian[lunarDay - 1]);
        printf("  |-----------------------------------------------------------------------------------------------------------\n");
        printf("  | 日程  |");

        getSchedule(year, month, day, lunarMonth, lunarDay);
    }
    printf("  |-----------------------------------------------------------------------------------------------------------\n");

    printf("\n\n按任意键返回菜单");
    _getch();
    system("cls");
}


// 显示该日期的所有记事
void displaySchedules(int year, int month, int day) {
    char date[20], line[256], * event;
    char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH];
    int scheduleCount = 0;

    // 构造待检索的日期字符串
    sprintf(date, "%d-%d-%d", year, month, day);

    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("无法打开文件 %s。\n", FILE_NAME);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // 去除行末的换行符
        if (strncmp(line, date, strlen(date)) == 0) {
            event = strchr(line, ':');
            if (event) {
                event++; // 跳过':'
                strncpy(schedules[scheduleCount], event, MAX_SCHEDULE_LENGTH - 1);
                schedules[scheduleCount][MAX_SCHEDULE_LENGTH - 1] = '\0'; // 确保字符串以'\0'结尾
                scheduleCount++;
            }
        }
    }
    fclose(file);

    // 打印日程
    printf("  ==============================\n");
    printf("  |  编号  |  记事\n  |-----------------------------\n");
    for (int i = 0; i < scheduleCount; i++) {
        printf("  |   %2d   |  %s\n", i + 1, schedules[i]);
        printf("  |-----------------------------\n");
    }

}

// 检查该日期是否有记事，返回0表示没有，返回1表示有
int findScheduleInFile(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int* scheduleCount) {
    FILE* file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        return 0;  // 如果文件打不开，直接返回0
    }

    *scheduleCount = 0;
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        int fileYear, fileMonth, fileDay;
        char schedule[MAX_SCHEDULE_LENGTH];

        // 解析每一行的日期和记事内容
        sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

        // 如果日期匹配
        if (fileYear == year && fileMonth == month && fileDay == day) {
            strcpy(schedules[*scheduleCount], schedule);
            (*scheduleCount)++;
        }
    }

    fclose(file);
    return (*scheduleCount > 0);
}

// 将新的记事添加到文件
void addScheduleToFile(int year, int month, int day, const char* schedule) {
    FILE* file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        return;
    }

    // 将新的记事添加到文件中
    fprintf(file, "%d-%d-%d: %s\n", year, month, day, schedule);
    fclose(file);
}

// 处理添加多个记事
void handleAddMultipleSchedules(int year, int month, int day) {
    system("cls");

    char schedule[MAX_SCHEDULE_LENGTH];
    char choice;

    do {
        printf("请输入记事内容：");
        getchar();  // 清空输入缓冲区
        fgets(schedule, MAX_SCHEDULE_LENGTH, stdin);
        schedule[strcspn(schedule, "\n")] = '\0';  // 去除末尾换行

        addScheduleToFile(year, month, day, schedule);  // 保存记事

        printf("是否继续添加记事？(y/n): ");
        scanf(" %c", &choice);  // 提示是否继续添加
    } while (choice == 'y' || choice == 'Y');
    /*printf("\n按任意键返回菜单");
    _getch();*/
    system("cls");
}

// 处理修改记事
void handleModifySchedule(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int scheduleCount) {
    system("cls");

    int modifyIndex;
    displaySchedules(year, month, day);

    printf("请选择要修改的记事编号：");
    scanf("%d", &modifyIndex);

    if (modifyIndex >= 1 && modifyIndex <= scheduleCount) {
        // 进行修改
        char newSchedule[MAX_SCHEDULE_LENGTH];
        printf("请输入新的记事内容：");
        getchar();  // 清空输入缓冲区
        fgets(newSchedule, MAX_SCHEDULE_LENGTH, stdin);
        newSchedule[strcspn(newSchedule, "\n")] = '\0';  // 去除末尾换行

        // 删除旧记事并添加新记事
        // 这里我们删除原有记事并重新写入文件
        FILE* file = fopen(FILE_NAME, "r");
        FILE* tempFile = fopen("temp.txt", "w");

        char line[200];
        while (fgets(line, sizeof(line), file)) {
            int fileYear, fileMonth, fileDay;
            char schedule[MAX_SCHEDULE_LENGTH];

            sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

            if (fileYear == year && fileMonth == month && fileDay == day) {
                // 跳过需要修改的记事
                if (strcmp(schedule, schedules[modifyIndex - 1]) != 0) {
                    fprintf(tempFile, "%d-%d-%d: %s\n", fileYear, fileMonth, fileDay, schedule);
                }
            }
            else {
                fprintf(tempFile, "%s", line);  // 保留其他记事
            }
        }

        fclose(file);
        fclose(tempFile);

        // 重新写入修改后的记事
        file = fopen(FILE_NAME, "w");
        tempFile = fopen("temp.txt", "r");
        while (fgets(line, sizeof(line), tempFile)) {
            fprintf(file, "%s", line);
        }
        fprintf(file, "%d-%d-%d: %s\n", year, month, day, newSchedule);  // 写入修改后的记事
        fclose(file);
        fclose(tempFile);
    }
    else {
        printf("无效的编号，请重新选择。\n");
    }
    printf("\n按任意键返回菜单");
    _getch();
    system("cls");
}


// 处理删除记事
void handleDeleteSchedule(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int scheduleCount) {
    system("cls");

    int deleteIndex;
    displaySchedules(year, month, day);
    printf("请选择要删除的记事编号：");
    scanf("%d", &deleteIndex);

    if (deleteIndex >= 1 && deleteIndex <= scheduleCount) {
        // 删除记事
        // 这里我们删除选中的记事并重新写入文件
        FILE* file = fopen(FILE_NAME, "r");
        FILE* tempFile = fopen("temp.txt", "w");

        char line[200];
        while (fgets(line, sizeof(line), file)) {
            int fileYear, fileMonth, fileDay;
            char schedule[MAX_SCHEDULE_LENGTH];

            sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

            if (fileYear == year && fileMonth == month && fileDay == day) {
                // 跳过需要删除的记事
                if (strcmp(schedule, schedules[deleteIndex - 1]) != 0) {
                    fprintf(tempFile, "%d-%d-%d: %s\n", fileYear, fileMonth, fileDay, schedule);
                }
            }
            else {
                fprintf(tempFile, "%s", line);  // 保留其他记事
            }
        }

        fclose(file);
        fclose(tempFile);

        // 重新写入剩余的记事
        file = fopen(FILE_NAME, "w");
        tempFile = fopen("temp.txt", "r");
        while (fgets(line, sizeof(line), tempFile)) {
            fprintf(file, "%s", line);
        }
        fclose(file);
        fclose(tempFile);

        remove("temp.txt");
        printf("记事已删除。\n");
    }
    else {
        printf("无效的编号，请重新选择。\n");
    }
    printf("\n按任意键返回菜单");
    _getch();
    system("cls");
}


// 4.记事本
void notebook() {
    system("cls");

    int year, month, day;

    // 输入年份
    do {
        printf("请输入年份（1900-2100）：");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("年份超出范围，请重新输入。\n");
        }
    } while (year < 1900 || year > 2100);

    // 输入月份
    do {
        printf("请输入月份（1-12）：");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("月份无效，请重新输入。\n");
        }
    } while (month < 1 || month > 12);

    // 输入日期
    do {
        printf("请输入日期（1-%d）：", getDaysInMonth(year, month));
        scanf("%d", &day);
        if (day < 1 || day > getDaysInMonth(year, month)) {
            printf("日期无效，请重新输入。\n");
        }
    } while (day < 1 || day > getDaysInMonth(year, month));
    system("cls");
    printf("输入的日期是：%04d-%02d-%02d\n", year, month, day);

    char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH];
    int scheduleCount = 0;

    // 查询该日期是否有记事
    if (findScheduleInFile(year, month, day, schedules, &scheduleCount)) {
        // 如果有记事，显示所有记事
        printf("  ==============================\n");
        printf("  |  编号  |  记事\n  |-----------------------------\n");
        for (int i = 0; i < scheduleCount; i++) {
            printf("  |   %2d   |  %s\n", i + 1, schedules[i]);
            printf("  |-----------------------------\n");
        }

        // 提示用户选择操作
        int option;
        printf("  请选择操作：  ");
        printf("1. 添加记事  ");
        printf("2. 修改记事  ");
        printf("3. 删除记事  ");
        printf("0. 返回\n");
        printf("  输入你的选择(0-3):");
        scanf("%d", &option);
        switch (option) {
        case 1:
            handleAddMultipleSchedules(year, month, day);  // 添加多个记事
            break;
        case 2:
            handleModifySchedule(year, month, day, schedules, scheduleCount);  // 修改记事
            break;
        case 3:
            handleDeleteSchedule(year, month, day, schedules, scheduleCount);  // 删除记事
            break;
        case 0:
            printf("返回。\n");
            system("cls");
            break;
        default:
            printf("无效选择，请重新选择。\n");
            break;
        }
    }
    else {
        // 如果没有记事，询问是否添加
        printf("该日期没有记事，是否添加新的记事？(y/n): ");
        char choice;
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            handleAddMultipleSchedules(year, month, day);  // 添加多个记事
        }
        else {
            printf("退出系统。\n");
            system("cls");
        }
    }
}


volatile int stop = 0;

void inputListener(void* arg) {
    while (!stop) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == '0' || ch == '0') {
                stop = 1;
                printf("\n定时关机已取消。\n");
            }
        }
        Sleep(100);
    }
    system("cls");
}

void countdown(int seconds) {
    while (seconds > 0 && !stop) {
        printf("关机倒计时: %d 秒(按0取消定时关机)\r", seconds);
        fflush(stdout);
        Sleep(1000);
        seconds--;
    }
    printf("\n");
}

void shutdownProgram() {
    printf("程序已关闭。\n");
    exit(0);
}

//定时关机程序
void shutdownTimer() {
    system("cls");
    int seconds;
    printf("请输入定时关机的时间（秒）: ");
    scanf("%d", &seconds);

    _beginthread(inputListener, 0, NULL); // 启动用户输入监听线程

    countdown(seconds);
    if (!stop) {
        shutdownProgram();
    }
    else {
        stop = 1;// 停止输入监听线程
    }
}

void drawCalendarTitle() {
    const char* calendar[] = {
        "                        ****   ***   *      *****  *   *  ****     ***    ****  ",
        "                       *      *   *  *      *      **  *  *    *  *   *   *   * ",
        "                       *      *****  *      *****  * * *  *    *  *****   ****  ",
        "                       *      *   *  *      *      *  **  *    *  *   *   *  *  ",
        "                        ****  *   *  *****  *****  *   *  ****    *   *   *   * "
    };

    for (int i = 0; i < sizeof(calendar) / sizeof(calendar[0]); i++) {
        printf("%s\n", calendar[i]);
    }
}


//菜单
void showMenu() {
    // 获取当前时间
    printf("\n");
    drawCalendarTitle();
    printf("\n");
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // 计算星期几
    const char* weekday = getWeekday(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

    // 获取农历日期
    char inputDate[20], lunarDate[200];
    int found = 0;
    sprintf(inputDate, "%d/%d/%d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    getLunarDate(inputDate, lunarDate, &found);

    if (found) {
        int lunarYear, lunarMonth, lunarDay;
        sscanf(lunarDate, "%d/%d/%d", &lunarYear, &lunarMonth, &lunarDay);

        char tianGanDiZhi[20];
        getTianGanDiZhi_and_Shengxiao(lunarYear, tianGanDiZhi);

        // 打印菜单
        printf("            -------------------------------------------------------------------------------\n");
        printf("            | 当前时间: %04d-%02d-%02d %02d:%02d:%02d  %s",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, weekday);
        printf("  %s年 %s%s            |\n", tianGanDiZhi, NongliYue[lunarMonth - 1], NongliTian[lunarDay - 1]);
        printf("            |-----------------------------------------------------------------------------|\n");
        printf("            |      1     |      2     |      3     |      4     |      5     |      0     |\n");
        printf("            |-----------------------------------------------------------------------------|\n");
        printf("            |  年历查询  |  月历查询  |  日历查询  |   记事本   |  定时关机  |  退出系统  |\n");
        printf("            |-----------------------------------------------------------------------------|\n\n");

        printf("                                     输入你的选择(0-5): ");
    }
}

int main() {
    int choice;
    while (1) {
        showMenu();
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            yearCalendarQuery();
            break;
        case 2:
            monthCalendarQuery();
            break;
        case 3:
            dayCalendarQuery();
            break;
        case 4:
            notebook();
            break;
        case 5:
            shutdownTimer();
            break;
        case 0:
            printf("退出系统。\n");
            return 0;
        default:
            printf("无效输入，请重新选择。\n");
        }
    }
    return 0;
}
