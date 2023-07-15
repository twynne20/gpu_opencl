__kernel void hello_kernel(__global char* greetings) {
    int global_id = get_global_id(0);
    char* greeting = greetings + global_id * 13; // 13 characters for "Hello, World!"
    greeting[0] = 'H';
    greeting[1] = 'e';
    greeting[2] = 'l';
    greeting[3] = 'l';
    greeting[4] = 'o';
    greeting[5] = ',';
    greeting[6] = ' ';
    greeting[7] = 'W';
    greeting[8] = 'o';
    greeting[9] = 'r';
    greeting[10] = 'l';
    greeting[11] = 'd';
    greeting[12] = '!';
}
