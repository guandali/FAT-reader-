#include <stdlib.h>
#include <stdio.h>
#include "boot_sect.h"
#include <string.h>
#include <stdint.h>

// struct root_directory
//similar to boot_sect_t
void sec_read(FILE *file, long off_set, char* sec_buf);
void directory_read(FILE* file, long cur_buf_pos, int count_3);
void clu_read(FILE *file,long  off_set,  char * clu_buf);
int ssize;
int csize;
int sec_num_first_FAT;
int sec_num_first_root;
int sec_num_first_data;
char cur_name [256];
int cur_name_count = 0;

// new struct for reading directory
typedef struct {
    uint8_t file_name [8];
    uint8_t ext  [3];
    uint8_t attri;
    uint8_t reserved [10];
    uint8_t time [2];
    uint8_t date [2];
    uint8_t start_cluster [2];
    uint8_t file_size [4];
    
}root_directory;


// read FAT table and returen a array of cluster
void* FAT_read(signed int cluster_num, FILE* file){
    unsigned int* clusters = malloc(sizeof(int)*10); // store clusters found
    int i;
    for (i=0; i<10; i++)
        clusters[i] = 0;
    int pos;
    int off_set = 512; // offset from boot
      i = 0;
    int count = 0;
    char* buf = malloc(2048); // As we know FAT table is 4 sectors 4*512 = 2048
    clu_read(file,off_set,  buf); // buf = whole FAT tabl
    
    while (1) {
        // used a while to find all clusters in FAT for certatin file
        int num_0;
        if (!((cluster_num>=2)&&(cluster_num<=4086))) {
            // if not a valid cluster num
            free(buf); // free buf at the end
            break;          //break while loop
        }
        else{
            clusters[count] = cluster_num; // store in an array
        count++;
        
        pos = (cluster_num/2)*3 ;
        // convert little endian and
        signed long num = (buf[pos] & 0xff) + ((buf[pos+1] & 0xff) <<8) + ((buf[pos+2] & 0xff) <<16);
        
        if(cluster_num%2){
            num_0 = num/4096;;
            
        }
        else {
            num_0 = num%4096;;
        }
        
            cluster_num = num_0; // reset cluster_num to new value
        
      }
    }
    return clusters; // return a set of clusters found

    
    
    
}
// print out a single directory
int print_directory (root_directory* rd, char * name_buf ){
    // check if it has Subdirectory
    int sub_flag = 0; // 0 for no subdirectory
    char fName[12]; // store file name + ext
    int start_cluster = (rd->start_cluster[1]<<8|rd->start_cluster[0]);
    int file_size = (rd->file_size[3]<<24 | rd->file_size[2]<<16 |rd->file_size[1]<<8|
                     rd->file_size[0]);
    if (rd->attri & 0x10){
        sub_flag = 1;
        //printf("%s\n"," has Subdirectory " );
    }
    //int i;
    int temp = -1;
    int i;
    printf("%s","Filename: " );
   
        
        for (i = 0; i<8;i++){
            if(rd->file_name[i]!=' '){
                //printf("%c",rd->file_name[i] );
                temp++;
                fName[temp] = rd->file_name[i];
                
            }
            
        }
        
        if(sub_flag==0) {
            temp++;
            fName[temp] = '.';
            //printf("%c",'.');
            for (i = 0;i< 3;i++){
                if(rd->ext[i]!=' '){
                    temp++;
                    fName[temp] = rd->ext[i];
                    //printf("%c",rd->ext[i] );
                    
                    
                }
            }
            
            
        }
       if (rd->file_name[0] == '.' ) {
//           printf("temp1 is : %d\n",temp);
//           temp++;
//           fName[temp] = '\\';
//           temp++;
//           fName[temp] = '\0';
           cur_name[cur_name_count] = '\\';
           cur_name_count++;
           printf("%s\n", cur_name);
           //printf("%s\n","= '.'************");
           //strcat(name_buf, fName);
           //printf("%s\n", name_buf);
           
       }
       else{
           if (sub_flag) {
               int k;
               for (k = 0; k< temp; k++) {
                   cur_name[cur_name_count] = fName[i];
                   cur_name_count++;
                   
               }
           }
           
           temp++;
           fName[temp] = '\0';
           printf("%s\n", fName);
           int count_4 ;
           if (sub_flag) {
               for (count_4 = 0; count_4<temp-1; count_4++) {
                   cur_name[cur_name_count] = fName[i];
               }
           }

 
       }
    
     // 0x10 = sub
    if (sub_flag==0) {
        printf("The size of the file :%d\n",file_size);
    }
    else{
        printf("%s\n","This file is a directory.");
        
        
    }
    
    printf("The number of the first cluster:%d\n",start_cluster);
    printf("\n");
    printf("\n");
    return start_cluster;
    
    
    
    
}
// read_directory read information of root directory
// call print_directory() to print

