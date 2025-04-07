#include<stdio.h>
int main(){
    int n;
    printf("entre number:");
    scanf("%d",&n);

    for (int i = 2; i <n ; i++)
    {
        if(n%i==0){
            printf("number %d is not paire",n);
            scanf("%d",&n);
            return 0;
        }
    }
    
    printf("number %d is paire",n);

    scanf("%d",&n);
    return 0;
}