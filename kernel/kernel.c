#include "include/kernel.h"

int main(int argc, char** argv){

    if(!iniciar_programa()) {
        terminar_programa();
        return EXIT_SUCCESS;
    }

    terminar_programa();

    return EXIT_SUCCESS;
}