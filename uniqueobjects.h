#ifndef UNIQUEOBJECTS_H
#define UNIQUEOBJECTS_H

#include <QtCore/QCoreApplication>

typedef struct
{
    short Elevation;
    short TexIndex;
} wrpformat[4096][4096];

class uniqueObjects
{
public:
    uniqueObjects(int argc, char *argv[]);
    void Open_Files(int argc, char *argv[]);
    void Read_Signature();
    void Read_Elevations();
    void Read_Textures();
    void Read_Objects();
    void Write_UniqueNames();
    void Close_Files();

private:
    FILE *map;
    FILE *UniqueNames;
    char sig[33],MyFileName[80];
    short TexIndex;
    int MapSize, NumObjs;
    // list of unique objects
    char UniqueObjList[4096][76];
    long dObjIndex;
};

#endif // UNIQUEOBJECTS_H
