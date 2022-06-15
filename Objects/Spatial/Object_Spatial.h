#ifndef OBJECT_SPATIAL
#define OBJECT_SPATIAL OBJECT_SPATIAL


#include "../Base/Object.h"

#include "glm/glm.hpp"

const auto OBJECT_CLASS_SPATIAL = "Object_Spatial";


class Object_Spatial : public Object {
	
	// shortcut constants for getting and setting spatial attributes
public:
	const char* ATTRIBUTE_SPATIAL_POSITION_X = "SpatialPosX";
	const char* ATTRIBUTE_SPATIAL_POSITION_Y = "SpatialPosY";
	const char* ATTRIBUTE_SPATIAL_POSITION_Z = "SpatialPosZ";

	const char* ATTRIBUTE_SPATIAL_FACING_X = "FacingX";
	const char* ATTRIBUTE_SPATIAL_FACING_Y = "FacingY";
	const char* ATTRIBUTE_SPATIAL_FACING_Z = "FacingZ";
	const char* ATTRIBUTE_SPATIAL_FACING_ROTATION = "FacingRotation";

	const char* ATTRIBUTE_SPATIAL_SCALE_X = "SpatialScaleX";
	const char* ATTRIBUTE_SPATIAL_SCALE_Y = "SpatialScaleY";
	const char* ATTRIBUTE_SPATIAL_SCALE_Z = "SpatialScaleZ";

	const char* ATTRIBUTE_SPATIAL_PARENT_TRANSFORMATIONS_INHERIT = "InheritParentTransform";


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