#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <stdint.h>
#include "md5.h"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
#define LENGTH 1 




cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue command_queue = NULL;
cl_mem memobj = NULL;
cl_program program = NULL;
cl_kernel kernel = NULL;
cl_platform_id *platforms = NULL;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;
char *source_str;
size_t source_size;

cl_device_id *devices;

void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}

void cyan() {
  printf("\033[0;36m");
}

void white() {
  printf("\033[0;37m");

}

void reset () {
  printf("\033[0m");
}


void GPU()
{


char string[MEM_SIZE];
 
FILE *fp;
char fileName[] = "./pwdCrack.cl";

fp = fopen(fileName, "r");
if (!fp) {
fprintf(stderr, "Failed to load kernel.\n");
exit(1);
}

source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
fclose(fp);
ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)*ret_num_platforms);
clGetPlatformIDs(ret_num_platforms, platforms, &ret_num_platforms);

char re[1024];


/* Get Platform and Device Info */
ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices);

devices = (cl_device_id*)malloc(sizeof(cl_device_id)*ret_num_devices);

clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, ret_num_devices, devices, &ret_num_devices);

char dvName[1024];
int i;

for(i = 0; i<3; i++){
    clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 1024, dvName, NULL);
    printf("[%d] Device name: ", i);
    white();
    printf("%s", dvName);
    reset();
    printf("\n");

}
int dvNumber;

printf("\n[*] Choose device from above: ");
scanf("%d", &dvNumber);


if(dvNumber == -1){
    exit(2);
}
device_id = devices[dvNumber];

}





int  startGPU(int totalSize, int wordList[], int found[], int md5IntBytes[])
{
   
context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

cl_event timing_event;
cl_ulong time_start, time_end, read_time;

/* Create Command Queue */
command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
cl_mem pwdmemObj = NULL;
cl_mem foundObj = NULL;
cl_mem foundBool = NULL;
cl_mem md5IntObj = NULL;

int pwdSize = totalSize;
int foundSize = 16;
int md5intSize = 17;

pwdmemObj = clCreateBuffer(context, CL_MEM_READ_WRITE, pwdSize * sizeof(int), NULL, &ret);
foundObj = clCreateBuffer(context, CL_MEM_READ_WRITE, foundSize * sizeof(int), NULL, &ret);
foundBool = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
md5IntObj = clCreateBuffer(context, CL_MEM_READ_WRITE, md5intSize* sizeof(int), NULL, &ret);



ret = clEnqueueWriteBuffer(command_queue, pwdmemObj, CL_TRUE, 0, pwdSize *sizeof(int),wordList, 0, NULL, NULL);
ret = clEnqueueWriteBuffer(command_queue, md5IntObj, CL_TRUE, 0, md5intSize *sizeof(int),md5IntBytes, 0, NULL, NULL);


/* Create Kernel Program from the source */
program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
(const size_t *)&source_size, &ret);


/* Build Kernel Program */
ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
kernel = clCreateKernel(program, "findMD5", &ret);


ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&pwdmemObj);
//ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&outObj);
ret =  clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&foundObj);
ret =  clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&foundBool);
ret =  clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&md5IntObj);



size_t global_item_size = pwdSize;
size_t local_item_size = 17;
ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
            &global_item_size, &local_item_size, 0, NULL, NULL);


/* Execute OpenCL Kernel */

/* Copy results from the memory buffer */

int *isFound = malloc(sizeof(int));


ret = clEnqueueReadBuffer(command_queue, foundObj, CL_TRUE, 0, foundSize * sizeof(int), found, 0, NULL, &timing_event);
ret = clEnqueueReadBuffer(command_queue, foundBool, CL_TRUE, 0, sizeof(int), isFound, 0, NULL, &timing_event);

clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

read_time = time_end - time_start;




ret = clFlush(command_queue);
ret = clFinish(command_queue);
ret = clReleaseKernel(kernel);
ret = clReleaseProgram(program);
ret = clReleaseMemObject(foundObj);
ret = clReleaseMemObject(foundBool);
ret = clReleaseMemObject(pwdmemObj);
ret = clReleaseMemObject(md5IntObj);



