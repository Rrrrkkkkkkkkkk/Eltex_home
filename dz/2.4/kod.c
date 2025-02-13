#include <stdio.h>
#include <stdlib.h>
#include <stdalign.h>
#include <stdint.h>

struct Person {
    char Name[30];
    char Surname[30];
    char University[20]; 
    int Age;
    float Height; // Рост
    float GPA;
};

#pragma pack(push, 1)
struct CompressedPerson {
    char Name[30];
    char Surname[30];
    uint8_t Age;      
    float Height;     
    float GPA;        
} ;
#pragma pack(pop)

int main() {
    printf("Size of Person: %lu bytes\n", sizeof(struct Person));
    printf("Size of CompressedPerson: %lu bytes\n", sizeof(struct CompressedPerson));

    printf("Alignof Person: %lu\n", alignof(struct Person));
    printf("Alignof CompressedPerson: %lu\n", alignof(struct CompressedPerson));

    return 0;
}
