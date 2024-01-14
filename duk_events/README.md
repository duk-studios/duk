# duk_events
- Defines event objects that can be used to invoke callbacks on n-listeners

# Usage
```cpp
int main() {
    int myInt = 0;
    
    duk::events::EventListener mylistener;
    duk::events::EventT<int> myIntEvent;

    std::cout << "myInt current value: " << myInt << std::endl;
    
    mylistener.listen(myIntEvent, [&myInt] (int value) {
        myInt = value;
    });
    
    myIntEvent.emit(10);

    std::cout << "myInt new value after the event was emitted is: " << myInt << std::endl;

    return 0;
}

```