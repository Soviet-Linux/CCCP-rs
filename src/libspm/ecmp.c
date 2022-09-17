#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"

#include "../../include/utils.h"
#include "../../include/libspm.h"
#include "../../include/ecmp.h"
#include <unistd.h>





int ecmp_package_parse(struct package* pkg,char* path)
{
    struct ecmp_section** sections = calloc(32,sizeof(struct ecmp_section*));
    printf("sections address is %p\n",&sections);

    // get sections from file
    // WARNING: this function allocates memory for sections
    printf("Parsing file %s\n",path);
    unsigned int section_count = ecmp_parse_file(&sections,path);
    printf("file is parsed , launching section analysis\n");
    printf("sections address is %p and value is %p\n",&sections,sections);

    // parse sections
    for (int i = 1; i < section_count; i++)
    {   
        printf("section is : %s\n",sections[i]->name);
        if (strcmp(sections[i]->name,"info") == 0)
        {
            printf("parsing info section\n");
            // parse info section
            int line_count;
            char** lines = split(sections[i]->buff,'\n',&line_count);
            for (int j = 0; j < line_count-2; j++)
            {
                printf("run %d\n",j);
                printf("line is : %s\n",lines[j]);
                int c_check;
                char** line = split(lines[j],'=',&c_check);
                printf("line[0] is %s and line[1] is %s\n",line[0],line[1]);
                if (c_check != 2 )
                {
                    printf("Error parsing line %d in section %s\n",j,sections[i]->name);
                    return 1;
                }

                if (strcmp(line[0],"name") == 0)
                {
                    pkg->name = line[1];
                }
                else if (strcmp(line[0],"type") == 0)
                {
                    pkg->type = line[1];
                }
                else if (strcmp(line[0],"version") == 0)
                {
                    pkg->version = line[1];
                }
                else if (strcmp(line[0],"license") == 0)
                {
                    printf("license is %s\n",line[1]);
                    pkg->license = line[1];
                    printf("license is %s\n",pkg->license);
                }
                else if (strcmp(line[0],"url") == 0)
                {
            
                    pkg->url = line[1];
                }
                else
                {
                    printf("Unknown info key: %s\n",line[0]);
                }
            }

            printf("info section is parsed\n");
                
        }
        else if (strcmp(sections[i]->name,"makedeps") == 0)
        {
           pkg->makedependencies = split(sections[i]->buff,'\n',&pkg->makedependenciesCount);

            
        }
        else if (strcmp(sections[i]->name,"dependencies") == 0)
        {
            // parse dependencies section
            pkg->dependencies = split(sections[i]->buff,'\n',&pkg->dependenciesCount);
        }
        else if (strcmp(sections[i]->name,"locations") == 0)
        {
            // parse scripts section
            pkg->locations = split(sections[i]->buff,'\n',&pkg->locationsCount);
        }
        else if (strcmp(sections[i]->name,"install") == 0)
        {
            // parse scripts section
            pkg->info.install = sections[i]->buff;
        }
        else if (strcmp(sections[i]->name,"special") == 0)
        {
            // parse scripts section
            pkg->info.special = sections[i]->buff;
        }
        else {
            // unknown section
            printf("Unknown section: %s\n",sections[i]->name);
        }

    }
    return 0;

}





unsigned int ecmp_parse_file(struct ecmp_section*** sections,char* path)
{
    // check if file exists
    if (access(path,F_OK) != 0)
    {
        printf("File %s does not exist\n",path);
        return 0;
    }

    // I had a small problem th t cause the section lis to start at 1 , so this solution is tupid
    int sec_count = 0;
    

    // read file line by line
    FILE* fp = fopen(path,"r");
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("Reading file %s\n",path);
    while((read = getline(&line,&len,fp)) != -1)
    {

        // remove spaces from line
        printf("Line: %s",line);
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] == ' ')
            {
                popcharn(line, strlen(line), i);
            }
        }
        printf("space removed: %s",line);

        // remove empty line
        if (strlen(line) < 1 ) continue;

        // ignore comments
        if(line[0] == '#') continue;
        
        
        if (line[0] == '[') {
            sec_count++;
            // allocate new section
            (*sections)[sec_count] = calloc(1,sizeof(struct ecmp_section));
            // get section name
            (*sections)[sec_count]->name = calloc(strlen(line),sizeof(char));

            // section name
            printf("parsing section header\n");
            for (int i = 1; i < strlen(line); i++) {
                
                if (line[i] == ']') {
                    (*sections)[sec_count]->name[i] = '\0';
                    break;
                }
                else {
                    (*sections)[sec_count]->name[i-1] = line[i];
                    printf("char: %c\n",line[i]); 
                }
            }
            printf("section name: %s\n",(*sections)[sec_count]->name);
            // allocate section buffer
            (*sections)[sec_count]->buff = calloc(512, sizeof(char));
            
            continue;
        }

        // check if buff is null
        if (sec_count == 0) {
            continue;
        }

        add_line:

        // append line to buffer
        // check if buffer is full
        if ((strlen((*sections)[sec_count]->buff) + strlen(line)) * sizeof(char) > malloc_usable_size((*sections)[sec_count]->buff)) {
            printf("buffer full");
            // reallocate buffer
            (*sections)[sec_count]->buff = realloc((*sections)[sec_count]->buff, malloc_usable_size((*sections)[sec_count]->buff) + 512);
            goto add_line;
        }
        strcat((*sections)[sec_count]->buff,line);
    }

    fclose(fp);
    printf("sections address is %p and value is %p\n",sections,*sections);
    
    printf("sec_coutn is %d\n",sec_count);
    return sec_count;
}