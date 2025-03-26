#include<stdio.h>
void main()
{
    int n,bucket=0,in,out,bsize,dropped;
    printf("Enter bucket size:");
    scanf("%d",&bsize);
    printf("Enter number of inputs:");
    scanf("%d",&n);
    printf("Enter the packet outgoing rate: ");
    scanf("%d", &out);
    while(n>0)
    { 
        printf("\nEnter the incoming packet size: ");
        scanf("%d", &in);
        printf("Incoming packet size: %d\n", in);
        if(in<=(bsize-bucket))
        {   
            bucket+=in;
            printf("Bucket status: %d out of %d\n", bucket, bsize);
        }
        else
        {
            dropped = in - (bsize - bucket);
            printf("Bucket overflow! %d packets dropped.\n", dropped);
            bucket = bsize; 
        }
        if(bucket>=out)
        {
            bucket-=out;
            
        }
        else
        {
            bucket=0;
        }
        printf("Bucket status: %d out of %d\n", bucket, bsize);
        n--;
    }
}
