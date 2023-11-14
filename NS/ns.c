#include "headers.h"

ss ss_list[100];         // List of all storage servers
int server_count = 0;    // Number of storage servers
packet send_buffer[100]; // Buffer to store packets to be sent
int send_count = 0;      // Number of packets in buffer                           // Size of UDP address

// Helper function to split string into tokens (n tokens)
char **processstring(char data[], int n)
{

    char **tokens = (char **)malloc(sizeof(char *) * n);
    for (int i = 0; i < n; i++)
    {
        tokens[i] = (char *)malloc(sizeof(char) * MAX_DATA_LENGTH);
    }

    int i = 0;
    char *token = strtok(data, "|");
    int j = 0;
    while (token != NULL && j < n)
    {
        j++;
        strcpy(tokens[i], token);
        token = strtok(NULL, "|");
        i++;
    }

    return tokens;
}

// Code to initialise nfs
void init_nfs()
{

    // nothing as of now but any global pointers declared will be malloced here

    return;
}

// Client requests handled here
void client_handler(char data[])
{

    // yet to work on based on client request format

    char **tokens = (char **)malloc(sizeof(char *) * 3);
    tokens = processstring(data, 3);
    char *path = (char *)malloc(sizeof(char) * MAX_DATA_LENGTH);
    strcpy(path, tokens[0]);
}

void make_backup(int ss_id){

    printf("Backing up!\n");
    if(ss_id>=server_count){
        printf("Invalid server id\n");
        return;
    }

    else{

        ss found_server = ss_list[ss_id];
        printf("Backing up server %s\n",found_server->port);
        pthread_mutex_lock(&found_server->lock);
        int count=0;
        for(int i=0;i<server_count;i++){

            if(ss_list[i]->has_backup==0 && strcmp(ss_list[i]->port,found_server->port)){


            
            printf("Backing up in port %s\n",ss_list[i]->port);
            count++;
            for(int j=0;j<found_server->path_count;j++){
                strcpy(ss_list[i]->backup_paths[j],found_server->paths[j]);
            }
            ss_list[i]->has_backup=1;
            found_server->is_backedup=1;
            strcpy(ss_list[i]->backup_port,found_server->port);
            if(count==2)break;


            }

        }
        // found_server->has_backup=1;
        pthread_mutex_unlock(&found_server->lock);

    }

}
// Code to add a new storage server in naming server list
void init_storage(char data[])
{
    // tokenise the string and create a new server object with extracted attributes

    // char **tokens = (char **)malloc(sizeof(char *) * 5);

    // for (int i = 0; i < 5; i++)
    // {
    //     tokens[i] = (char *)malloc(sizeof(char) * MAX_DATA_LENGTH);
    // }
    char** tokens = processstring(data, 4);
    ss new_ss = (ss)malloc(sizeof(ss_info));
    strcpy(new_ss->ip, tokens[1]);
    strcpy(new_ss->port, tokens[3]);
    strcpy(new_ss->client_port, tokens[2]);
    new_ss->path_count = 0;
    new_ss->is_backedup=0;
    new_ss->has_backup=0;
    new_ss->status=1;
    pthread_mutex_init(&new_ss->lock, NULL);

    pthread_mutex_lock(&server_lock);

    int check_flag=0;
    int id=-1;
    for(int i=0;i<server_count;i++){
        if(strcmp(ss_list[i]->port,new_ss->port)==0){
            check_flag=1;
            id=i;
            break;
        }
    }
    if(check_flag==1){
        printf(GREEN("%s is back online!\n"),new_ss->port);
        ss_list[id]=new_ss;
           
    }
    else{

    // printf("hi\n");
    ss_list[server_count] = new_ss;
    server_count++;
    id=server_count-1;
    
    }
    pthread_mutex_unlock(&server_lock);
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, &server_handler, (void *)ss_list[id]);

    return;
}

int main()
{

    init_nfs(); // initialises ns server

    // declaring thread variables
    
    pthread_t receive_thread;
    pthread_t backup_thread_idx;

    // TCP socket to check for new requests

    // constructing threads for listening to TCP sockets
    
    pthread_create(&receive_thread, NULL, &receive_handler, NULL);
    pthread_create(&backup_thread_idx, NULL, &backup_thread, NULL);
    // joining threads
    pthread_join(receive_thread, NULL);
    pthread_join(backup_thread_idx, NULL);
    
    while(1){}

    return 0;
}
