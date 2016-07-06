#include <pic32mz.h>
#include <libc.h>


volatile int32_t t2 = 0;

void irq_timer(){
    t2++;
}

int main() {

    while (1){
        udelay(1000000);
   }

    return 0;
}
