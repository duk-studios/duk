//
// Created by Ricardo on 14/04/2023.
//

#ifndef DUK_MACROS_MACROS_H
#define DUK_MACROS_MACROS_H

#define DUK_NO_DISCARD [[nodiscard]]

// Stringifies a macro
#define DUK_STRINGIFY(x) DUK_DO_STRINGIFY(x)
#define DUK_DO_STRINGIFY(x) #x

#endif//DUK_MACROS_MACROS_H
