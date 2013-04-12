#include <stdlib.h>
#include <stdio.h>
#include <mxml.h>

mxml_node_t *tree;
mxml_node_t * db_load(char fname[])
{
    FILE *xmlf;
    mxml_node_t *tree;
    xmlf = fopen(fname, "r");
    if (xmlf == NULL)
    {
        printf("Unable to open file\n");
        exit(1);
    }
    tree = mxmlLoadFile(NULL, xmlf, MXML_TEXT_CALLBACK);
    fclose(xmlf);
    return tree;    
}

void db_save(mxml_node_t *tree, char fname[])
{
    FILE *xmlf;
    int i;
    xmlf = fopen(fname, "w");
    if (xmlf == NULL)
    {
        printf("Unable to open file\n");
        exit(1);
    }
    mxmlSaveFile(tree, xmlf, MXML_NO_CALLBACK);
    fclose(xmlf);      
}

static void print_node(mxml_node_t *node)
{
    const char *element;

    if (node==NULL) return;

    switch (mxmlGetType(node))
    {
        case MXML_IGNORE:
            printf("Ignore node\n");
            break;
        case MXML_ELEMENT:
            element = mxmlGetElement(node);
            if (strcmp(element, "room") == 0)
            {
                printf("room: name=%s points=%s\n",
                        mxmlElementGetAttr(node, "name"),
                        mxmlElementGetAttr(node, "points"));
            }
            else if (strcmp(element, "item") == 0)
            {
                printf("item: name=%s points=%s\n",
                        mxmlElementGetAttr(node, "name"),
                        mxmlElementGetAttr(node, "points"));
            }
            else if (strcmp(element, "path") == 0)
            {
                printf("path: command=%s dest=%s\n",
                        mxmlElementGetAttr(node, "command"),
                        mxmlElementGetAttr(node, "dest"));
            }
            else if (strcmp(element, "player") == 0)
            {
                printf("player: start=%s points=%s\n",
                        mxmlElementGetAttr(node, "room"),
                        mxmlElementGetAttr(node, "points"));
            }
            else
            {
                printf("Unknown element: %s\n", element);
            }
            break;
        case MXML_INTEGER:
            printf("Integer node\n");
            break;
        case MXML_OPAQUE:
            printf("Opaque node\n");
            break;
        case MXML_REAL:
            printf("Real node\n");
            break;
        case MXML_TEXT:
            printf("Text: '%s'\n", mxmlGetText(node, NULL));
            break;
        case MXML_CUSTOM:
            printf("Custom node\n");
            break;
    }
}

void print_room_paths(char room[])
{
    mxml_node_t *node;
    node = tree;
    node = mxmlFindElement(tree, tree, "room", "name", room, MXML_DESCEND);
    node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_DESCEND);
    while(node != NULL)
    {
        print_node(node);
        node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_NO_DESCEND);
    }
}


//Take player node and points to add
void add_points(int i)
{
    mxml_node_t *node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
    const char *points;
    const char *newpoints;
    char buffer[4];
    points = mxmlElementGetAttr(node, "points");
    sprintf(buffer, "%d", atoi(points) + i);
    newpoints = &buffer;
    printf("%s\n",newpoints);
    mxmlElementSetAttr(node, "points", newpoints);
}

char *execute_command(char *command)
{
    mxml_node_t *node;
    mxml_node_t *node2;
    if(strcmp(command, "look\n") == 0)
    {
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        node2 = mxmlFindElement(tree, tree, "room", "name", mxmlElementGetAttr(node, "room"), MXML_DESCEND);
        return mxmlElementGetAttr(node2, "summary");
        
    }
    else if(strcmp(command, "save\n") == 0)
    {
        db_save(tree, "GameSave.xml");
        return "Game saved under GameSave.xml";
    }
    else if(strcmp(command, "exit\n") == 0)
    {
        return "exit";
    }
    else
    {
        return "Command Not Found";
    }
}

//Got this sweet input code from http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
char * input_command()
{
    printf("What would you like to do?\n");
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return execute_command(linep);
}

int output_command(char *output)
{
    if(strcmp(output, "exit") != 0)
    {
        printf("%s\n", output);
        return 0;
    }
    else
    {
        printf("Goodbye!\n");
        return -1;
    }
}

int main()
{
    int i = 0;
    tree = db_load("Game.xml");     //Loads game information from Game.xml
    print_room_paths("room2");      //Prints all paths from room2
    add_points(6);                  //Adds 6 points to player
    
    while(i == 0)
    {
       i = output_command(input_command());
    }
    
         
}
