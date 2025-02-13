#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define QUEUE_NAME "\\\\.\\mailslot\\ChessQueue"
#define SEM_WHITE "SemWhite"
#define SEM_BLACK "SemBlack"

typedef struct {
    long type;
    char move[10];
} ChessMove;

void game_manager() {
    HANDLE hQueue = CreateMailslot(QUEUE_NAME, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hQueue == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Ошибка создания очереди! Код ошибки: %lu\n", error);
        return;
    }

    HANDLE semWhite = CreateSemaphore(NULL, 1, 1, SEM_WHITE);
    HANDLE semBlack = CreateSemaphore(NULL, 0, 1, SEM_BLACK);
    if (!semWhite || !semBlack) {
        printf("Ошибка создания семафоров! Код ошибки: %lu\n", GetLastError());
        return;
    }

    ChessMove moves[] = {
        {1, "e2e4"}, {2, "e7e5"}, {1, "g1f3"}, {2, "b8c6"}, {1, "f1c4"}, {2, "g8f6"},
    };

    for (int i = 0; i < sizeof(moves)/sizeof(moves[0]); i++) {
        WaitForSingleObject((moves[i].type == 1) ? semWhite : semBlack, INFINITE);
        DWORD written;
        BOOL success = WriteFile(hQueue, &moves[i], sizeof(ChessMove), &written, NULL);
        if (!success) {
            printf("Ошибка записи в очередь! Код ошибки: %lu\n", GetLastError());
            break;
        }
        printf("Менеджер отправил ход: %s\n", moves[i].move);
    }

    CloseHandle(hQueue);
    CloseHandle(semWhite);
    CloseHandle(semBlack);
}

void player(int type) {
    HANDLE hQueue = CreateFile(QUEUE_NAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hQueue == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Ошибка открытия очереди! Код ошибки: %lu\n", error);
        return;
    }

    HANDLE sem = OpenSemaphore(SYNCHRONIZE, FALSE, (type == 1) ? SEM_WHITE : SEM_BLACK);
    HANDLE semOther = OpenSemaphore(SYNCHRONIZE, FALSE, (type == 1) ? SEM_BLACK : SEM_WHITE);
    if (!sem || !semOther) {
        printf("Ошибка открытия семафоров! Код ошибки: %lu\n", GetLastError());
        return;
    }

    while (1) {
        WaitForSingleObject(sem, INFINITE);
        ChessMove move;
        DWORD read;
        BOOL success = ReadFile(hQueue, &move, sizeof(ChessMove), &read, NULL);
        if (!success || read == 0) break;

        printf("Игрок %d сделал ход: %s\n", type, move.move);
        ReleaseSemaphore(semOther, 1, NULL);
    }

    CloseHandle(hQueue);
    CloseHandle(sem);
    CloseHandle(semOther);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <manager|1|2>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "manager") == 0) {
        game_manager();
    } else if (strcmp(argv[1], "1") == 0) {
        player(1);
    } else if (strcmp(argv[1], "2") == 0) {
        player(2);
    } else {
        printf("Неверный аргумент!\n");
        return 1;
    }

    return 0;
}
