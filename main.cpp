/*
* Auther: sNj
* Time Creation: 07.10.2023
*
*
* Description:
* Entry point.
* 
*
*/



/*
* Contains vk-part of applicaion.
*/
#include "BladeForgeBin.h"




int main() {
    BladeForge app;
    
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}