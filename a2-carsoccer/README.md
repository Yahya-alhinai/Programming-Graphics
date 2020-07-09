# YAHYA ALHINAI #5181597 - Assignment 2

# Dscriptions of design decisions

- In the joystick_direction(), The car's speed and angle are detected. As well as the bounds of the wall for the car. If there is a car/wall contact, the car position will be pushed back to adjuest the contact to be just touching.
- The timeStep between two frames in magnified by 100 times to use it in the car/ball motion.
- When ever the ball is collided with the wall it loses 10% of its momentum. Otherwise, it constantly losing a small proportion of its momentum because of air friction.
- the car acceleration increases progressively as long as the the button is pressed upto a maximum and it loses acceleration degradly when the button is released. to mimic real world application.
- The collection force of the car-ball depends on the car acceleration (assuming the ball is light to have an effect on the collection). The ball reflect on the normal vector of the collision.
- The function UpdateSimulation() keep detecting any collision between the ball and the wall to reflect the ball speed to the other direction with a slight loss of momentum because of friction.
