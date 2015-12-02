#include <stdlib.h>
#include <stdio.h>
#include "boot_sect.h"
#include <string.h>
#include <stdint.h>

// struct root_directory
//similar to boot_sect_t
void* sec_read(FILE *file, long off_set);
void directory_read(char* buf_, long cur_buf_pos);
int ssize;
int sec_num_first_FAT;
int sec_num_first_root;
int sec_num_first_data;

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
void FAT_read(int cluster_num, char* buf){
    printf("cluster num: %d\n",cluster_num);
    //int pos = ((cluster_num - 2) *4) * ssize + ssize;
//    long num = (buf[pos] & 0xff) + ((buf[pos+1] & 0xff) <<8) + ((buf[pos+2] & 0xff) <<16);
//    int first_num = num%4096;
//    int second_num = num/4096;
//    printf("%d\n",first_num);
//    printf("%d\n",second_num);
      int pos;
      int off_set = 512;

    //int cluster_array[10];
    while (1) {
        int num_0;

        pos = (cluster_num/2)*3 + off_set;
        printf("pos %d\n",pos);
        long num = (((buf[pos+2] & 0xff) <<16)|((buf[pos+1] & 0xff) <<8)|(buf[pos] & 0xff) );
//        
//              int first_num = num%4096;
//              int second_num = num/4096;
//              printf("%d\n",first_num);
//              printf("%d\n",second_num);

        
        if(cluster_num%2){
            num_0 = num/4096;;
            
        }
        else {
            num_0 = num%4096;;
        }

         printf("num_0 %d\n",num_0);
        int cur_buf_pos = ((cluster_num + 2) * 4 )*ssize + sec_num_first_data * ssize;
        //directory_read( buf, cur_buf_pos);
        cluster_num = num_0;
        if (cluster_num  == 4095) {
            printf("%s\n","end of FAT ");
            return;
        }
        
    }
//        if (((cluster_num>=2))&(cluster_num<=4086)) {
//            printf("uesed %d\n",cluster_num);
//
//        }
//        else{
//            printf("%s\n","end of cluster");
//            return;
//        }
        
        
//        printf("%d\n",num_);
//        cluster_array[i] = num_;
//        i++;

//    }
    

//    for (int i = 0; i<10; i++) {
//        printf("cluster array: %d\n",cluster_array[i]);
//    }
    
    
    //long cur_pos  = (sec_num_first_root * ssize) + ((num_ - 2) *4) * ssize;
//    printf("%s\n","********************read*******************1");
//    //directory_read(buf, cur_pos);
//    if ((num_ >= 4088)&(num_ <= 4095)){
//        printf("%s\n", "end ");
//        return;
//    
//    }
//    if ((num_>=2)&(num_<=4079) ){
//        cur_pos = num_ * ssize;
//        //directory_read(buf, cur_pos);
//        //    FAT_read(cluster_num, buf);
//    }
//    FAT_read(cluster_num, buf);

}

void print_directory (root_directory* rd, char* buf){
    // check if it has Subdirectory
    int sub_flag = 0;
    int start_cluster = (rd->start_cluster[1]<<8|rd->start_cluster[0]);
    int file_size = (rd->file_size[3]<<24 | rd->file_size[2]<<16 |rd->file_size[1]<<8|
                     rd->file_size[0]);
    if (rd->attri & 0x10){
        sub_flag = 1;
        //printf("%s\n"," has Subdirectory " );
    }
    printf("%s","Filename: \\" );
    for (int i = 0; i<8;i++){
        if(rd->file_name[i]!=' '){
            printf("%c",rd->file_name[i] );
        
        }

    }
    int i;
    if(sub_flag==0) {
        printf("%c",'.');
        for (int i = 0;i< 3;i++){
            if(rd->ext[i]!=' '){
                printf("%c",rd->ext[i] );
                
            }
        }
        
    }
    
    // 0x10 = sub
    printf("\n");
    if (sub_flag==0) {
        printf("The size of the file :%d\n",file_size);
    }
    else{
        printf("%s\n","This file is a directory.");
        //FAT_read(start_cluster,buf);
    
    }

    printf("The number of the first cluster:%d\n",start_cluster);


    if (sub_flag == 1) {
        FAT_read(start_cluster,buf);
        
    }
    printf("\n");
    printf("\n");

    


}
// read_directory read information of root directory
// call print_directory() to print

void directory_read(char* buf_, long cur_buf_pos){

    int DIRECTORY_SIZE  = 32;
    for (int i = 0 ; i < 32; i++) {
        // read 512 bytes each time
        // printf("%d\n",cur_buf_pos);
        //char* buf_ = sec_read(file, cur_buf_pos );
        for ( int  j = 0; j<16; j++) {
            //printf("%c\n",buf_[cur_buf_pos]);
            if(buf_[cur_buf_pos] == 0){
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
            print_directory(rd, buf_);
            free(rd);
            cur_buf_pos = cur_buf_pos + DIRECTORY_SIZE;
            
            
            
            
        }
        
        
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
     boot->prevsect[0] = buf[28];
     boot->prevsect[1] = buf[29];
     return boot;

}
void* sec_read(FILE *file, long off_set){

    char* sec_buf = malloc(512);
    int READ_SIZE = 512;
    fseek(file, off_set, SEEK_SET);
    fread(sec_buf,READ_SIZE,1,file); // Read 512 bytes
                                     //fseek(file, 0L, SEEK_SET);
    return sec_buf;
    


}
void* clu_read(FILE *file,long  off_set){
    
    char* clu_buf = malloc(2048);
    fseek(file, 0L, off_set);
    fread(clu_buf,2048,1,file); // Read 4 sectors
    fseek(file, 0L, SEEK_SET);
    return clu_buf;
    
    
    
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
    fread(buf,buff_size,1,file);
//    for (int i = 0; i<buff_size; i++) {
//         printf("%c",buf[i]);
//    }
    //printf("%d",buf[11]);
    long boot_off = 0;
    boot_sect_t* boot = boot_read(sec_read(file, boot_off)); // read boot and store it in boot_sect_t
//    int buf_pos = 0;
//    for (int i = 0; i<3;i++){
//        boot->jump_code[i] = buf[buf_pos];
//        buf_pos++;
//    
//    }
    
    ssize = (boot->ssize[1]<<8|boot->ssize[0]); //its sector size.
    printf("Sector Size: %i\n",ssize);
    printf("cluster size in sectors: %i\n",boot->csize);
    int reserved = (boot->reserved[1]<<8|boot->reserved[0]);//the number of reserved sectors on the disk.
    printf("The number of reserved sctor: %i\n",reserved);
    printf("The number of FAT copies: %i\n",boot->numfat);
    int numroot = (boot->numroot[1] <<8|boot->numroot[0] ); //the number of entries in its root directory
    printf("The number of entries in its root directory: %i\n",numroot);
    int sectors16 = (boot->sectors16[1]<<8|boot->sectors16[0]);//Total number of sectors in the filesystem
    printf("The total number of sectors in filesystem: %i\n",sectors16);
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
    directory_read(buf,cur_buf_pos);

    
    fclose(file); // close file
                  //free(buf);
    
   }