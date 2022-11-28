#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "stdbool.h" // for declaration of bool var

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

typedef struct { // declared also in server.c
    int id; //902001-902020
    int AZ; 
    int BNT;
    int Moderna; 
} Order;

//  ============================== Lock utilities ==============================
#define read_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
    struct flock lock; // flock: l_type, l_start, l_whence, l_len
    lock.l_type = type; /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset; /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len; /* #bytes (0 means to EOF) */
    return(fcntl(fd, cmd, &lock));
}

int read_lock_at(int fd, int index) {
    return read_lock(fd, index * sizeof(Order), SEEK_SET, sizeof(Order));
}

int write_lock_at(int fd, int index) {
    return write_lock(fd, index * sizeof(Order), SEEK_SET, sizeof(Order));
}

int unlock_at(int fd, int index) {
    return un_lock(fd, index * sizeof(Order), SEEK_SET, sizeof(Order));
}


//  ==================== Read/Write/Print Orders utilities ===========================

void read_db_at(int fd, int index, char* res) {
    Order order;
    int ret = pread(fd, &order, sizeof(Order), index * sizeof(Order));

    if (ret < 0) { 
        ERR_EXIT("Error reading db\n"); 
    }
    if(order.AZ==1&&order.BNT==2&&order.Moderna==3)
        sprintf(res, "Your preference order is AZ > BNT > Moderna.\n");
    else if(order.AZ==1&&order.Moderna==2&&order.BNT==3)
        sprintf(res, "Your preference order is AZ > Moderna > BNT.\n");
    else if(order.AZ==2&&order.Moderna==1&&order.BNT==3)
        sprintf(res, "Your preference order is Moderna > AZ > BNT.\n");
    else if(order.AZ==3&&order.Moderna==1&&order.BNT==2)
        sprintf(res, "Your preference order is Moderna > BNT > AZ.\n");
    else if(order.AZ==2&&order.Moderna==3&&order.BNT==1)
        sprintf(res, "Your preference order is BNT > AZ > Moderna.\n");
    else if(order.AZ==3&&order.Moderna==2&&order.BNT==1)
        sprintf(res, "Your preference order is BNT > Moderna > AZ.\n");


    return;
}

void write_db_at(int fd, int index, int order_AZ,int order_BNT,int order_Moderna, char* res) {
    Order order;
    /* Read the order first, and then re-write it */
    
    int ret = pread(fd, &order, sizeof(Order), index * sizeof(Order));
    

    if (ret < 0) { 
        ERR_EXIT("Error reading db\n"); 
    }

    if(order_AZ==1&&order_BNT==2&&order_Moderna==3){
        order.AZ = 1;
        order.BNT = 2;
        order.Moderna =3;

    } 
        
    else if(order_AZ==1&&order_BNT==3&&order_Moderna==2)
    {
        order.AZ=1;
        order.BNT=3;
        order.Moderna=2;
    }
     else if(order_AZ==2&&order_BNT==1&&order_Moderna==3)
    {
        order.AZ=2;
        order.BNT=1;
        order.Moderna=3;
    }
     else if(order_AZ==2&&order_BNT==3&&order_Moderna==1)
    {
        order.AZ=2;
        order.BNT=3;
        order.Moderna=1;
    }
     else if(order_AZ==3&&order_BNT==1&&order_Moderna==2)
    {
        order.AZ=3;
        order.BNT=1;
        order.Moderna=2;
    }
     else if(order_AZ==3&&order_BNT==2&&order_Moderna==1)
    {
        order.AZ=3;
        order.BNT=2;
        order.Moderna=1;
    }
    else {
        sprintf(res, "[Error] Operation failed. Please try again.\n");
        return;
    }

    ret = pwrite(fd, &order, sizeof(Order), index * sizeof(Order));

    if (ret < 0) { 
        ERR_EXIT("Error writing db\n"); 
    }

   if(order_AZ==1&&order_BNT==2&&order_Moderna==3)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is AZ > BNT > Moderna.\n", index + 902001);
   }
   else if(order_AZ==1&&order_BNT==3&&order_Moderna==2)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is AZ > Moderna > BNT.\n", index + 902001);
   }
   else if(order_AZ==2&&order_BNT==1&&order_Moderna==3)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is BNT > AZ > Moderna.\n", index + 902001);
   }
   else if(order_AZ==2&&order_BNT==3&&order_Moderna==1)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is Moderna > AZ > BNT.\n", index + 902001);
   }
   else if(order_AZ==3&&order_BNT==1&&order_Moderna==2)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is BNT > Moderna > AZ.\n", index + 902001);
   }
   else if(order_AZ==3&&order_BNT==2&&order_Moderna==1)
   {
     sprintf(res, "Preference order for %d modified successed, new preference order is Moderna > BNT > AZ.\n", index + 902001);
   }


    return;
}

