
#pragma once

#include <min-ray/object.h>

namespace min::ray {

/**
 * \brief Superclass of all emitters
 */
class Emitter : public NoriObject {
 public:
  /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
  EClassType getClassType() const { return EEmitter; }
};

}  // namespace min::ray
