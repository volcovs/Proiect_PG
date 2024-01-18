#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        //cele 3 axe ale sistemului de coordonate camera
        this->cameraUpDirection = glm::normalize(cameraUp);
        this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));

    }


    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    glm::vec3 Camera::getCameraPosition() {
        return this->cameraPosition;
    }

    void Camera::setCameraPosition(glm::vec3 cameraPosition) {
        this->cameraPosition = cameraPosition;
    }

    void Camera::updateDirection(glm::vec3 cameraTarget) {
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }


    //update the camera internal paarameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUpDirection));

        switch (direction) {
        case MOVE_FORWARD: 
            this->cameraPosition += speed * cameraDirection;
            this->cameraTarget += speed * cameraDirection;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            }
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        case MOVE_BACKWARD: 
            this->cameraPosition -= speed * cameraDirection;
            this->cameraTarget -= speed * cameraDirection;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            }
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        case MOVE_LEFT: 
            this->cameraPosition -= speed * cameraRight;
            this->cameraTarget -= speed * cameraRight;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            } 
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        case MOVE_RIGHT: 
            this->cameraPosition += speed * cameraRight;
            this->cameraTarget += speed * cameraRight;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            }
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        case MOVE_UP:
            this->cameraPosition += this->cameraUpDirection * speed;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            }
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        case MOVE_DOWN:
            this->cameraPosition -= this->cameraUpDirection * speed;

            if (this->cameraPosition.y < 0.5f) {
                this->cameraPosition.y = 0.5f;
            }

            if (this->cameraPosition.x < -5.5f) {
                this->cameraPosition.x = -5.5f;
            }
            else if (this->cameraPosition.x > 5.5f) {
                this->cameraPosition.x = 5.5f;
            }

            if (this->cameraPosition.z < -5.5f) {
                this->cameraPosition.z = -5.5f;
            }
            else if (this->cameraPosition.z > 5.5f) {
                this->cameraPosition.z = 5.5f;
            }
            break;
        default: 
            this->cameraPosition = this->cameraPosition;
        }
   
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //this code works, but there is a better implementation
        /*
        glm::mat4 x_rotation = glm::mat4(1.0f);
        glm::mat4 y_rotation = glm::mat4(1.0f);

        x_rotation = glm::rotate(x_rotation, yaw, glm::vec3(1.0f, 0.0f, 0.0f));
        y_rotation = glm::rotate(y_rotation, pitch, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec4 pos = x_rotation * y_rotation * glm::vec4(this->cameraPosition, 1.0f);
        glm::vec4 target = x_rotation * y_rotation * glm::vec4(this->cameraTarget, 1.0f);

        this->cameraPosition = glm::vec3(pos.x, pos.y, pos.z);
        this->cameraTarget = glm::vec3(target.x, target.y, target.z);

        this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraRightDirection));
        */

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraTarget = glm::normalize(direction);
    }
}