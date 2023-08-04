int gcd(int y){
    int arr[10];
    int i;
    i = 5;
    if(i>4){ 
        arr[i] = 10; 
        i = 4;
    }
    else {
        arr[i] = 5;
            if(i<1){ 
                arr[i] = 10; 
                i = 4;
            }
            else {
                arr[i] = 5;
            }
    }
    y = 5 + 3 * 4;
    y = arr[4] + 5;
    arr[i] = 5;
    return y+5;
}

void main(void) {
    int a;
    a = 5 + 3 * 4;
    a = gcd(a);
    return a+5;
}