#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>  // ���� _kbhit() �� _getch()
#include <process.h> // ���� _beginthread
#include <time.h>
#include <iostream>


#define FILE_NAME "schedules.txt"

#define MAX_SCHEDULES_PER_DAY 10  // ÿ�����֧�ֵ��ճ���
#define MAX_SCHEDULE_LENGTH 200    // ÿ���ճ����200���ַ�
#define MAX_DATE_LENGTH 20         // ���ڸ�ʽ��󳤶�
#define TABLE_SIZE 10000 // ���С


const char* Tiangan[] = { "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" };
const char* Dizhi[] = { "��", "��", "��", "î", "��", "��", "��", "δ", "��", "��", "��", "��" };
const char* Shengxiao[] = { "��", "ţ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" };
const char* NongliYue[] = { " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ʮ�� ", "ʮһ��", " ���� " };
const char* NongliTian[] = { " ��һ ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ���� ", " ��ʮ ", 
                             " ʮһ ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ʮ�� ", " ��ʮ ",
                             " إһ ", " إ�� ", " إ�� ", " إ�� ", " إ�� ", " إ�� ", " إ�� ", " إ�� ", " إ�� ", " ��ʮ " };
const char* Week[] = { "������", "����һ", "���ڶ�", "������", "������", "������", "������" };
const char* Jieqi[12][2] = { { " С�� ", " �� " }, 
                             { " ���� ", " ��ˮ " }, 
                             { " ���� ", " ���� " }, 
                             { " ���� ", " ���� " }, 
                             { " ���� ", " С�� " }, 
                             { " â�� ", " ���� " }, 
                             { " С�� ", " ���� " }, 
                             { " ���� ", " ���� " }, 
                             { " ��¶ ", " ��� " }, 
                             { " ��¶ ", " ˪�� " }, 
                             { " ���� ", " Сѩ " }, 
                             { " ��ѩ ", " ���� " } };
const char* gregorianFestivals[] = { " Ԫ�� ", "��Ů��", "���˽�", "ֲ����", "�Ͷ���", "�����", "��ͯ��", "������", "������", "�����", "��ʦ��", "��ʥ��", "ʥ����" };
const char* lunarFestivals[] = { " ���� ", "Ԫ����", "��̧ͷ", "�����", "��Ϧ��", "��Ԫ��", "�����", "������", "���˽�" };
int gregorianFestivalDates[][2] = { {1, 1}, {3, 8}, {2, 14}, {3, 12}, {5, 1}, {5, 4}, {6, 1}, {7, 1}, {8, 1}, {10, 1}, {9, 10}, {10, 31}, {12, 25} };
int lunarFestivalDates[][2] = { {1, 1}, {1, 15}, {2, 2}, {5, 5}, {7, 7}, {7, 15}, {8, 15}, {9, 9}, {12, 8} };


// �ж�ĳ�����Ƿ�Ϊ����
int isFestival(int month, int day, int lunarMonth, int lunarDay) {
    // ��鹫������
    for (int i = 0; i < sizeof(gregorianFestivalDates) / sizeof(gregorianFestivalDates[0]); ++i) {
        if (month == gregorianFestivalDates[i][0] && day == gregorianFestivalDates[i][1]) {
            printf("%s", gregorianFestivals[i]); // ��ӡ��������
            return 1; // �ǽ���
        }
    }

    // ���ũ������
    for (int i = 0; i < sizeof(lunarFestivalDates) / sizeof(lunarFestivalDates[0]); ++i) {
        if (lunarMonth == lunarFestivalDates[i][0] && lunarDay == lunarFestivalDates[i][1]) {
            printf("%s", lunarFestivals[i]); // ��ӡ��������
            return 1; // �ǽ���
        }
    }

    return 0; // ���ǽ���
}

// ��ڵ�ṹ
typedef struct Node {
    int identifier;    // Ψһ��ʶ����������ϳɵ����֣�
    struct Node* next; // ָ����һ���ڵ㣨�����ͻ��
} Node;

// ȫ�ֱ�
Node* table[TABLE_SIZE];

// ɢ�к���
unsigned int hashFunc(int identifier) {
    return identifier % TABLE_SIZE;
}

