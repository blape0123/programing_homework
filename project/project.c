#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "ledger.csv"
#define MAX_TRANSACTIONS 1000
#define MAX_DATE 11
#define MAX_CATEGORY 32
#define MAX_DESC 128
#define MAX_LINE 256

typedef struct {
    char date[MAX_DATE];
    char category[MAX_CATEGORY];
    char description[MAX_DESC];
    double amount;
    int isIncome; // 0: expense, 1: income
} Transaction;

void trimNewline(char *text) {
    size_t len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
    }
}

void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trimNewline(buffer);
}

double readAmount(const char *prompt) {
    char buffer[MAX_LINE];
    double amount;

    while (1) {
        readLine(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%lf", &amount) == 1 && amount >= 0.0) {
            return amount;
        }
        printf("유효한 금액을 숫자로 입력하세요.\n");
    }
}

int readMenuChoice(void) {
    char buffer[MAX_LINE];
    int choice;

    readLine("선택: ", buffer, sizeof(buffer));
    if (sscanf(buffer, "%d", &choice) == 1) {
        return choice;
    }
    return -1;
}

void printTransaction(const Transaction *t, int index) {
    printf("%2d | %s | %-8s | %-20s | %10.2f | %s\n",
           index + 1,
           t->date,
           t->category,
           t->description,
           t->amount,
           t->isIncome ? "수입" : "지출");
}

void viewTransactions(const Transaction *transactions, int count) {
    if (count == 0) {
        printf("저장된 거래가 없습니다.\n");
        return;
    }

    printf("\n번호 | 날짜       | 카테고리   | 내용                 | 금액       | 구분\n");
    printf("--------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printTransaction(&transactions[i], i);
    }
    printf("--------------------------------------------------------------------------\n");
}

void addTransaction(Transaction *transactions, int *count) {
    if (*count >= MAX_TRANSACTIONS) {
        printf("거래 개수가 최대값(%d)을 초과했습니다.\n", MAX_TRANSACTIONS);
        return;
    }

    Transaction *t = &transactions[*count];
    readLine("날짜 (YYYY-MM-DD): ", t->date, sizeof(t->date));
    readLine("카테고리: ", t->category, sizeof(t->category));
    readLine("내용: ", t->description, sizeof(t->description));
    t->amount = readAmount("금액: ");

    while (1) {
        char buffer[MAX_LINE];
        readLine("구분 선택 (1: 수입, 2: 지출): ", buffer, sizeof(buffer));
        if (strcmp(buffer, "1") == 0) {
            t->isIncome = 1;
            break;
        } else if (strcmp(buffer, "2") == 0) {
            t->isIncome = 0;
            break;
        }
        printf("1 또는 2를 입력하세요.\n");
    }

    (*count)++;
    printf("거래가 추가되었습니다.\n");
}

void saveLedger(const char *filename, const Transaction *transactions, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        return;
    }

    fprintf(file, "date,category,description,amount,isIncome\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%s,%s,%.2f,%d\n",
                transactions[i].date,
                transactions[i].category,
                transactions[i].description,
                transactions[i].amount,
                transactions[i].isIncome);
    }

    fclose(file);
    printf("거래내역이 '%s'에 저장되었습니다.\n", filename);
}

void loadLedger(const char *filename, Transaction *transactions, int *count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("저장된 파일을 찾을 수 없습니다: %s\n", filename);
        return;
    }

    char line[MAX_LINE];
    int loaded = 0;
    fgets(line, sizeof(line), file); // 헤더 건너뛰기

    while (fgets(line, sizeof(line), file) != NULL && loaded < MAX_TRANSACTIONS) {
        trimNewline(line);
        char *token = strtok(line, ",");
        if (token == NULL) continue;
        strncpy(transactions[loaded].date, token, MAX_DATE - 1);
        transactions[loaded].date[MAX_DATE - 1] = '\0';

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(transactions[loaded].category, token, MAX_CATEGORY - 1);
        transactions[loaded].category[MAX_CATEGORY - 1] = '\0';

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(transactions[loaded].description, token, MAX_DESC - 1);
        transactions[loaded].description[MAX_DESC - 1] = '\0';

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        transactions[loaded].amount = atof(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        transactions[loaded].isIncome = atoi(token);

        loaded++;
    }

    fclose(file);
    *count = loaded;
    printf("%d개의 거래를 '%s'에서 불러왔습니다.\n", *count, filename);
}

void printSummary(const Transaction *transactions, int count) {
    double income = 0.0;
    double expense = 0.0;

    for (int i = 0; i < count; i++) {
        if (transactions[i].isIncome) {
            income += transactions[i].amount;
        } else {
            expense += transactions[i].amount;
        }
    }

    printf("\n[요약]\n");
    printf("총 수입: %.2f\n", income);
    printf("총 지출: %.2f\n", expense);
    printf("잔액: %.2f\n", income - expense);
}

int main(void) {
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount = 0;
    int choice;

    printf("가계부를 시작합니다. 저장된 파일이 있으면 자동으로 불러옵니다.\n");
    loadLedger(DATA_FILE, transactions, &transactionCount);

    while (1) {
        printf("\n=== 가계부 메뉴 ===\n");
        printf("1. 거래 추가\n");
        printf("2. 거래 조회\n");
        printf("3. 파일로 저장\n");
        printf("4. 저장된 파일 불러오기\n");
        printf("5. 요약 보기\n");
        printf("6. 종료\n");

        choice = readMenuChoice();
        switch (choice) {
            case 1:
                addTransaction(transactions, &transactionCount);
                break;
            case 2:
                viewTransactions(transactions, transactionCount);
                break;
            case 3:
                saveLedger(DATA_FILE, transactions, transactionCount);
                break;
            case 4:
                loadLedger(DATA_FILE, transactions, &transactionCount);
                break;
            case 5:
                printSummary(transactions, transactionCount);
                break;
            case 6:
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("유효한 메뉴를 선택하세요.\n");
                break;
        }
    }

    return 0;
}
