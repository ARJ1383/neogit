#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#define _XOPEN_SOURCE

char main_dir[1000];
int cnt;
char branch_name[100];
char author_name[200];
char author_email[200];
int init(int argc, char *const argv[]);
int create_configs();
int add(int argc, char *const argv[]);
void add_file(const char *filepath, char *name);
int stage_check(char *path, char *name);
int compareFiles(const char *file1, const char *file2);
void reset(int argc, char *const argv[]);
void remove_from_staging(char *path, char *name);
int stage_khali();
int get_commit_id();
int compare_commit_staging(int id);
void new_commit(int id, char *message);
int compareFolders(char *path1, char *path2);
void edit_config(int id);
void number_of_files_in_folder(char *path);
void made_log_file(int id, char *massage, int file_numbers);
void log1(int argc, char *argv[]);
void print_all_log();
void print_branch_log(const char *branch);
void print_n_log(int n);
void print_name_log(const char *author);
void print_date_log(const char *date1, int sb);
void print_massage_log(const char *massage);
int parseDate(const char *dateStr, struct tm *tmStruct);
void edit_branch_file(int id);
void branch(int argc, char *const argv[]);
void change_branch(char *branch_name);
void checkout(int argc, char *const argv[]);
void status(int argc, char *const argv[]);
void head_checkout();
int head_id();
int get_current_id();
void global_alias(char *alias_name, char *command);
void local_alias(char *alias_name, char *command);
int compareFolders2(char *path, char *fullpath);
void Print_D_Files(char *directory);
void set1(int argc, char *const argv[]);
void remove1(int argc, char *const argv[]);
void replace1(int argc, char *const argv[]);
int track_check(char *name);
void headn_checkout(char *arg);
int headn_id(int n);
int isNumeric(const char *str);
int adad_branch(int adad);
void checkout_n(int id);
void tag1(int argc, char *const argv[]);
void merge(int argc, char *const argv[]);
void revert(int argc, char *const argv[]);
int get_last_stash(){
    char cwd[1000];
    getcwd(cwd,sizeof(cwd));
    int x=-1;
    int y;
    chdir(main_dir);
    chdir(".neogit/stash");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir))!=NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            sscanf(entry->d_name,"%d",&y);
            if(y>x){
                x=y;
            }
        }
    }
    chdir(cwd);
    return x;
}
int get_last_merge(){
    int x=0;
    char cwd[1000];
    getcwd(cwd,sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit/merge");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry = readdir(dir))!=NULL){
        if(entry->d_type == DT_REG){
            sscanf(entry->d_name,"%d",&x);
        }
    }
    chdir(cwd);
    return x;
}
void get_author()
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit");
    FILE *config = fopen("config", "r");
    fscanf(config, "username: %s\n", author_name);
    fscanf(config, "email: %s\n", author_email);
    fclose(config);
    chdir(cwd);
}
void global_alias(char alias_name[], char *command)
{
    chdir("/mnt/d/global/alias");
    char name[50];
    for (int i = 0; i < strlen(alias_name) - 5; i++)
    {
        name[i] = alias_name[i + 6];
    }
    FILE *file = fopen(name, "w");
    fputs(command, file);
}
void local_alias(char *alias_name, char *command)
{
    chdir(main_dir);
    chdir(".neogit/alias");
    char name[50];
    for (int i = 0; i < strlen(alias_name) - 5; i++)
    {
        name[i] = alias_name[i + 6];
    }
    FILE *file = fopen(name, "w");
    fputs(command, file);
}
int get_main_dir()
{
    char tmp_cwd[1024];
    struct dirent *entry;
    do
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            perror("Error opening current directory");
            return 0;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
            {
                getcwd(main_dir, sizeof(main_dir));
                return 1;
            }
        }
        closedir(dir);

        // update current working directory
        getcwd(tmp_cwd, sizeof(tmp_cwd));

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0)
        {
            chdir("..");
        }

    } while (strcmp(tmp_cwd, "/") != 0);
    return 0;
}
void update_config()
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char username[1024];
    char email[1024];
    FILE *global = fopen("/mnt/d/global/global_config", "r");
    if (global == NULL)
        return;
    fscanf(global, "%s\n", username);
    fscanf(global, "%s\n", email);
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "username: %s\n", username);
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "email: %s\n", email);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fclose(config_1);
    fclose(config_2);
    remove("config");
    rename("config2", "config");
    chdir(cwd);
}
void get_branch_name()
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit");
    FILE *config = fopen("config", "r");
    char chert[200];
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    fscanf(config, "branch: %s\n", branch_name);
    fclose(config);
    chdir(cwd);
}
void change_global_username(char *name)
{
    FILE *global = fopen("/mnt/d/global/global_config", "w");
    fprintf(global, "%s\n", name);
    fclose(global);
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);
    FILE *file = fopen("/mnt/d/global/time", "w");
    fprintf(file, "%s", time_str);
    fclose(file);
}
void change_global_email(char *name)
{
    FILE *global = fopen("/mnt/d/global/global_config", "a");
    fprintf(global, "%s\n", name);
    fclose(global);
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);
    FILE *file = fopen("/mnt/d/global/time", "w");
    fprintf(file, "%s", time_str);
    fclose(file);
}
void change_username(char *name)
{
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "username: %s\n", name);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    remove("config");
    rename("config2", "config");
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);
    FILE *file = fopen("time", "w");
    fprintf(file, "%s", time_str);
    fclose(file);
}
void change_email(char *name)
{
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "email: %s\n", name);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    remove("config");
    rename("config2", "config");
    time_t current_time = time(NULL);
    char *time_str = ctime(&current_time);
    FILE *file = fopen("time", "w");
    fprintf(file, "%s", time_str);
    fclose(file);
}
int init(int argc, char *const argv[])
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;

    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do
    {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
            {
                exists = true;
            }
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
            return 1;

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0)
        {
            if (chdir("..") != 0)
                return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);

    // return to the initial cwd
    if (chdir(cwd) != 0)
        return 1;

    if (!exists)
    {
        if (mkdir(".neogit", 0777) != 0)
            return 1;
        return create_configs();
    }
    else
    {
        perror("neogit repository has already initialized");
    }
    return 0;
}
int create_configs()
{
    get_main_dir();
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    char username[1024];
    char email[1024];
    FILE *global = fopen("/mnt/d/global/global_config", "r");
    if (global == NULL)
    {
        return 1;
    }
    fscanf(global, "%s", username);
    fscanf(global, "%s", email);
    if (chdir(cwd) != 0)
        return 1;
    FILE *file = fopen(".neogit/config", "w");
    if (file == NULL)
        return 1;

    fprintf(file, "username: %s\n", username);
    fprintf(file, "email: %s\n", email);
    fprintf(file, "last_commit_ID: %d\n", 0);
    fprintf(file, "branch: %s\n", "master");
    fprintf(file, "current_commit_ID: %d\n", 0);
    fclose(file);

    if (mkdir(".neogit/commits", 0777) != 0)
        return 1;
    if (mkdir(".neogit/staging", 0777) != 0)
        return 1;
    if (mkdir(".neogit/tracks", 0777) != 0)
        return 1;
    if (mkdir(".neogit/stash", 0777) != 0)
        return 1;
    if (mkdir(".neogit/log", 0777) != 0)
        return 1;
    if (mkdir(".neogit/branches", 0777) != 0)
        return 1;
    if (mkdir(".neogit/alias", 0777) != 0)
        return 1;
    if (mkdir(".neogit/shortcuts", 0777) != 0)
        return 1;
    if (mkdir(".neogit/tags", 0777) != 0)
        return 1;
    if (mkdir(".neogit/merge", 0777) != 0)
        return 1;
    chdir(".neogit/branches");
    FILE *master = fopen("master", "w");
    fclose(master);
    chdir("/mnt/d/global/alias");
    char alias[1000];
    getcwd(alias, sizeof(alias));
    chdir(main_dir);
    chdir(".neogit/alias");
    char command[1000];
    snprintf(command, sizeof(command), "cp -r %s/* %s", alias, ".");
    system(command);
    printf("Repository installed\n");
    return 0;
}
int head_check()
{
    int id1 = get_current_id();
    int id2 = head_id();
    if (id1 == id2)
    {
        return 1;
    }
    if (id1 == 0)
    {
        return 1;
    }
    return 0;
}
int check_for_delete_file(char *name)
{
    int id = get_current_id();
    char current_id[5];
    sprintf(current_id, "%d", id);
    chdir(main_dir);
    chdir(".neogit/commits");
    chdir(current_id);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}