// ���뵽��
void insertToTable(int identifier) {
    unsigned int index = hashFunc(identifier);
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("�ڴ����ʧ��");
        exit(EXIT_FAILURE);
    }
    newNode->identifier = identifier;
    newNode->next = table[index];
    table[index] = newNode;
}

// ���ұ�
int searchTable(int identifier) {
    unsigned int index = hashFunc(identifier);
    Node* current = table[index];
    while (current) {
        if (current->identifier == identifier) {
            return 1; // �ҵ�
        }
        current = current->next;
    }
    return 0; // δ�ҵ�
}

// ��ʼ����
void initTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = NULL;
    }
}

// �����
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

// �����ļ����ݵ���
void loadTermsData(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("�޷����ļ�");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int data[25] = { 0 }; // 12���µ����ڣ�ÿ�������� + ���
        int index = 0;

        // ����һ������
        char* token = strtok(line, ",");
        while (token != NULL && index < 25) {
            data[index++] = atoi(token);
            token = strtok(NULL, ",");
        }

        int year = data[24];
        for (int month = 1; month <= 12; ++month) {
            int day1 = data[(month - 1) * 2];       // ��һ������
            int day2 = data[(month - 1) * 2 + 1];   // �ڶ�������

            if (day1 > 0) {
                int id1 = year * 10000 + month * 100 + day1; // ����Ψһ��ʶ
                insertToTable(id1);
            }
            if (day2 > 0) {
                int id2 = year * 10000 + month * 100 + day2; // ����Ψһ��ʶ
                insertToTable(id2);
            }
        }
    }

    fclose(file);
}

// �ж��Ƿ�Ϊ��ʮ�Ľ�����
int isTerm(int year, int month, int day) {
    //��ʼ���� 
    initTable();
    // �����ļ����ݵ���
    loadTermsData("SolarTermsData.txt");
    int id = year * 10000 + month * 100 + day;
	void clearTable();
    return searchTable(id);
}

//��ӡ����
void printJieqi(int month, int day) {
    if (day < 10) {
        day = 0;
    }
	else {
		day = 1;
	}
	printf("%s", Jieqi[month - 1][day]);
}

// ����ṹ��洢������ũ������
typedef struct {
    char solar[20];
    int lunarYear;
    int lunarMonth;
    int lunarDay;
} Calendar;

// �����ϣ��Ľڵ�
typedef struct HashNode {
    char solar[20];
    int lunarYear;
    int lunarMonth;
    int lunarDay;
    struct HashNode* next;
} HashNode;

// �����ϣ��
#define TABLE_SIZE 5000
HashNode* hashTable[TABLE_SIZE];

// ��ϣ����
unsigned int hash(char* key) {
    unsigned int hashValue = 0;
    while (*key) {
        hashValue = (hashValue << 5) + *key++;
    }
    return hashValue % TABLE_SIZE;
}

// �������ݵ���ϣ��
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

// ��������
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

// �ж��Ƿ�������
int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ��ȡĳ���µ�����
int getDaysInMonth(int year, int month) {
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return days[month - 1];
}

