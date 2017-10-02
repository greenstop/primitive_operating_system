#define lineBufferSize 80
#define sectorSize 512
#define maxFileSize 13312
#define endl interrupt(0x21, 0, "\n\r\0", 0, 0)
#define DEBUG 1
#define print(X) interrupt(0x21, 0, X, 0, 0)
#define DB1 if(0)
#define DB2 if(1)
#define DB3 if(1)
#define cin interrupt(0x21, 1, 0, 0, 0)
#define nCommands 10

int stringEquals(char *, char *);
void stringSubset(char *, char *, int , int );
void printBufferInHex(char *, int);
void trimTrailingSpace(char *, char *);
void extractOptions(char *, char *);
int stringLength(char *);
int splitString(char *, char, int *);
void printArrayOfStringAdd(int *);
void printHexValue(int);
int estimateSectorSize(char *);
int mod(int, int);
int div(int, int);
void listDirectory();
void pressEnterToContinue();
void create(char*, char*);
int atoi(char *); 

int main()
{
    
    char line[80];
    int commands[nCommands];
    int splittedStrings[8];
    int i = 999, j;
    char out[512];
    char buffer[maxFileSize];

    enableInterrupts();
    
    commands[0] = "test\0";
    commands[1] = "cat\0";
    commands[2] = "open\0";
    commands[3] = "rm\0";
    commands[4] = "cp\0";
    commands[5] = "ls\0";
    commands[6] = "create\0";
    commands[7] = "ps\0";
    commands[8] = "kill\0";
    commands[9] = "fg\0";
    
	DB2 interrupt(0x21, 0, "Starting\n\r\0", 0, 0);
    
	while(1) {
        
        interrupt(0x21, 0, "Shell>\0", 0, 0);
        interrupt(0x21, 1, line, 0, 0);
        /*DB2 interrupt(0x21, 0, line, 0, 0);*/
        *out = 0x0;
        
        /*
        i = stringEquals(commands[2], line);
        
        if ( i == 999) interrupt(0x21, 0, "cmd not matched\n\r\0", 0, 0);
        if ( i == 0) interrupt(0x21, 0, "cmd matched\n\r\0", 0, 0);
        if ( i > 0) interrupt(0x21, 0, "cmd mismatch >0\n\r\0", 0, 0);
        if ( i < 0) interrupt(0x21, 0, "cmd mismatch <0\n\r\0", 0, 0);
         */
        
        /*if(line[0] == 'q') interrupt(0x21, 4, "tstpr2\0", 0x6000, 0);*/
        
        for (i=0; i< nCommands; i++) {
            if ( stringEquals(commands[i], line) == 0 ) {
                
                /*DB2 interrupt(0x21, 0, "cmd matched \0", 0, 0);*/
                switch(i)
                {
                    case 0:
                        DB2 interrupt(0x21, 0, "cmd Test Mode\n\r\0", 0, 0);
                        /*pressEnterToContinue();
                        splitString(line, ' ', splittedStrings);
                        printArrayOfStringAdd(splittedStrings);*/
                        for(j=0; j < 10000; j++)
                            interrupt(0x21,0,"XXXXXTESTXXXXXX", 0, 0);
                        break;
                    case 1:
                        DB2 interrupt(0x21, 0, "cmd cat\n\r\0", 0, 0);
                        extractOptions(commands[i], line);
                        interrupt(0x21, 3, line, out, 0, 0);
                        interrupt(0x21, 0, out, 0, 0);
                        break;
                    case 2:
                        DB2 interrupt(0x21, 0, "cmd open\n\r\0", 0, 0);
                        extractOptions(commands[i], line);
                        
                        DB2 interrupt(0x21, 0, "file name:\n\r\0", 0, 0, 0);
                        DB2 printBufferInHex(line, 20);
                        DB2 interrupt(0x21, 0, "\n\rfile name:\n\r\0", 0, 0, 0);
                        DB2 interrupt(0x21, 0, line, 0, 0, 0);
                        DB2 endl;
                        
                        interrupt(0x21, 4, line, 0, 0);
                        break;
                    case 3:
                        DB2 interrupt(0x21, 0, "cmd delete\n\r\0", 0, 0);
                        extractOptions(commands[i], line);
                        interrupt(0x21, 7, line, 0, 0, 0);
                        break;
                    case 4:
                        DB2 print("cmd copy\n\r\0");
                        splitString(line,' ', splittedStrings);
                        interrupt(0x21, 3, splittedStrings[1], buffer, 0);
                        interrupt(0x21, 8, splittedStrings[2], buffer,
                                  estimateSectorSize(buffer));
                        break;
                    case 5:
                        DB2 print("cmd ls");DB2 endl;
                        listDirectory();
                        break;
                    case 6:
                        DB2 print("cmd create");endl;
                        
                        DB3 print("Entered Test\n\r\0");
                        DB3 printBufferInHex(line, 20);
                        DB3 endl;

                        splitString(line,' ', splittedStrings);
                        create(splittedStrings[1], line);
                        break;
                    case 7:
                        DB2 print("cmd ps\n\r\0");
                        interrupt(0x21, 97, 0, 0, 0);
                        break;
                    case 8:
                        DB2 print("cmd kill\n\r\0");
                        splitString(line,' ', splittedStrings);
                        DB2 printHexValue(atoi(splittedStrings[1]));
                        interrupt(0x21, 9, atoi(splittedStrings[1]), 0, 0);
                        break;
                    case 9:
                        DB2 print("cmd fg\n\r\0");
                        splitString(line,' ', splittedStrings);
                        interrupt(0x21, 10, splittedStrings[1], 0, 0);
                        break;
                }
                
                break;
            }
        }
    }
    
    DB2 interrupt(0x21, 0, "Exiting Shell\n\r\0", 0, 0);
    interrupt(0x21, 5, 0, 0, 0);
}

