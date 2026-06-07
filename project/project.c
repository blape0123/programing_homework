#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    float balance = 0.0;
    int choice;
    float amount;
    char description[100];
    
    char records[100][200];
    int recordCount = 0;
    
    printf("=== 용돈 관리 프로그램 ===\n");
    
    while (1) {
        printf("초기 잔액을 입력하세요: ");
        scanf("%f", &balance);
        if (balance < 0) {
            printf("잘못된 입력입니다. 다시 입력해주십시오.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("\n현재 잔액: %.2f원\n", balance);
        printf("1. 수입 입력\n");
        printf("2. 지출 입력\n");
        printf("3. 내역 보기\n");
        printf("4. 종료\n");
        printf("선택: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            printf("수입 금액을 입력하세요: ");
            scanf("%f", &amount);
            if (amount < 0) {
                printf("잘못된 입력입니다. 다시 입력해주십시오.\n");
                continue;
            }
            printf("수입 활동을 입력하세요: ");
            scanf(" %s", description);
            balance += amount;
            if (recordCount < 100) {
                snprintf(records[recordCount], sizeof(records[recordCount]), "수입: +%.2f원, 활동: %s, 잔액: %.2f원", amount, description, balance);
                recordCount++;
            }
            printf("수입 %.2f원이 추가되었습니다.\n", amount);
        } else if (choice == 2) {
            while (1) {
                printf("지출 금액을 입력하세요: ");
                scanf("%f", &amount);
                if (amount < 0) {
                    printf("잘못된 입력입니다. 다시 입력해주십시오.\n");
                    continue;
                }
                printf("지출 활동을 입력하세요: ");
                scanf(" %s", description);
                balance -= amount;
                if (recordCount < 100) {
                    snprintf(records[recordCount], sizeof(records[recordCount]), "지출: -%.2f원, 활동: %s, 잔액: %.2f원", amount, description, balance);
                    recordCount++;
                }
                printf("지출 %.2f원이 차감되었습니다.\n", amount);
                break;
            }
        } else if (choice == 3) {
            printf("\n=== 거래 내역 ===\n");
            if (recordCount == 0) {
                printf("등록된 내역이 없습니다.\n");
            } else {
                for (int i = 0; i < recordCount; i++) {
                    printf("%d. %s\n", i + 1, records[i]);
                }
            }
        } else if (choice == 4) {
            break;
        } else {
            printf("잘못된 입력입니다. 다시 입력해주십시오.\n");
        }
    }

    printf("\n프로그램을 종료합니다. 최종 잔액: %.2f원\n", balance);
    return 0;
}