// ����ĳ��ĳ��1�������ڼ� (�������տ�ʼ��0Ϊ�����գ�6Ϊ������)
int getWeekday(int year, int month) {
    int days = 0;
    for (int y = 1900; y < year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    for (int m = 1; m < month; m++) {
        days += getDaysInMonth(year, m);
    }
    return (days + 1) % 7; // 1900��1��1��������һ������+1
}

// ��ȡ�ļ����ݲ�����ϣ��
void loadCalendarData(HashNode* table[]) {
    FILE* file = fopen("Calendar Comparison.txt", "r");
    if (!file) {
        printf("�޷����ļ� Calendar Comparison.txt��\n");
        return;
    }

    char solar[20];
    int lunarYear, lunarMonth, lunarDay;
    while (fscanf(file, "%[^,],%d/%d/%d\n", solar, &lunarYear, &lunarMonth, &lunarDay) != EOF) {
        insert(table, solar, lunarYear, lunarMonth, lunarDay);
    }

    fclose(file);
}

// ��ӡ����
void printMonthCalendar(int year, int month) {
    printf("  ================================================================\n");
    printf("  |                          %d��%2d��                          |\n", year, month);
    printf("  |--------------------------------------------------------------|\n");
    printf("  | ������ | ����һ | ���ڶ� | ������ | ������ | ������ | ������ |\n");
    printf("  |--------|--------|--------|--------|--------|--------|--------|\n");
    int firstDay = getWeekday(year, month);
    int daysInMonth = getDaysInMonth(year, month);

    int day = 1;
    int lunarStartDay = 1;

    for (int week = 0; week < 6; week++) {
        int isEmptyWeek = 1;
        printf("  |");
        //����
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

        //ũ�����ս���
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
                    
                    // �ȼ���Ƿ��ǽ���
                    if (isFestival(month, lunarDay, lunarMonth, lunarDayInLunar)) {
                        printf(" |");
                        continue; // ����ǽ��գ�ֱ���������Ѿ���ӡ
                    }

                    // ��μ���Ƿ��ǽ���
                    if (isTerm(year, month, lunarDay)) {
                        printJieqi(month, lunarDay);
                        printf(" |");
                        continue; // ����ǽ�����ֱ���������Ѿ���ӡ
                    }

                    // ����Ȳ��ǽ���Ҳ���ǽ��������ӡũ��
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

// ���������ݶ�Ӧ����ɵ�֧�����Ф
void getTianGanDiZhi_and_Shengxiao(int year, char* result) {
    int baseYear = 1804; // ��׼��Ϊ1804�꣬��������
    int diff = year - baseYear;

    int tianGanIndex = (diff % 10 + 10) % 10;
    int diZhiIndex = (diff % 12 + 12) % 12;

    sprintf(result, "%s%s��%s��", Tiangan[tianGanIndex], Dizhi[diZhiIndex], Shengxiao[diZhiIndex]);
}

// �������ڼ������չ�ʽ��
const char* getWeekday(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }

    int K = year % 100;
    int J = year / 100;

    int h = (day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    // ���ض�Ӧ���ڼ�������
    const char* weekdays[] = { "������", "������", "����һ", "���ڶ�", "������", "������", "������" };
    return weekdays[h];
}

// ����1��������ѯ
void yearCalendarQuery() {
    system("cls");
    int year;

    // ��ʼ����ϣ��
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    // ����ũ������
    loadCalendarData(hashTable);

    // �������
    do {
        printf("��������ݣ�1900-2100����");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("��ݳ�����Χ�����������롣\n");
        }
    } while (year < 1900 || year > 2100);
    system("cls");
    printf("����������ǣ�%04d\n", year);

    // ѭ����ӡÿ���µ�����
    for (int month = 1; month <= 12; month++) {
        printMonthCalendar(year, month);
    }
    printf("\n\n����������ز˵�");
    _getch();  // �ȴ��û��������
    system("cls");
}

// ����2��������ѯ���򻯰棩
void monthCalendarQuery() {
    system("cls");
    int year, month;

    // ��ʼ����ϣ��
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }

    // ����ũ������
    loadCalendarData(hashTable);

    // �������
    do {
        printf("��������ݣ�1900-2100����");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("��ݳ�����Χ�����������롣\n");
        }
    } while (year < 1900 || year > 2100);

    // �����·�
    do {
        printf("�������·ݣ�1-12����");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("�·���Ч�����������롣\n");
        }
    } while (month < 1 || month > 12);
    system("cls");
    printf("����������ǣ�%04d-%02d\n", year, month);
    printMonthCalendar(year, month); // ��ӡ���µ�����
    printf("\n\n����������ز˵�");
    _getch();  // �ȴ��û��������
    system("cls");
}