int atoi(char *str) {
    
    int res=0, i;
    for(i =0; str[i] != '\0'; i++) 
        res=res*10+str[i]-'0';     
    return res; 
}

int stringEquals(char * a, char * b) {
    
    /*
    interrupt(0x21, 0, "stringEquals\n\r\0", 0, 0, 0);
    interrupt(0x21, 0, "a: \0", 0, 0, 0);
    interrupt(0x21, 0, a, 0, 0, 0);
    interrupt(0x21, 0, "\n\r\0", 0, 0, 0);
    interrupt(0x21, 0, "b: \0", 0, 0, 0);
    interrupt(0x21, 0, b, 0, 0, 0);
    interrupt(0x21, 0, "\n\r\0", 0, 0, 0);
    */
    
    for ( ;*a == *b || *b == ' '; a++, b++)
        if (*a == '\0')
            return 0;
    
    return *a - *b;
}

void create( char*fileName, char *line) {
    char buffer[maxFileSize];
    char fileNameTrunc[7];
    char *lineBeginning;
    char *fnPtr, *bufferPtr;
    int lines = 0, i = 0;
    
    lineBeginning = line;
    fnPtr = fileNameTrunc;
    bufferPtr = buffer;
   
    DB3 print("filename\n\r\0");
    DB3 printBufferInHex(fileName, 20);
    DB3 endl;
    DB3 print(fileName);
    DB3 endl;
    DB3 print("line\n\r\0");
    DB3 printBufferInHex(line, 20);
    DB3 endl;
    DB3 print("Pointers\n\r\0");
    DB3 printHexValue(*fnPtr);
    DB3 endl;
    DB3 pressEnterToContinue();
    
    for(; *fileName != 0x0; fnPtr++, fileName++) {
        DB3 printHexValue(*fnPtr);
        DB3 printHexValue(*fileName);
        DB3 endl;
        *fnPtr = *fileName;
        DB3 printHexValue(*fnPtr);
        DB3 printHexValue(*fileName);
        DB3 endl;
    }
    *(fileNameTrunc+6) = 0x0;
       
    DB3 print("filenameTrunc\n\r\0");
    DB3 printBufferInHex(fileNameTrunc, 20);
    DB3 endl;
    DB3 print(fileNameTrunc);
    DB3 endl;
    DB3 print("line\n\r\0");
    DB3 printBufferInHex(line, 20);
    DB3 endl;
    DB3 pressEnterToContinue();

    while (1) {
        lines++;
        for(i=0; i < lineBufferSize; i++)
            line[i]=0x0;
        print("Enter text line:\n\r\0");
        interrupt(0x21, 1, line, 0, 0);

        if(*line == 0x0) break;

        DB3 printBufferInHex(line, 20);
        DB3 endl;
        DB3 pressEnterToContinue();

        for (; *line != 0x0; line++, bufferPtr++)
            *bufferPtr=*line;

        *bufferPtr='\n';
        bufferPtr++;
        *bufferPtr='\r';
        bufferPtr++;

        line=lineBeginning;
        if(bufferPtr == buffer+maxFileSize)
            break;
    }


    DB3 printBufferInHex(buffer, 90);
    DB3 endl;
    DB3 print("Number of Sectors: \n\r\0");
    DB3 printHexValue(lines);
    DB3 printHexValue(div(lines*lineBufferSize,sectorSize)+1);
    DB3 endl;
    DB3 pressEnterToContinue();
    *bufferPtr = 0x0;
    if(*buffer == 0x0) {
        print("File Empty\n\r\0");
        return;
    }
    
    interrupt(0x21, 8, fileNameTrunc, buffer, div(lines*lineBufferSize,sectorSize)+1);
    
}

