int main(void) {

    interrupt(0x10,0xe*256+'Q',0,0,0);
    interrupt(0x21,5,0,0,0);
}
