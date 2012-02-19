// The registry maps board data values to corresponding fragment shaders.
// The registry is 1-indexed. Valid indices range from 1 to registry_size - 1.

#pragma once

typedef struct registry * registry_handle;

registry_handle registry_new(void);

void registry_delete(registry_handle r);

unsigned int registry_add(
    registry_handle r, const char * shader, size_t shader_len);

// Return the filename of the fragment shader corresponding to index.
const char * registry_get(registry_handle r, unsigned int index);

unsigned int registry_size(registry_handle r);