//����Ƿ����ճ�
void getSchedule(int year, int month, int day, int lunarMonth, int lunarDay) {
    // ���������ַ����Ͷ�ȡ�еĻ�����
    char date[20], line[256], * event;
    // ��ʽ�������ַ���
    sprintf(date, "%d-%d-%d", year, month, day);

    // ���ճ��ļ�
    FILE* file = fopen("schedules.txt", "r");
    if (!file) {
        printf("�޷����ļ� schedules.txt��\n");
        return;
    }

    int found = 0; // ����Ƿ��ҵ��ճ̻����
    // ���ж�ȡ�ļ�����
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // ȥ����ĩ�Ļ��з�
        // ��������Ƿ�ƥ��
        if (strncmp(line, date, strlen(date)) == 0) {
            found = 1;
            event = strchr(line, ':'); // �����¼���������ʼλ��
            if (event) {
                event++; // ����':'
                printf(" %s", event); // ��ӡ�¼�����
            }
        }
    }
    fclose(file); // �ر��ļ�

    // ����Ƿ�����������
    for (int i = 0; i < sizeof(gregorianFestivalDates) / sizeof(gregorianFestivalDates[0]); i++) {
        if (gregorianFestivalDates[i][0] == month && gregorianFestivalDates[i][1] == day) {
            found = 1;
            printf(" %s", gregorianFestivals[i]); // ��ӡ��������
        }
    }

    // ����Ƿ���ũ������
    for (int i = 0; i < sizeof(lunarFestivalDates) / sizeof(lunarFestivalDates[0]); i++) {
        if (lunarFestivalDates[i][0] == lunarMonth && lunarFestivalDates[i][1] == lunarDay) {
            found = 1;
            printf(" %s", lunarFestivals[i]); // ��ӡũ������
        }
    }

    // ���û���ҵ��κ��ճ̻���գ���ӡ��ʾ��Ϣ
    if (!found) {
        printf("  ����û�а����ճ̡�\n");
    }
    else {
        printf("\n"); // �������ճ̺ͽ��������Ϻ���ӻ��з�
    }
}

void getLunarDate(const char* inputDate, char* lunarDate, int* found) {
    // ���ļ� "Calendar Comparison.txt" ���ж�ȡ
    FILE* file = fopen("Calendar Comparison.txt", "r");
    if (!file) {
        // ����ļ��޷��򿪣���ӡ������Ϣ������ found Ϊ 0
        printf("�޷����ļ� Calendar Comparison.txt��\n");
        *found = 0;
        return;
    }

    char solar[20], lunar[200];
    // ���ж�ȡ�ļ�����
    while (fscanf(file, "%[^,],%s\n", solar, lunar) != EOF) {
        char trimmedSolar[20], trimmedLunar[200];
        // ������ȡ����������ũ������
        sscanf(solar, "%s", trimmedSolar);
        sscanf(lunar, "%s", trimmedLunar);

        // ����ҵ�ƥ�����������
        if (strcmp(inputDate, trimmedSolar) == 0) {
            *found = 1;
            // ���ƶ�Ӧ��ũ�����ڵ� lunarDate
            strcpy(lunarDate, trimmedLunar);
            break;
        }
    }
    // �ر��ļ�
    fclose(file);
    // ���δ�ҵ�ƥ����������ڣ���ӡ��ʾ��Ϣ
    if (!*found) {
        printf(" δ�ҵ��������� %s �Ķ�Ӧũ�����ڡ�\n", inputDate);
    }
}

//3.������ѯ
void dayCalendarQuery() {
    system("cls");
    int year, month, day;
    char inputDate[20], lunarDate[200];
    int found = 0;

    do {
        printf("��������ݣ�1900-2100����");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("��ݳ�����Χ�����������롣\n");
        }
    } while (year < 1900 || year > 2100);

    do {
        printf("�������·ݣ�1-12����");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("�·���Ч�����������롣\n");
        }
    } while (month < 1 || month > 12);

    do {
        printf("���������ڣ�1-%d����", getDaysInMonth(year, month));
        scanf("%d", &day);
        if (day < 1 || day > getDaysInMonth(year, month)) {
            printf("������Ч�����������롣\n");
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
        printf("  | ����  |  %d��%2d��%2d�� %s\n", year, month, day, getWeekday(year, month, day));
        printf("  |-----------------------------------------------------------------------------------------------------------\n");
        printf("  | ũ��  |  %s�� %s%s\n", tianGanDiZhi, NongliYue[lunarMonth - 1], NongliTian[lunarDay - 1]);
        printf("  |-----------------------------------------------------------------------------------------------------------\n");
        printf("  | �ճ�  |");

        getSchedule(year, month, day, lunarMonth, lunarDay);
    }
    printf("  |-----------------------------------------------------------------------------------------------------------\n");

    printf("\n\n����������ز˵�");
    _getch();
    system("cls");
}


