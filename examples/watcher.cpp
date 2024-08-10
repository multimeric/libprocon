#include <stdio.h>
#include "procon.h"

int main(){
    // Continually listend for events
    for (auto event: proc_iter()){
        switch(event->what){
            // Whenever a process exits, log it to stdout
            case PROC_EVENT_EXIT:
                {
                    auto exit = event->event_data.exit;
                    std::cout << "PID: " << exit.process_pid << " exited with code " << exit.exit_code << "\n";
                }
            break;

            default:
            break;
        }
    }
    return 0;
}