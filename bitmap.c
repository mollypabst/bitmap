Purpose: This program allows
 a user to operate on bitmap
 images. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RGB_COMPONENT_COLOR 255

// GLOBAL VARIABLES
char Matrix[3][3] = {{ 0,-1, 0}, {-1, 4,-1},
    { 0,-1, 0} };


// STRUCTURES
struct HEADER
{   unsigned short int Type;        /* Magic identifier       */
    unsigned int Size;              /* File size in bytes     */
    unsigned short int Reserved1, Reserved2;
    unsigned int Offset;            /* Offset to data (in B)  */
} Header;                           /* -- 14 Bytes --         */

struct INFOHEADER
{  unsigned int Size;               /* Header size in bytes    */
    int Width, Height;              /* Width / Height of image */
    unsigned short int Planes;      /* Number of colour planes */
    unsigned short int Bits;        /* Bits per pixel          */
    unsigned int Compression;       /* Compression type        */
    unsigned int ImageSize;         /* Image size in bytes     */
    int xResolution, yResolution;   /* Pixels per meter        */
    unsigned int Colors;            /* Number of colors        */
    unsigned int ImportantColors;   /* Important colors        */
} InfoHeader;                       /* -- 40 Bytes --          */

struct PIXEL
{   unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
} Pixel;

// PROTOTYPES
unsigned char *bitmap(FILE *fp, struct INFOHEADER *infoHeader);
void edge(struct PIXEL ***before, struct PIXEL ***after, char *inputName);
void shade(struct PIXEL ***before, struct PIXEL ***after, char *inputName, int red, int green, int blue);

// FUNCTIONS
// this function reads in the header and info header structure variables and prints them out
unsigned char *bitmap(FILE *fp, struct INFOHEADER *infoHeader)
{
    struct HEADER fileHeader;
    unsigned char *imageData;
    
    // reads file header variables
    fread(&Header.Type, 2, 1, fp);
    fread(&Header.Size, 4, 1, fp);
    fread(&Header.Reserved1, 2, 1, fp);
    fread(&Header.Reserved2, 2, 1, fp);
    fread(&Header.Offset, 4, 1, fp);
    
    // reads information header variables
    fread(infoHeader, sizeof(struct INFOHEADER), 1, fp);
    fseek(fp, fileHeader.Offset, SEEK_SET);
    
    // allocates space to put pixels into
    imageData = (unsigned char*) malloc(infoHeader->ImageSize);
    
    // reads image size
    fread(imageData, infoHeader->ImageSize, 1, fp);
    
    // prints out data
    printf("File type          = %hd\n", Header.Type);
    printf("File size          = %d bytes\n", Header.Size);
    printf("Data offset        = %u bytes\n", Header.Offset);
    printf("Reserved1          = %u\n", Header.Reserved1);
    printf("Reserved2          = %u\n", Header.Reserved2);
    printf("Info header size   = %d bytes\n", infoHeader->Size);
    printf("Width              = %d pixels\n", infoHeader->Width);
    printf("Height             = %d pixels\n", infoHeader->Height);
    printf("Planes             = %d\n", infoHeader->Planes);
    printf("Bit count          = %d bits/pixel\n", infoHeader->Bits);
    printf("Compression        = %d\n", infoHeader->Compression);
    printf("Size image         = %d bytes\n", infoHeader->ImageSize);
    printf("X pixels per meter = %d\n", infoHeader->xResolution);
    printf("Y pixels per meter = %d\n", infoHeader->yResolution);
    printf("Color used         = %u colors\n", infoHeader->Colors);
    printf("Important Color    = %d color\n", infoHeader->ImportantColors);
    
    // set cursor to beginning of file
    rewind(fp);
    
    return imageData;
 }

// this function serves as an "edge detector" and changes each pixel corresponding to the matrix
void edge(struct PIXEL ***before, struct PIXEL ***after, char *inputName){
    FILE *fp;
    int height, width, h, w;
    char name[256] = {'\0'};
    
    // appends (edge).bmp to output file
    strcpy(name, inputName);
    strcat(name, "(edge).bmp");
    
    fp = fopen(name, "wb");
    
    // writes header to output file
    fwrite(&Header, 14, 1, fp);
    fwrite(&InfoHeader, 40, 1, fp);
    
    // checks for edge
    for(height = 0; height < InfoHeader.Height; height++){
        for(width = 0; width < InfoHeader.Width; width++){
            if(height==0 || height == InfoHeader.Height-1 || width == 0 || width == InfoHeader.Width-1){
                after[height][width]->Red = before[height][width]->Red;
                after[height][width]->Green = before[height][width]->Green;
                after[height][width]->Blue = before[height][width]->Blue;
            }
            // multiplies by corresponding matrix value since edge was checked for
            else{
                for(h = 0; h < 3; h++){
                    for(w = 0; w < 3; w++){
                        after[height][width]->Red += before[height+h-1][width+w-1]->Red * Matrix[h][w];
                        after[height][width]->Green += before[height+h-1][width+w-1]->Green * Matrix[h][w];
                        after[height][width]->Blue += before[height+h-1][width+w-1]->Blue * Matrix[h][w];
                    }
                }
            }
            // writes new pixels to output file
            fwrite(&after[height][width]->Blue, 1, 1, fp);
            fwrite(&after[height][width]->Green, 1, 1, fp);
            fwrite(&after[height][width]->Red, 1, 1, fp);
        }
    }
    fclose(fp);
}