// ��ʾ�����ڵ����м���
void displaySchedules(int year, int month, int day) {
    char date[20], line[256], * event;
    char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH];
    int scheduleCount = 0;

    // ����������������ַ���
    sprintf(date, "%d-%d-%d", year, month, day);

    FILE* file = fopen(FILE_NAME, "r");
    if (!file) {
        printf("�޷����ļ� %s��\n", FILE_NAME);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // ȥ����ĩ�Ļ��з�
        if (strncmp(line, date, strlen(date)) == 0) {
            event = strchr(line, ':');
            if (event) {
                event++; // ����':'
                strncpy(schedules[scheduleCount], event, MAX_SCHEDULE_LENGTH - 1);
                schedules[scheduleCount][MAX_SCHEDULE_LENGTH - 1] = '\0'; // ȷ���ַ�����'\0'��β
                scheduleCount++;
            }
        }
    }
    fclose(file);

    // ��ӡ�ճ�
    printf("  ==============================\n");
    printf("  |  ���  |  ����\n  |-----------------------------\n");
    for (int i = 0; i < scheduleCount; i++) {
        printf("  |   %2d   |  %s\n", i + 1, schedules[i]);
        printf("  |-----------------------------\n");
    }

}

// ���������Ƿ��м��£�����0��ʾû�У�����1��ʾ��
int findScheduleInFile(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int* scheduleCount) {
    FILE* file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        return 0;  // ����ļ��򲻿���ֱ�ӷ���0
    }

    *scheduleCount = 0;
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        int fileYear, fileMonth, fileDay;
        char schedule[MAX_SCHEDULE_LENGTH];

        // ����ÿһ�е����ںͼ�������
        sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

        // �������ƥ��
        if (fileYear == year && fileMonth == month && fileDay == day) {
            strcpy(schedules[*scheduleCount], schedule);
            (*scheduleCount)++;
        }
    }

    fclose(file);
    return (*scheduleCount > 0);
}

// ���µļ�����ӵ��ļ�
void addScheduleToFile(int year, int month, int day, const char* schedule) {
    FILE* file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        printf("�޷����ļ�����д�롣\n");
        return;
    }

    // ���µļ�����ӵ��ļ���
    fprintf(file, "%d-%d-%d: %s\n", year, month, day, schedule);
    fclose(file);
}

// ������Ӷ������
void handleAddMultipleSchedules(int year, int month, int day) {
    system("cls");

    char schedule[MAX_SCHEDULE_LENGTH];
    char choice;

    do {
        printf("������������ݣ�");
        getchar();  // ������뻺����
        fgets(schedule, MAX_SCHEDULE_LENGTH, stdin);
        schedule[strcspn(schedule, "\n")] = '\0';  // ȥ��ĩβ����

        addScheduleToFile(year, month, day, schedule);  // �������

        printf("�Ƿ������Ӽ��£�(y/n): ");
        scanf(" %c", &choice);  // ��ʾ�Ƿ�������
    } while (choice == 'y' || choice == 'Y');
    /*printf("\n����������ز˵�");
    _getch();*/
    system("cls");
}

// �����޸ļ���
void handleModifySchedule(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int scheduleCount) {
    system("cls");

    int modifyIndex;
    displaySchedules(year, month, day);

    printf("��ѡ��Ҫ�޸ĵļ��±�ţ�");
    scanf("%d", &modifyIndex);

    if (modifyIndex >= 1 && modifyIndex <= scheduleCount) {
        // �����޸�
        char newSchedule[MAX_SCHEDULE_LENGTH];
        printf("�������µļ������ݣ�");
        getchar();  // ������뻺����
        fgets(newSchedule, MAX_SCHEDULE_LENGTH, stdin);
        newSchedule[strcspn(newSchedule, "\n")] = '\0';  // ȥ��ĩβ����

        // ɾ���ɼ��²�����¼���
        // ��������ɾ��ԭ�м��²�����д���ļ�
        FILE* file = fopen(FILE_NAME, "r");
        FILE* tempFile = fopen("temp.txt", "w");

        char line[200];
        while (fgets(line, sizeof(line), file)) {
            int fileYear, fileMonth, fileDay;
            char schedule[MAX_SCHEDULE_LENGTH];

            sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

            if (fileYear == year && fileMonth == month && fileDay == day) {
                // ������Ҫ�޸ĵļ���
                if (strcmp(schedule, schedules[modifyIndex - 1]) != 0) {
                    fprintf(tempFile, "%d-%d-%d: %s\n", fileYear, fileMonth, fileDay, schedule);
                }
            }
            else {
                fprintf(tempFile, "%s", line);  // ������������
            }
        }

        fclose(file);
        fclose(tempFile);

        // ����д���޸ĺ�ļ���
        file = fopen(FILE_NAME, "w");
        tempFile = fopen("temp.txt", "r");
        while (fgets(line, sizeof(line), tempFile)) {
            fprintf(file, "%s", line);
        }
        fprintf(file, "%d-%d-%d: %s\n", year, month, day, newSchedule);  // д���޸ĺ�ļ���
        fclose(file);
        fclose(tempFile);
    }
    else {
        printf("��Ч�ı�ţ�������ѡ��\n");
    }
    printf("\n����������ز˵�");
    _getch();
    system("cls");
}


