#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>
#include "uniqueobjects.h"

static wrpformat wrp;

uniqueObjects::uniqueObjects(int argc, char *argv[])
{
    Open_Files(argc, argv);
    Read_Signature();
    Read_Elevations();
    Read_Textures();
    Read_Objects();
    Write_UniqueNames();
}


void uniqueObjects::Open_Files(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Not enough parameters!\n\n\tUniqueObjects Source.WRP\n\n");
    }
    printf ("Source WRP: %s\n", argv[1]);

    map = fopen (argv[1], "rb");
    if (!map)
    {
        printf ("error in %s\n", argv[1]);
        exit (1);
    }

    printf ("Opened %s\n", argv[1]);

    sprintf (MyFileName, "%s_Unique_Objects.txt", argv[1]);
    UniqueNames = fopen (MyFileName, "wt");
    if (!UniqueNames)
    {
        printf ("error at Unique_Objects file\n");
        exit (1);
    }

    printf ("Opened %s\n", MyFileName);
}


void uniqueObjects::Read_Signature()
{
    fread (sig,4,1,map);
    sig[4] = 0;
    fread(&MapSize,4,1,map);
    fread(&MapSize,4,1,map);

    if (MapSize > 4096)
    {
        printf("MapSize %i is too large! exiting!\n",MapSize);
        exit(1);
    };

    printf ("Signature: %s\nMapSize: %i\nReading elevations...", sig, MapSize);
}



void uniqueObjects::Read_Elevations()
{
    // read elevations
    int x = 0,z = 0;
    for (int zx = 0; zx < MapSize*MapSize; zx++)
    {
        fread(&wrp[x][z].Elevation,sizeof(wrp[x][z].Elevation),1,map);

        x++;
        if (x == MapSize)
        {
            z++; x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }

    printf(" Done\nReading Textures...");
}


void uniqueObjects::Read_Textures()
{
    // read textures IDs
    int x = 0, z = 0;
    TexIndex = 0;

    for (int tx = 0; tx < MapSize*MapSize; tx++)
    {
        wrp[x][z].TexIndex = 0;
        fread(&wrp[x][z].TexIndex,sizeof(wrp[x][z].TexIndex),1,map);

        x++;
        if (x == MapSize)
        {
            z++;
            x = 0;
        }
        if (z == MapSize)
        {
            z = 0;
        }
    }

    printf(" Done\nReading Texture names...");

    //textures 32 char length and total of 512
    for (int ix = 0; ix < 512; ix++)
    {
        sig[0] = 0;
        fread(sig,32,1,map);
    }

    printf(" Done\nReading 3dObjects...");
}


void uniqueObjects::Read_Objects()
{
    char dObjName[76],EmptyObject[76];
    float dDir, dDirX = 0, dDirZ = 0;

    // number of unique objects
    int MatchFound = 0;

    // clear the objname variable
    for (int ci = 0; ci < 76; ci++) EmptyObject[ci] = 0;

    while (!feof(map))
    {
        dDir = 1;
        fread(&dDir,4,1,map);
        dDir = 0;
        fread(&dDir,4,1,map);
        fread(&dDir,4,1,map);
        fread(&dDir,4,1,map);
        dDir = 1;
        fread(&dDir,4,1,map);
        dDir = 0;
        fread(&dDir,4,1,map);
        fread(&dDir,4,1,map);
        fread(&dDir,4,1,map);
        dDir = 1;
        fread(&dDir,4,1,map);

        // X coord
        fread(&dDirX,4,1,map);

        // here we do our magic for aling to ground
        fread(&dDir,4,1,map);

        // Z coord
        fread(&dDirZ,4,1,map);

        dObjIndex++;
        fread(&dObjIndex,4,1,map);
        // make it empty!
        strcpy(dObjName, EmptyObject);
        fread(dObjName,76,1,map);

        // start
        strlwr(dObjName);
        // debug disabled
        //printf ("new dObjName: %s\n", dObjName);

        MatchFound = 0;

        // if we just started...
        if (NumObjs == 0)
        {
            // copy the first object name into our array.
            strcpy(UniqueObjList[0], dObjName);
            // debug disabled
            //printf("\nNumObjs: %i. UniqueObjects[0]: %s\n", NumObjs, UniqueObjList[0]);
            // increment the number of objects by one.
            NumObjs++;
            MatchFound = 1;
        };

    //printf ("And now NumObjs: %i\n", NumObjs);
        // loop until all unique objects are checked
        for (int xi = 0; xi < NumObjs; xi++)
        {
            if (strcmp(dObjName, UniqueObjList[xi]) == 0)
            {
                MatchFound = 1;
                // debug disabled
                //printf("SKIP! match in dObjName, UniqueObjects[%i]: %s, %s\n\n", xi, dObjName, UniqueObjList[xi]);
            };
        };
    //this doesnt work, gives error: printf ("And after the for int i = 0 loop, the i is: %i\n", i);

        if (MatchFound == 0)
        {
            strcpy(UniqueObjList[NumObjs],dObjName);
            // debug disabled
            //printf("BRAND NEW OBJ: NumObjs: %i - dObjName: %s\n\n", NumObjs, dObjName);
    //			fprintf (UniqueNames, "%s\n", dObjName);
            NumObjs++;
        };
    };

    printf(" Done\nWe read %ld 3dObjects\n", dObjIndex);
}


void uniqueObjects::Write_UniqueNames()
{
    // loop until all unique objects are checked
    printf("No we will finish with a loop until all unique objects are checked...\n");
    for (int zi = 0; zi < NumObjs; zi++)
    {
        if (strlen (UniqueObjList[zi]) > 1)
        {
            // debug disabled
            //printf("UniqueObject[%i]: %s\n", zi, UniqueObjList[zi]);
            fprintf (UniqueNames, "%s\n", UniqueObjList[zi]);
        }
    }

    // display statistics
    printf("WRP has: %ld objects and %i unique objects.\n", dObjIndex, NumObjs);
}


void uniqueObjects::Close_Files()
{
    fclose(map);
    fclose(UniqueNames);

    printf("Files closed, exiting. Have a nice day.\n");
}