int add(int argc, char *const argv[])
{
    if (argc < 3)
    {
        perror("please specify a file");
        return 1;
    }
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        perror("Error opening current directory");
        return 1;
    }
    if (strcmp("-f", argv[2]) == 0)
    {
        char adress[1000];
        getcwd(adress, sizeof(adress));
        char *prime_argv[3];
        for (int i = 3; i < argc; i++)
        {
            chdir(adress);
            prime_argv[2] = argv[i];
            add(3, prime_argv);
        }
    }
    else if (strcmp("-n", argv[2]) == 0)
    {
        char here[100];
        getcwd(here, sizeof(here));
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
            {
                chdir(here);
                int x = 0;
                char fullpath[100];
                realpath(entry->d_name, fullpath);
                x = stage_check(fullpath, entry->d_name);
                if (x == 0)
                {
                    printf("%s Stage\n", entry->d_name);
                }
                else if (x == 1 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
                {
                    printf("%s Unstage\n", entry->d_name);
                }
            }
        }
    }
    else
    {
        char current[1000];
        getcwd(current, sizeof(current));
        for (int i = 2; i < argc; i++)
        {
            chdir(current);
            int flag_exist = 0;
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, argv[i]) == 0)
                {
                    if (entry->d_type == DT_DIR)
                    {
                        chdir(argv[i]);
                        char dir_dir[1000];
                        DIR *dir1 = opendir(".");
                        getcwd(dir_dir, sizeof(dir_dir));
                        struct dirent *dir_entry;
                        while ((dir_entry = readdir(dir1)) != NULL)
                        {
                            char fullpath[100];
                            chdir(dir_dir);
                            if (dir_entry->d_type == DT_REG)
                            {
                                flag_exist = 1;
                                realpath(dir_entry->d_name, fullpath);
                                add_file(fullpath, dir_entry->d_name);
                            }
                        }
                        closedir(dir1);
                    }
                    else if (entry->d_type == DT_REG)
                    {
                        flag_exist = 1;
                        add_file(realpath(argv[i], NULL), argv[2]);
                    }
                }
            }
            if (flag_exist == 0)
            {
                int delete = check_for_delete_file(argv[i]);
                if (delete == 0)
                {
                    printf("%s does not exist\n", argv[i]);
                }
                if (delete == 1)
                {
                    chdir(main_dir);
                    chdir(".neogit/staging");
                    FILE *file = fopen(argv[i], "w");
                    fprintf(file, "dddddd1\n");
                    fclose(file);
                }
            }
        }
    }
    closedir(dir);
    return 0;
}
void createDirectoriesAndFile(const char *relativePath, char *name)
{
    char *directoryPath = strdup(relativePath);

    char *token = strtok(directoryPath, "/");
    while (token != NULL && strcmp(token, name) != 0)
    {
        mkdir(token, 0777);
        chdir(token);
        token = strtok(NULL, "/");
    }

    FILE *file = fopen(name, "a");
    fclose(file);

    free(directoryPath);
}
void add_file(const char *sourcePath, char *name)
{
    int sourceFile, destinationFile;
    struct stat statSource;
    char *result = strstr(sourcePath, main_dir);
    size_t remaining_size = strlen(result + strlen(main_dir));
    char remaining_str[1000];
    strncpy(remaining_str, result + strlen(main_dir), remaining_size);
    remaining_str[remaining_size] = '\0';
    char des_nam[16] = ".neogit/staging";
    char des_name[1000];
    strcpy(des_name, des_nam);
    strcat(des_name, remaining_str);

    chdir(main_dir);
    char destinationPath[1000];
    realpath(des_name, destinationPath);
    createDirectoriesAndFile(des_name, name);
    int check = compareFiles(destinationPath, sourcePath);
    if (check == 0)
    {
        printf("%s has already staging\n", remaining_str);
        return;
    }
    sourceFile = open(sourcePath, O_RDONLY);
    if (sourceFile == -1)
    {
        perror("Error opening source file");
        return;
    }

    destinationFile = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    /* if (destinationFile == -1)
    {
        perror("Error opening destination file");
        close(sourceFile);
        return;
    } */

    fstat(sourceFile, &statSource);

    ssize_t bytesSent = sendfile(destinationFile, sourceFile, 0, statSource.st_size);

    close(sourceFile);
    close(destinationFile);

    if (bytesSent == -1)
    {
        perror("Error staging File");
    }
    else
    {
        printf("(%s) Staged successfully\n", name);
    }
}
int stage_check(char *path, char *name)
{
    char cwd[1000];
    ;
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit/staging");
    char *result = strstr(path, main_dir);
    size_t remaining_size = strlen(result + strlen(main_dir));
    char remaining_str[1000];
    strncpy(remaining_str, result + strlen(main_dir), remaining_size);
    remaining_str[remaining_size] = '\0';
    char str3[100];
    int x = strlen(remaining_str);
    int y = strlen(name);
    x = x - y;
    x--;
    memcpy(str3, remaining_str, x);
    if (str3[0] == '/')
    {
        for (int i = 0; i < strlen(str3) - 1; i++)
        {
            str3[i] = str3[i + 1];
        }
        str3[strlen(str3) - 2] = '\0';
    }
    chdir(str3);
    DIR *dir = opendir(".");
    struct dirent *entry;
    int flag = 1;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(name, entry->d_name) == 0)
        {
            flag = 0;
            char fullpath[100];
            realpath(entry->d_name, fullpath);
            if (entry->d_type == DT_DIR)
            {
                flag = compareFolders2(path, fullpath);
            }
            else
            {
                flag = compareFiles(path, fullpath);
            }
        }
    }
    chdir(cwd);
    return flag;
}
int commit_check(char *path, char *name)
{
    char cwd[1000];
    ;
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit/commits");
    int id = get_current_id();
    char commit_id[5];
    sprintf(commit_id, "%d", id);
    chdir(commit_id);
    char *result = strstr(path, main_dir);
    size_t remaining_size = strlen(result + strlen(main_dir));
    char remaining_str[1000];
    strncpy(remaining_str, result + strlen(main_dir), remaining_size);
    remaining_str[remaining_size] = '\0';
    char str3[100];
    int x = strlen(remaining_str);
    int y = strlen(name);
    x = x - y;
    x--;
    memcpy(str3, remaining_str, x);
    if (str3[0] == '/')
    {
        for (int i = 0; i < strlen(str3) - 1; i++)
        {
            str3[i] = str3[i + 1];
        }
        str3[strlen(str3) - 2] = '\0';
    }
    // printf("%s",str3);
    chdir(str3);
    DIR *dir = opendir(".");
    struct dirent *entry;
    int flag = 2;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(name, entry->d_name) == 0)
        {
            flag = 0;
            char fullpath[100];
            realpath(entry->d_name, fullpath);
            if (entry->d_type == DT_DIR)
            {
                flag = compareFolders2(path, fullpath);
            }
            else
            {
                flag = compareFiles(path, fullpath);
            }
        }
    }
    chdir(cwd);
    return flag;
}
int compareFiles(const char *file1, const char *file2)
{
    FILE *fptr1, *fptr2;

    fptr1 = fopen(file1, "rb");
    if (fptr1 == NULL)
    {
        perror("Error opening the first file");
        return 2;
    }

    fptr2 = fopen(file2, "rb");
    if (fptr2 == NULL)
    {
        perror("Error opening the second file");
        fclose(fptr1);
        return 2;
    }

    int byte1, byte2;
    size_t position = 0;

    do
    {
        byte1 = fgetc(fptr1);
        byte2 = fgetc(fptr2);
        position++;

        if (byte1 != byte2)
        {
            cnt++;
            return 1;
            break;
        }

    } while (byte1 != EOF && byte2 != EOF);

    if (byte1 == EOF && byte2 == EOF)
    {
        return 0;
    }
    else
    {
        cnt++;
        return 1;
    }

    fclose(fptr1);
    fclose(fptr2);
}
void reset(int argc, char *const argv[])
{
    if (argc < 3)
    {
        perror("please specify a file");
        return;
    }
    if (strcmp("-f", argv[2]) == 0)
    {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        char *prime_argv[3];
        for (int i = 3; i < argc; i++)
        {
            chdir(cwd);
            prime_argv[2] = argv[i];
            reset(3, prime_argv);
        }
    }
    else
    {
        struct stat file_info;

        if (stat(argv[2], &file_info) == 0)
        {
            if (S_ISDIR(file_info.st_mode))
            {
                chdir(main_dir);
                chdir(".neogit/staging");
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", argv[2]);
                int x = system(command);
                if (x == 0)
                {
                    printf("%s remove successfully\n", argv[2]);
                }
            }
            else if (S_ISREG(file_info.st_mode))
            {
                remove_from_staging(realpath(argv[2], NULL), argv[2]);
            }
        }
    }
}
void remove_from_staging(char *path, char *name)
{
    char *result = strstr(path, main_dir);
    size_t remaining_size = strlen(result + strlen(main_dir));
    char remaining_str[1000];
    strncpy(remaining_str, result + strlen(main_dir), remaining_size);
    remaining_str[remaining_size] = '\0';
    chdir(main_dir);
    chdir(".neogit/staging");
    char *directoryPath = strdup(remaining_str);
    char *token = strtok(directoryPath, "/");
    while (token != NULL && strcmp(token, name) != 0)
    {
        mkdir(token, 0777);
        chdir(token);
        token = strtok(NULL, "/");
    }
    char x[1000];
    DIR *dir = opendir(".");
    struct dirent *entry;
    int flag = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, name) == 0)
        {
            remove(name);
            printf("%s removed successfully\n", name);
            flag = 1;
            break;
        }
    }
    if (flag == 0)
    {
        printf("%s is not staging\n", name);
    }
}
void commit(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Invalid Command\n");
        return;
    }
    if (strcmp("-m", argv[2]) != 0 && strcmp("-s", argv[2]) != 0)
    {
        printf("Invalid Command\n");
        return;
    }
    if (strlen(argv[3]) > 72)
    {
        printf("massage length should be under 72\n");
        return;
    }
    int stat = 1;
    stat = stage_khali();
    if (stat == 0)
    {
        printf("There is no File for commit\n");
        return;
    }
    int last_commit_id = get_commit_id();
    cnt = 0;
    if (last_commit_id != 0)
        stat = compare_commit_staging(last_commit_id);
    else
    {
        compare_commit_staging(last_commit_id);
    }
    if (stat == 0)
    {
        printf("There is no File for commit\n");
        return;
    }

    int file_numbers = cnt;
    if (strcmp("-m", argv[2]) == 0)
    {
        new_commit(last_commit_id, argv[3]);
        made_log_file(last_commit_id, argv[3], file_numbers);
    }
    else if (strcmp("-s", argv[2]) == 0)
    {
        int flag_massage = 0;
        char massage[100];
        chdir(main_dir);
        chdir(".neogit/shortcuts");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, argv[3]) == 0)
            {
                FILE *file = fopen(argv[3], "r");
                fgets(massage, sizeof(massage), file);
                fclose(file);
                flag_massage = 1;
            }
        }
        if (flag_massage == 0)
        {
            printf("There is a no shortcut with this name\n");
            return;
        }
        new_commit(last_commit_id, massage);
        made_log_file(last_commit_id, massage, file_numbers);
    }
    edit_branch_file(last_commit_id);
    edit_config(last_commit_id);
}
void edit_branch_file(int id)
{
    id++;
    chdir(main_dir);
    chdir(".neogit/branches");
    get_branch_name();
    FILE *file = fopen(branch_name, "a");
    fprintf(file, "%d\n", id);
    fclose(file);
}
void made_log_file(int id, char *massage, int file_numbers)
{
    chdir(main_dir);
    chdir(".neogit");
    char username[1000];
    char email[1000];
    char chert[100];
    char branch[100];
    FILE *config = fopen("config", "r");
    fscanf(config, "username: %s\n", username);
    fscanf(config, "email: %s\n", email);
    fgets(chert, sizeof(chert), config);
    fscanf(config, "branch: %s\n", branch);
    time_t currentTime;
    time(&currentTime);
    char *timeString = asctime(localtime(&currentTime));
    chdir(main_dir);
    chdir(".neogit/log");
    id++;
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    FILE *log = fopen(commit_id, "w");
    fputs(timeString, log);
    fprintf(log, "%s\n", massage);
    fprintf(log, "%s %s\n", username, email);
    fprintf(log, "%d\n", id);
    fprintf(log, "%s\n", branch);
    fprintf(log, "%d\n", file_numbers);
    time_t currentTime2;
    time(&currentTime2);
    struct tm *localTime = localtime(&currentTime2);
    char dateString[17];
    strftime(dateString, sizeof(dateString), "%Y/%m/%d %H:%M", localTime);
    fprintf(log, "%s\n", dateString);
    fclose(log);
}
void made_revert_log(int id, int id2, int file_numbers)
{
    chdir(main_dir);
    chdir(".neogit");
    char username[1000];
    char email[1000];
    char chert[100];
    char branch[100];
    FILE *config = fopen("config", "r");
    fscanf(config, "username: %s\n", username);
    fscanf(config, "email: %s\n", email);
    fgets(chert, sizeof(chert), config);
    fscanf(config, "branch: %s\n", branch);
    time_t currentTime;
    time(&currentTime);
    char *timeString = asctime(localtime(&currentTime));
    chdir(main_dir);
    chdir(".neogit/log");
    id++;
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char commit_id2[3];
    sprintf(commit_id2, "%d", id2);
    FILE *log = fopen(commit_id, "w");
    FILE *file = fopen(commit_id2,"r");
    char chertt[100];
    fgets(chert,sizeof(chert),file);
    char massage[100];
    fgets(massage,sizeof(massage),file);
    fclose(file);
    fputs(timeString, log);
    fprintf(log, "%s\n", massage);
    fprintf(log, "%s %s\n", username, email);
    fprintf(log, "%d\n", id);
    fprintf(log, "%s\n", branch);
    fprintf(log, "%d\n", file_numbers);
    time_t currentTime2;
    time(&currentTime2);
    struct tm *localTime = localtime(&currentTime2);
    char dateString[11];
    strftime(dateString, sizeof(dateString), "%Y/%m/%d %H:%M", localTime);
    fprintf(log, "%s\n", dateString);
    fclose(log);
}
void edit_config(int id)
{
    id++;
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "last_commit_ID: %d\n", id);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fprintf(config_2, "current_commit_ID: %d\n", id);
    fclose(config_1);
    fclose(config_2);
    remove("config");
    rename("config2", "config");
}
void edit_config2(int id)
{
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fprintf(config_2, "current_commit_ID: %d\n", id);
    fclose(config_1);
    fclose(config_2);
    remove("config");
    rename("config2", "config");
}
int stage_khali()
{
    chdir(main_dir);
    chdir(".neogit/staging");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            return 1;
        }
    }
    return 0;
}
int get_commit_id()
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit");
    FILE *config = fopen("config", "r");
    char chert[1000];
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    int id;
    fscanf(config, "last_commit_ID: %d\n", &id);
    chdir(cwd);
    return id;
}
int get_current_id()
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit");
    FILE *config = fopen("config", "r");
    char chert[1000];
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    fgets(chert, sizeof(chert), config);
    int id;
    fscanf(config, "current_commit_ID: %d\n", &id);
    chdir(cwd);
    return id;
}
int compare_commit_staging(int id)
{
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    chdir(main_dir);
    chdir(".neogit/staging");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            int flag = 1;
            char entry_path[1000];
            realpath(entry->d_name, entry_path);
            chdir(main_dir);
            chdir(".neogit/commits");
            chdir(commit_id);
            DIR *dir_commit = opendir(".");
            struct dirent *entry_commit;
            while ((entry_commit = readdir(dir_commit)) != NULL)
            {
                if (strcmp(entry_commit->d_name, entry->d_name) == 0)
                {
                    flag = 0;
                    char entry_commit_path[1000];
                    realpath(entry_commit->d_name, entry_commit_path);
                    if (entry_commit->d_type == DT_DIR && entry->d_type == DT_DIR)
                    {
                        compareFolders(entry_path, entry_commit_path);
                    }
                    else
                    {
                        compareFiles(entry_path, entry_commit_path);
                    }
                }
            }
            if (flag == 1 && entry->d_type == DT_REG)
            {
                cnt++;
            }
            else if (flag == 1)
            {
                number_of_files_in_folder(entry_path);
            }
            chdir(main_dir);
            chdir(".neogit/staging");
        }
    }
    if (cnt > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void number_of_files_in_folder(char *path)
{
    chdir(path);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            chdir(entry->d_name);
            char address[1000];
            getcwd(address, sizeof(address));
            number_of_files_in_folder(address);
        }
        else if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            cnt++;
        }
    }
}
int compareFolders(char *path1, char *path2)
{
    chdir(path1);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char entry_path[1000];
            realpath(entry->d_name, entry_path);
            int flag = 1;
            chdir(path2);
            DIR *dir2 = opendir(".");
            struct dirent *entry2;
            while ((entry2 = readdir(dir2)) != NULL)
            {
                if (strcmp(entry2->d_name, entry->d_name) == 0)
                {
                    flag = 0;
                    char entry_commit_path[1000];
                    realpath(entry2->d_name, entry_commit_path);
                    if (entry2->d_type == DT_DIR && entry->d_type == DT_DIR)
                    {
                        compareFolders(entry_path, entry_commit_path);
                    }
                    else
                    {
                        compareFiles(entry_path, entry_commit_path);
                    }
                }
            }
            if (flag == 1 && entry->d_type == DT_REG)
            {
                cnt++;
            }
            else if (flag == 1)
            {
                number_of_files_in_folder(entry_path);
            }
            chdir(path1);
        }
    }
}
int compareFolders2(char *path1, char *path2)
{
    chdir(path1);
    DIR *dir = opendir(".");
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char entry_path[1000];
            realpath(entry->d_name, entry_path);
            int flag = 1;
            chdir(path2);
            DIR *dir2 = opendir(".");
            struct dirent *entry2;
            while ((entry2 = readdir(dir2)) != NULL)
            {
                if (strcmp(entry2->d_name, entry->d_name) == 0)
                {
                    flag = 0;
                    char entry_commit_path[1000];
                    realpath(entry2->d_name, entry_commit_path);
                    if (entry2->d_type == DT_DIR && entry->d_type == DT_DIR)
                    {
                        flag = compareFolders(entry_path, entry_commit_path);
                    }
                    else
                    {
                        flag = compareFiles(entry_path, entry_commit_path);
                    }
                    if (flag == 1)
                    {
                        return 1;
                    }
                }
            }
            if (flag == 1)
            {
                return 1;
            }
            chdir(path1);
        }
    }
    return 0;
}
void new_commit(int id, char *meesage)
{
    char last_commit_id[3];
    sprintf(last_commit_id, "%d", id);
    id++;
    printf("commit id : %d\n", id);
    puts(meesage);
    time_t currentTime;
    time(&currentTime);
    char *timeString = asctime(localtime(&currentTime));
    printf("Current time: %s", timeString);
    chdir(main_dir);
    char tracks[1000];
    chdir(".neogit/tracks");
    getcwd(tracks, sizeof(tracks));
    chdir("..");
    char _command[1000];
    snprintf(_command, sizeof(_command), "cp -r %s %s", "staging/*", tracks);
    system(_command);
    chdir(main_dir);
    chdir(".neogit");
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r %s %s", "staging", "commits");
    system(command);
    chdir(main_dir);
    chdir(".neogit/commits");
    char command2[1000];
    snprintf(command2, sizeof(command2), "mv  %s %s", "staging", commit_id);
    system(command2);
    chdir(main_dir);
    chdir(".neogit");
    char command3[1000];
    snprintf(command3, sizeof(command3), "rm -r staging");
    system(command3);
    mkdir("staging", 0777);
    chdir("commits");
    char fullpath[500];
    if (id > 1)
    {
        realpath(commit_id, fullpath);
        char command4[1000];
        snprintf(command4, sizeof(command4), "cp -n %s/* %s", last_commit_id, fullpath);
        system(command4);
    }
    chdir(main_dir);
    chdir(".neogit/commits");
    chdir(commit_id);
    int flag_delete = 0;
    DIR *dir = opendir(".");
    DIR *dir2 = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            FILE *file = fopen(entry->d_name, "r");
            char check[100];
            fscanf(file, "%s", check);
            if (strcmp(check, "dddddd1") == 0)
            {
                flag_delete = 1;
            }
            if (flag_delete == 1)
            {
                struct dirent *entry2;
                while ((entry2 = readdir(dir2)) != NULL)
                {
                    if (strcmp(entry->d_name, entry2->d_name) == 0)
                    {
                        if (entry2->d_type == DT_REG)
                        {
                            char command6[1000];
                            snprintf(command6, sizeof(command6), "rm %s", entry2->d_name);
                            system(command6);
                        }
                        if (entry2->d_type == DT_DIR)
                        {
                            char command6[1000];
                            snprintf(command6, sizeof(command6), "rm -r %s", entry2->d_name);
                            system(command6);
                        }
                    }
                }
            }
        }
    }
}
void new_commit_b1(int id)
{
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    chdir(main_dir);
    chdir(".neogit/commits");
    int id2 = get_commit_id();
    id2++;
    char commit_id2[3];
    sprintf(commit_id2, "%d", id2);
    mkdir (commit_id2, 0777);
    char command[1000];
    snprintf(command,sizeof(command),"cp -r %s/* %s",commit_id,commit_id2);
    system(command);
}
void new_commit_b2(int id, char*meesage)
{
    int id2 = get_commit_id();
    id2++;
    char commit_id2[3];
    sprintf(commit_id2, "%d", id2);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    printf("commit id : %d\n", id2);
    puts(meesage);
    time_t currentTime;
    time(&currentTime);
    char *timeString = asctime(localtime(&currentTime));
    printf("Current time: %s", timeString);
    chdir(main_dir);
    chdir(".neogit/commits");
    char command[1000];
    snprintf(command,sizeof(command),"cp -r %s/* %s",commit_id,commit_id2);
    system(command);
}
void log1(int argc, char *argv[])
{
    if (argc == 2)
    {
        print_all_log();
    }
    else
    {
        if (strcmp(argv[2], "-n") == 0)
        {
            int n;
            sscanf(argv[3], "%d", &n);
            print_n_log(n);
        }
        else if (strcmp(argv[2], "-branch") == 0)
        {
            print_branch_log(argv[3]);
        }
        else if (strcmp(argv[2], "-author") == 0)
        {
            print_name_log(argv[3]);
        }
        else if (strcmp(argv[2], "-since") == 0)
        {
            print_date_log(argv[3], 1);
        }
        else if (strcmp(argv[2], "-before") == 0)
        {
            print_date_log(argv[3], -1);
        }
        else if (strcmp(argv[2], "-search") == 0)
        {
            print_massage_log(argv[3]);
        }
    }
}
void print_massage_log(const char *massage)
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    char chert[200];
    char massage2[100];
    for (int i = num; i > 0; i--)
    {
        char name[3];
        sprintf(name, "%d", i);
        FILE *massage_check = fopen(name, "r");
        fgets(chert, sizeof(chert), massage_check);
        fscanf(massage_check, "%s", massage2);
        if (strstr(massage2, massage) == NULL)
        {
            continue;
        }
        fclose(massage_check);
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
    }
}
int parseDate(const char *time_str1, struct tm *time_info1)
{
    sscanf(time_str1, "%d/%d/%d %d:%d", &time_info1->tm_year, &time_info1->tm_mon,
           &time_info1->tm_mday, &time_info1->tm_hour, &time_info1->tm_min);

    time_info1->tm_year -= 1900;
    time_info1->tm_mon--;
}
void print_date_log(const char *date1, int sb)
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    char chert[200];
    char date2[100];
    int sb2;
    for (int i = num; i > 0; i--)
    {
        char name[3];
        sprintf(name, "%d", i);
        FILE *branch_check = fopen(name, "r");
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fscanf(branch_check, "%s", date2);
        struct tm tmStruct1, tmStruct2;
        time_t time1, time2;
        if (!parseDate(date1, &tmStruct1) || !parseDate(date2, &tmStruct2))
        {
            printf("Error parsing dates\n");
        }
        time1 = mktime(&tmStruct1);
        time2 = mktime(&tmStruct2);
        if (time1 < time2)
            sb2 = 1;
        else if (time1 > time2)
            sb2 = -1;
        if (sb != sb2)
            continue;
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
    }
}
void print_name_log(const char *author)
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    char chert[200];
    char author2[100];
    for (int i = num; i > 0; i--)
    {
        char name[3];
        sprintf(name, "%d", i);
        FILE *name_check = fopen(name, "r");
        fgets(chert, sizeof(chert), name_check);
        fgets(chert, sizeof(chert), name_check);
        fscanf(name_check, "%s", author2);
        if (strcmp(author2, author) != 0)
        {
            continue;
        }
        fclose(name_check);
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
    }
}
void print_branch_log(const char *branch)
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    char chert[200];
    char branch2[100];
    for (int i = num; i > 0; i--)
    {
        char name[3];
        sprintf(name, "%d", i);
        FILE *branch_check = fopen(name, "r");
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fgets(chert, sizeof(chert), branch_check);
        fscanf(branch_check, "%s", branch2);
        if (strcmp(branch2, branch) != 0)
        {
            continue;
        }
        fclose(branch_check);
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
    }
}
void print_n_log(int n)
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    int z = 0;
    for (int i = num; i > 0; i--)
    {
        z++;
        char name[3];
        sprintf(name, "%d", i);
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
        if (z == n)
        {
            break;
        }
    }
}
void print_all_log()
{
    int num = get_commit_id();
    chdir(main_dir);
    chdir(".neogit/log");
    char line[200];
    for (int i = num; i > 0; i--)
    {
        char name[3];
        sprintf(name, "%d", i);
        FILE *log = fopen(name, "r");
        fgets(line, sizeof(line), log);
        printf("TIME : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("MASSAGE : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("USER : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("COMMIT_ID : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("BRANCH : ");
        puts(line);
        fgets(line, sizeof(line), log);
        printf("NUMBER_OF_FILES : ");
        puts(line);
        printf("/////////////////////////////////////////////////////////////////////////////////////////\n");
    }
}
void branch(int argc, char *const argv[])
{
    if (argc == 2)
    {
        chdir(main_dir);
        chdir(".neogit/branches");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                printf("%s\n", entry->d_name);
            }
        }
        return;
    }
    else if (argc == 3)
    {
        chdir(main_dir);
        chdir(".neogit/branches");
        DIR *dir = opendir(".");
        struct dirent *entry;
        int flag = 1;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, argv[2]) == 0)
            {
                flag = 0;
            }
        }
        if (flag == 0)
        {
            printf("There is a branch with this name\n");
            return;
        }
        if (flag == 1)
        {
            int id = get_commit_id();
            FILE *file = fopen(argv[2], "w");
            fprintf(file, "%d\n", id);
            fclose(file);
            // change_branch(argv[2]);
        }
    }
}
void change_branch(char *branch)
{
    chdir(main_dir);
    chdir(".neogit");
    FILE *config_1 = fopen("config", "r");
    FILE *config_2 = fopen("config2", "w");
    char line[1000];
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fgets(line, sizeof(line), config_1);
    fprintf(config_2, "branch: %s\n", branch);
    fgets(line, sizeof(line), config_1);
    fputs(line, config_2);
    fclose(config_1);
    fclose(config_2);
    remove("config");
    rename("config2", "config");
}
void checkout(int argc, char *const argv[])
{
    if (strcmp(argv[2], "HEAD") == 0)
    {
        head_checkout();
    }
    else if (strstr(argv[2], "HEAD") != NULL)
    {
        headn_checkout(argv[2]);
    }
    else
    {
        chdir(main_dir);
        chdir(".neogit/branches");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, argv[2]) == 0)
            {
                change_branch(argv[2]);
                head_checkout();
                return;
            }
        }
        if (isNumeric(argv[2]))
        {
            int adad;
            sscanf(argv[2], "%d", &adad);
            int last_commit_id = get_commit_id();
            if (adad < last_commit_id)
            {
                int checkk = adad_branch(adad);
                if (checkk == 0)
                {
                    printf("This commit is not in this branch\n");
                    return;
                }
                checkout_n(adad);
            }
        }
    }
}
int adad_branch(int adad)
{
    int x = 0;
    get_branch_name();
    chdir(main_dir);
    chdir(".neogit/branches");
    FILE *file = fopen(branch_name, "r");
    while (fscanf(file, "%d\n", &x) != EOF)
    {
        if (x == adad)
        {
            return 1;
        }
    }
    return 0;
}
void checkout_n(int id)
{
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
    edit_config2(id);
}
void revert_n(char *commit_id)
{
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
}
void revert_c(char *const argv[],char *commit_id)
{
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
    int id2 = get_commit_id();
    int last_commit_id=id2;
    id2++;
    char id_2[10];
    sprintf(id_2,"%d",id2);
    chdir(".neogit/commits");
    mkdir(id_2,0777);
    chdir(main_dir);
    char command2[1000];
    snprintf(command2, sizeof(command2), "cp -r .neogit/commits/%s/* .neogit/commits/%s", commit_id, id_2);
    system(command2);
    if(strcmp(argv[2],"-m")==0){
        made_log_file(last_commit_id, argv[3], 0);
    }
    else{
        int id;
        sscanf(commit_id,"%d",&id);
        made_revert_log(last_commit_id,id,0);
    }
    edit_branch_file(last_commit_id);
    edit_config(last_commit_id);
}
int isNumeric(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
}
void head_checkout()
{
    int id = head_id();
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
    edit_config2(id);
}
void head_revert()
{
    int id = head_id();
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
}
void merge_c(){
    int id = head_id();
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    chdir(commit_id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
}
void headn_checkout(char *arg)
{
    int n;
    sscanf(arg, "HEAD-%d", &n);
    int id = headn_id(n);
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
    edit_config2(id);
}
void headn_revert(char *const argv[],char *arg)
{
    int n;
    sscanf(arg, "HEAD-%d", &n);
    int id = headn_id(n);
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    chdir(main_dir);
    char commit_id[3];
    sprintf(commit_id, "%d", id);
    char command[1000];
    snprintf(command, sizeof(command), "cp -r .neogit/commits/%s/* %s", commit_id, main_dir);
    system(command);
    int id2 = get_commit_id();
    int last_commit_id=id2;
    id2++;
    char id_2[10];
    sprintf(id_2,"%d",id2);
    chdir(".neogit/commits");
    mkdir(id_2,0777);
    chdir(main_dir);
    char command2[1000];
    snprintf(command2, sizeof(command2), "cp -r .neogit/commits/%s/* .neogit/commits/%s", commit_id, id_2);
    system(command2);
    if(strcmp(argv[2],"-m")==0){
        made_log_file(last_commit_id, argv[3], 0);
    }
    else{
        made_revert_log(last_commit_id,id,0);
    }
    edit_branch_file(last_commit_id);
    edit_config(last_commit_id);
}
int headn_id(int n)
{
    get_branch_name();
    chdir(main_dir);
    chdir(".neogit/branches");
    FILE *file;
    char ch;
    int line_count = 0;
    file = fopen(branch_name, "r");
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            line_count++;
        }
    }
    fclose(file);
    FILE *file2 = fopen(branch_name, "r");
    int id = 0;
    int x;
    for (int i = 0; i < (line_count - n); i++)
    {
        fscanf(file2, "%d\n", &x);
        if (x > 0)
        {
            id = x;
        }
    }
    fclose(file2);
    return id;
}
int track_check(char *name)
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(main_dir);
    chdir(".neogit/tracks");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, name) == 0)
        {
            chdir(cwd);
            return 1;
        }
    }
    chdir(cwd);
    return 0;
}
int head_id()
{
    get_branch_name();
    chdir(main_dir);
    chdir(".neogit/branches");
    FILE *file = fopen(branch_name, "r");
    int id = 0;
    int x;
    while (fscanf(file, "%d\n", &x) != EOF)
    {
        if(x==11111){
            perror("This branch is destroyed\n");
            return 0;
        }
        if (x > 0)
        {
            id = x;
        }
    }
    fclose(file);
    return id;
}
int parse_date2(const char *date_str, struct tm *tm_struct)
{
    return sscanf(date_str, "%*s %*s %d %d:%d:%d %d",
                  &tm_struct->tm_mday, &tm_struct->tm_hour, &tm_struct->tm_min,
                  &tm_struct->tm_sec, &tm_struct->tm_year);
}
double time_diffrense()
{
    char date_str1[100];
    char date_str2[100];
    chdir(main_dir);
    FILE *local = fopen(".neogit/time", "r");
    if (local == NULL)
    {
        return -1;
    }
    fgets(date_str1, sizeof(date_str1), local);
    if (date_str1 == NULL)
    {
        return -1;
    }
    fclose(local);
    FILE *global = fopen("/mnt/d/global/time", "r");
    if (global == NULL)
    {
        return 1;
    }
    fgets(date_str2, sizeof(date_str2), global);
    if (date_str2 == NULL)
    {
        return 1;
    }
    fclose(global);
    struct tm tm1, tm2;
    parse_date2(date_str1, &tm1) != 5;
    parse_date2(date_str2, &tm2) != 5;
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);
    double difference = difftime(time1, time2);
    return difference;
}
void status(int argc, char *const argv[])
{
    char directory[1000];
    getcwd(directory, sizeof(directory));
    DIR *dir = opendir(".");
    struct dirent *entry;
    int flag_stage;
    int flag_commit;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            char fullpath[1000];
            realpath(entry->d_name, fullpath);
            flag_stage = stage_check(fullpath, entry->d_name);
            flag_commit = commit_check(fullpath, entry->d_name);
            if (flag_commit == 0)
            {
                continue;
            }
            printf("%s ", entry->d_name);
            if (flag_stage == 0)
            {
                printf("+");
            }
            else if (flag_stage == 1)
            {
                printf("-");
            }
            if (flag_commit == 1)
            {
                printf("M\n");
            }
            if (flag_commit == 2)
            {
                printf("A\n");
            }
        }
    }
    Print_D_Files(directory);
}
void Print_D_Files(char *path)
{
    chdir(main_dir);
    chdir(".neogit/commits");
    int id = get_current_id();
    char commit_id[5];
    sprintf(commit_id, "%d", id);
    chdir(commit_id);
    char *result = strstr(path, main_dir);
    size_t remaining_size = strlen(result + strlen(main_dir));
    char remaining_str[1000];
    strncpy(remaining_str, result + strlen(main_dir), remaining_size);
    remaining_str[remaining_size] = '\0';
    char str3[100];
    int x = strlen(remaining_str);
    printf("%s", remaining_str);
    chdir(remaining_str);
    DIR *dir = opendir(".");
    char cwd[1000];
    ;
    getcwd(cwd, sizeof(cwd));
    struct dirent *entry;
    int flag_stage;
    int flag;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            flag = 0;
            chdir(path);
            DIR *dir2 = opendir(".");
            struct dirent *entry2;
            while ((entry2 = readdir(dir2)) != NULL)
            {
                if (strcmp(entry2->d_name, entry->d_name) == 0)
                {
                    flag = 1;
                }
            }
            if (flag == 0)
            {
                printf("%s ", entry->d_name);
                char fullpath[1000];
                realpath(entry->d_name, fullpath);
                flag_stage = stage_check(fullpath, entry->d_name);
                if (flag_stage == 0)
                {
                    printf("+D\n");
                }
                if (flag_stage == 1)
                {
                    printf("-D\n");
                }
                else
                {
                    printf("+D\n");
                }
            }
        }
    }
}
void set1(int argc, char *const argv[])
{
    if (argc < 6)
    {
        printf("Invlaid Command\n");
        return;
    }
    if (strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0)
    {
        printf("Invlaid Command\n");
        return;
    }
    chdir(main_dir);
    chdir(".neogit/shortcuts");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, argv[5]) == 0)
        {
            printf("There is a shortcut with this name\n");
            return;
        }
    }
    FILE *file = fopen(argv[5], "w");
    fputs(argv[3], file);
    fclose(file);
}
void remove1(int argc, char *const argv[])
{
    if (argc < 4)
    {
        printf("Invlaid Command\n");
        return;
    }
    if (strcmp(argv[2], "-s") != 0)
    {
        printf("Invlaid Command\n");
        return;
    }
    chdir(main_dir);
    chdir(".neogit/shortcuts");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, argv[3]) == 0)
        {
            remove(argv[3]);
            return;
        }
    }
    printf("There is a no shortcut with this name\n");
    return;
}
void replace1(int argc, char *const argv[])
{
    if (argc < 6)
    {
        printf("Invlaid Command\n");
        return;
    }
    if (strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0)
    {
        printf("Invlaid Command\n");
        return;
    }
    chdir(main_dir);
    chdir(".neogit/shortcuts");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, argv[5]) == 0)
        {
            FILE *file = fopen(argv[5], "w");
            fputs(argv[3], file);
            fclose(file);
            return;
        }
    }
    printf("There is a no shortcut with this name\n");
    return;
}
void tag1(int argc, char *const argv[])
{
    if (argc == 2)
    {
        chdir(main_dir);
        chdir(".neogit/tags");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                printf("%s\n", entry->d_name);
            }
        }
    }
    else if (strcmp(argv[argc - 1], "-f") == 0 && strcmp("-a", argv[2]) == 0)
    {
        chdir(main_dir);
        chdir(".neogit/tags");
        FILE *file = fopen(argv[3], "w");
        fprintf(file, "tag %s\n", argv[3]);
        if (strcmp(argv[argc - 3], "-c") == 0)
        {
            fprintf(file, "commit %s\n", argv[argc - 2]);
        }
        else
        {
            int id = get_current_id();
            fprintf(file, "commit %d\n", id);
        }
        get_author();
        fprintf(file, "Author: %s %s\n", author_name, author_email);
        time_t currentTime;
        time(&currentTime);
        char *timeString = asctime(localtime(&currentTime));
        fprintf(file, "Date: %s", timeString);
        if (strcmp(argv[argc - 3], "-m") == 0)
        {
            fprintf(file, "Message: %s\n", argv[argc - 2]);
        }
        else if (strcmp(argv[argc - 5], "-m") == 0)
        {
            fprintf(file, "Message: %s\n", argv[argc - 4]);
        }
        fclose(file);
    }
    else if (strcmp("-a", argv[2]) == 0)
    {
        chdir(main_dir);
        chdir(".neogit/tags");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(argv[3], entry->d_name) == 0)
            {
                printf("There is a tag with this name\n");
                return;
            }
        }
        FILE *file = fopen(argv[3], "w");
        fprintf(file, "tag %s\n", argv[3]);
        if (strcmp(argv[argc - 2], "-c") == 0)
        {
            fprintf(file, "commit %s\n", argv[argc - 1]);
        }
        else
        {
            int id = get_current_id();
            fprintf(file, "commit %d\n", id);
        }
        get_author();
        fprintf(file, "Author: %s %s\n", author_name, author_email);
        time_t currentTime;
        time(&currentTime);
        char *timeString = asctime(localtime(&currentTime));
        fprintf(file, "Date: %s", timeString);
        if (strcmp(argv[argc - 2], "-m") == 0)
        {
            fprintf(file, "Message: %s\n", argv[argc - 1]);
        }
        else if (strcmp(argv[argc - 4], "-m") == 0)
        {
            fprintf(file, "Message: %s\n", argv[argc - 3]);
        }
        fclose(file);
    }
    else if (strcmp("show", argv[2]) == 0)
    {
        chdir(main_dir);
        chdir(".neogit/tags");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, argv[3]) == 0)
            {
                char print[200];
                FILE *file = fopen(argv[3], "r");
                fgets(print, sizeof(print), file);
                puts(print);
                fgets(print, sizeof(print), file);
                puts(print);
                fgets(print, sizeof(print), file);
                puts(print);
                fgets(print, sizeof(print), file);
                puts(print);
                char print2[200];
                if (fgets(print2, sizeof(print2), file) != NULL)
                    puts(print2);
            }
        }
    }
}
void merge(int argc, char *const argv[]){
    chdir(main_dir);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
        {
            int flag_track = track_check(entry->d_name);
            if (entry->d_type == DT_DIR && flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                system(command);
            }
            else if (flag_track == 1)
            {
                char command[1000];
                snprintf(command, sizeof(command), "rm %s", entry->d_name);
                system(command);
            }
        }
    }
    change_branch(argv[4]);
    merge_c();
    int id1 = head_id();
    new_commit_b1(id1);
    edit_branch_file(11110);
    change_branch(argv[3]);
    int id2 = head_id();
    merge_c();
    int last_commit_id = get_commit_id();
    char massage[1000];
    sprintf(massage,"merged %s %s",argv[3],argv[4]);
    new_commit_b2(id2,massage);
    made_log_file(last_commit_id, massage, 0);
    edit_branch_file(last_commit_id);
    edit_config(last_commit_id);
    chdir(main_dir);
    chdir("merge");
    int id3 = get_commit_id();
    char id_3[10];
    sprintf(id_3,"%d",id3);
    FILE *file = fopen(id_3,"w");
}
void revert(int argc, char *const argv[]){
    if(strcmp(argv[2],"-n")==0){
        if(argc==3){
            head_revert();
            return;
        }
        else{
            revert_n(argv[3]);
        }
    }
    else if(strstr(argv[argc-1],"HEAD")!=NULL){
        headn_revert(argv,argv[argc-1]);
    }
    else{
        int last_merge=get_last_merge();
        int id;
        sscanf(argv[argc-1],"%d",&id);
        if(id<=last_merge){
            perror("There was a merge\n");
            return;
        }
        else{
            revert_c(argv,argv[argc-1]);
        }
    }
    
}
void stash(int argc, char *const argv[]){
    if(strcmp(argv[2],"push")==0){
        int stashh=get_last_stash();
        chdir(main_dir);
        chdir(".neogit/stash");
        for(int i=stashh;i>=0;i--){
            char name1[3];
            char name2[3];
            sprintf(name1,"%d",i);
            sprintf(name2,"%d",(i+1));
            rename(name1,name2);
        }
        mkdir("0",0777);
        chdir(main_dir);
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
            {
                int flag_track = track_check(entry->d_name);
                if (entry->d_type == DT_DIR && flag_track == 1)
                {
                    char command2[1000];
                    snprintf(command2, sizeof(command2), "cp -r %s .neogit/stash/0", entry->d_name);
                    system(command2);
                    char command[1000];
                    snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                    system(command);
                }
                else if (flag_track == 1)
                {
                    char command2[1000];
                    snprintf(command2, sizeof(command2), "cp %s .neogit/stash/0", entry->d_name);
                    system(command2);
                    char command[1000];
                    snprintf(command, sizeof(command), "rm %s", entry->d_name);
                    system(command);
                }
            }
        }
        head_revert();
        chdir(main_dir);
        chdir(".neogit/stash/0");
        FILE *file = fopen("info","w");
        get_branch_name();
        fprintf(file,"branch: %s\n",branch_name);
        if(argc>3){
            if(strcmp(argv[3],"-m")==0){
                fprintf(file,"message: %s\n",argv[4]);
            }
        }
        
        fclose(file);
    }
    else if(strcmp(argv[2],"list")==0){
        chdir(main_dir);
        chdir(".neogit/stash");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while((entry=readdir(dir))!=NULL){
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                printf("index %s\n",entry->d_name);
                char name[10];
                snprintf(name,sizeof(name),"%s/info",entry->d_name);
                FILE *file = fopen(name,"r");
                char line[200];
                while (fgets(line,sizeof(line),file)!=NULL)
                {
                    puts(line);
                }
                printf("////////////////////////////////////////////////\n");
            }
        }
    }
    else if(strcmp(argv[2],"pop")==0){
        chdir(main_dir);
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".neogit") != 0)
            {
                int flag_track = track_check(entry->d_name);
                if (entry->d_type == DT_DIR && flag_track == 1)
                {
                    char command[1000];
                    snprintf(command, sizeof(command), "rm -r %s", entry->d_name);
                    system(command);
                }
                else if (flag_track == 1)
                {
                    char command[1000];
                    snprintf(command, sizeof(command), "rm %s", entry->d_name);
                    system(command);
                }
            }
        }
        char command[1000];
        snprintf(command, sizeof(command), "cp -r .neogit/stash/0/* %s", main_dir);
        system(command);
        char command2[1000];
        snprintf(command2, sizeof(command2), "rm info");
        system(command2);
        int stashh= get_last_stash();
        chdir(".neogit/stash");
        char command3[1000];
        snprintf(command3, sizeof(command3), "rm -r 0");
        system(command3);
        for(int i=1;i<=stashh;i++){
            char name1[3];
            char name2[3];
            sprintf(name1,"%d",i);
            sprintf(name2,"%d",(i-1));
            rename(name1,name2);
        }
    }
    else if(strcmp(argv[2],"clear")==0)
    {
        chdir(main_dir);
        chdir(".neogit/stash");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while((entry=readdir(dir))!=NULL){
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                char command3[1000];
                snprintf(command3, sizeof(command3), "rm -r %s",entry->d_name);
                system(command3);
            }
        }
    }
    else if(strcmp(argv[2],"drop")==0){
        int stashh= get_last_stash();
        chdir(".neogit/stash");
        char command3[1000];
        snprintf(command3, sizeof(command3), "rm -r 0");
        system(command3);
        for(int i=1;i<=stashh;i++){
            char name1[3];
            char name2[3];
            sprintf(name1,"%d",i);
            sprintf(name2,"%d",(i-1));
            rename(name1,name2);
        }
    }
}
void grep(int argc, char *const argv[]){
    if(strcmp(argv[2],"-f")!=0 || strcmp(argv[4],"-p")!=0){
        printf("Invalid command\n");
        return;
    }
    else{
        if(strcmp(argv[argc-1],"-n")==0){
            FILE *file = fopen(argv[3],"r");
            char line[200];
            int i=1;
            while (fgets(line,sizeof(line),file)!=NULL)
            {
                char line2[200];
                strcpy(line2,line);
                if(strstr(line2,argv[5])!=NULL){
                    printf("%d. ",i);
                    puts(line);
                }
                i++;
            }
            fclose(file);
        }
        else{
            FILE *file = fopen(argv[3],"r");
            char line[200];
            while (fgets(line,sizeof(line),file)!=NULL)
            {
                char line2[200];
                strcpy(line2,line);
                if(strstr(line2,argv[5])!=NULL){
                    puts(line);
                }
            }
            fclose(file);
        }
    }
}
void tree(){
    int flag1=0;
    int flag2=0;
    int flag3=0;
    int slash;
    char name2[50];
    chdir(main_dir);
    chdir(".neogit/branches");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry=readdir(dir))!=NULL){
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "master") != 0){
            strcpy(name2,entry->d_name);
            printf("M = master\nX = %s\n",name2);
        }
    }
    FILE *master = fopen("master","r");
    FILE *file = fopen(name2,"r");
    fscanf(file,"%d\n",&slash);
    int id1;
    int id2;
    fscanf(master,"%d\n",&id1);
    printf("M:%d\n",id1);
    while(fscanf(master,"%d\n",&id1)!=EOF){
        if(id1==slash){
            printf("|\n");
            printf("M:%d\n",id1);
            printf("|  \\\n");
            flag1=1;
        }
        else if(flag1==1){
            flag2=1;
            printf("M:%d",id1);
            flag1=0;
        }
        else{
            printf("|");
            if(flag2==0){
                printf("\n");
                printf("M:%d",id1);
            }
        }
        if(flag2 ==1){
            if(fscanf(file,"%d\n",&id2)!=EOF){
                if(id2==11111){
                    printf("  /\n");
                    printf("M:%d",id1);
                    flag2=0;
                }
                else if(flag3==0){
                    printf(" X:%d",id2);
                    flag3=1;
                }
                else{
                    printf("   |\n");
                    printf("M:%d",id1);
                    printf(" X:%d",id2);
                }
            }
        }
        if(flag2==0&&flag1==1){

        }
        else{
            printf("\n");
        }
    }
}
int main(int argc, char *argv[])
{
    char currentwd[1000];
    getcwd(currentwd, sizeof(currentwd));
    if (argc < 2)
    {
        perror("Invalid Command!\n");
        return 1;
    }
    if (strcmp("init", argv[1]) == 0)
    {
        init(argc, argv);
    }
    if (strcmp("config", argv[1]) == 0 && strcmp("-global", argv[2]) == 0)
    {
        if (strcmp("user.name", argv[3]) == 0)
        {
            change_global_username(argv[4]);
        }
        if (strcmp("email", argv[3]) == 0)
        {
            change_global_email(argv[4]);
        }
        else if (strstr(argv[3], "alias") != NULL)
        {
            global_alias(argv[3], argv[4]);
        }
    }
    int x = get_main_dir();
    if (x == 0)
    {
        printf("There is no repository\n");
        return 1;
    }
    else if (strcmp("config", argv[1]) == 0)
    {
        if (strcmp("user.name", argv[2]) == 0)
        {
            change_username(argv[3]);
        }
        if (strcmp("email", argv[2]) == 0)
        {
            change_email(argv[3]);
        }
        else if (strstr(argv[2], "alias") != NULL)
        {
            local_alias(argv[2], argv[3]);
        }
    }
    double t_check = time_diffrense();
    if (t_check < 0)
    {
        printf("1\n");
        update_config();
    }
    chdir(currentwd);
    if (strcmp("add", argv[1]) == 0)
    {
        int head = head_check();
        if (head == 0)
        {
            printf("You should be on head to do this\n");
            return 1;
        }
        chdir(currentwd);
        add(argc, argv);
    }
    if (strcmp("reset", argv[1]) == 0)
    {
        int head = head_check();
        if (head == 0)
        {
            printf("You should be on head to do this\n");
            return 1;
        }
        chdir(currentwd);
        reset(argc, argv);
    }
    if (strcmp("commit", argv[1]) == 0)
    {
        int head = head_check();
        if (head == 0)
        {
            printf("You should be on head to do this\n");
            return 1;
        }
        chdir(currentwd);
        commit(argc, argv);
    }
    if (strcmp("status", argv[1]) == 0)
    {
        int head = head_check();
        if (head == 0)
        {
            printf("You should be on head to do this\n");
            return 1;
        }
        chdir(currentwd);
        status(argc, argv);
    }
    if (strcmp("log", argv[1]) == 0)
    {
        log1(argc, argv);
    }
    if (strcmp("set", argv[1]) == 0)
    {
        set1(argc, argv);
    }
    if (strcmp("remove", argv[1]) == 0)
    {
        remove1(argc, argv);
    }
    if (strcmp("replace", argv[1]) == 0)
    {
        replace1(argc, argv);
    }
    if (strcmp("branch", argv[1]) == 0)
    {
        int head = head_check();
        if (head == 0)
        {
            printf("You should be on head to do this\n");
            return 1;
        }
        chdir(currentwd);
        branch(argc, argv);
    }
    if (strcmp("checkout", argv[1]) == 0)
    {
        checkout(argc, argv);
    }
    if (strcmp("tag", argv[1]) == 0)
    {
        tag1(argc, argv);
    }
    if (strcmp("merge", argv[1]) == 0)
    {
        merge(argc, argv);
    }
    if (strcmp("revert", argv[1]) == 0)
    {
        revert(argc, argv);
    }
    if (strcmp("stash", argv[1]) == 0)
    {
        stash(argc, argv);
    }
    if (strcmp("grep", argv[1]) == 0)
    {
        grep(argc, argv);
    }
    if (strcmp("tree", argv[1]) == 0)
    {
        tree();
    }
    else if (argc == 2)
    {
        char cwd[1000];
        char command[1000];
        char command2[1000];
        getcwd(cwd, sizeof(cwd));
        chdir(main_dir);
        chdir(".neogit/alias");
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, argv[1]) == 0)
            {
                FILE *file = fopen(entry->d_name, "r");
                fgets(command, sizeof(command), file);
                snprintf(command2, sizeof(command2), "%s\n", command);
                chdir(cwd);
                system(command2);
            }
        }
    }
    return 0;
}