#include <iostream>
#include <duk_event/event.h>

int main() {
    int myInt = 0;
    
    duk::event::Listener mylistener;
    duk::event::EventT<int> myIntEvent;

    std::cout << "myInt current value: " << myInt << std::endl;
    
    mylistener.listen(myIntEvent, [&myInt] (int value) {
        myInt = value;
    });
    
    myIntEvent.emit(10);

    std::cout << "myInt new value after the event was emitted is: " << myInt << std::endl;

    return 0;
}
