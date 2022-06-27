#ifndef OBJECT_SPATIAL
#define OBJECT_SPATIAL OBJECT_SPATIAL


#include "../Base/Object.h"

#include "glm/glm.hpp"

const auto OBJECT_CLASS_SPATIAL = "Object_Spatial";

const auto ATTRIBUTE_SPATIAL_POSITION_X = "SpatialPosX";
const auto ATTRIBUTE_SPATIAL_POSITION_Y = "SpatialPosY";
const auto ATTRIBUTE_SPATIAL_POSITION_Z = "SpatialPosZ";

// object rotation about the x-axis
const auto ATTRIBUTE_SPATIAL_ROTATE_NOD = "SpatialNod";
// object rotation about the y-axis
const auto ATTRIBUTE_SPATIAL_ROTATE_TURN = "SpatialTurn"; 
// rotation about the vector resultant from the z-axis transformed by the previous two rotations
const auto ATTRIBUTE_SPATIAL_ROTATE_TILT = "SpatialTilt"; 

const auto ATTRIBUTE_SPATIAL_SCALE_X = "SpatialScaleX";
const auto ATTRIBUTE_SPATIAL_SCALE_Y = "SpatialScaleY";
const auto ATTRIBUTE_SPATIAL_SCALE_Z = "SpatialScaleZ";

// perform parent transformations in addition to own, default=true
const auto ATTRIBUTE_SPATIAL_PARENT_TRANSFORMATIONS_INHERIT = "InheritParentTransform";

class Object_Spatial : public Object {
	
public:
	Object_Spatial();
	virtual ~Object_Spatial();

	/* calculates the transformation matrix of the object,
	function is virtual to allow for additional transformations, such as camera-specific transformations */
	virtual glm::mat4 getTransformationMatrix(glm::mat4 childTransform = glm::mat4(1.0));
	

protected:
	// builds a rotation matrix using a starting orientation and an ending orientation
	glm::mat4 rotateToVec(glm::vec3 rotateFrom, glm::vec3 rotateTo);


};

#endif //OBJECT_SPATIAL