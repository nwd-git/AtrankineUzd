#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>
#include <regex.h>

//variables
int fcount, rcount = 0;
char root[512];
char text[512];
char ignore[512];
regex_t regex;
int reg;

//functions
void search(const char *path);
void listdir(const char *name);
static int parse_opt (int key, char *arg, struct argp_state *state);

//Argp parser
static int parse_opt (int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
        case 'f':
        {
            snprintf(root, sizeof(root), "%s", arg);
            break;        
            
        }
        case 't':
        {
            snprintf(text, sizeof(root), "%s", arg);
            break;
        }
        case 'i':
        {
            snprintf(ignore, sizeof(ignore), "%s",  arg);
            break;          
        }
        case ARGP_KEY_END:
        {
            if(strcmp(root, "") == 0)
            {
                argp_failure(state, 1, 0, "no --folder option");
            }
            else if(strcmp(text, "") == 0)
            {
                argp_failure(state, 1, 0, "no --text option");
            }
            break;
        }
    }
    return 0;
}
//Function that goes through directories and their files recursively
void listdir(const char *name)
{
    DIR *dir;
    struct dirent *entry;
    
    //open directory
    if (!(dir = opendir(name)))
        return;

    //check if -i option is used
    switch(strcmp(ignore,"") == 0)
    {
        case true:
        {
            while ((entry = readdir(dir)) != NULL)
            {
                char path[1024];
                //check if its file or directory
                if (entry->d_type == DT_DIR)
                {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                        continue;
                    snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                    listdir(path);
                }
                else
                {
                    snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                    fcount++;
                    search(path);
                }     
            }
        }
        case false:
        {
            while ((entry = readdir(dir)) != NULL)
            {
                char path[1024];
                reg = regexec(&regex, entry->d_name, 0, NULL, 0);
                if(reg)
                {
                    //check if its file or directory
                    if (entry->d_type == DT_DIR)
                    {
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                            continue;
                        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                        listdir(path);
                    }
                    else
                    {
                        snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
                        fcount++;
                        search(path);
                    }     
                }
            }
        }
        closedir(dir);
    }
}

//Function to search for text in files
void search(const char *path)
{
    bool found = false;
    FILE *f = fopen(path, "r");
    int linenum = 1;
    char temp[1024];

    if((f == NULL))
    {
        return;
    }
    while(fgets(temp,1024, f) != NULL)
    {
        if((strstr(temp, text)) != NULL)
        {
            found = true;
            printf("%s:%d\n", path, linenum);
        }
        linenum++;
    }
    if(found)
    {
        rcount++;
    }
    fclose(f);
}

//main
int main(int argc, char **argv) {
    
    struct argp_option options[] = 
    {
        { "folder", 'f', "dir", 0, "Specify directory you want to search in"},
        { "text", 't', "text", 0, "Specify text you're looking for" },
        {"ignore", 'i', "text", 0, "Specify data type or name to ignore"},
        {0}
    };
    struct argp argp = { options, parse_opt, 0, 0};
    argp_parse(&argp, argc, argv, 0, 0, 0);
    reg = regcomp(&regex, ignore, 0);
    listdir(root);
    printf("Failų patikrinta: %d\n", fcount);
    printf("Failų, kuriuose rastas tekstas: %d\n", rcount);
    return 0;
}
