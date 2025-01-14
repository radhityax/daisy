#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LENGTH 1000

typedef struct markdown {
	int bold;
	int quote;
	int italic;
	int headingone;
	int headingtwo;
	int headingthree;
} markdown;

markdown mdt;

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
        printf("penggunaan: %s <berkas_input>\natau %s -h untuk info lebih lanjut\n", argv[0], argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp("-v", argv[i]) == 0) {
            printf("daisy © radhitya.org\n");
	    return 0;
        }
	else if(strcmp("-h", argv[i]) == 0) {
		printf("ini -h\n");
		return 0;
	}
	else if(strcmp("new", argv[i]) == 0) {	
		struct stat st;	
		if(stat("./source", &st) == 0) {
			if(stat("./target", &st) == 0) {
			printf("./source dan ./target telah dibuat\n");
		} else {
			mkdir("./target", 0700);
		}
		} else {
			mkdir("./source", 0700);
			mkdir("./target", 0700);
		}

		fptr = fopen("./source/data.txt", "rb+");
		if (fptr == NULL) {
			fptr = fopen("./source/data.txt", "wb");
			if (fptr == NULL) {
				perror("galat buat ./source/data.txt (fptr)");
			exit(1);
			} else {
				printf("sukses buat ./source/data.txt\n");
			}
		} else {
			printf("./source/data.txt sudah ada\n");
			fclose(fptr);
		}
		return 0;
	}
    }

    char txt[MAX_LENGTH], of[MAX_LENGTH];

    int i, len;
    mdt.italic = 0, mdt.bold = 0, mdt.quote = 0;

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
    while (fgets(txt, MAX_LENGTH, fptr) != NULL) {
	    len = strlen(txt);

	    if (len > 0 && txt[len-1] == '\n') {
		    txt[--len] = '\0';
	    }

	    mdt.headingone = 0;
	    mdt.quote = 0;
	    if(len > 0 && txt[0] == '#' && )
	    fprintf(fpto, "<p>");

	    for(i = 0; i < len; i++) {
		    
		    if (txt[i] == '_' && (i == 0 || txt[i-1] != '\\')) {
			    if(!mdt.italic) {
				    fprintf(fpto, "<i>");
				    mdt.italic = 1;
			    } else {
				    fprintf(fpto, "</i>");
				    mdt.italic = 0;
			    }
		    }

		    else if (i + 1 < len && txt[i] == '*' && txt[i+1] == '*' && (i == 0 || txt[i-1] != '\\')) {
			    if(mdt.bold) {
				    fprintf(fpto, "<b>");
				    mdt.bold = 1;
				    i++;
			    } else {
				    fprintf(fpto, "</b>");
				    mdt.bold = 0;
				    i++;
			    }
		    }

		    else if (txt[i] == '>' && (i == 0 || txt[i-1] != '\\')) {
			    if(!mdt.quote) {
				    fprintf(fpto, "<blockquote>");
				    mdt.quote = 1;
			    } else {
				    fprintf(fpto, "</blockquote>");
				    mdt.quote = 0;
			    }
		    }

		    else {
			    fputc(txt[i], fpto);
		    }
	    }

    if(mdt.italic) {
	    fprintf(fpto, "</i>");
    };
    if(mdt.bold) {
	    fprintf(fpto, "</b>");
    }
    if(mdt.quote) {
	    fprintf(fpto, "</blockquote>");
    }
    if(mdt.headingone) {
	    fprintf(fpto, "</h1>");
    }
    fprintf(fpto, "</p>\n");
    mdt.italic = mdt.bold = mdt.quote = mdt.headingone = 0;
    }

    fprintf(fpto, "</body>\n</html>");
    fclose(fptr);
    fclose(fpto);
    return 0;
}

