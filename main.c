#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

FILE *fptr;

void checker() {
	fptr = fopen("./source/data.txt", "rb+");

	if (fptr == NULL) {
		perror("Galat checker buat ./source/data.txt");
			exit(1);
		}
	}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Penggunaan: %s <berkas_input>\natau %s -h untuk info lebih lanjut\n", argv[0], argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp("-v", argv[i]) == 0) {
            printf("daisy © radhitya.org\n");
	    return 1;
        }
	else if(strcmp("-h", argv[i]) == 0) {
		printf("ini -h\n");
		return 1;
	}
	else if(strcmp("new", argv[i]) == 0) {	
		struct stat st;
		
		if(stat("./source", &st) == 0 || stat("./target", &st) == 0) {
			printf("./source dan ./target telah dibuat\n");
		}

		if(stat("./source", &st) == -1) {
			mkdir("./source", 0700);
		}

		if(stat("./target", &st) == -1) {
			mkdir("./target", 0700);
		}

		fptr = fopen("./source/data.txt", "rb+");
		if (fptr == NULL) {
			printf("Sukses buat ./source/data.txt\n");
			fptr = fopen("./source/data.txt", "wb");
			if (fptr == NULL) {
				perror("Galat buat ./source/data.txt");
			exit(1);
			}
		}
		return 1;
	}
    }

    char txt[50], of[50];
    int i, len, in_italic = 0;
    const char *file_name = strrchr(argv[1], '/');
    if (file_name == NULL) {
	    file_name = argv[1];
    } else {
	    file_name++;
    }

    snprintf(of, sizeof(of), "target/%s.html", file_name);


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
    
    fprintf(fpto, "<html>\n<body>\n");
    while (fgets(txt, 50, fptr) != NULL) {
	    len = strlen(txt);
	    if (len > 0 && txt[len-1] == '\n') {
		    txt[--len] = '\0';
	    }
	    fprintf(fpto, "<p>");

	    for(i = 0; i < len; i++) {
		    if (txt[i] == '_' && (i == 0 || txt[i-1] != '\\')) {
			    if(!in_italic) {
				    fprintf(fpto, "<i>");
				    in_italic = 1;
			    } else {
				    fprintf(fpto, "</i>");
				    in_italic = 0;
			    }
		    } else {
			    fputc(txt[i], fpto);
		    }
	    }

    if(in_italic) {
	    fprintf(fpto, "</i>");
    };
    fprintf(fpto, "</p>\n");
    in_italic = 0;
    }

    fprintf(fpto, "</body>\n</html>");
    fclose(fptr);
    fclose(fpto);
    return 0;
}

