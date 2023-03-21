//Technical University Of Crete
//Date:09/03/2022
//HRY 419:CAD tools for vlsi design
//Professor: Apostolos Dollas
//Author: Ioannis Peridis
//****************************************************************************************
//This program calculates the root of a polynomial up to 5th grade with two different ways
//First Method is Newton-Rapshon
//Second Method is Secant
//Also it calculates the cost of each method
//****************************************************************************************
#include <stdio.h>
#include <stdlib.h>
//function declaration
float fp(float x,int a,int b,int c,int d,int e,int f);
float fpDerivative(float x,int a,int b,int c,int d,int e);

int main()
{
    int grade,a,b,c,d,e,f;
    //Program input, the grade of the polynomial and its parameters
    printf("Please enter the grade of your polynomial (max grade=5):\n");
    scanf("%d",&grade);
    switch(grade){
    case 1:printf("Please enter the 2 parameter of your polynomial.\nIf a parameter does not exist it should be enter as 0 and not be skipped:\n");
           scanf("%d%d",&e,&f);
           a=0;b=0;c=0;d=0;
           printf("The polynomial you entered is: (%d)*x) + (%d)\n",e,f);
           break;
    case 2:printf("Please enter the 3 parameters of your polynomial.\nIf a parameter does not exist it should be enter as 0 and not be skipped:\n");
           scanf("%d%d%d",&d,&e,&f);
           a=0;b=0;c=0;
           printf("The polynomial you entered is: (%d)*x^2 + (%d)*x + (%d)\n",d,e,f);
           break;
    case 3:printf("Please enter the 4 parameters of your polynomial.\nIf a parameter does not exist it should be enter as 0 and not be skipped:\n");
           scanf("%d%d%d%d",&c,&d,&e,&f);
           a=0;b=0;
           printf("The polynomial you entered is: (%d)*x^3 + (%d)*x^2 + (%d)*x + %d\n",c,d,e,f);
           break;
    case 4:printf("Please enter the 5 parameters of your polynomial.\nIf a parameter does not exist it should be enter as 0 and not be skipped:\n");
           scanf("%d%d%d%d%d",&b,&c,&d,&e,&f);
           a=0;
           printf("The polynomial you entered is: (%d)*x^4 + (%d)*x^3 + (%d)*x^2 + (%d)*x + (%d)\n",b,c,d,e,f);
           break;
    case 5:printf("Please enter the 6 parameters of your polynomial.\nIf a parameter does not exist it should be enter as 0 and not be skipped:\n");
           scanf("%d%d%d%d%d%d",&a,&b,&c,&d,&e,&f);
           printf("The polynomial you entered is: (%d)*x^5 + (%d)*x^4 + (%d)*x^3 + (%d)*x^2 + (%d)*x + (%d)\n",a,b,c,d,e,f);
           break;

    }

    //Implementation of the Newton-Raphson method
    //===========================================
    float xo,x1,root;
    //Epsilon is used to find the acceptable error approach
    float epsilon=0.001;
    //xo is the first root estimation it is randomly putted to 1
    xo=1;
    printf("\n*******Newton-Raphson*******:\n");
    printf("The first root estimation xo=%f\n",xo);
    int reps=1;
    int numOfMuls,numOfDevs,numOfAddsAndSubs=0;

    //We have 25 repetitions maximum to find where the root converges
    for(int i=0;i<26;i++){
        //Implementation of the type: Xn+1=Xn-f(Xn)/f'(Xn)
        x1=xo-fp(xo,a,b,c,d,e,f)/fpDerivative(xo,a,b,c,d,e);
        //Calculating the cost of the type
        numOfDevs++;
        numOfAddsAndSubs+=9;
        numOfMuls+=29;
        //calculating the cost of the comparisson
        numOfAddsAndSubs+=4;
        numOfMuls+=15;
       printf("For repetition %d :xk=%f\n",i,x1);
        //Termination condition, the acceptable error approach is: |f(Xn)|<epsilon
        if(abs(fp(xo,a,b,c,d,e,f))<epsilon){
            root=x1;
            //If you find the root exit the method successfully
            printf("The root of your polynomial is : %f\n",root);
            printf("Number of Repetitions:%d\n",reps);
            printf("The total cost of the Newton-Rapshon method is:\nNumber of Additions and Substractions: %d\nNumber of Multiplications: %d\nNumber of Divisions: %d\n\n",numOfAddsAndSubs,numOfMuls,numOfDevs);
            break;
        }else if(i==25){
            //If there are more than 25 repetitions the root does not converges so we exit the method unsuccessfully
            printf("You have reached the maximum number 25 of repetitions and the root does not converges\n");
            break;
        }
        //We do this so we continue the method for the other repetitions
        xo=x1;
        //counting the repetitions
        reps++;
    }



    //Implementation of the Secant method
    //===================================
    xo=1;
    printf("***********Secant***********:\n");
    printf("The first root estimation xo=%f\n",xo);
    x1=0;
    root=0;
    reps=1;
    numOfMuls=0;
    numOfDevs=0;
    numOfAddsAndSubs=0;
    //Delta is used for the approximate calculation of the derivative
    float delta=0.001;
    float dx,dy;

    //We have 25 repetitions maximum to find where the root converges
    for(int i=0;i<26;i++){
        //Finding the derivative without using the analytical function
        //dx=Xn*(1+delta)-Xn
        dx=xo*(1+delta)-xo;
        //dy=f(Xn*(1+delta))-f(Xn)
        dy=fp(xo*(1+delta),a,b,c,d,e,f)-fp(xo,a,b,c,d,e,f);
        //Implementation of the type: Xn+1=Xn-(delta*f(Xn))/(dy/dx)
        x1=xo-fp(xo,a,b,c,d,e,f)/(dy/dx);
        //Calculating the cost of the type
        numOfDevs+=2;
        numOfAddsAndSubs+=16;
        numOfMuls+=46;
        //calculating the cost of the comparisson
        numOfAddsAndSubs+=4;
        numOfMuls+=15;
        printf("For repetition %d :xk=%f\n",i,x1);
        //Termination condition, the acceptable error approach is: |f(Xn)|<epsilon
        if(abs(fp(x1,a,b,c,d,e,f))<epsilon){
            root=x1;
            //If you find the root exit the method successfully
            printf("The root of your polynomial is : %f\n",root);
            printf("Number of Repetitions:%d\n",reps);
            printf("The total cost of the Secant method is:\nNumber of Additions and Substractions: %d\nNumber of Multiplications: %d\nNumber of Divisions: %d\n\n",numOfAddsAndSubs,numOfMuls,numOfDevs);
            break;
        }else if(i==25){
            //If there are more than 25 repetitions the root does not converges so we exit the method unsuccessfully
            printf("You have reached the maximum number 25 of repetitions and the root did not converges\n");
            break;
        }
        //We do this so we continue the method for the other repetitions
        xo=x1;
        //counting the repetitions
        reps++;
    }

    return 0;
}
//Body of the function that calculates f(xo)
 float fp(float x,int a,int b,int c,int d,int e,int f){
     return (a*x*x*x*x*x + b*x*x*x*x + c*x*x*x + d*x*x + e*x +f);
 }
//Body of the function that calculates f'(xo)
 float fpDerivative(float x,int a,int b,int c,int d,int e){
    return (a*5*x*x*x*x + b*4*x*x*x + c*3*x*x + d*2*x + e);
 }


