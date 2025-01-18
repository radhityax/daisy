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

Markdown mdt = { 
    .italic = 0, 
    .bold = 0,
    .quote = 0,
    .headingone = 0,
    .headingtwo = 0, 
    .headingthree = 0,
    .code = 0
};

FILE *fptr, *fpto, *fcss, *fcss_target;
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

void add_css(void) {
    FILE *fpcss = fopen("./media/style.css", "r");
    if (fpcss == NULL) {
        fpcss = fopen("./media/style.css", "w");
        if (fpcss == NULL) {
            perror("galat membuat css");
            exit(1);
        }
        fclose(fpcss);
    } else {
        fclose(fpcss);
    }
}

void copy_css(void) {
    const char *css = "./media/style.css";
    const char *css_target = "./target/style.css";
    int x;

    FILE *fcss = fopen(css, "r");
    if (fcss == NULL) {
        perror("Tidak bisa membuka file masukan CSS");
        return; 
    }

    FILE *fcss_target = fopen(css_target, "w");
    if (fcss_target == NULL) {
        perror("Tidak bisa membuka file keluaran CSS");
        fclose(fcss);
        return;
    }

    while ((x = fgetc(fcss)) != EOF) {
        fputc(x, fcss_target);
    }

    fclose(fcss);
    fclose(fcss_target);
}

void newsetup(void) {
    struct stat st;
    if (stat("./source", &st) == -1) mkdir("./source", 0700);
    if (stat("./target", &st) == -1) mkdir("./target", 0700);
    if (stat("./media", &st) == -1) {
        mkdir("./media", 0700);
        add_css();
    }

    fptr = fopen("./source/index", "r");
    if (fptr == NULL) {
        fptr = fopen("./source/index", "w");
        if (fptr == NULL) {
            perror("galat buat ./source/index (fptr)");
            exit(1);
        } else {
            printf("sukses buat ./source/index\n");
        }
    } else {
        printf("./source/index sudah ada\n");
        fclose(fptr);
    }
}
void checker(void) {
    fptr = fopen("./source/index", "rb+");
    if (fptr == NULL) {
        perror("galat checker buat ./source/index");
        exit(1);
    }
}

void
doitalic(void) {
    if (!mdt.italic) {
        fprintf(fpto, "<i>");
        mdt.italic = 1;
    } else {
        fprintf(fpto, "</i>");
        mdt.italic = 0;
    }
}

void
dobold(void) {
    if (!mdt.bold) {
        fprintf(fpto, "<b>");
        mdt.bold = 1;
        i++;
    } else {
        fprintf(fpto, "</b>");
        mdt.bold = 0;
        i++;
    }
}

void
doblockquote(void) {
    fprintf(fpto, "<blockquote>");
    mdt.quote = 1;
}

void
doimage(void) {
    if (i + 1 < len && txt[i+1] == '[' && (i == 0 || txt[i] != '\\')) {
        int alt_start = i + 2;
        int alt_end = -1;
        int url_start = -1;
        int url_end = -1;
        int j;

        for (j = alt_start; j < len; j++) {
            if (txt[j] == ']' && (j == 0 || txt[j-1] != '\\')) {
                alt_end = j;
                break;
            }
        }

        if (alt_end != -1) {
            for (j = alt_end + 1; j < len; j++) {
                if (txt[j] == '(' && (j == 0 || txt[j-1] != '\\')) {
                    url_start = j + 1;
                    break;
                }
            }

            if (url_start != -1) {
                for (j = url_start; j < len; j++) {
                    if (txt[j] == ')' && (j == 0 || txt[j-1] != '\\')) {
                        url_end = j;
                        break;
                    }
                }
            }
        }

        if (alt_end != -1 && url_end != -1) {
            char alt_text[100], url[100];
            int alt_len = alt_end - alt_start;
            int url_len = url_end - url_start;

            strncpy(alt_text, txt + alt_start, alt_len);
            alt_text[alt_len] = '\0';

            strncpy(url, txt + url_start, url_len);
            url[url_len] = '\0';

            fprintf(fpto, "<img src=\"%s\" alt=\"%s\">", url, alt_text);

            i = url_end;
        } else {
            fprintf(fpto, "!");
        }
    } else {
        fprintf(fpto, "!");
    }
}

