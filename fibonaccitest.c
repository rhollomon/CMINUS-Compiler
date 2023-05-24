int A[25];
// Calculate fibonacci number at position x
int fibonacci(int x){

    if(x == 1) return x;
    if(x == 0) return x;
    else {
        return (fibonacci(x-1) + fibonacci(x-2));
    }
} // end fibonacci



void main(void){
    int x;
    int i;

    write "Enter the number of terms in the sequence (0-25): ";
    read x;

    // Put fibonacci numbers into array
    i = 0;
    while(i < x){
        A[i] = fibonacci(i);
        i = i + 1;
    } // end while

    // Print fibonacci numbers stored in array
    i = 0;
    write "\n\nFibonacci series:";
    while(i < x){
        write " ";
        write A[i];
        i = i + 1;
    } // end while
} // end main
