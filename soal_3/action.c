#include <stdio.h>
#include <string.h>

char* gap(float jarak) {
    if (jarak < 3.5)
        return "Gogogo";
    else if (jarak >= 3.5 && jarak <= 10)
        return "Push";
    else
        return "Stay out of trouble";
}

char* fuel(int talit) {
    if (talit > 80)
        return "Push Push Push";
    else if (talit >= 50 && talit <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}

char* tire(int kanisir) {
    if (kanisir > 80)
        return "Go Push Go Push";
    else if (kanisir >= 50 && kanisir <= 80)
        return "Good Tire Wear";
    else if (kanisir >= 30 && kanisir < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}

char* tire_change(char* current_tire) {
    if (strcmp(current_tire, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(current_tire, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}

char* MauMuApaSu(){
    return "GOBLOG";

}


