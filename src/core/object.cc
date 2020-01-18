
#include <min-ray/object.h>

namespace min::ray {

void NoriObject::addChild(NoriObject *) {
  throw NoriException(
      "NoriObject::addChild() is not implemented for objects of type '%s'!",
      classTypeName(getClassType()));
}

void NoriObject::activate() { /* Do nothing */
}
void NoriObject::setParent(NoriObject *) { /* Do nothing */
}

std::map<std::string, NoriObjectFactory::Constructor> *NoriObjectFactory::m_constructors = nullptr;

void NoriObjectFactory::registerClass(const std::string &name, const Constructor &constr) {
  if (!m_constructors)
    m_constructors = new std::map<std::string, NoriObjectFactory::Constructor>();
  (*m_constructors)[name] = constr;
}

}  // namespace min::ray
