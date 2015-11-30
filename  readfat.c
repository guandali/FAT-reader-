#include <stdlib.h>
#include <stdio.h>
#include "boot_sect.h"
#include <string.h>
#include <stdint.h>
/// try 1
// try 2 


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

void print_directory (root_directory* rd){
    // check if it has Subdirectory
    int sub_flag = 0;
    if (rd->attri & 0x10){
        sub_flag = 1;
        printf("%s\n"," has Subdirectory " );
    }
    printf("%s","Filename: \\" );
    for (int i = 0; i<8;i++){
        if(rd->file_name[i]!=' '){
            printf("%c",rd->file_name[i] );
        
        }

    }
    printf("%c",'.');
    for (int i = 0;i< 3;i++){
        if(rd->ext[i]!=' '){
            printf("%c",rd->ext[i] );
            
        }
    }
    printf("\n");
    int start_cluster = (rd->start_cluster[1]<<8|rd->start_cluster[0]);
    int file_size = (rd->file_size[3]<<24 | rd->file_size[2]<<16 |rd->file_size[1]<<8|
    rd->file_size[0]);
    printf("The number of the first cluster:%d\n",start_cluster);
    printf("The size of the file :%d\n",file_size);
    printf("%s\n", "************************************");
    
    


}
// read_directory read information of root directory
// call print_directory() to print

void read_directory(char* buf_, int cur_buf_pos, int entries_num){

    int DIRECTORY_SIZE  = 32;
    for (int i = 0; i<512; i++) {
        if(buf_[cur_buf_pos]==0){
           printf("%s\n", "************END***********");
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
        print_directory(rd);
        free(rd);
        cur_buf_pos = cur_buf_pos + DIRECTORY_SIZE;
        
    
    
    
    }
}
void* boot_read(char* buf){
     boot_sect_t* boot = malloc(sizeof(boot_sect_t));
     int buf_pos = 0;
     for (int i = 0; i<3;i++){
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
     return boot;

}




int main(){
    FILE *file = fopen("fat_volume.dat","rb");
    if (file==NULL) {
        printf("No such a file");
        exit(0);
    }
    printf("File found   ");
    fseek(file, 0L, SEEK_END);
    long buff_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char *buf = malloc(buff_size+1);// create a buffer
    fread(buf,buff_size,1,file); // Read 512 bytes
    for (int i = 0; i<buff_size; i++) {
         printf("%c",buf[i]);
    }
    //printf("%d",buf[11]);
    boot_sect_t* boot = boot_read(buf);
//    int buf_pos = 0;
//    for (int i = 0; i<3;i++){
//        boot->jump_code[i] = buf[buf_pos];
//        buf_pos++;
//    
//    }
    
    int ssize = (boot->ssize[1]<<8|boot->ssize[0]); //its sector size.
    printf("Sector Size: %i\n",ssize);
    printf("cluster size in sectors: %i\n",boot->csize);
    int reserved = (boot->reserved[1]<<8|boot->reserved[0]);//the number of reserved sectors on the disk.
    printf("The number of reserved sctor: %i\n",reserved);
    printf("The number of FAT copies: %i\n",boot->numfat);
    int numroot = (boot->numroot[1] <<8|boot->numroot[0] ); //the number of entries in its root directory
    printf("The number of entries in its root directory: %i\n",numroot);
    int sectors16 = (boot->sectors16[1]<<8|boot->sectors16[0]);//Total number of sectors in the filesystem
    printf("The total number of sectors in filesystem: %i\n",sectors16);
    //printf("%u",buf[21]);
    int sectperfat16 = (boot->sectperfat16[1]<<8|boot->sectperfat16[0]);
    printf("The number of sectors in FAT: %i\n",sectperfat16);
    int sectpertrack = (boot->sectpertrack[1]<<8|boot->sectpertrack[0]);
    int heads = (boot->heads[1]<<8|boot->heads[0]);
    
    int num_hidden_sec = (buf[29]<<8|buf[28]); //Number of hidden sectors
    printf("The number of hidden sectors on the disk: %i\n",num_hidden_sec);
   
    int sec_num_first_FAT = 0+reserved;
    printf("The sector number of the first copy of FAT: %i\n",sec_num_first_FAT);
    int sec_num_first_root = reserved+(boot->numfat*sectperfat16);
    printf("The sector number of the first sector of root dictory: %i\n",sec_num_first_root);
    int sec_num_first_data = ((numroot*32)/ssize)+sec_num_first_root;
    printf("The sector number of the first sector of data: %i\n",sec_num_first_data);
    // printf("Data from file:\n%u",buf[sec_num_first_data] );
    /***************************************************************************/
    //end of boot sec
    int cur_buf_pos = (sec_num_first_root)*ssize;
    read_directory(buf,cur_buf_pos, numroot);

    
    fclose(file); // close file
    free(buf);
    
   }