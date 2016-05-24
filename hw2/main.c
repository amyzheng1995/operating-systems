
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int expression(char expr[]){
    int i = 1;
    int operands [16]= {0};
    int count = 0; //the number of operands 
    char operation;
    if((expr[i] == '+') || (expr[i] == '*') || (expr[i] =='/') || ((expr[i] == '-') && (expr[i+i] ==' '))){
        operation = expr[i];
        printf("PROCESS %d: Starting '%c' operation\n", getpid(), operation);
    }
    else{
        char str_oper[10];
        int j = 0;
        while (j < strlen(expr)){
            if (expr[i] != ' ' && (!isdigit(expr[i])) && expr[i] !='\n'){
                str_oper[j] = expr[i];
                j++;
                i++;
            }
            else{
                j++;
                i++;
            }
        }
        printf("PROCESS %d: ERROR: unknown '%s' operator\n",(int)getpid(),str_oper);
        exit(1);
    }
    while (i < strlen(expr)){
        int p[2];
        int rc= pipe(p);
        if (rc == -1){
            fprintf(stderr, "pipe() failed" );
            return EXIT_FAILURE;
        }
        if (expr[i] == ')') {
            if (count < 2){
                fprintf(stderr, "PROCESS %d: ERROR: not enough operands\n", getpid());
                exit(1);
            }
            int j;
            int result = 0;
            if (operation == '+'){
                for (j = 0; j < count; j++){
                    result = result + operands[j];
                }
            }
            else if(operation == '-'){
                result = operands[0];
                for (j = 1; j < count; j++){
                    result = result - operands[j];
                }
            }
            else if (operation == '*'){
                result = 1;
                for (j = 0; j < count; j++)
                    result = result * operands[j];
            }
            else if(operation == '/'){
                result = operands[0];
                for (j = 1; j< count; j++){
                    if (operands[j] == 0){
                        result = 0; 
                        printf("Dividing by 0\n");
                        return 0;
                    }
                    else{
                        result = result/operands[j];
                    }
                }
            }
            return result;
        }
        int a =0;
        int temp = 1;
        if (expr[i] == '(' || isdigit(expr[i]) || (expr[i]== '-' && isdigit(expr[i+1]))){
            if(isdigit(expr[i]) || (expr[i]== '-' && isdigit(expr[i+1]))){
                if (expr[i]== '-' && isdigit(expr[i+1])){
                    temp = -1;
                    i = i+1;
                }
                a = expr[i] - '0';
                char num  = expr[i+1];
                while ( num >= '0' && num <= '9'){
                    a = a*10 + num - '0';
                    i++;
                    num = expr[i+1];
                }
               a = a * temp;
            }
            int pid = fork();
            if (pid == -1){
                fprintf(stderr, "fork() failed" );
                return EXIT_FAILURE;
            }
            if (pid == 0){ //child
                if (expr[i] == '('){
                    char substring[500];
                    int p_count = 0;
                    int s = i;
                    int end = 0;
                    while (i<strlen(expr)){
                        if (expr[i]=='('){
                            p_count++;
                        }
                        else if (expr[i]==')'){
                            p_count--;
                        }
                        if (p_count==0){
                            end = i;
                            break;
                        }
                        i++;
                    }
                    strncpy(substring, expr+s, (end-s+1));
                    substring[end-s+1] = '\0';
                    int tmp = expression(substring);
                    if (tmp != 0){
                        close(p[0]);
                        write(p[1], &tmp, 4);
                        printf( "PROCESS %d: Sending '%d' on pipes to parent\n", getpid(),tmp );

                    }
                }
                else if(isdigit(expr[i])){
                    close(p[0]);
                    p[0] = -1;
                    write(p[1], &a, 4);
                    printf( "PROCESS %d: Sending '%d' on pipes to parent\n", getpid(),a );

                }
                exit(0);
            }
            else if (pid>0){
                if(expr[i] == '('){
                    int p_count = 0;
                    while (i<strlen(expr)){
                        if (expr[i]=='('){
                            p_count++;
                        }
                        else if (expr[i]==')'){
                            p_count--;
                        }
                        if (p_count==0){
                            break;
                        }
                        i++;
                    }
                }
                close (p[1]);
                p[1] = -1;
                int status;
                pid_t c_pid = wait(&status);
                if ( WIFSIGNALED( status ) ){
                      printf( "child %d terminated abnormally\n", (int) c_pid );
                }
                else if ( WIFEXITED( status ) ){
                    int rc = WEXITSTATUS( status );
                    if (rc !=0 ){           
                     printf( "PARENT: child %d terminated with nonzero exit status %d", (int)c_pid, rc );
                  }
                }
                read(p[0], &operands[count], 4);
                count++;
            }
        }
        i++;
    }
    return 0;
    
}


int main(int argc, char * argv[]){
//arugment checks
     if (argc != 2){
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> \n");
        return EXIT_FAILURE;
     }

    FILE *file;
    file = fopen(argv[1], "r");
    char line[500];
    if (file){
        while (fgets(line, sizeof line, file)){
            if (line[0] == '#' || line[0] == '\n')
                continue;
            fscanf(file, "%s", line);
        }
    }
    else{
        fprintf(stderr, "ERROR: File is failed to open\n");
        return EXIT_FAILURE;
    }
    fclose(file);
    if (line[0] == '('){
        int result = expression(line);
        if (line[strlen(line)-1]=='\n')
            line[strlen(line)-1] = '\0';
        else if(line[strlen(line)]=='\n')
            line[strlen(line)] = '\0';
        printf("PROCESS %d: Processed '%s'; the final answer is %d\n", getpid(), line, result);
    }
    return EXIT_SUCCESS;
}

