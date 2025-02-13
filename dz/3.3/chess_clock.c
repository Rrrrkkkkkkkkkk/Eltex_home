#include <windows.h>
#include <stdio.h>

#define TIME_LIMIT 30  
#define SHARED_MEMORY_NAME "Global\\ChessClockSharedMemory"
#define MUTEX_NAME "Global\\ChessClockMutex"

typedef struct {
    ULONGLONG player1_time;
    ULONGLONG player2_time;
    ULONGLONG last_switch;
    int last_player; // 1 - белые, 2 - черные
} SharedData;

void player_turn(int player) {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME);
    if (!hMapFile) {
        printf("Ошибка: Не удалось открыть разделяемую память.\n");
        return;
    }

    SharedData *shm_data = (SharedData *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
    if (!shm_data) {
        printf("Ошибка: Не удалось отобразить память.\n");
        CloseHandle(hMapFile);
        return;
    }

    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
    if (!hMutex) {
        printf("Ошибка: Не удалось открыть мьютекс.\n");
        UnmapViewOfFile(shm_data);
        CloseHandle(hMapFile);
        return;
    }

    WaitForSingleObject(hMutex, INFINITE); // Захватываем мьютекс

    if (shm_data->last_player == player) {
        printf("Ошибка: нельзя ходить два раза подряд\n");
        ReleaseMutex(hMutex);
        UnmapViewOfFile(shm_data);
        CloseHandle(hMapFile);
        return;
    }

    ULONGLONG current_time = GetTickCount64();
    ULONGLONG elapsed = (current_time - shm_data->last_switch) / 1000; 

    if (shm_data->last_player == 1) {
        shm_data->player1_time += elapsed;
    } else if (shm_data->last_player == 2) {
        shm_data->player2_time += elapsed;
    }

    if (shm_data->player1_time > TIME_LIMIT) {
        printf("Техническое поражение белых\n");
        ReleaseMutex(hMutex);
        UnmapViewOfFile(shm_data);
        CloseHandle(hMapFile);
        return;
    }
    if (shm_data->player2_time > TIME_LIMIT) {
        printf("Техническое поражение черных\n");
        ReleaseMutex(hMutex);
        UnmapViewOfFile(shm_data);
        CloseHandle(hMapFile);
        return;
    }

    shm_data->last_switch = current_time;
    shm_data->last_player = player;

    printf("Игрок %d сделал ход. Время: Белые - %llu сек, Черные - %llu сек\n",
           player, shm_data->player1_time, shm_data->player2_time);

    ReleaseMutex(hMutex);
    UnmapViewOfFile(shm_data);
    CloseHandle(hMapFile);
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (argv[1][0] != '1' && argv[1][0] != '2')) {
        printf("Использование: %s <1|2> (где 1 - белые, 2 - черные)\n", argv[0]);
        return 1;
    }

    int player = argv[1][0] - '0';

    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), SHARED_MEMORY_NAME);
    SharedData *shm_data = (SharedData *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));

    if (!shm_data) {
        printf("Ошибка: Не удалось отобразить память.\n");
        CloseHandle(hMapFile);
        return 1;
    }

    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        shm_data->player1_time = 0;
        shm_data->player2_time = 0;
        shm_data->last_player = 2; // Черные начинают первыми
        shm_data->last_switch = GetTickCount64();
    }

    player_turn(player);

    UnmapViewOfFile(shm_data);
    CloseHandle(hMapFile);
    CloseHandle(hMutex);

    return 0;
}