void print_db(int fd){
    Order order;
    printf("=== CURRENT DB STATUS ===\n");
    lseek(fd, 0, SEEK_SET); // set fd offset to 0 bytes
    while(read(fd, &order, sizeof(Order)))
        printf("id = %d, AZ = %d, BNT = %d, Moderna = %d\n", order.id, order.AZ, order.BNT, order.Moderna);
    return;
}


//  ========================== Server ============================
typedef struct {
    char hostname[512];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
    // you don't need to change this.
    int id;
    bool wait_for_write;  /* Write request has two stages.
        1. send student_id
        2. send request.
        wfw default = 0, and after stage 1, it is set to true to wait for write.
        */
} request;

server svr;  // server
request* requestP = NULL;  // point to a list of requests
int maxfd;  // size of open file descriptor table, size of request list
bool write_intra_lock[128]; // Because fcntl can only lock inter process, we declare a variable to mock intra process lock. i'th bit mean i'th student's lock.

const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";

static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static void free_request(request* reqP);
// free resources used by a request instance

int handle_read(request* reqP) {
    char buf[512];
    int ret = read(reqP->conn_fd, buf, sizeof(buf));
    if(ret == -1){
        ERR_EXIT("handle_read");
    }
    memcpy(reqP->buf, buf, strlen(buf));
    return ret;
}


