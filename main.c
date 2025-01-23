#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_LENGTH 10000
#define SUBTITLE "daisy"
#define POSTS_PER_PAGE 5
#define MAX_PATH 200

typedef struct {
    char name[MAX_PATH];
} Post;

typedef struct {
    int bold;
    int quote;
    int italic;
    int hyperlink;
    int code;
    int blockcode;
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
int i, len, judul;
char *header, *footer;
char *headertwo;
char *doinsert(const char * nama);
    
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

int compare_posts(const void *a, const void *b) {
    return strcmp(((Post *)a)->name, ((Post*)b)->name);
}

int count_files(const char *path) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;

    if((dir = opendir(path)) == NULL) {
	perror("Galat buka directory opendir(path)");
	return -1;
    }

    while ((ent = readdir(dir)) != NULL) {
	if(ent->d_type == DT_REG) {
	    count++;
	}
    }
    closedir(dir);
    return count;
}

void get_posts(const char *path, Post *posts, int *count) {
    DIR *dir;
    struct dirent *ent;
    int index = 0;

    if((dir = opendir(path)) == NULL) {
	perror("Error opening directory");
	return;
    }

    while((ent = readdir(dir)) != NULL) {
	if(ent->d_type == DT_REG) {
	    strncpy(posts[index].name, ent->d_name, MAX_PATH - 1);
	    posts[index].name[MAX_PATH - 1] = '\0';
	    index++;
	}
    }
    *count = index;
    closedir(dir);

    qsort(posts, *count, sizeof(Post), compare_posts);
    
}

