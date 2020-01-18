
#pragma once

#include <min-ray/object.h>

namespace min::ray {

/**
 * \brief Load a scene from the specified filename and
 * return its root object
 */
extern NoriObject *loadFromXML(const std::string &filename);

}  // namespace min::ray
