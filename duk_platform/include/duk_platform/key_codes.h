#ifndef DUK_PLATFORM_KEY_CODES_H
#define DUK_PLATFORM_KEY_CODES_H

namespace duk::platform {

struct KeyModifiers {
    enum Bits : uint8_t {
        CTRL = 1,
        SHIFT = 2,
        ALT = 4,
    };

    static constexpr uint32_t kCount = 3;
    using Mask = uint8_t;
};
    
enum class MouseButton {
    LEFT,
    MIDDLE,
    RIGHT
};
    
enum class KeyAction : int {
    PRESS = 0,
    RELEASE = 1
};
    
enum class Keys {
    BACKSPACE,
    TAB,
    CLEAR,
    ENTER,
    SHIFT,
    CTRL,
    ALT,
    PAUSE,
    CAPS_LOCK,
    ESC,
    SPACE_BAR,
    PAGE_UP,
    PAGE_DOWN,
    END,
    HOME,
    LEFT_ARROW,
    UP_ARROW,
    RIGHT_ARROW,
    DOWN_ARROW,
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LWIN,
    RWIN,
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    MULTIPLY,
    ADD,
    SEPARATOR,
    SUBTRACT,
    DECIMAL,
    DIVIDE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    SEMICOLON,//0xBF
    COLON,//0xBF
    COMMA, //0xBC
    DASH, //0xBD
    DOT, //0xBE
    SLASH, //0xBF
    QUESTION_MARK, //0xBF
    
    
};
}

#endif //DUK_PLATFORM_KEY_CODES_H