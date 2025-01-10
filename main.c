#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fptr;

void sf() {
    fptr = fopen("data.txt", "rb+");
    if ( fptr == NULL ) {
        printf("OK! data.txt\n");
        fptr = fopen("data.txt", "wb");
    }
}

/*
void parser(char * s) {
    int n = strlen(s);
    int start = 0, end = 0;
    int i, j;

    for(i = 0; i < n; i++) {
        if(s[i] == '_') {

}
*/

int main(int argc, char * argv[]) {
    sf();

    if(argc == 1) {
        printf("not enough parameter\n");
        return -1;
    }

    char *txt = malloc(1024);

    fptr = fopen(argv[1], "rt");

    FILE *fpto = fopen("data.html", "wb");

    while(fgets(txt, 1024, fptr ) != NULL) {
        for(int i = 0; i < strlen(txt); i++) {
            if(txt[i] == '_') {
            }
        fprintf(fpto, "<p>%s</p>\n", txt);
        }

    fclose(fptr);
    fclose(fpto);
    free(txt);
    return 0;
}