// ����ɾ������
void handleDeleteSchedule(int year, int month, int day, char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH], int scheduleCount) {
    system("cls");

    int deleteIndex;
    displaySchedules(year, month, day);
    printf("��ѡ��Ҫɾ���ļ��±�ţ�");
    scanf("%d", &deleteIndex);

    if (deleteIndex >= 1 && deleteIndex <= scheduleCount) {
        // ɾ������
        // ��������ɾ��ѡ�еļ��²�����д���ļ�
        FILE* file = fopen(FILE_NAME, "r");
        FILE* tempFile = fopen("temp.txt", "w");

        char line[200];
        while (fgets(line, sizeof(line), file)) {
            int fileYear, fileMonth, fileDay;
            char schedule[MAX_SCHEDULE_LENGTH];

            sscanf(line, "%d-%d-%d: %[^\n]", &fileYear, &fileMonth, &fileDay, schedule);

            if (fileYear == year && fileMonth == month && fileDay == day) {
                // ������Ҫɾ���ļ���
                if (strcmp(schedule, schedules[deleteIndex - 1]) != 0) {
                    fprintf(tempFile, "%d-%d-%d: %s\n", fileYear, fileMonth, fileDay, schedule);
                }
            }
            else {
                fprintf(tempFile, "%s", line);  // ������������
            }
        }

        fclose(file);
        fclose(tempFile);

        // ����д��ʣ��ļ���
        file = fopen(FILE_NAME, "w");
        tempFile = fopen("temp.txt", "r");
        while (fgets(line, sizeof(line), tempFile)) {
            fprintf(file, "%s", line);
        }
        fclose(file);
        fclose(tempFile);

        remove("temp.txt");
        printf("������ɾ����\n");
    }
    else {
        printf("��Ч�ı�ţ�������ѡ��\n");
    }
    printf("\n����������ز˵�");
    _getch();
    system("cls");
}