void
docode(void) {
    if (!mdt.code) {
        fprintf(fpto, "<code>");
        mdt.code = 1;
    } else {
        fprintf(fpto, "</code>");
        mdt.code = 0;
    }
}

void
dohyperlink(void) {
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

}
void build(void) {
}

void generate(void) {
    fprintf(fpto, "<html>\n<head>\n");
    fprintf(fpto, "<meta charset=\"utf-8\"/>\n");
    fprintf(fpto, "<title>daisy homepage</title>\n");
    fprintf(fpto, "<link rel=\"stylesheet\" href=\"style.css\">\n"); 
    fprintf(fpto, "</head>\n<body>\n");
    
    int judul = 0;
    
    while (fgets(txt, MAX_LENGTH, fptr) != NULL) {
        len = strlen(txt);

        if (len > 0 && txt[len - 1] == '\n') {
            txt[--len] = '\0';
        }

        mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.quote = 0;

        if (len == 0) {
            fprintf(fpto, "\n");
        } else {
            i = 0;

            if (txt[0] == '#') {
                int heading_count = 0;

                while (i < len && txt[i] == '#') { 
                    heading_count++; 
                    i++; 
                }

                if (i < len && txt[i] == ' ') {
                    i++;
                    
                    char *title_start = txt + i;
                    
                    if (heading_count == 1) {
                        judul = 1;
                        fseek(fpto, 0, SEEK_SET);
                        fprintf(fpto, "<html>\n<head>\n<meta charset=\"utf-8\"/>\n"
                                "<title>%s</title>\n<link rel=\"stylesheet\" href=\"style.css\">\n"
                                "</head>\n<body>\n", title_start);
                        fseek(fpto, 0, SEEK_END);
                        fprintf(fpto, "<h1>%s</h1>", title_start);
                    } else {
                        fprintf(fpto, "<h%d>%s</h%d>\n", heading_count, title_start, heading_count);
                    }
                    continue;
                }
            }


            else if (len > 1 && txt[0] == '>' && txt[1] != '\\') {
                doblockquote();
                while (i < len && (txt[i] == '>' || txt[i] == ' ')) i++;
            } else {
                fprintf(fpto, "<p>");
                i = 0;
            }

            for (; i < len; i++) {
                if (txt[i] == '_' && (i == 0 || txt[i - 1] != '\\')) {
                    doitalic();
                } else if (txt[i] == '!' && (i == 0 || txt[i - 1] != '\\')) {
                    doimage();
                } else if (txt[i] == '[' && (i == 0 || txt[i - 1] != '\\')) {
                    dohyperlink();
                } else if (txt[i] == '`' && (i == 0 || txt[i - 1] != '\\')) {
                    docode(); 
                } else if (i + 1 < len && txt[i] == '*' && txt[i + 1] == '*' && (i == 0 || txt[i - 1] != '\\')) {
                    dobold(); 
                } else {
                    fputc(txt[i], fpto);
                }
            }

            if (mdt.italic) fprintf(fpto, "</i>");
            else if (mdt.code) fprintf(fpto, "</code>");
            else if (mdt.bold) fprintf(fpto, "</b>");
            else if (mdt.quote) fprintf(fpto, "</blockquote>\n");
            else if (mdt.headingone) fprintf(fpto, "</h1>\n");
            else if (mdt.headingtwo) fprintf(fpto, "</h2>\n");
            else if (mdt.headingthree) fprintf(fpto, "</h3>\n");
            if (!(mdt.headingone || mdt.headingtwo || mdt.headingthree || mdt.quote)) fprintf(fpto, "</p>\n");

            mdt.italic = mdt.bold = mdt.quote = mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.code = 0;
        }
    }

    fprintf(fpto, "</body>\n</html>");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("penggunaan: %s <command>\natau %s help untuk info lebih lanjut\n", argv[0], argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp("about", argv[i]) == 0) {
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

                        generate();
                        copy_css();

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
        } else if (strcmp("help", argv[i]) == 0) {
            printf("help : help\nnew : new site\nbuild : build all files in source/ to HTML\n");
            return 0;
        } else if (strcmp("new", argv[i]) == 0) {
            newsetup();
            return 0;
        } else {
            printf("salah perintah\n");
            return 0;
        }
    }

    printf("salah perintah\n");
    return 1;
}
