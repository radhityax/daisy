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

    char txt[50], of[50];
    
    strcpy(of, argv[1]);
    strcat(of, ".html");

    fptr = fopen(argv[1], "rt");
    if (fptr == NULL) {
        perror("Galat membuka masukan berkas fptr\n");
        return 1;
    }

    FILE *fpto = fopen(of, "wb");
    if (fpto == NULL) {
        perror("Galat membuat keluaran berkas fpto\n");
        fclose(fptr);
        return 1;
    }
    
    fprintf(fpto, "<html>\n");
    while (fgets(txt, 50, fptr) != NULL) {
        fprintf(fpto, "<p>%s</p>\n", txt);
    }
    fprintf(fpto, "</html>");

    fclose(fptr);
    fclose(fpto);
    return 0;
}