// 4.���±�
void notebook() {
    system("cls");

    int year, month, day;

    // �������
    do {
        printf("��������ݣ�1900-2100����");
        scanf("%d", &year);
        if (year < 1900 || year > 2100) {
            printf("��ݳ�����Χ�����������롣\n");
        }
    } while (year < 1900 || year > 2100);

    // �����·�
    do {
        printf("�������·ݣ�1-12����");
        scanf("%d", &month);
        if (month < 1 || month > 12) {
            printf("�·���Ч�����������롣\n");
        }
    } while (month < 1 || month > 12);

    // ��������
    do {
        printf("���������ڣ�1-%d����", getDaysInMonth(year, month));
        scanf("%d", &day);
        if (day < 1 || day > getDaysInMonth(year, month)) {
            printf("������Ч�����������롣\n");
        }
    } while (day < 1 || day > getDaysInMonth(year, month));
    system("cls");
    printf("����������ǣ�%04d-%02d-%02d\n", year, month, day);

    char schedules[MAX_SCHEDULES_PER_DAY][MAX_SCHEDULE_LENGTH];
    int scheduleCount = 0;

    // ��ѯ�������Ƿ��м���
    if (findScheduleInFile(year, month, day, schedules, &scheduleCount)) {
        // ����м��£���ʾ���м���
        printf("  ==============================\n");
        printf("  |  ���  |  ����\n  |-----------------------------\n");
        for (int i = 0; i < scheduleCount; i++) {
            printf("  |   %2d   |  %s\n", i + 1, schedules[i]);
            printf("  |-----------------------------\n");
        }

        // ��ʾ�û�ѡ�����
        int option;
        printf("  ��ѡ�������  ");
        printf("1. ��Ӽ���  ");
        printf("2. �޸ļ���  ");
        printf("3. ɾ������  ");
        printf("0. ����\n");
        printf("  �������ѡ��(0-3):");
        scanf("%d", &option);
        switch (option) {
        case 1:
            handleAddMultipleSchedules(year, month, day);  // ��Ӷ������
            break;
        case 2:
            handleModifySchedule(year, month, day, schedules, scheduleCount);  // �޸ļ���
            break;
        case 3:
            handleDeleteSchedule(year, month, day, schedules, scheduleCount);  // ɾ������
            break;
        case 0:
            printf("���ء�\n");
            system("cls");
            break;
        default:
            printf("��Чѡ��������ѡ��\n");
            break;
        }
    }
    else {
        // ���û�м��£�ѯ���Ƿ����
        printf("������û�м��£��Ƿ�����µļ��£�(y/n): ");
        char choice;
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            handleAddMultipleSchedules(year, month, day);  // ��Ӷ������
        }
        else {
            printf("�˳�ϵͳ��\n");
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
                printf("\n��ʱ�ػ���ȡ����\n");
            }
        }
        Sleep(100);
    }
    system("cls");
}

void countdown(int seconds) {
    while (seconds > 0 && !stop) {
        printf("�ػ�����ʱ: %d ��(��0ȡ����ʱ�ػ�)\r", seconds);
        fflush(stdout);
        Sleep(1000);
        seconds--;
    }
    printf("\n");
}

void shutdownProgram() {
    printf("�����ѹرա�\n");
    exit(0);
}

//��ʱ�ػ�����
void shutdownTimer() {
    system("cls");
    int seconds;
    printf("�����붨ʱ�ػ���ʱ�䣨�룩: ");
    scanf("%d", &seconds);

    _beginthread(inputListener, 0, NULL); // �����û���������߳�

    countdown(seconds);
    if (!stop) {
        shutdownProgram();
    }
    else {
        stop = 1;// ֹͣ��������߳�
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


//�˵�
void showMenu() {
    // ��ȡ��ǰʱ��
    printf("\n");
    drawCalendarTitle();
    printf("\n");
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // �������ڼ�
    const char* weekday = getWeekday(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

    // ��ȡũ������
    char inputDate[20], lunarDate[200];
    int found = 0;
    sprintf(inputDate, "%d/%d/%d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    getLunarDate(inputDate, lunarDate, &found);

    if (found) {
        int lunarYear, lunarMonth, lunarDay;
        sscanf(lunarDate, "%d/%d/%d", &lunarYear, &lunarMonth, &lunarDay);

        char tianGanDiZhi[20];
        getTianGanDiZhi_and_Shengxiao(lunarYear, tianGanDiZhi);

        // ��ӡ�˵�
        printf("            -------------------------------------------------------------------------------\n");
        printf("            | ��ǰʱ��: %04d-%02d-%02d %02d:%02d:%02d  %s",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, weekday);
        printf("  %s�� %s%s            |\n", tianGanDiZhi, NongliYue[lunarMonth - 1], NongliTian[lunarDay - 1]);
        printf("            |-----------------------------------------------------------------------------|\n");
        printf("            |      1     |      2     |      3     |      4     |      5     |      0     |\n");
        printf("            |-----------------------------------------------------------------------------|\n");
        printf("            |  ������ѯ  |  ������ѯ  |  ������ѯ  |   ���±�   |  ��ʱ�ػ�  |  �˳�ϵͳ  |\n");
        printf("            |-----------------------------------------------------------------------------|\n\n");

        printf("                                     �������ѡ��(0-5): ");
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
            printf("�˳�ϵͳ��\n");
            return 0;
        default:
            printf("��Ч���룬������ѡ��\n");
        }
    }
    return 0;
}