void generate_pagination(Post *posts, int total_posts, int posts_per_page) {
     header = doinsert("./media/header.html");
     int total_pages = (total_posts + posts_per_page - 1) / posts_per_page;

    FILE *archive = fopen("./target/archive.html", "w");
    if (archive == NULL) {
	perror("Galat archive.html");
	return;
    }
    printf("%s", header);
    fprintf(archive, "<html>\n<head>\n<meta charset=\"utf-8\"/>\n");
    fprintf(archive, "<title>Archive - %s</title>", SUBTITLE);
    fprintf(archive, "<link rel=\"stylesheet\" href=\"style.css\">\n</head>\n");
    	 if (header != NULL) {
        fprintf(archive, "%s", header);
    } else {
        fprintf(stderr, "ga bisa buat header wqwqwq");
    }

    fprintf(archive, "<body>");
      
    fprintf(archive,"<h1>Archive</h1><ul>");
	for(int page = 0; page < total_pages; page++) {
	    fprintf(archive, "<li><a href='page%d.html'>Page %d</a></li>", page+1, page+1);
	    
	    char page_name[100];
	    snprintf(page_name, sizeof(page_name), "./target/page%d.html", page+1);
	    FILE *page_file = fopen(page_name, "w");
	    if(page_file == NULL) {
		perror("Gx bisa buat page");
		continue;
	    }

	    fprintf(page_file, "<html>\n<head>\n<meta charset=\"utf-8\"/>\n");
	    fprintf(page_file, "<title>Page %d - %s</title>", page+1, SUBTITLE);
	    fprintf(page_file, "<link rel=\"stylesheet\" href=\"style.css\">\n</head>");
	   
    	 if (header != NULL) {
        fprintf(page_file, "%s", header);
    } else {
        fprintf(stderr, "ga bisa buat header wqwqwq");
    }

	    fprintf(page_file,"<h1>Page %d</h1><ul>", page + 1);
		    
  int start = page * posts_per_page;
        int end = start + posts_per_page;
        if (end > total_posts) end = total_posts;

        for (int i = start; i < end; i++) {
	    char *html_name = gantihuruf(posts[i].name);
            fprintf(page_file, "<li><a href='%s'>%s</a></li>", html_name, posts[i].name);
        }
        fprintf(page_file, "</ul>\n<a href='archive.html'>kembali</a>");
	       footer = doinsert("./media/footer.html");
	 if (footer != NULL) {
        fprintf(page_file, "%s", footer);
    } else {
        fprintf(stderr, "ga bisa buat footer wqwqwq");
    }
	 fprintf(page_file,
		"</body></html>"
	    );
        fclose(page_file);
    }

	fprintf(archive, "</ul>");
    
 if (footer != NULL) {
        fprintf(archive, "%s", footer);
    } else {
        fprintf(stderr, "ga bisa buat footer wqwqwq");
    }
    
    
	fprintf(archive,"</body></html>");
    fclose(archive);
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

char *doinsert(const char * nama) {
    FILE *fheader = fopen(nama, "r");
    if(fheader == NULL) {
	fheader = fopen(nama, "w");
    if (fheader == NULL) {
        perror("galat membuka header");
        return NULL;
    }
    }

    static char buffer[MAX_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, fheader);
    buffer[bytes_read] = '\0';

    fclose(fheader);
    if (bytes_read == 0) {
        fprintf(stderr, "Berkas kosong atau ada kesalahan membaca %s\n", nama);
        return NULL;
    }

    return buffer;
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
    DIR *dir;

    if ((dir = opendir("./media")) != NULL) {
        printf("Direktori media ditemukan.\n");
        closedir(dir);
    } else {
        if ((dir = opendir("./source")) != NULL) {
            printf("Direktori source ditemukan.\n");
            closedir(dir);
        } else {
            if ((dir = opendir("./target")) != NULL) {
                printf("Direktori target ditemukan.\n");
                closedir(dir);
            } else {
                printf("Tidak ada direktori yang ditemukan: media, source, atau target.\n");
            }
        }
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
    header = doinsert("./media/header.html");

    fprintf(fpto, "<html>\n<head>\n<meta charset=\"utf-8\"/>\n<title>homepage - %s</title><link rel=\"stylesheet\" href=\"style.css\">\n</head>\n<body>", SUBTITLE);

    if (header == NULL) {
        printf("header NULL\n");
    } else {
        fprintf(fpto, "%s\n", header);
    }
    
    while (fgets(txt, MAX_LENGTH, fptr) != NULL) {
        len = strlen(txt);

        if (len > 0 && txt[len - 1] == '\n') {
            txt[--len] = '\0';
        }

        mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.quote = mdt.blockcode = 0;

        if (len == 0) {
            fprintf(fpto, "\n");
        } else {
            i = 0;
            judul = 0;

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
                        if (!judul) {               
                            judul = 1;
                            fseek(fpto, 0, SEEK_SET);
                            fprintf(fpto, "<html>\n<head>\n<meta charset=\"utf-8\"/>\n"
                                    "<title>%s - %s</title>\n<link rel=\"stylesheet\" href=\"style.css\">\n"
                                    "</head><body>", title_start, SUBTITLE);
                            fprintf(fpto, "%s\n", header);
                        }
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
            }
            else if (len > 2 && txt[0] == '`' && txt[1] == '`' && txt[2] == '`' && (i == 0 || txt[i-1] != '\\')) {
                if (mdt.blockcode == 0) {
                    fprintf(fpto, "<pre><code>");
                    mdt.blockcode = 1;
                    i += 3;
                } else {
                    fprintf(fpto, "</code></pre>");
                    mdt.blockcode = 0;
                    i += 3;
                }
                continue;
            } else if (mdt.blockcode == 1) {
                fprintf(fpto, "%s\n", txt);
                continue;
            } else {
                fprintf(fpto, "<p>");
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
            if (mdt.code) fprintf(fpto, "</code>");
            if (mdt.bold) fprintf(fpto, "</b>");
            if (mdt.quote) fprintf(fpto, "</blockquote>\n");
            if (mdt.headingone) fprintf(fpto, "</h1>\n");
            if (mdt.headingtwo) fprintf(fpto, "</h2>\n");
            if (mdt.headingthree) fprintf(fpto, "</h3>\n");
            if (!(mdt.headingone || mdt.headingtwo || mdt.headingthree || mdt.quote || mdt.blockcode)) fprintf(fpto, "</p>\n");

            mdt.italic = mdt.bold = mdt.quote = mdt.headingone = mdt.headingtwo = mdt.headingthree = mdt.code = 0;
        }
    }

    footer = doinsert("./media/footer.html");
    
    if (footer == NULL) {
        printf("gak ada footer bro :(\n");
    } else {
        fprintf(fpto, "%s", footer);
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
           
		    int count = count_files("./source");
		    if(count == -1) {
			perror("ga bisa ngitung hehe");
			return 1;
		    }

		    Post *posts = malloc(count * sizeof(Post));
		    if (posts == NULL) {
			perror("Memory allocation failed");
			return 1;
		    }
		            get_posts("./source", posts, &count);
        generate_pagination(posts, count, POSTS_PER_PAGE);
        free(posts);

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
                    }
                }
	        printf("dokumen telah berhasil dibuat\n");
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
