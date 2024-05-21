#ifndef _GLGLM_H_
#define _GLGLM_H_

//#define GLM_FORCE_ALIGNED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>

using vec3Array = std::vector<glm::vec3>;


static const glm::vec3 g_XNormal{ 1., 0., 0. };
static const glm::vec3 g_YNormal{ 0., 1., 0. };
static const glm::vec3 g_ZNormal{ 0., 0., 1. };
static const glm::mat4 g_Mat4Normal = glm::mat4{ 1 };


#endif // !_GLGLM_H_
