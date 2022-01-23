#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int main()
{

//    read file~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FILE *fpr = fopen("SOURCE.OBJ", "r");
    char c;
//    read header~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    char header[19] ;
    for(int i=0;i<19;i++)
    {
        c=getc(fpr);
        header[i]=c;
    }
    char program_name[7];
    char starting_add_of_program_string[7];
    char len_of_program_string[7];
    for(int i=0;i<19;i++)
    {
        if(i>=1 && i<=6)
        {
            program_name[i-1]=header[i];
        }
        else if(i>=7 && i<=12)
        {
            starting_add_of_program_string[i-7]=header[i];
        }
        else
        {
            len_of_program_string[i-13]=header[i];
        }
    }
    program_name[6]='\0';
    starting_add_of_program_string[6]='\0';
    len_of_program_string[6]='\0';
    printf("%s\n",program_name);
    printf("%s\n",starting_add_of_program_string);
    printf("%s\n",len_of_program_string);

//    memory simulation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int len_of_program=0;//byte
    for(int i=0;i<6;i++)//Hex to Dec
    {
        if(len_of_program_string[i]<='9')
        {
            len_of_program+=(len_of_program_string[i]-'0')*pow(16,5-i);
        }
        else
        {
            len_of_program+=(len_of_program_string[i]-'A'+10)*pow(16,5-i);
        }
    }
    printf("len_of_program=%d\n",len_of_program);
    const int msize = 20480;//10pages,10240byte
    const int page_size = 1024;//1024byte
    char memory[msize];
    int load_addr;

    //hex to dec
    int starting_add_of_program=0;
    for(int i=0;i<6;i++)
    {
        printf("%d ",(starting_add_of_program_string[i]-'0'));
        if(starting_add_of_program_string[i]<='9')
        {
            starting_add_of_program+=(starting_add_of_program_string[i]-'0')*pow(16,5-i);
        }
        else
        {
            starting_add_of_program+=(starting_add_of_program_string[i]-'A'+10)*pow(16,5-i);
        }
        printf("%d ",starting_add_of_program);
    }


    if(starting_add_of_program>0)
    {
        printf("absolute\n");
        load_addr=starting_add_of_program;//dec
        printf("load_addr=%d\n",load_addr);
    }
    else if(starting_add_of_program==0)
    {
        printf("relocation\n");
        int min = 1024;//512byte
        int max = (msize/2) - len_of_program*1.25;//(index/2)-->byte
        int pages = (max-min) / page_size;//n=(byte/byte)
        srand(time(NULL));
        int pnum = rand()%pages;
        load_addr = min+(pnum*page_size);//byte
        printf("load_addr=%d\n",load_addr);
    }
    else
    {
        printf("memory error\n");
    }
    for(int i=0;i<msize;i++)//initialize memory
    {
        memory[i]='F';
    }
//    read content~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int T_starting_addr;
    int T_len;
    int M_starting_addr;
    int M_modify_field;
    int M_modify_index_start;
    int M_modify_addr;
    int quot;//for dec to hex
    int remain;//for dec to hex
    char transfer_addr[7];
    int transfer_addr_int;
    while(1)
    {
        while(1)
        {
            c=getc(fpr);//read'\n'or'+''-'after M
            if(c=='\n')
            {
                break;//I don't want to deal with Revised M record
            }
        }
        c=getc(fpr);//read'T'or'M'or'E'
        if(c=='T')
        {
            printf("T\n");
            if(starting_add_of_program>0)
            {
                T_starting_addr=0;//abs
            }
            else if(starting_add_of_program==0)
            {
                T_starting_addr=load_addr;//relocated
            }
            else
            {
                printf("T_starting_addr error\n");
            }

            for(int i=0;i<6;i++)//hex to dec
            {
                c=getc(fpr);
                printf("%d ",c);
                if(c<='9')
                {
                    T_starting_addr+=(c-'0')*pow(16,5-i);
                }
                else
                {
                    T_starting_addr+=(c-'A'+10)*pow(16,5-i);
                }
            }
            printf("T_starting_addr=%d\n",T_starting_addr);
            T_len=0;
            for(int i=0;i<2;i++)
            {
                c=getc(fpr);
                printf("%d ",c);
                if(c<='9')
                {
                    T_len+=(c-'0')*pow(16,1-i);
                }
                else
                {
                    T_len+=(c-'A'+10)*pow(16,1-i);
                }
            }
            printf("T_len=%d\n",T_len);
            for(int i=0;i<2*T_len;i++)//to memory
            {
                c=getc(fpr);
                printf("%c ",c);
                memory[2*T_starting_addr+i]=c;//2*byte=index
            }
            printf("\n");
        }
        else if(c=='M')//must relocated
        {
            printf("M\n");
            M_starting_addr=load_addr;
            for(int i=0;i<6;i++)//hex to dec
            {
                c=getc(fpr);
                printf("%d ",c);
                if(c<='9')
                {
                    M_starting_addr+=(c-'0')*pow(16,5-i);
                }
                else
                {
                    M_starting_addr+=(c-'A'+10)*pow(16,5-i);
                }
            }
            printf("M_starting_addr=%d ",M_starting_addr);
            M_modify_field=0;
            for(int i=0;i<2;i++)//hex to dec
            {
                c=getc(fpr);
                printf("%d ",c);
                if(c<='9')
                {
                    M_modify_field+=(c-'0')*pow(16,1-i);
                }
                else
                {
                    M_modify_field+=(c-'A'+10)*pow(16,1-i);
                }
            }
            printf("M_modify_field=%d\n",M_modify_field);
            if(M_modify_field%2==0)//even
            {
                M_modify_index_start=2*M_starting_addr;
            }
            else//odd
            {
                M_modify_index_start=(2*M_starting_addr)+1;
            }
            printf("M_modify_index_start=%d\n",M_modify_index_start);
            M_modify_addr=0;
            for(int i=0;i<M_modify_field;i++)
            {
                printf("%c ",memory[M_modify_index_start+i]);
                if(memory[M_modify_index_start+i]<='9')
                {
                    M_modify_addr+=(memory[M_modify_index_start+i]-'0')*pow(16,M_modify_field-1-i);
                }
                else
                {
                    M_modify_addr+=(memory[M_modify_index_start+i]-'A'+10)*pow(16,M_modify_field-1-i);
                }
            }
            M_modify_addr+=load_addr;//dec+dec
            quot=M_modify_addr;
            for(int i=0;i<M_modify_field;i++)
            {
                remain=quot%16;
                quot=quot/16;
                //dec to hex
                printf("\nM_modify_index_start+M_modify_field-1-i=%d\n",M_modify_index_start+M_modify_field-1-i);
                if(remain<=9)
                {
                    memory[M_modify_index_start+M_modify_field-1-i]=remain+'0';
                }
                else if(remain>=10)
                {
                    memory[M_modify_index_start+M_modify_field-1-i]=remain-10+'A';
                }
                else
                {
                    memory[M_modify_index_start+M_modify_field-1-i]='0';
                }
                printf("%d %c\n",remain,memory[M_modify_index_start+M_modify_field-1-i]);
            }
            printf("\n");
        }

        else
        {
            printf("E\n");
            for(int i=0;i<6;i++)
            {
                c=getc(fpr);
                transfer_addr[i]=c;
            }
            transfer_addr[6]='\0';
            printf("transfer_addr=%s\n",transfer_addr);
            if(starting_add_of_program==0)//reloated
            {
                //hex to dec
                transfer_addr_int=load_addr;
                for(int i=0;i<6;i++)
                {
                    if(transfer_addr[i]<='9')
                    {
                        transfer_addr_int+=(transfer_addr[i]-'0')*pow(16,5-i);
                    }
                    else
                    {
                        transfer_addr_int+=(transfer_addr[i]-'A'+10)*pow(16,5-i);
                    }
                }
                printf("transfer_addr_int=%d",transfer_addr_int);
                //dec to hex
                quot=transfer_addr_int;
                for(int i=0;i<6;i++)
                {
                    remain=quot%16;
                    quot=quot/16;
                    //dec to hex
                    if(remain<=9)
                    {
                        transfer_addr[6-1-i]=remain+'0';
                    }
                    else if(remain>=10)
                    {
                        transfer_addr[6-1-i]=remain-10+'A';
                    }
                    else
                    {
                        transfer_addr[6-1-i]='0';
                    }
                }
                printf("transfer_addr=%s\n",transfer_addr);
            }

            break;
        }
    }

//    for(int i=0;i<msize;i++)
//    {
//        printf("%c",memory[i]);
//    }
//    fclose(fpr);

//    prepare used memory
    int used_memory_size=(2*len_of_program)+((2*len_of_program)/64)+1+1;//(2*len_of_program)/64 for '\n', +1 for first'\n', +1 for'\0'
    char used_memory[used_memory_size];
    int count=0;
    for(int i=0;i<2*len_of_program;i++)
    {
        if(count%65==0)//64+'\n'
        {
            used_memory[count]='\n';
            count++;
        }
        used_memory[count]=memory[2*load_addr+i];
        count++;
    }
    used_memory[used_memory_size-1]='\0';
    for(int i=0;i<used_memory_size;i++)
    {
        printf("%c",used_memory[i]);
    }
//    write file~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FILE *fpw = fopen("SICIMG", "w");
    char load_addr_str[6];
    itoa(load_addr,load_addr_str,16);//Dec to Hex
    char memo_header[26]="ISICIMG";//000000010240000000\n
    strcat(memo_header,"00");//10240>load_addr>1024
    if(load_addr<4096)
    {
        strcat(memo_header,"0");
    }
    strcat(memo_header,load_addr_str);
    strcat(memo_header,len_of_program_string);
    strcat(memo_header,transfer_addr);

    fwrite(memo_header, 1, sizeof(memo_header), fpw);
    fclose(fpw);

    FILE *fpww = fopen("SICIMG", "a");
    fwrite(used_memory, 1, sizeof(used_memory), fpww);
    fclose(fpww);
    return 0;
}
