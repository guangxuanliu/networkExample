//
// Created by guang on 2020/3/16.
//
#include <iostream>
#include <event2/event.h>
using namespace std;

void checkEventBase();

int main()
{
    checkEventBase();


    return 0;
}

void checkEventBase()
{
    struct event_base *base;
    int f;

    base = event_base_new();
    if(!base)
    {
        puts("Couldn't get an event_base!\n");
    }
    else
    {
        printf("Using libevent using backend method: %s",
                event_base_get_method(base));
    }

    f = event_base_get_features(base);
    printf("%d",f);



}

void getVersionAndSupportedMethod()
{
    cout << "Version: " << event_get_version() << endl;

    const char **result = event_get_supported_methods();
    for(int i = 0;result[i] != NULL;++i)
    {
        cout << result[i] << endl;
    }
}
