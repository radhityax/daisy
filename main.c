/* daisy
 * alif@radhitya.org
 *
 * thank you stackoverflow, geeksforgeeks, grok
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_LENGTH 1000

typedef struct {
    int bold;
    int quote;
    int italic;
    int hyperlink;
    int code;
    int headingone;
    int headingtwo;
    int headingthree;
} Markdown;

Markdown mdt;

FILE *fptr, *fpto;
char txt[MAX_LENGTH], of[MAX_LENGTH];
int i, len;

char *gantihuruf(const char *kata) {
    static char output[100];
    char *dot = strrchr(kata, '.');
    
    if (dot == NULL || dot == kata) {
        snprintf(output, sizeof(output), "%s.html", kata);
    } else {
        int len = dot - kata;
        strncpy(output, kata, len);
        output[len] = '\0';
        strcat(output, ".html");
    }
    
    return output;
}

void checker(void) {
    fptr = fopen("./source/data.txt", "rb+");
    if (fptr == NULL) {
        perror("galat checker buat ./source/data.txt");
        exit(1);
    }
}

void build(void) {
    mdt.italic = mdt.bold = mdt.quote = mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.code = 0;

    fprintf(fpto, "<html>\n<body>\n<head>\n<meta charset=\"utf-8\"/>\n<title>daisy homepage</title>\n</head>\n");
    fprintf(fpto, "<style>body{background: #4caf50}</style>\n</head>");

    while (fgets(txt, MAX_LENGTH, fptr) != NULL) {
        len = strlen(txt);

        if (len > 0 && txt[len-1] == '\n') {
            txt[--len] = '\0';
        }

        mdt.headingone = mdt.headingtwo = mdt.quote = 0;
        if (len == 0) {
            fprintf(fpto, "\n");
        } else {
            i = 0;

            if (len > 1 && txt[0] == '#' && txt[1] != '\\') {
                if (len > 2 && txt[1] == '#' && txt[2] != '\\') {
                    if (len > 3 && txt[2] == '#' && txt[3] != '\\') {
                        fprintf(fpto, "<h3>");
                        mdt.headingthree = 1;
                        while (i < len && (txt[i] == '#' || txt[i] == ' ')) i++;
                    } else {
                        fprintf(fpto, "<h2>");
                        mdt.headingtwo = 1;
                        while (i < len && (txt[i] == '#' || txt[i] == ' ')) i++;
                    }
                } else {
                    fprintf(fpto, "<h1>");
                    mdt.headingone = 1;
                    while (i < len && (txt[i] == '#' || txt[i] == ' ')) i++;
                }
            } else if (len > 1 && txt[0] == '>' && txt[1] != '\\') {
                fprintf(fpto, "<blockquote>");
                mdt.quote = 1;
                while (i < len && (txt[i] == '>' || txt[i] == ' ')) i++;
            } else {
                fprintf(fpto, "<p>");
            }

            for (; i < len; i++) {
                if (txt[i] == '_' && (i == 0 || txt[i-1] != '\\')) {
                    if (!mdt.italic) {
                        fprintf(fpto, "<i>");
                        mdt.italic = 1;
                    } else {
                        fprintf(fpto, "</i>");
                        mdt.italic = 0;
                    }
                } else if (txt[i] == '[' && (i == 0 || txt[i-1] != '\\')) {
                    int start_linkname = i + 1;
                    int end_linkname = -1;
                    int start_linkurl = -1;
                    int end_linkurl = -1;
                    int j;

                    for (j = start_linkname; j < len; j++) {
                        if (txt[j] == ']' && (j == 0 || txt[j-1] != '\\')) {
                            end_linkname = j;
                            break;
                        }
                    }

                    if (end_linkname != -1) {
                        for (j = end_linkname + 1; j < len; j++) {
                            if (txt[j] == '(' && (j == 0 || txt[j-1] != '\\')) {
                                start_linkurl = j + 1;
                                break;
                            }
                        }

                        if (start_linkurl != -1) {
                            for (j = start_linkurl; j < len; j++) {
                                if (txt[j] == ')' && (j == 0 || txt[j-1] != '\\')) {
                                    end_linkurl = j;
                                    break;
                                }
                            }
                        }
                    }

                    if (end_linkname != -1 && end_linkurl != -1) {
                        char linkname[100], linkurl[100];
                        int linkname_len = end_linkname - start_linkname;
                        int linkurl_len = end_linkurl - start_linkurl;

                        strncpy(linkname, txt + start_linkname, linkname_len);
                        linkname[linkname_len] = '\0';

                        strncpy(linkurl, txt + start_linkurl, linkurl_len);
                        linkurl[linkurl_len] = '\0';

                        fprintf(fpto, "<a href=\"%s\">%s</a>", linkurl, linkname);

                        i = end_linkurl;
                    } else {
                        fprintf(fpto, "[");
                    }
                } else if (txt[i] == '`' && (i == 0 || txt[i-1] != '\\')) {
                    if (!mdt.code) {
                        fprintf(fpto, "<code>");
                        mdt.code = 1;
                    } else {
                        fprintf(fpto, "</code>");
                        mdt.code = 0;
                    }
                } else if (i + 1 < len && txt[i] == '*' && txt[i+1] == '*' && (i == 0 || txt[i-1] != '\\')) {
                    if (!mdt.bold) {
                        fprintf(fpto, "<b>");
                        mdt.bold = 1;
                        i++;
                    } else {
                        fprintf(fpto, "</b>");
                        mdt.bold = 0;
                        i++;
                    }
                } else {
                    fputc(txt[i], fpto);
                }
            }

            if (mdt.italic) fprintf(fpto, "</i>");
	    if (mdt.code) fprintf(fpto, "</code>");
	    if (mdt.bold) fprintf(fpto, "</b>");
	    if (mdt.quote) fprintf(fpto, "</blockquote>");
	    else if (mdt.headingone) fprintf(fpto, "</h1>\n");
	    else if (mdt.headingtwo) fprintf(fpto, "</h2>\n");
	    else if (mdt.headingthree) fprintf(fpto, "</h3>\n");
	    if (!(mdt.headingone || mdt.headingtwo || mdt.headingthree || mdt.quote)) fprintf(fpto, "</p>\n");
        }
	    mdt.italic = mdt.bold = mdt.quote = mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.code = 0;

    }

    fprintf(fpto, "</body>\n</html>");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("penggunaan: %s <command> [berkas_input]\natau %s -h untuk info lebih lanjut\n", argv[0], argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp("-v", argv[i]) == 0) {
            printf("daisy © radhitya.org\n");
            return 0;
        } else if (strcmp("build", argv[i]) == 0) {
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir("./source")) != NULL) {
                while ((ent = readdir(dir)) != NULL) {
		/* d_type dari dirent.h
		 * DT_REG == file reguler (Directory Type Regular file)
		 */
                    if (ent->d_type == DT_REG) { 
                        char path[MAX_LENGTH];
                        snprintf(path, sizeof(path), "./source/%s", ent->d_name);
                        fptr = fopen(path, "rt");
                        if (fptr == NULL) {
                            perror("galat membuka berkas");
                            continue;
                        }

                        char *new_name = gantihuruf(ent->d_name);
                        snprintf(of, sizeof(of), "./target/%s", new_name);
                        
                        fpto = fopen(of, "w");
                        if (fpto == NULL) {
                            perror("galat membuat berkas keluaran");
                            fclose(fptr);
                            continue;
                        }

                        build();  

                        fclose(fptr);
                        fclose(fpto);
                        printf("dokumen %s telah berhasil dibuat\n", of);
                    }
                }
                closedir(dir);
            } else {
                perror("galat membuka direktori ./source");
                return 1;
            }
            return 0;
        } else if (strcmp("-h", argv[i]) == 0) {
            printf("-h : help\nnew : new site\nbuild : build all files in source/ to HTML\n");
            return 0;
        } else if (strcmp("new", argv[i]) == 0) {
            struct stat st;
            if (stat("./source", &st) == -1) mkdir("./source", 0700);
            if (stat("./target", &st) == -1) mkdir("./target", 0700);
            if (stat("./media", &st) == -1) mkdir("./media", 0700);

            fptr = fopen("./source/data.txt", "r");
            if (fptr == NULL) {
                fptr = fopen("./source/data.txt", "w");
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
        } else {
            printf("salah perintah\n");
            return 0;
        }
    }

    printf("salah perintah\n");
    return 1;
}
