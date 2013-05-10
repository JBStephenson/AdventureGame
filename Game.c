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

int isDirection(char str[])
{
    if(mxmlFindElement(tree, tree, "path", "command", str, MXML_DESCEND) != NULL)
    {
        return 1;
    }
    return 0;
    
}

int isItem(char str[])
{
    if(mxmlFindElement(tree, tree, "item", "name", str, MXML_DESCEND) != NULL)
    {
        return 1;
    }
    return 0;
    
}

int isChallenge(char str[])
{
    if(mxmlFindElement(tree, tree, "challenge", "command", str, MXML_DESCEND) != NULL)
    {
        return 1;
    }
    return 0;
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
    //printf("%s\n",newpoints);
    mxmlElementSetAttr(node, "points", newpoints);
}

char *execute_command(char *command)
{
    char *firstinput = strtok(command, " \n");
    char *secondinput = strtok(NULL, " \n");
    char buffer[100];
    mxml_node_t *node;
    mxml_node_t *node2;
    mxml_node_t *tempnode;
    if(0==1)
    {
        return 0;
    }
    else if(isDirection(firstinput) == 1)
    {
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        char *playerroom = mxmlElementGetAttr(node, "room");
        node = mxmlFindElement(tree, tree, "room", "name", playerroom, MXML_DESCEND);
        node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_DESCEND);
        while(node != NULL)
        {
            if(strcmp(firstinput, mxmlElementGetAttr(node, "command")) == 0)
            {
                char *room = mxmlElementGetAttr(node, "dest");
                node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
                mxmlElementSetAttr(node, "room", room);
                node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
                node2 = mxmlFindElement(tree, tree, "room", "name", mxmlElementGetAttr(node, "room"), MXML_DESCEND);
                return mxmlElementGetAttr(node2, "description");
            }
            node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_NO_DESCEND);
        }
        return "You can't go that way";
        
        
    }
    else if(strcmp(firstinput, "score") == 0)
    {
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        return mxmlElementGetAttr(node, "points"); 
    }
    
    else if(strcmp(firstinput, "inventory") == 0)
    {
        char buffer[1024];
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_DESCEND);
            while(node != NULL)
            {
                sprintf(buffer, "%s\n", mxmlElementGetAttr(node, "name"));
                node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_DESCEND);
            }
        return &buffer;        
    }
    else if(strcmp(firstinput, "look") == 0)
    {
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        node2 = mxmlFindElement(tree, tree, "room", "name", mxmlElementGetAttr(node, "room"), MXML_DESCEND);
        return mxmlElementGetAttr(node2, "description");
        
    }
    else if(strcmp(firstinput, "save") == 0)
    {
        db_save(tree, "GameSave1.xml");
        return "Game saved under GameSave.xml";
    }
    else if(strcmp(firstinput, "get") == 0)
    {
        if(isItem(secondinput) == 1)
        {
            node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
            char *playerroom = mxmlElementGetAttr(node, "room");
            node = mxmlFindElement(tree, tree, "room", "name", playerroom, MXML_DESCEND);
            node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_DESCEND);
            while(node != NULL)
            {
                if(strcmp(secondinput, mxmlElementGetAttr(node, "name")) == 0)
                {
                    add_points(atoi(mxmlElementGetAttr(node, "points")));
                    mxmlElementSetAttr(node, "points", "0");
                    tempnode = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
                    mxmlAdd(tempnode, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
                    sprintf(buffer, "You got the %s", secondinput);
                    return &buffer;
                }
                node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_NO_DESCEND);
            }
        }
        return "Nothing to get by that name.";
    }
    else if(strcmp(firstinput, "drop") == 0)
    {
        if(isItem(secondinput) == 1)
        {
            node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
            char *playerroom = mxmlElementGetAttr(node, "room");
            node2 = mxmlFindElement(tree, tree, "room", "name", playerroom, MXML_DESCEND);
            node = mxmlFindElement(node, tree, "item", NULL, NULL, MXML_DESCEND);
            while(node != NULL)
            {
                if(strcmp(secondinput, mxmlElementGetAttr(node, "name")) == 0)
                {
                    mxmlAdd(node2, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
                    //Remove from player
                    sprintf(buffer, "You droped the %s", secondinput);
                    return &buffer;
                }
                node = mxmlFindElement(node, tree, "path", NULL, NULL, MXML_NO_DESCEND);
            }
        }
        return "Nothing to drop by that name.";
    }
    else if(isChallenge(firstinput) == 1)
    {
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        char *playerroom = mxmlElementGetAttr(node, "room");
        node2 = mxmlFindElement(tree, tree, "room", "name", playerroom, MXML_DESCEND);
        node = mxmlFindElement(node2, tree, "challenge", "command", firstinput, MXML_DESCEND);
        if(node == NULL)
        {
            return "Cannot do that in this room"; //Add what this is
        }
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        node = mxmlFindElement(node, tree, "item", "name", secondinput, MXML_DESCEND);
        if(node == NULL)
        {
            return "Not the correct item";
        }
        node = mxmlFindElement(tree, tree, "player", NULL, NULL, MXML_DESCEND);
        playerroom = mxmlElementGetAttr(node, "room");
        node2 = mxmlFindElement(tree, tree, "room", "name", playerroom, MXML_DESCEND);
        node = mxmlFindElement(node2, tree, "challenge", "command", firstinput, MXML_DESCEND);
        //if()
        mxmlElementSetAttr(node, "completed", "yes");
        return "You completed the challenge!";
        
        
    }
    else if(strcmp(firstinput, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        return "Command Not Found";
    }
}

//Got this input code from http://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
//Need to fix memory leak
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
    printf("%s\n", output);
}

int main()
{
    tree = db_load("Game.xml");     //Loads game information from Game.xml
    //output_command(); //Need to make this do a look
    while(1)
    {
       output_command(input_command());
    }
    
         
}
