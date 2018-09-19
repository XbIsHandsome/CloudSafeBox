#include "task.h"

int main(void)
{
    task_init();

    while(1)
    {
        task_driver();
    }
}