// this function changes the shade of each pixel based on user input
void shade(struct PIXEL ***before, struct PIXEL ***after, char *inputName, int red, int green, int blue){
    FILE *fp;
    int height=0, width=0;
    char name[256] = {'\0'};
    
    // appends (shade).bmp to output file
    strcpy(name, inputName);
    strcat(name, "(shade).bmp");
    
    // opens output file
    fp = fopen(name, "wb");
    
    // writes header to output file
    fwrite(&Header, 14, 1, fp);
    fwrite(&InfoHeader, 40, 1, fp);
    
    // transforms the pixels
    for(height = 0; height < InfoHeader.Height; height++){
        for(width = 0; width < InfoHeader.Width; width++){
            // adds input shade values to corresponding pixel color
            after[height][width]->Red = before[height][width]->Red + red;
            after[height][width]->Green = before[height][width]->Green + green;
            after[height][width]->Blue = before[height][width]->Blue + blue;
            
            // make sure each value is between 0 and 255
            if((before[height][width]->Red < after[height][width]->Red) && red < 0){
                after[height][width]->Red = 0;
            }
            else if ((before[height][width]->Red > after[height][width]->Red) && red >0){
                after[height][width]->Red = RGB_COMPONENT_COLOR;
            }
            
            if((before[height][width]->Green < after[height][width]->Green) && green < 0){
                after[height][width]->Green = 0;
            }
            else if ((before[height][width]->Green > after[height][width]->Green) && green >0){
                after[height][width]->Green = RGB_COMPONENT_COLOR;
            }
            
            if((before[height][width]->Blue < after[height][width]->Blue) && blue < 0){
                after[height][width]->Blue = 0;
            }
            else if ((before[height][width]->Blue > after[height][width]->Blue) && blue >0){
                after[height][width]->Blue = RGB_COMPONENT_COLOR;
            }
            
            // writes new pixel value to output file
            fwrite(&after[height][width] ->Blue, 1, 1, fp);
            fwrite(&after[height][width] ->Green, 1, 1, fp);
            fwrite(&after[height][width] ->Red, 1, 1, fp);
        }
    }
    fclose(fp);
}


int main(int argc, const char * argv[]) {
    struct INFOHEADER infoHeader;
    struct PIXEL ***before, ***after;
    int height, width, red, green, blue;
    char output[256] = {'\0'};
    FILE* fp;
    
    // makes output file name the same as name of second command line argument
    strncpy(output, argv[2], (strlen(argv[2]) - 4));
    
    // open input file and make sure it exists
    if((fp = fopen(argv[1], "rb")) == NULL){
        printf("Cannot open file: %s\n\n", argv[1]);
    }
    
    // get user input of shade
    sscanf(argv[3], "%i", &red);
    if(red < -200 || red > 200){
        printf("Please enter a number between -200 and 200.\n");
        exit(1);
    }
    
    sscanf(argv[4], "%i", &green);
    if(green < -200 || green > 200){
        printf("Please enter a number between -200 and 200.\n");
        exit(1);
    }
    
    sscanf(argv[5], "%i", &blue);
    if(blue < -200 || blue > 200){
        printf("Please enter a number between -200 and 200.\n");
        exit(1);
    }
    
    // print header information
    bitmap(fp, &infoHeader);
    
    // read header
    fread(&Header, 14, 1, fp);
    fread(&InfoHeader, 40, 1, fp);
    
    // allocate memory
    before = (struct PIXEL ***)calloc(InfoHeader.Height, sizeof(struct PIXEL *));
    after = (struct PIXEL ***)calloc(InfoHeader.Height, sizeof(struct PIXEL*));
    
    // reads in initial pixel value
    for(height = 0; height < InfoHeader.Height; height++){
        before[height] = (struct PIXEL **)calloc(InfoHeader.Width, sizeof(struct PIXEL *));
        after[height] = (struct PIXEL **)calloc(InfoHeader.Width, sizeof(struct PIXEL *));
        for(width = 0; width < InfoHeader.Width; width++){
            before[height][width] = (struct PIXEL *)calloc(1, sizeof(struct PIXEL));
            after[height][width] = (struct PIXEL *)calloc(1, sizeof(struct PIXEL));
            fread(&before[height][width]->Blue, 1, 1, fp);
            fread(&before[height][width]->Green, 1, 1, fp);
            fread(&before[height][width]->Red, 1, 1, fp);
        }
    }
    
    // call edge and shade function
    edge(before, after, output);
    shade(before, after, output, red, green, blue);
    
    fclose(fp);
    
    return 0;
}