void directory_read(FILE *file, long cur_buf_pos, int count_3){
    // printf("current cur_buf_pos%d\n",cur_buf_pos);
    int DIRECTORY_SIZE  = 32;
    int i;
    char* buf_ = malloc(512);
    sec_read(file, cur_buf_pos,buf_);
    int curpos = cur_buf_pos;
        // read 512 bytes each time
        int j;
        for (j = 0; j<16; j++) {
            cur_buf_pos = j * 32;
            // if first byte of directory is 0 means end of file
            if(buf_[cur_buf_pos] == 0){
                free(buf_);
                return;
            }
            root_directory* rd = malloc(sizeof(root_directory));
            rd->file_name[0] = buf_[cur_buf_pos];
            rd->file_name[1] = buf_[cur_buf_pos+1];
            rd->file_name[2] = buf_[cur_buf_pos+2];
            rd->file_name[3] = buf_[cur_buf_pos+3];
            rd->file_name[4] = buf_[cur_buf_pos+4];
            rd->file_name[5] = buf_[cur_buf_pos+5];
            rd->file_name[6] = buf_[cur_buf_pos+6];
            rd->file_name[7] = buf_[cur_buf_pos+7];
            rd->ext[0] = buf_[cur_buf_pos+8];
            rd->ext[1] = buf_[cur_buf_pos+9];
            rd->ext[2] = buf_[cur_buf_pos+10];
            rd->attri = buf_[cur_buf_pos+11];
            rd->start_cluster [0] = buf_[cur_buf_pos+26];
            rd->start_cluster [1] = buf_[cur_buf_pos+27];
            rd->file_size[0] = buf_[cur_buf_pos+28];
            rd->file_size[1] = buf_[cur_buf_pos+29];
            rd->file_size[2] = buf_[cur_buf_pos+30];
            rd->file_size[3] = buf_[cur_buf_pos+31];
            //cur_buf_pos = cur_buf_pos + DIRECTORY_SIZE;
            char first = rd->file_name[0];
            int start_cluster;
            if(first!=229){
                char * name_buf;
                start_cluster = print_directory(rd, name_buf);
            }
            else{
                  break;
            }
            uint8_t attri = rd->attri;
            free(rd);
            
            if ((attri & 0x10)&&(start_cluster>0)){
                // if has subdirectory, go read FAT
                int* clusters = FAT_read(start_cluster, file);//FAT_read return an array of clusters
                                                              //int count_2 = 0;
                count_3 = 0;
                    for (count_3; count_3<10; count_3++) {
                        // condition: if a valid cluster number and not subdirectory 
                        if ((clusters[count_3] >= 2)&&(clusters[count_3]<4095)&&first!='.'){
                        long  new_pos = ((clusters[count_3] - 2) * csize )*ssize + (sec_num_first_data * ssize);
                            directory_read(file, new_pos,count_3+1);
                        }else{
                        }
                    }
            
            }else{
                
            }

        }
    // }
    
}
// used to read boot
void* boot_read(char* buf){
    boot_sect_t* boot = malloc(sizeof(boot_sect_t));
    int buf_pos = 0;
    int i;
    for ( i = 0; i<3;i++){
        boot->jump_code[i] = buf[buf_pos];
        buf_pos++;
        
    }
    boot->ssize[0] = buf[11];
    boot->ssize[1] = buf[12];
    boot->csize = buf[13];// its cluster size in sectors.
    boot->reserved[0] = buf[14];
    boot->reserved[1] = buf[15];
    boot->numfat = buf[16];
    boot->numroot[0] = buf[17];
    boot->numroot[1] = buf[18];
    boot->sectors16[0] = buf[19];
    boot->sectors16[1] = buf[20];
    boot->media[0] = buf[21];
    boot->sectperfat16[0] = buf[22];
    boot->sectperfat16[1] = buf[23];
    boot->sectpertrack[0] = buf[24];
    boot->sectpertrack[1] = buf[25];
    boot->heads[0] = buf[26];
    boot->heads[1] = buf[27];
    boot->prevsect[0] = buf[28];
    boot->prevsect[1] = buf[29];
    return boot;
    
}
// read 512 bytes once
void sec_read(FILE *file, long off_set, char* sec_buf){
    
    
    int READ_SIZE = 512;
    fseek(file, off_set, SEEK_SET);
    fread(sec_buf,READ_SIZE,1,file); // Read 512 bytes
                                     //fseek(file, 0L, SEEK_SET);
                                     //return sec_buf;
    
    
    
}
// read 2048 bytes once
void clu_read(FILE *file,long  off_set,  char * clu_buf){
    
    
    int READ_SIZE = 2048;
    fseek(file, off_set, SEEK_SET);
    fread(clu_buf,READ_SIZE,1,file);
    
    
    
    
}




