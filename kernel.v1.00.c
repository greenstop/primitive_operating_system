#define smallBufferSize 80
#define sectorSize 512
#define maxFileSize 13312
#define NProcess 8
#define en; interrupt(0x10,0xe*256+'\n',0,0,0);
#define dl; interrupt(0x10,0xe*256+'\r',0,0,0);
#define cin readLine(0);
#define DB1 if(1)
#define DB2 if(1)
#define DB3 if(0)

void printString(char*);
void readLine(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void writeSector(char*, int);
void handleInterrupt21(int, int , int, int);
int readFile(char *, char*);
void printInt(char, int);
void executeProgram(char *);
void terminate();
void decimal_hex(int, char *);
void runShell();
void deleteFile(char *);
void printBufferInHex(char *, int);
void printChars(char, char, char, char, char, char);
void writeFile(char*, char*, int);
int matchFilenameInDirectory(char *, char[]);
void printValInHex(int);
void handleTimerInterrupt(int, int);
void processTableStatus();
void killProcess(int);
void wait(char *);

struct process {
    int active;
    int address;
    int waiting;
    int waitingOn;
    char name[16];
} processTable[NProcess];
int currentProcess = 0;
char smallCharArray[8];

int main(void) {

    int i;
    
    for (i=0; i <8; i++) {
       processTable[i].active = 0;
       processTable[i].waiting = 0;
       processTable[i].waitingOn = 0;
       processTable[i].address = 0xff00;
    }
    currentProcess = 0;

    makeTimerInterrupt();

    /*
    processTable[1].active = 0;
    processTable[1].address = 1234;
    printChars('P','_','T','a','b','l');
    en;dl;
    printValInHex(processTable[1].address); */

    runShell();
    while(1);
    return 1;
}

void runShell() {
    
    char line[6];
    line[0]='s';
    line[1]='h';
    line[2]='e';
    line[3]='l';
    line[4]='l';
    line[5]='\0';
    makeInterrupt21();
    interrupt(0x21, 4, line, 0, 0);
}

void clearMemory(int segment) {
    
    int i;
    
    for (i=0; i<sectorSize*2; i++) {
        putInMemory(segment, i, 0x0);
    }
}

void terminate() {

    int i;
    /*
    char line[6];
    line[0]='s';
    line[1]='h';
    line[2]='e';
    line[3]='l';
    line[4]='l';
    line[5]='\0';
    interrupt(0x21, 4, line, 0, 0);*/
    
    setKernelDataSegment();
    for(i=0; i < NProcess; i++)
        if(processTable[i].waitingOn == currentProcess)
            processTable[i].waiting = 0;
    i = currentProcess;
    restoreDataSegment();
    killProcess(i);

    while(1);
}

void executeProgram(char * name) {
    char buffer[maxFileSize];
    char out[smallBufferSize];
    char *outPtr;
    int i = 0, j=0;
    int numberOfSectors = 0;
    int condition = 1;
    int processNumber = 999;

    if(*name==0x0) {
        DB2 printChars('E','r','r','o','r','X');
        return;
    }
    
    for(i=0; *(name+i) != 0x0; i++) {
        out[i] = name[i];
    }
    out[i] = 0x0;
    printString(out);

    setKernelDataSegment();
    for(i=0; i < NProcess; i++) {
        DB2 printChars('L','o','o','k','_','P');/*
        DB2 printValInHex(i);
        DB2 printValInHex(processTable[i].active);
        DB2 printValInHex(processNumber);*/
        if(processTable[i].active == 0 && processTable[i].waiting != 1) {
            processNumber = i;
            for(j=0; out[j] != 0x0 && j < 16; j++) {
               processTable[i].name[j] = out[j]; 
            }
            processTable[i].name[j] = 0x0;
            break;
        }
    }
    restoreDataSegment();
    currentProcess=processNumber;

    if(processNumber == 999) { 
        /*DB2 printChars('E','r','r','o','r','P');*/
        return;
    }
    
    numberOfSectors = readFile(name, buffer);
    /*DB2 printInt('S', numberOfSectors);*/
    
    DB2 printChars('C','p','_','M','e','m');
    
    for(i = 0; i < (numberOfSectors*sectorSize); i++) {
        /*if( !(buffer[i-1] == 0x0 && buffer[i-2] == 0x0 && buffer[i-3] == 0x0 && i>2) || i <= 2) {
            decimal_hex(buffer[i], out);
            printString(out);
        }*/
        putInMemory((processNumber+2)*0x1000, i, buffer[i]);
    }
    

    DB2 printChars('E','x','c','P','r','g');
    DB2 printValInHex((processNumber+2)*0x1000);
    /*DB2 printValInHex(currentProcess);
    DB2 printChars('X','X','X','X','X','X');*/
    initializeProgram((processNumber+2)*0x1000);

    setKernelDataSegment();
    processTable[processNumber].active = 1;
    restoreDataSegment();
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
    
    switch(ax) {
        case 0:
            printString(bx);
            break;
        case 1:
            readLine(bx);
            break;
        case 2:
            readSector(bx,cx);
            break;
        case 3:
            readFile(bx, cx);
            break;
        case 4:
            executeProgram(bx);
            break;
        case 5:
            terminate();
            break;
        case 6:
            writeSector(bx,cx);
            break;
        case 7:
            deleteFile(bx);
            break;
        case 8:
            writeFile(bx, cx, dx);
            break;
        case 9:
            killProcess(bx);
            break;
        case 10:
            wait(bx);
            break;
        case 97:
            processTableStatus();
            break;
        case 98:
            printInt(bx, cx);
            break;
        case 99:
            decimal_hex(bx, cx);
            break;
    }
}

void printString(char *line) {
    
    int i;

    for(i =0; line[i] != '\0'; i++) {

        interrupt(0x10,0xe*256+line[i],0,0,0);
    }
}


int readFile(char *fileName, char *buffer) {
    
    int sectors = 0;
    int isSame = 1;
    char directoryBuffer[512];
    char * ptrDirectoryBuffer;
    char * ptrFileName;
    char * ptrBuffer;

    int i;

    DB2 printChars('R','d','_','B','u','f');
    
    if (*fileName == 0x0) {
        /*DB2 printChars('E','r','r','o','f','F');*/
        return 0;
    }
    
    interrupt(0x21, 2, directoryBuffer, 2, 0);
    
    ptrDirectoryBuffer = matchFilenameInDirectory(fileName, directoryBuffer);
    
    if(ptrDirectoryBuffer == 0x0){
        /*DB2 printChars('E','r','r','R','d','D');*/
        return 0;
    } /*else DB2 printChars('F','o','u','n','d','F');*/
    
    ptrDirectoryBuffer += 6;
    ptrBuffer = buffer;
    i = 0;
    
    /*DB2 printChars('L','d','_','M','e','m');*/
    while((*ptrDirectoryBuffer != 0x0) && isSame) {/*
        DB2 printChars('S','c','t','N','u','m');
        DB2 printValInHex(*ptrDirectoryBuffer);*/
        sectors++;
        ptrBuffer=buffer+i*512;
        interrupt(0x21, 2, ptrBuffer, *ptrDirectoryBuffer, 0);
        i++;
        ptrDirectoryBuffer++;
    }
    /*DB2 printChars('S','c','t','E','n','d');*/

    return sectors;
}

void printInt(char c, int i) {
    
    char line[6];
    line[0] = c;
    line[1] = '0'+div(i,100);
    line[2] = '0'+div(mod(i,100),10);
    line[3] = '0'+mod(i,10);
    line[4] = ' ';
    line[5] = 0x0;
    interrupt(0x21,0,line,0,0);
    
}

void readLine(char * line) {
    
    int i;
    char currentChar;
    int j;

    /*DB2 printChars('R','d','L','i','n','e');*/
    
    for(i = 0; i < smallBufferSize; i++) {
        currentChar = interrupt(0x16,0,0,0,0);
        interrupt(0x10, 0xe*256+currentChar, 0x0, 0x0, 0x0);
        
        if(i>=(smallBufferSize-1)) {
            line[i] = '\0';
            break;
        }

        if(currentChar==0xd) {
            line[i] = '\0';
            interrupt(0x10, 0xe*256+'\n', 0x0, 0x0, 0x0);
            break;
        }
        
        if(currentChar==0x7f || currentChar==0x8) {
            i=i-2;
        } else {
            line[i]=currentChar;
        }
    }
}

int mod(int a, int b) {
    while( a >= b) {
        a = a - b;
    }

    return a;
}

int div(int a, int b) {

    int q = 0;
    if(a == 0) return a;

    while( (q*b) <= a ) {
        q = q + 1;
    }
    return q-1;
}

void readSector(char* buffer, int sector) {
    int relativeSector = mod(sector,18)+1;
    int headNumber = mod(div(sector,18), 2);
    int trackNumber = div(sector,36);
    
    /*DB2 printChars('R','d','_','S','e','c');*/

    interrupt(0x13, 0x2*256+0x1, buffer, trackNumber*256+relativeSector,
        headNumber*256+0);
    /*DB2 printChars('R','d','X','S','e','c');*/

}

void writeSector(char* buffer, int sector) {
    int relativeSector = mod(sector,18)+1;
    int headNumber = mod(div(sector,18), 2);
    int trackNumber = div(sector,36);
    
    /*DB2 printChars('W','r','_','S','e','c');*/
    
    interrupt(0x13, 0x3*256+0x1, buffer, trackNumber*256+relativeSector,
              headNumber*256+0);
    
}

void deleteFile(char *fileName) {
    
    char out[smallBufferSize];
    char map[sectorSize];
    char dir[sectorSize];
    char * dirPtr;
    char * mapPtr;
    
    if (*fileName == 0x0) {
        /*DB2 printChars('E','r','r','R','m','F');*/
        return;
    }
    
    interrupt(0x21, 2, map, 1, 0);
    interrupt(0x21, 2, dir, 2, 0);
    
    dirPtr = matchFilenameInDirectory(fileName, dir);
    
    if(dirPtr == 0x0) {
        /*DB2 printChars('E','r','r','R','m','D');*/
        return;
    } /*else DB2 printChars('F','i','l','e','F','d');*/
    
    *dirPtr = 0x0; /*zeroize the first byte of filename in dir*/
    dirPtr = dirPtr+6;
    
    while(*dirPtr != 0x0) {
        
        mapPtr = map+*dirPtr;
        *mapPtr = 0x0;
        dirPtr++;
    }
    
    interrupt(0x21, 6, map, 1, 0);
    interrupt(0x21, 6, dir, 2, 0);
    /*DB2 printChars('D','e','l','_','O','K');DB2 en;DB2 dl;*/
    
}

int matchFilenameInDirectory(char *fileName, char dir[]) {
    
    char * fileNamePtr;
    char *dirPtr;
    int isSame = 1;
    int i;
    
    /*DB2 printChars('M','a','t','c','h','r');*/
    
    /*16 is the number of file entries at most in the directory sector*/
    for(i=0; i <16; i++) {
        
        dirPtr = dir+i*32;/*32 is the length of each entry*/
        fileNamePtr = fileName;
        isSame = 1;
        
        while(*fileNamePtr != '\0') {
            if(*fileNamePtr != *dirPtr) {
                isSame = 0;
                break;
            }
            fileNamePtr++;
            dirPtr++;
        }
        if(isSame) {
            dirPtr = dir+i*32;
            /*
            DB2 printChars('M','a','t','c','h','S');DB2 en;DB2 dl;
            printBufferInHex(dirPtr, 20);DB2 en;DB2 dl;*/
            break;
        }
    }
    
    if(isSame == 0) {
        /*DB2 printChars('F','N','o','t','F','d');DB2 en;DB2 dl;*/
        dirPtr = 0x0;
    }
    
    return dirPtr;
}

void writeFile(char *fileName, char*buffer, int numberOfSectors) {
    
    char out[smallBufferSize];
    char sectionToWrite[sectorSize];
    char map[sectorSize];
    char dir[sectorSize];
    char * dirPtr;
    char * mapPtr;
    char * fileNamePtr;
    char * sectionToWritePtr;
    int i, j, k;

    if (*fileName == 0x0) {
        /*DB2 printChars('E','r','r','o','W','F');*/
        return;
    }
    
    interrupt(0x21, 2, map, 1, 0);
    interrupt(0x21, 2, dir, 2, 0);
    
    dirPtr = dir;
    mapPtr = map;
    fileNamePtr = fileName;
    sectionToWritePtr = sectionToWrite;

    while (*dirPtr != 0x0) {/*find empty entry slot*/
        
        if (dirPtr == dir+17*32) {
            /*DB2 printChars('D','i','r','F','u','l');DB2 en;DB2 dl;*/
            return;
        }
        
        dirPtr+=32;
    }
    
    for (i=0; i<6; i++)
        dirPtr[i] = 0x0;
    
    for(i = 0; *fileNamePtr != '\0';fileNamePtr++,i++)
        dirPtr[i] = *fileNamePtr;
    
    dirPtr+=6;
    
    for (i=j=0; i < numberOfSectors; i++, dirPtr++) {/*each sector write 512byte piece of buffer*/
        while (*mapPtr != 0x0) {
            if (mapPtr == map+sectorSize+1) {
                /*DB2 printChars('M','a','p','F','u','l');DB2 en;DB2 dl;*/
                return;
            }
            mapPtr++;
            j++;
        }
        *dirPtr=j;
        *mapPtr = 0xFF;
        
        for (k = sectorSize*i, sectionToWritePtr = sectionToWrite;
             k < (sectorSize*(i+1));
             k++, sectionToWritePtr++) {

            *sectionToWritePtr = buffer[k];
        }

        interrupt(0x21, 6, sectionToWrite, j, 0);
    }

    interrupt(0x21, 6, map, 1, 0);
    interrupt(0x21, 6, dir, 2, 0);
}

void printChars(char a, char b, char c, char d, char e, char f) {
    interrupt(0x10,0xe*256+a,0,0,0);
    interrupt(0x10,0xe*256+b,0,0,0);
    interrupt(0x10,0xe*256+c,0,0,0);
    interrupt(0x10,0xe*256+d,0,0,0);
    interrupt(0x10,0xe*256+e,0,0,0);
    interrupt(0x10,0xe*256+f,0,0,0);
    interrupt(0x10,0xe*256+' ',0,0,0);
}

void printBufferInHex(char *in, int length) {
    char buffer[smallBufferSize];
    int i;
    
    for (i=0; i<length; i++, in++) {
        interrupt(0x21, 99, *in, buffer,0);
        interrupt(0x21, 0, buffer, 0, 0);
    }
}

void printValInHex(int n) {
    
    char out[16];
    
    decimal_hex(n, out);
    printString(out);
    
}

void decimal_hex(int n, char *hex) /* Function to convert decimal to hexadecimal. */
{
    int i=0,rem, j, k, endIndex;
    char a;
    
    if(n == 0) {
        hex[i] = '0';
        i++;
    }
    
    while (n!=0)
    {
        rem=mod(n,16);
        /*printInt('m',rem);*/
        switch(rem)
        {
            case 10:
                hex[i]='A';
                break;
            case 11:
                hex[i]='B';
                break;
            case 12:
                hex[i]='C';
                break;
            case 13:
                hex[i]='D';
                break;
            case 14:
                hex[i]='E';
                break;
            case 15:
                hex[i]='F';
                break;
            default:
                hex[i]='0'+rem;
                break;
        }
        i++;
        n= div(n,16);
        /*printInt('n',n);*/
    }
    
    hex[i] = ' ';
    i++;
    hex[i]='\0';
    
    /* Reverse string */
    endIndex = i-2;
    i = endIndex;
    
    for (j=0; j < endIndex; j++) {
        
        for (k=0; k < i; k++) {
            
            a = hex[k];
            hex[k] = hex[k+1];
            hex[k+1] = a;
        }
        i--;
    }
}
/*void pressEnterToContinue() {
    printChars('P','r','e','s','s','E');
    cin;
}*/

void handleTimerInterrupt(int segment, int sp) {

    int i, loop=1;
    
    /*
    printString("Tic\n\r\0");
    printChars('T','i','c','T','o','c');
    en;dl;
    DB3 printChars('K','_','_','_','_','_');
    DB3 printValInHex(segment);*/

    setKernelDataSegment();
    if(segment != 0x1000) processTable[currentProcess].address = sp;
    /*else DB3 printChars('1','0','0','0','X','X');*/
    /*DB3 printValInHex(processTable[currentProcess].active);
    if(processTable[currentProcess].active == 1) {
        processTable[currentProcess].address = sp;
        if( processTable[currentProcess].address == sp)
            printChars('Y','Y','Y','X','X','X');
        if( segment != 0x1000)
            printChars('1','0','0','0','X','X');
    }*/
    if(processTable[currentProcess].active == 0 && segment != 0x1000) {
        processTable[currentProcess].address = 0xff00;
    }

    for(i = currentProcess + 1; i < NProcess; i++) {
        if(processTable[i].active == 1) {
            /*DB3 printValInHex(i);
            DB3 printChars('Y','Y','Y','X','X','X');*/
            if(processTable[i].waiting == 1) continue;
            currentProcess = i;
            segment = (i+2)*0x1000;
            sp = processTable[i].address;
            loop = 0;
            break;
        }
    }
    if(loop) {
        for(i = 0; i < currentProcess+1 ; i++) {
            if(processTable[i].active == 1) {
                /*DB3 printValInHex(i);
                DB3 printChars('Z','Z','Z','X','X','X');*/
                if(processTable[i].waiting == 1) continue;
                currentProcess = i;
                segment = (i+2)*0x1000;
                sp = processTable[i].address;
                break;
            }
        }
    }
    restoreDataSegment();
    /*DB3 printValInHex(segment);*//*
    if(sp == 0xff00) { 
        DB3 printChars('f','f','0','0','X','X');
    }*/

    returnFromTimer(segment, sp);

}

void processTableStatus() {
    
    int i;

    setKernelDataSegment();
    /*printChars('P','r','o','c','T','b');*/
    en;dl;
    for(i=0; i < NProcess; i++) {
        
        printValInHex(i);
        printValInHex(processTable[i].active);
        if(processTable[i].address == 0xff00)
            printValInHex(1);
        else printValInHex(0);
        printValInHex(processTable[i].waiting);
        /*printValInHex(processTable[i].waitingOn);
        printString(processTable[i].name);*/
        en;dl;
    }
    restoreDataSegment();
}

void killProcess(int id) {

    if(id < 0 || id > 7) return;
    setKernelDataSegment();
    processTable[id].active = 0;
    restoreDataSegment();
}

void wait(char *fileName) {
    
    int i,j,b=1, addr;
    char tempBuffer[16];
    char *ptr;
    
    for(i=0; i < 16; i++) {
        tempBuffer[i] = fileName[i];
    }
    addr = tempBuffer;
    setKernelDataSegment();
    ptr = addr;
    for(i=0; i < NProcess; i++) {
        for(j=0; processTable[i].name[j] != 0x0; j++) {
            printChars(processTable[i].name[j],'_',ptr[j],'W','T','b');/*tempBuffer is blank due to kernel DS segment shift*/
            if( processTable[i].name[j] != ptr[j]) {
                b = 0;
                break;
            }
        }
    }
    if(b == 1){
        processTable[currentProcess].waitingOn = i;
        processTable[currentProcess].waiting = 1;
    }     
    restoreDataSegment();
}




