//
// Created by guang on 2020/3/16.
//
#include <iostream>
#include <event2/event.h>
using namespace std;

void checkEventBase();
void getVersionAndSupportedMethod();

int main()
{

//    getVersionAndSupportedMethod();
    checkEventBase();


    return 0;
}

void checkEventBase()
{
    struct event_base *base;
    struct event_config *config;
    int f;

    config = event_config_new();

    event_config_avoid_method(config,"epoll");
    event_config_avoid_method(config,"poll");

    base = event_base_new_with_config(config);
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
