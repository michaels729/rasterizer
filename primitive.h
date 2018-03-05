#ifndef PRIMITIVE_H
#define PRIMITIVE_H

class Primitive
{
public:
    virtual std::vector<glm::vec3> getVertices() const = 0;
    virtual std::vector<glm::vec3> getColors() const = 0;
};

#endif // PRIMITIVE_H
