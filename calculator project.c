#include<stdio.h>
void sum();
void max();
void checkEvenOdd();
void factoral();
int main()
{
    int choice;
    printf("Enter your choice:\n");
    printf("1. Sum of two numbers\n");
    printf("2. Maximum of two numbers\n");
    printf("3. Check Even or Odd\n");
    printf("4. Factorial of a number\n");
    scanf("%d", &choice);
    
    switch(choice)
    {
        case 1:
            sum();
            break;
        case 2:
            max();
            break;
        case 3:
            checkEvenOdd();
            break;
        case 4:
            factoral();
            break;
        default:
            printf("Invalid choice!\n");
    }while (1);
    return 0;
}
void sum(){
    int a,b;
    printf("Enter a No. : ");
    scanf("%d %d",&a,&b);
    printf("sum of %d and %d = %d\n",a ,b ,a+b);
}
void max(){
    int a,b;
    printf("Enter a No. : ");
    scanf("%d %d",&a,&b);
    printf("Maximum of %d and %d = %d\n",a ,b ,(a>b)?a:b);
}
void checkEvenOdd(){
    int a;
    printf("Enter a No. : ");
    scanf("%d",&a);
    if(a%2==0)
        printf("%d is Even\n",a);
    else
        printf("%d is Odd\n",a);
}
void factoral(){
    int a,fact=1;
    printf("Enter a No. : ");
    scanf("%d",&a);
    for(int i=1;i<=a;i++)
        fact=fact*i;
    printf("Factorial of %d = %d\n",a,fact);
}