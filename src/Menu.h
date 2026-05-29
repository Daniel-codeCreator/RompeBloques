#ifndef MENU_H
#define MENU_H

#include "../Api´s/ApiClient.h"

class Menu {
private:
    ApiClient& api;

public:
    explicit Menu(ApiClient& apiClient);
    void mostrar();
};

#endif