
#ifndef IENTITY_HPP_
# define IENTITY_HPP_

# include <GL/gl.h>
# include <SFML/Graphics.hpp>
# include <glm/glm.hpp>
# include "Shader.hpp"
# include "AObject.hpp"

class IEntity: public AObject
{
public:
    virtual ~IEntity() {};
    
    virtual void draw(Shader *shader) = 0;
};

#endif /* IENTITY_HPP_ */