int main(int argc, char** argv) {
    
    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in cliaddr;  // used by accept()
    int clilen;

    int conn_fd;  // fd for a new connection with client
    int file_fd;  // fd for file that we open for reading
    char buf[512];

    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Loop for handling connections
    fprintf(stdout, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", \
        svr.hostname, svr.port, svr.listen_fd, maxfd);

    #ifdef READ_SERVER
        printf("server type: read_server\n");
        file_fd = open("registerRecord", O_RDONLY);
    #else 
        printf("server type: write_server\n");
        file_fd = open("registerRecord", O_RDWR);
    #endif
    
    fd_set readfds;

    while(true) {
        // TODO: Add IO multiplexing

        
        FD_ZERO(&readfds);

        // 2. set interested node to 1
        FD_SET(svr.listen_fd, &readfds);
        for(int i = 0; i < maxfd; i++) {
            if(requestP[i].conn_fd != -1) { // not empty
                FD_SET(requestP[i].conn_fd, &readfds); // set to monitored
            }
        }
        maxfd = 512;

        // 3. select
        int activity = select(maxfd, &readfds, NULL, NULL, NULL); // wait indefinitely
        if(activity < 0) {
            ERR_EXIT("select error.");
        }

        // //  3-1. master == 1 -> new connection
        if(FD_ISSET(svr.listen_fd, &readfds)) { // new connection
            // Check new connection
            clilen = sizeof(cliaddr);
            conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
            
            if (conn_fd < 0) {
                if (errno == EINTR || errno == EAGAIN) continue;  // try again
                if (errno == ENFILE) {
                    (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
                    continue;
                }
                ERR_EXIT("accept");
            }
            requestP[conn_fd].conn_fd = conn_fd; // setup new request
            strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
            printf("getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);

            strcpy(buf, "Please enter your id (to check your preference order):\n");

            write(requestP[conn_fd].conn_fd, buf, strlen(buf));
        }

 
        // //  3-2. slave == 1 -> close or read
        for(int i = 0; i < maxfd; i++) {
            if(requestP[i].conn_fd == -1 || requestP[i].conn_fd == svr.listen_fd) continue;
            

            if(FD_ISSET(requestP[i].conn_fd, &readfds)){

                int ret = handle_read(&requestP[i]);

                if(ret == 0){ 
                    // end of file -> can close
                    printf("Host disconnected , ip %s, fd %d\n", requestP[i].host, requestP[i].conn_fd);   
                    // Close the socket
                    close(requestP[i].conn_fd);   
                    free_request(&requestP[i]); 
                }else{ 
                    // recv message -> handle
                    printf("Host received data from, ip %s, fd %d\n", requestP[i].host, requestP[i].conn_fd);

                    #ifdef READ_SERVER
                        /*
                        Handle read request
                        */
                        int student_num = atoi(requestP[i].buf) - 902001;
                        if(student_num < 0 || student_num >= 20) {
                            strcpy(buf, "[Error] Operation failed. Please try again.\n");
                            write(requestP[i].conn_fd, buf, strlen(buf));
                            close(requestP[i].conn_fd);
                            free_request(&requestP[conn_fd]);
                            continue;
                        }

                        requestP[i].id = student_num; //request[i].id is the student number.
                        int ret = read_lock_at(file_fd, student_num);
                        printf("ret = %d, read_lock_at %d\n", ret, student_num);
                        if(ret < 0) {
                            strcpy(buf, "Locked.\n");
                            write(requestP[i].conn_fd, buf, strlen(buf));
                        } else {
                            read_db_at(file_fd, student_num, buf);
                            unlock_at(file_fd, student_num);
                            write(requestP[i].conn_fd, buf, strlen(buf));
                        }

                        close(requestP[i].conn_fd);
                        free_request(&requestP[i]);
                    #else //WRITE SERVER
                        if(requestP[i].wait_for_write == false){
                            /*
                            Handle write request - Stage 1
                            */
                            int student_num = atoi(requestP[i].buf) - 902001;
                            if(student_num < 0 || student_num >= 20) {
                                strcpy(buf, "[Error] Operation failed. Please try again.\n");
                                write(requestP[i].conn_fd, buf, strlen(buf));
                                close(requestP[i].conn_fd);
                                free_request(&requestP[conn_fd]);
                                continue;
                            }

                            printf("student_num = %d\n", student_num);
                            requestP[i].id = student_num;
                            int ret = write_lock_at(file_fd, student_num);

                            int ret2 = write_intra_lock[requestP[i].id] == true ? -1 : 0;
                            write_intra_lock[requestP[i].id] = true;

                            printf("ret = %d, write_lock_at %d\n", ret, student_num);
                            if(ret < 0 || ret2 < 0){
                                strcpy(buf, "Locked.\n");
                                write(requestP[i].conn_fd, buf, strlen(buf));

                                close(requestP[i].conn_fd);
                                free_request(&requestP[i]);
                            }else{
                                read_db_at(file_fd, student_num, buf);
                                strcat(buf, "Please input your preference order respectively(AZ,BNT,Moderna):\n");
                                write(requestP[i].conn_fd, buf, strlen(buf));
                                requestP[i].wait_for_write = true;
                            }
                        }else{
                            /*
                            Handle write request - Stage 2
                            */

                            // wait_for_write = true -> handle request
                            // handle recv message
                            
                            int order_AZ;
                            int order_BNT;
                            int order_Moderna;

                            sscanf(requestP[i].buf, "%d %d %d", &order_AZ,&order_BNT,&order_Moderna); //requestP[i].buf is your preference order

                            write_db_at(file_fd, requestP[i].id,order_AZ,order_BNT,order_Moderna , buf);
                            unlock_at(file_fd, requestP[i].id);
                            write_intra_lock[requestP[i].id] = false;

                            write(requestP[i].conn_fd, buf, strlen(buf));
                            
                            close(requestP[i].conn_fd);
                            free_request(&requestP[i]);
                        }
                    #endif
                    print_db(file_fd);
                }
            }
        }
        
    }
    free(requestP);
    return 0;
}

// ======================================================================================================
// You don't need to know how the following codes are working

static void init_request(request* reqP) {
    reqP->conn_fd = -1;
    reqP->buf_len = 0;
    reqP->id = 0;
    reqP->wait_for_write = false;
}

static void free_request(request* reqP) {
    /*if (reqP->filename != NULL) {
        free(reqP->filename);
        reqP->filename = NULL;
    }*/
    init_request(reqP);
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    tmp = 1;
    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        ERR_EXIT("bind");
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }

    // Get file descripter table size and initize request table
    maxfd = getdtablesize();
    printf("fd table size = %d\n", maxfd);

    requestP = (request*) malloc(sizeof(request) * maxfd);
    if (requestP == NULL) {
        ERR_EXIT("out of memory allocating all requests");
    }
    for (int i = 0; i < maxfd; i++) {
        init_request(&requestP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);

    return;
}