void listDirectory() {
    char dir[512];
    char *dirPtr, *fileNamePtr;
    int i;
    
    interrupt(0x21, 2, dir, 2, 0);
    print("Listing..");endl;
    dirPtr = dir;
    
    while (dirPtr != dir+512) {
        fileNamePtr = dirPtr;
        print("[]: ");
        for ( i=fileNamePtr+6; *fileNamePtr != 0x0 && fileNamePtr != i; fileNamePtr++) 
            interrupt(0x10,0xe*256+*fileNamePtr,0,0,0);
        dirPtr+=32;endl;
    }
}

int estimateSectorSize(char *in) {
    int n=0;
    
    while (in[n] > 0) {/*max number of files*/
        n+=512;
    }
    return div(n,512);
}

void extractOptions(char *cmd, char *in) {
    int lengthCmd = stringLength(cmd);
    int lengthfileName = stringLength(in);
    
    stringSubset(in, in, lengthCmd, lengthfileName);
    trimTrailingSpace(in, in);
    
}

int splitString(char *in, char delimiter, int * splitted) {
    
    int length = stringLength(in);
    int i = 0, j = 0;
    char * startOfString;
    char out[8];
    
    trimTrailingSpace(in,in);
    startOfString = in;
    
    DB1 interrupt(0x21, 0, "Splitting string\n\r\0", 0, 0, 0);
    DB1 interrupt(0x21, 0, "line:\n\r\0", 0, 0, 0);
    DB1 interrupt(0x21, 0, in, 0, 0, 0);
    DB1 endl;
    DB1 printBufferInHex(in, 20);
    DB1 endl;
    
    while(*in != 0x0) {
        while (*in != delimiter && *in != 0x0) {
            
            DB1 printHexValue(*in);
            
            i++;
            in++;
        }
        DB1 interrupt(0x21, 0, " - up to delimiter\n\r\0", 0, 0, 0);
        DB1 printHexValue(*in);
        DB1 interrupt(0x21, 1, out, 0, 0);
        
        if (*in == delimiter || *in == 0x0) {
            
            *in=0x0;
            
            DB1 printHexValue(*in);
            DB1 print("Before, splitted[j]: ");
            DB1 printBufferInHex(splitted[j], 10);
            DB1 endl;
            DB1 print("startOfString:");
            DB1 printBufferInHex(startOfString,10);
            DB1 endl;
            
            
            splitted[j] = startOfString;
            
            DB1 print("After, splitted[j]: ");
            DB1 printBufferInHex(splitted[j], 10);
            DB1 endl;
            DB1 print(splitted[j]);
            DB1 endl;
            
            j++;
            in++;
        }
        DB1 interrupt(0x21, 0, "- set variables\n\r\0", 0, 0, 0);
        DB1 printHexValue(*in);
        DB1 interrupt(0x21, 1, out, 0, 0);
        
        while (*in == delimiter) {
            DB1 printHexValue(*in);
            
            in++;
        }
        startOfString = in;
        
        DB1 interrupt(0x21, 0, "- Next line\n\r\0", 0, 0, 0);
        DB1 printHexValue(*in);
        DB1 interrupt(0x21, 1, out, 0, 0);
    }
    
    splitted[j] = 0x0;
    return j;
    
}

