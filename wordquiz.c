#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

typedef enum {
    C_ZERO,
    C_LIST,
    C_SHOW,
    C_TEST,
    C_EXIT,
} command_t;

char * custom_strndup(const char *s, size_t n) {
    char *result;
    size_t len = strlen(s);
    if (n < len)
        len = n;
    result = (char *)malloc(len + 1);
    if (!result)
        return NULL;
    result[len] = '\0';
    return (char *)memcpy(result, s, len);
}

char * read_a_line(FILE * fp) {
    static char buf[BUFSIZ];
    static int buf_n = 0;
    static int curr = 0;

    if (feof(fp) && curr == buf_n - 1)
        return NULL;

    char * s = NULL;
    size_t s_len = 0;
    do {
        int end = curr;
        while (!(end >= buf_n || !iscntrl(buf[end]))) {
            end++;
        }
        if (curr < end && s != NULL) {
            curr = end;
            break;
        }
        curr = end;
        while (!(end >= buf_n || iscntrl(buf[end]))) {
            end++;
        }
        if (curr < end) {
            if (s == NULL) {
                s = custom_strndup(buf + curr, end - curr);
                s_len = end - curr;
            } else {
                s = realloc(s, s_len + end - curr + 1);
                strncat(s, buf + curr, end - curr);
                s_len = s_len + end - curr;
            }
        }
        if (end < buf_n) {
            curr = end + 1;
            break;
        }
        buf_n = fread(buf, 1, sizeof(buf), fp);
        curr = 0;
    } while (buf_n > 0);
    return s;
}

void print_menu() {
    printf("1. List all wordbooks\n");
    printf("2. Show the words in a wordbook\n");
    printf("3. Test with a wordbook\n");
    printf("4. Exit\n");
}

int get_command() {
    int cmd;
    printf(">");
    scanf("%d", &cmd);
    return cmd;
}

void list_wordbooks() {
    system("cls"); // clear the terminal
    DIR * d = opendir("wordbooks");
    if (!d) {
        printf("Failed to open directory 'wordbooks'.\n");
        return;
    } // edited by lkm 20240519

    printf("\n  ----\n");

    struct dirent * wb;
    while ((wb = readdir(d)) != NULL) {
        if (strcmp(wb->d_name, ".") != 0 && strcmp(wb->d_name, "..") != 0) {
            printf("  %s\n", wb->d_name);
        }
    }
    closedir(d);

    printf("  ----\n");
}

void show_words() {
    char wordbook[128];
    char filepath[256];

    list_wordbooks();

    printf("Type in the name of the wordbook?\n");
    printf(">");
    scanf("%s", wordbook);

    sprintf(filepath, "wordbooks/%s", wordbook);

    FILE * fp = fopen(filepath, "r");
    if (!fp) {
        printf("Failed to open wordbook '%s'.\n", wordbook);
        return;
    } // edited by lkm 20240519

    printf("\n  -----\n");
    char * line;
    while ((line = read_a_line(fp)) != NULL) {
        char * word = strtok(line, "\"");
        strtok(NULL, "\"");
        char * meaning = strtok(NULL, "\"");

        if (word && meaning) {
            printf("  %s : %s\n", word, meaning);
        }

        free(line);
    }
    printf("  -----\n\n");

    fclose(fp);
}

void run_test() {
    char wordbook[128];
    char filepath[256];
    system("cls"); // clear the terminal
    list_wordbooks();
    printf("Type in the name of the wordbook?\n");
    printf(">");
    scanf("%s", wordbook);

    sprintf(filepath, "wordbooks/%s", wordbook);

    FILE * fp = fopen(filepath, "r");
    if (!fp) {
        printf("Failed to open wordbook '%s'.\n", wordbook);
        return;
    } // edited by lkm 20240519

    printf("\n-----\n");

    int n_questions = 0;
    int n_correct = 0;

    char * line;
    while ((line = read_a_line(fp)) != NULL) {
        char * word = strtok(line, "\"");
        strtok(NULL, "\"");
        char * meaning = strtok(NULL, "\"");

        if (word && meaning) {
            printf("Q. %s\n", meaning);
            printf("?  ");

            char answer[128];
            scanf("%s", answer);

            if (strcmp(answer, word) == 0) {
                printf("- correct\n");
                n_correct++;
            } else {
                printf("- wrong: %s\n", word);
            }

            n_questions++;
        }
        free(line);
    }

    printf("(%d/%d)\n", n_correct, n_questions);
    printf("-----\n\n");

    fclose(fp);
}

int main() {
    printf(" *** Word Quiz *** \n\n");

    int cmd;
    do {
        print_menu();
        cmd = get_command();
        switch (cmd) {
            case 1: // C_LIST
                list_wordbooks();
                break;
            case 2: // C_SHOW
                show_words();
                break;
            case 3: // C_TEST
                run_test();
                break;
            case 4: // C_EXIT
                return EXIT_SUCCESS;
            default:
                printf("Invalid command. Please try again.\n");
                break;
        }
    } while (cmd != 4);

    return EXIT_SUCCESS;
}
