#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fptr;

void sf() {
    fptr = fopen("data.txt", "rb+");
    if (fptr == NULL) {
        printf("Sukses buat data.txt\n");
        fptr = fopen("data.txt", "wb");
        if (fptr == NULL) {
            perror("Galat buat data.txt");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Penggunaan: %s <berkas_input>\n", argv[0]);
        return 1;
    }

    sf();

    for (int i = 1; i < argc; i++) {
        if (strcmp("-v", argv[i]) == 0) {
            printf("ini -v\n");
        }
    }

    char *txt = malloc(1024);
    if (txt == NULL) {
        perror("Galat alokasi memori");
        return 1;
    }

    fptr = fopen(argv[1], "rt");
    if (fptr == NULL) {
        perror("Galat membuka masukan berkas");
        free(txt);
        return 1;
    }

    FILE *fpto = fopen("data.html", "wb");
    if (fpto == NULL) {
        perror("Galat membuat keluaran berkas");
        fclose(fptr);
        free(txt);
        return 1;
    }

    while (fgets(txt, 1024, fptr) != NULL) {
        fprintf(fpto, "<p>%s</p>\n", txt);
    }

    fclose(fptr);
    fclose(fpto);
    free(txt);
    return 0;
}