void printArrayOfStringAdd(int * strings) {
    
    interrupt(0x21, 0, "Printing Array of String addresses\n\r\0", 0, 0, 0);
    while (*strings != 0x0) {
        interrupt(0x21, 0, *strings, 0, 0, 0);
        endl;
        strings++;
    }
}

int stringLength(char *s) {
    
    int i=0;
    char out[8];
    
    /*
    interrupt(0x21, 0, "stringLength\n\r\0", 0, 0, 0);
    printBufferInHex(s, 20);
    interrupt(0x21, 0, "\n\r\0", 0, 0, 0);
     */
    while (*s != 0x0) {
        /*
        interrupt(0x21, 99, *s, out,0);
        interrupt(0x21, 0, out, 0, 0);
         */
        i++;
        s++;
    }
    
    /*
    interrupt(0x21, 0, "\n\r\0", 0, 0, 0);
     */
    return i;
}

void stringSubset(char *in, char *out, int beginning, int end) {
    
    int i;
    if(end > stringLength(in)) end = stringLength(in);
    
    for (i = beginning; i < end; i++, out++) {
        *out = in[i];
    }
    *out = 0x0;
}

void trimTrailingSpace(char *in, char *out) {
    int length = stringLength(in);
    char *inBeginning = in;
    char printOut[8];
    
    /*interrupt(0x21, 0, "trimTrailingSpace\0", 0, 0, 0);*/
    if (length == 0) return;
    
    while (*inBeginning == ' ') inBeginning++;
    in = in+(length-1);
    while (*in == ' ') *in=0x0;
    
    for (; *inBeginning != 0x0; inBeginning++, out++) {
        /*
        interrupt(0x21, 0, "[\0", 0, 0, 0);
        interrupt(0x21, 99, *inBeginning, printOut,0);
        interrupt(0x21, 0, printOut, 0, 0);
        interrupt(0x21, 99, *out, printOut,0);
        interrupt(0x21, 0, printOut, 0, 0);
        interrupt(0x21, 0, ", \0", 0, 0, 0);*/
        
        *out = *inBeginning;
        
        /*
        interrupt(0x21, 99, *inBeginning, printOut,0);
        interrupt(0x21, 0, printOut, 0, 0);
        interrupt(0x21, 99, *out, printOut,0);
        interrupt(0x21, 0, printOut, 0, 0);
        interrupt(0x21, 0, "] \0", 0, 0, 0);*/
    }
    *out = 0x0;
    
    /*
    interrupt(0x21, 0, "Ending\n\r\0", 0, 0, 0);
    interrupt(0x21, 99, *out, printOut,0);
    interrupt(0x21, 0, printOut, 0, 0);
    interrupt(0x21, 0, "\n\r\0", 0, 0, 0);*/
}

void printBufferInHex(char *in, int length) {
    char buffer[lineBufferSize];
    int i;
    
    for (i=0; i<length; i++, in++) {
        interrupt(0x21, 99, *in, buffer,0);
        interrupt(0x21, 0, buffer, 0, 0);
    }
}

void printHexValue(int n) {
    
    char out[8];
    
    interrupt(0x21, 99, n, out,0);
    interrupt(0x21, 0, out, 0, 0);
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

void pressEnterToContinue() {
    print("Press Enter");cin;
}













