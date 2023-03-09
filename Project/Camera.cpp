#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition); //cameraDirection
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
       // this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));


    }


    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    glm::vec3 Camera::getCameraPosition() {
        return cameraPosition;
    }

    void Camera::setPosition(const glm::vec3& position) {
        this->cameraPosition = position;
        //view = glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    void Camera::updateCameraVectors()
    {
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    // Set the target of the camera
    void Camera::setTarget(glm::vec3 target) {
        this->cameraTarget = target;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::moveAt(glm::vec3 position, glm::vec3 front)
    {
        this->cameraPosition = position;
        this->cameraTarget = front;
    }


    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO

        switch (direction) {
        case MOVE_FORWARD:
            this->cameraPosition = this->cameraPosition + this->cameraFrontDirection * speed;
            this->cameraPosition = this->cameraPosition - this->cameraUpDirection * speed;
            break;

        case MOVE_BACKWARD:
            this->cameraPosition = this->cameraPosition - this->cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition -= glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
            break;

        case MOVE_LEFT:
            cameraPosition += glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
            break;

        case MOVE_UP:
            this->cameraPosition = this->cameraPosition + this->cameraUpDirection * speed;
            break;

        case MOVE_DOWN:
            this->cameraPosition = this->cameraPosition - this->cameraUpDirection * speed;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        
        glm::vec3 direction;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFrontDirection = glm::normalize(direction);
    }

   
}