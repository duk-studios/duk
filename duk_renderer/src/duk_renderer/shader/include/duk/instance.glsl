#ifndef DUK_INSTANCE_GLSL
#define DUK_INSTANCE_GLSL

#define DUK_INSTANCE_DECLARE(Type) Type _instances[]

#define DUK_INSTANCE_GET(binding, instance) (binding._instances[instance])

#endif