int main(int argc, char *argv[]){
    FILE *file;
    if (argc < 2) {
        /* read input */
        /* give error message and exit */
        fprintf(stderr, "Must pass an argument!\n");
        exit(1);
    }
    file = fopen(argv[1],"rb"); // open file
    //FILE *file = fopen("fat_volume.dat","rb");
    if (file==NULL) {
        printf("No such a file");
        exit(0);
    }
    printf("File found   ");
    fseek(file, 0L, SEEK_END);
    long buff_size = ftell(file);
    
    fseek(file, 0L, SEEK_SET);
    char *buf = malloc(buff_size+1);// create a buffer
    
    fread(buf,buff_size,1,file);
    
    long boot_off = 0;
    char* sec_buf = malloc(512);
    sec_read(file, boot_off,sec_buf); // read boot and store it in boot_sect_t
    boot_sect_t* boot = boot_read(sec_buf);
    free(sec_buf);
    ssize = (boot->ssize[1]<<8|boot->ssize[0]); //its sector size.
    printf("Sector Size: %i\n",ssize);
    csize = boot->csize;
    printf("cluster size in sectors: %i\n",boot->csize);
    int reserved = (boot->reserved[1]<<8|boot->reserved[0]);//the number of reserved sectors on the disk.
    printf("The number of reserved sctor: %i\n",reserved);
    printf("The number of FAT copies: %i\n",boot->numfat);
    int numroot = (boot->numroot[1] <<8|boot->numroot[0] ); //the number of entries in its root directory
    printf("The number of entries in its root directory: %i\n",numroot);
    int sectors16 = (boot->sectors16[1]<<8|boot->sectors16[0]);//Total number of sectors in the filesystem
                                                               //printf("The total number of sectors in filesystem: %i\n",sectors16);
    int sectperfat16 = (boot->sectperfat16[1]<<8|boot->sectperfat16[0]);
    printf("The number of sectors in FAT: %i\n",sectperfat16);
    int sectpertrack = (boot->sectpertrack[1]<<8|boot->sectpertrack[0]);
    int heads = (boot->heads[1]<<8|boot->heads[0]);
    int num_hidden_sec = (boot->prevsect[1]<<8|boot->prevsect[0]); //Number of hidden sectors
    printf("The number of hidden sectors on the disk: %i\n",num_hidden_sec);
    sec_num_first_FAT = 0+reserved;
    printf("The sector number of the first copy of FAT: %i\n",sec_num_first_FAT);
    sec_num_first_root = reserved+(boot->numfat*sectperfat16);
    printf("The sector number of the first sector of root dictory: %i\n",sec_num_first_root);
    sec_num_first_data = ((numroot*32)/ssize)+sec_num_first_root;
    printf("The sector number of the first sector of data: %i\n",sec_num_first_data);
    // printf("Data from file:\n%u",buf[sec_num_first_data] );
    /***************************************************************************/
    //end of boot sec
    long cur_buf_pos = (sec_num_first_root)*ssize;
    //printf("%c\n",buf[cur_buf_pos]);
    directory_read(file,cur_buf_pos, 0);
    
    
    fclose(file); // close file
                  //free(buf);
    
}