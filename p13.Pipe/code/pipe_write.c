#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    FILE *wpipe;
    if (!(wpipe = popen("od -c", "w")))
    {
        perror("open pipe error!");
        exit(EXIT_FAILURE);
    }

    char *data = "data";
    fwrite(data, sizeof(char), strlen(data), wpipe);
    pclose(wpipe);

    return EXIT_SUCCESS;
}