ret = clReleaseCommandQueue(command_queue);
ret = clReleaseContext(context);


if(isFound[0] == 1) return 0;
else return 1;

}



typedef struct {

    char *plainPwd;
    int *md5Pwd;

} pwdData;


int main(int argc, char **argv)
{
 

if(argc < 3) {
    red();
    printf("[!] Invalid arguments\n");

    printf("[*] Syntax: ./program pwdlist.txt f3fab3b8301dfa7dcfbc68a772f1b863\n");
    printf("\tNote: f3fab3b8301dfa7dcfbc68a772f1b863 is the hash.\n");
    reset();
    exit(2);

} 
FILE *fp;
 


fp = fopen(argv[1], "r");

if(!fp){

    printf("Error opening file\n");
    exit(1);
}

clock_t t;
t = clock();

char chunk[128];
int md5Chunk[256];
int linect = 0;
int totalSize = 0;
int avgSize = 0;
int maxWordSize = 0;


printf("\n\n[*] Started process on file: ");
yellow();
printf("%s\n",argv[1]);
reset();

char* md5Hash = argv[2];
printf("[*] Hash: ");
yellow();
printf("%s\n", md5Hash);
reset();
char *pos = md5Hash;
unsigned char md5Bytes[16];
int md5IntBytes[17];

for(size_t count = 0; count<16; count++){
    sscanf(pos, "%2hhx", &md5Bytes[count]);
    md5IntBytes[count] = md5Bytes[count];
    pos += 2;
}
md5IntBytes[16] = 0;

while(fgets(chunk, sizeof(chunk), fp) != NULL ){

    if(strlen(chunk) >0){
       
        int wSize = strlen(chunk);
        totalSize += wSize;
        linect++;
    }
}


printf("[*] Total passwords in file: ");
yellow();
printf("%d\n", linect);
reset();

 pwdData* pwArr_Data = malloc(linect * sizeof(pwdData) +1);


int i = 0;
rewind(fp);

int processed = 0;

int *md5wordList = malloc((linect *17) * sizeof(int) );
GPU();
int chunkCount = 0;

uint8_t *result;
int md5Index =0;

while(fgets(chunk, sizeof(chunk), fp) != NULL ){
    if(strlen(chunk) >0){
       

        chunk[strcspn(chunk, "\n")] = 0;
        result = md5String(chunk);

        pwArr_Data[processed].plainPwd = (char*)malloc(strlen(chunk) * sizeof(char) +2);
        pwArr_Data[processed].md5Pwd = (int*)malloc(16 * sizeof(int) +2);

        char* pwPlainPwd = pwArr_Data[processed].plainPwd;
        int* pwmd5Pwd = pwArr_Data[processed].md5Pwd;
        strcpy(pwPlainPwd, chunk);

         processed++;

        for (i = 0; i<16; i++){
            pwmd5Pwd[i] = result[i];
            md5wordList[md5Index] = result[i];
            

            md5Index++;
        }


       
        md5wordList[md5Index] = 0;

     

        md5Index++;
        
        
        
        }
        
        
    }

    int sizeofPwd = md5Index;
    int found[16];
    int res = startGPU(sizeofPwd, md5wordList,found, md5IntBytes);
    if(res == 0) {

        for(i=0; i<processed; i++){

            if(memcmp(pwArr_Data[i].md5Pwd, found, 16)==0) {
                printf("[*] Password found: ");
                cyan();
                printf("%s\n", pwArr_Data[i].plainPwd);
                reset();
            }
            
        }
       
        
    }
    else {
        printf("[*] Not found\n");
    }
 free(result);

t = clock() -t;
printf("\n[*] Elapsed time: %f\n", ((double)t)/CLOCKS_PER_SEC );
for(i=0; i<processed; i++){
    free(pwArr_Data[i].plainPwd);
    free(pwArr_Data[i].md5Pwd);
}
free(md5wordList);
//free(pwArr_Data);
free(source_str);
free(devices);
free(platforms);
fclose(fp);


return 0;
}
