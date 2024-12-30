#include "ofApp.h"
#include "ofxAssimpModelLoader.h"


void ofApp::setNewPizzaDeliveryLocation() {
    if (!houses.empty()) {
        int randomHouseIndex = ofRandom(0, houses.size());
        pizzaDeliveryLocationPosition = houses[randomHouseIndex].deliveryTokenPosition;
    }
}
//--------------------------------------------------------------
void ofApp::setup() {

    // Load the first model (CarWithColour.obj)
        std::string carModelPath = "data/assets/CarWithColour.obj";  // Relative path to the first model file
        std::string carFullPath = ofFilePath::join(ofFilePath::getCurrentExeDir(), carModelPath);

        if (carModel.loadModel(carFullPath)) {
            carModel.setScale(0.5, 0.5, 0.5);
            carModel.enableColors();
            carModel.enableMaterials();
        } else {
            ofLogError("setup") << "Failed to load the CarWithColour.obj model!";
        }

        // Load the second model (PizzaShop.obj)
            std::string pizzaShopModelPath = "data/assets/PizzaShop.obj";  // Relative path to the second model file
            std::string pizzaShopFullPath = ofFilePath::join(ofFilePath::getCurrentExeDir(), pizzaShopModelPath);

            if (pizzaShopModel.loadModel(pizzaShopFullPath)) {
                pizzaShopModel.setScale(0.5, 0.5, 0.5);
                pizzaShopModel.enableColors();
                pizzaShopModel.enableMaterials();
            } else {
                ofLogError("setup") << "Failed to load the PizzaShop.obj model!";
            }

         // Load the first model (House.obj)
            std::string houseModelPath = "data/assets/House.obj";  // Relative path to the first model file
            std::string houseFullPath = ofFilePath::join(ofFilePath::getCurrentExeDir(), houseModelPath);

            if (houseModel.loadModel(houseFullPath)) {
                houseModel.setScale(0.5, 0.5, 0.5);
                houseModel.enableColors();
                houseModel.enableMaterials();
            } else {
                ofLogError("setup") << "Failed to load the CarWithColour.obj model!";
            }

    // Initialize block properties
    blockWidth = 50; //

///    blockPosition = glm::vec3(ofRandom(-200, 200), 0, ofRandom(-200, 200));
    blockPosition = glm::vec3(0, 0, 0);
    block.setPosition(blockPosition.x, blockPosition.y, blockPosition.z);

    // Initialize the maximum possible distance in the game world
    maxPossibleDistance = 3000.0f;

    failedDeliveryCount = 0;
    showFiredHUD = false;

    showStrikeMessage = false;

    // Initialize camera
    camera.setAutoDistance(false);
    camera.disableMouseInput();

    // Initialize token properties
    token.setRadius(20);
    tokenPosition = glm::vec3(100, 0, 100);
    token.setPosition(tokenPosition.x, tokenPosition.y, tokenPosition.z);

    pizzaAcquiredToken.setRadius(20);
    pizzaAcquiredTokenPosition = glm::vec3(500, 0, 500);
    pizzaAcquiredToken.setPosition(pizzaAcquiredTokenPosition.x, pizzaAcquiredTokenPosition.y, pizzaAcquiredTokenPosition.z);

    pizzaDeliveryLocation.setRadius(20);
    pizzaDeliveryLocationPosition = glm::vec3(-1000, 0, -1000);
    pizzaDeliveryLocation.setPosition(pizzaDeliveryLocationPosition.x, pizzaDeliveryLocationPosition.y, pizzaDeliveryLocationPosition.z);

    // Set initial state for the orange token interaction
    orangeTokenInteractionOccurred = false;

    // Start by following the car
    cameraFollowsCar = true;

    // Initialize car controls
    speed = 0;
    acceleration = 0.1;
    deceleration = 0.2;
    maxSpeed = 5;

    // Initialize person controls
    personSpeed = 0;
    personAcceleration = 0.1;
    personMaxSpeed = 2;

    pizzaPickupTime = 0;
        displayPizzaPickedUpMessage = false;

        currentXP = 0; // Starting XP

        currentLevel = 1; // Starting level

        /*
        // Set the position of the red house
         redHouse.position = pizzaAcquiredTokenPosition - glm::vec3(0, 0, 50); // Adjust the offset as needed
         redHouse.size = 50;
         redHouse.color = ofColor::red;
         redHouse.pizzaPickupTokenPosition = pizzaAcquiredTokenPosition; // Set it to the same position as the orange token
        */

    // Initialize delivery speed factor and extra time buffer
    deliverySpeedFactor = 0.5;
    extraTimeBuffer = 5;

    // Define the number of houses and their minimum separation radius
    int numHouses = 60;
    float houseRadius = 100.0f;
    int maxAttempts = 100;

    glm::vec3 center(0, 0, 0);
    float exclusionRadius = 300.0f;
    float mapBoundaryRadius = 3000.0f;

    houses.clear();

    // Generate house positions
    for (int i = 0; i < numHouses; ++i) {
        bool positionFound = false;
        int attempts = 0;

        while (!positionFound && attempts < maxAttempts) {
            float angle = ofRandom(0, TWO_PI);
            float distance = sqrt(ofRandom(pow(exclusionRadius, 2), pow(mapBoundaryRadius, 2)));
            glm::vec3 newPos = center + glm::vec3(distance * cos(angle), 0, distance * sin(angle));

            if (!isOverlapping(newPos, houseRadius)) {
                positionFound = true;
                House house;
                house.position = newPos;
                house.size = 50;
                house.color = ofColor::green;
                glm::vec3 offset = glm::normalize(glm::vec3(ofRandom(-1, 1), 0, ofRandom(-1, 1))) * house.size;
                house.deliveryTokenPosition = house.position + offset;
                houses.push_back(house);
            }
            attempts++;
        }

        if (!positionFound) {
            ofLogNotice() << "Failed to place a house without overlap after " << maxAttempts << " attempts";
        }
    }

    // Set initial pizza delivery location
    generateRandomPizzaDeliveryLocation();
}



void ofApp::generateRandomPizzaDeliveryLocation() {
    if (!houses.empty()) {
        int randomHouseIndex = ofRandom(0, houses.size());
        pizzaDeliveryLocationPosition = houses[randomHouseIndex].deliveryTokenPosition;
    }
}

//--------------------------------------------------------------
void ofApp::update() {

    // Update the countdown timer if active
    if (isCountdownActive) {
           countdownTimer -= ofGetLastFrameTime();
           if (countdownTimer <= 0 && !showFiredHUD) {
               countdownTimer = 0;
               isCountdownActive = false;
               failedDeliveryCount++;
               if (failedDeliveryCount >= 3) {
                   showFiredHUD = true;
               } else {
                   showStrikeMessage = true;
                   strikeMessageStartTime = ofGetElapsedTimef();
               }
           }
       }

       if (showStrikeMessage && ofGetElapsedTimef() - strikeMessageStartTime > strikeMessageDuration) {
           showStrikeMessage = false;
       }

    glm::vec3 oldPosition = blockPosition;
    float distanceBehind = 300.0; // Adjust this value as needed
    float distanceAbove = 150.0;   // Adjust this value as needed

    if (cameraFollowsCar) {
        if (ofGetKeyPressed(OF_KEY_LEFT)) {
            block.rotateDeg(1, glm::vec3(0, 1, 0));
        }

        if (ofGetKeyPressed(OF_KEY_RIGHT)) {
            block.rotateDeg(-1, glm::vec3(0, 1, 0));
        }

        glm::vec3 forwardDirection = block.getLookAtDir();

        if (ofGetKeyPressed(OF_KEY_UP)) {
            speed += acceleration;
            speed = ofClamp(speed, 0, maxSpeed);
            blockPosition += forwardDirection * speed;
        } else if (ofGetKeyPressed(OF_KEY_DOWN)) {
            speed -= acceleration;
            speed = ofClamp(speed, -maxSpeed, maxSpeed);
            blockPosition += forwardDirection * speed;
        } else {
            speed *= 0.95;
            blockPosition += forwardDirection * speed;
        }

        if (abs(speed) < 0.05) {
            speed = 0;
        }

        block.setPosition(blockPosition.x, blockPosition.y, blockPosition.z);


        // Check interaction with orange token first
        if (!orangeTokenInteractionOccurred && glm::distance(blockPosition, pizzaAcquiredTokenPosition) < (blockWidth / 2 + pizzaAcquiredToken.getRadius())) {
            orangeTokenInteractionOccurred = true;
            interactionStartTime = ofGetElapsedTimeMillis();
            pizzaAcquiredTokenPosition = glm::vec3(0, -1000, 0); // Hide the pizza acquired token
            generateRandomPizzaDeliveryLocation();
            isPizzaDeliveryActive = true;
            pizzaPickupTime = ofGetElapsedTimef();
            displayPizzaPickedUpMessage = true;

            // Calculate distance to delivery location
            float distanceToDelivery = glm::distance(blockPosition, pizzaDeliveryLocationPosition);

            // Calculate the base time to reach the delivery location at top speed
            float baseTimeToReach = distanceToDelivery / maxSpeed;

            // Further reduce the scaling factor
            float timeScalingFactor = 0.05f; // Adjust this value based on testing

            // Calculate final countdown timer
            countdownTimer = baseTimeToReach * timeScalingFactor;

            // Optional: Consider a very minimal extra time buffer
            float minimalExtraBuffer = 0.5f; // 0.5 seconds or even consider removing it
            countdownTimer += minimalExtraBuffer;

            isCountdownActive = true;
        }
        // Hide the pizza picked up message after 10 seconds
            if (displayPizzaPickedUpMessage && ofGetElapsedTimef() - pizzaPickupTime > 10.0f) {
                displayPizzaPickedUpMessage = false;
            }

            // Check interaction with yellow token (pizza delivery)
                    if (orangeTokenInteractionOccurred && glm::distance(blockPosition, pizzaDeliveryLocationPosition) < (blockWidth / 2 + pizzaDeliveryLocation.getRadius())) {
                        interactionCount++;
                        if (isCountdownActive && countdownTimer > 0) {
                            currentXP += xpPerDelivery;
                            if (currentXP >= maxXP) {
                                currentXP = 0; // Reset XP to zero
                                currentLevel++; // Increment level
                            }
                        }
                        pizzaDeliveryLocationPosition = glm::vec3(0, -1000, 0);
                        pizzaAcquiredTokenPosition = glm::vec3(500, 0, 500);
                        orangeTokenInteractionOccurred = false;
                        isPizzaDeliveryActive = false;
                        isCountdownActive = false; // Reset the countdown timer
                    }


        // Update the target position to follow the car
        glm::vec3 targetPosition = blockPosition - block.getLookAtDir() * distanceBehind + glm::vec3(0, distanceAbove, 0);
        camera.setPosition(targetPosition.x, targetPosition.y, targetPosition.z);
        camera.lookAt(blockPosition);
    } else {
        for (auto& person : persons) {
            if (ofGetKeyPressed(OF_KEY_LEFT)) {
                person.rotateDeg(1, glm::vec3(0, 1, 0));
            }

            if (ofGetKeyPressed(OF_KEY_RIGHT)) {
                person.rotateDeg(-1, glm::vec3(0, 1, 0));
            }

            glm::vec3 forwardDirection = person.getLookAtDir();

            if (ofGetKeyPressed(OF_KEY_UP)) {
                personSpeed += personAcceleration;
                personSpeed = ofClamp(personSpeed, 0, personMaxSpeed);
                person.setPosition(person.getPosition() + forwardDirection * personSpeed);
            } else if (ofGetKeyPressed(OF_KEY_DOWN)) {
                personSpeed -= personAcceleration;
                personSpeed = ofClamp(personSpeed, -personMaxSpeed, personMaxSpeed);
                person.setPosition(person.getPosition() + forwardDirection * personSpeed);
            }
        }

        // Reset the orange token interaction flag after a certain duration
        if (orangeTokenInteractionOccurred && ofGetElapsedTimeMillis() - interactionStartTime >= 2000) {
            orangeTokenInteractionOccurred = false;
        }
    }
    // Member variables for hysteresis and smoothing
    float lastDirectionChangeTime = 0;
    float directionChangeDelay = 1.0; // delay in seconds
    std::string lastDirectionIndicator = "";

    if (isPizzaDeliveryActive) {
            glm::vec3 toPizzaDelivery = glm::normalize(pizzaDeliveryLocationPosition - blockPosition);
            glm::vec3 forwardDirection = glm::normalize(block.getLookAtDir());

            // Calculate the angle in degrees between forward direction and toPizzaDelivery
            float angle = glm::degrees(acos(glm::dot(forwardDirection, toPizzaDelivery)));
            // Determine the sign of the angle
            float crossProductY = glm::cross(forwardDirection, toPizzaDelivery).y;

            // Use angular thresholds to determine direction
            if (angle < 15) {  // Small angle: Go forward
                directionIndicator = "Forward";
            } else if (angle > 165) {  // Nearly opposite: Turn around
                directionIndicator = "Turn Around";
            } else {
                // Check the sign for left or right
                if (crossProductY > 0) {
                    directionIndicator = "Left";
                } else {
                    directionIndicator = "Right";
                }
            }

            // Implementing hysteresis and smoothing
            float currentTime = ofGetElapsedTimef();
            if (directionIndicator != lastDirectionIndicator) {
                if (currentTime - lastDirectionChangeTime > directionChangeDelay) {
                    lastDirectionChangeTime = currentTime;
                    lastDirectionIndicator = directionIndicator;
                } else {
                    directionIndicator = lastDirectionIndicator;
                }
            }
        }
    }



//--------------------------------------------------------------
void ofApp::draw() {
    camera.begin(); // Begin 3D camera

    // Draw a grid on the floor
    ofPushMatrix();
    ofRotateXDeg(0); // Rotate to lay flat
    ofDrawGrid(500, 10, false, false, true, false); // Draw a 500x500 grid
    ofPopMatrix();


    // Draw the loaded 3D model (car)
    ofSetColor(ofColor::grey); // Set the color before drawing
    carModel.setPosition(blockPosition.x, blockPosition.y, blockPosition.z);
//    model.setRotation(0, -ofRadToDeg(headingAngle), 0, 1, 0); // Adjust rotation based on your model's orientation
    carModel.setRotation(0, -block.getHeadingDeg(), 0, 1, 0); // Adjust rotation based on your model's orientation
    carModel.drawFaces();


    // Draw the pizzaAcquiredToken
    ofSetColor(ofColor::orange);
    if (!orangeTokenInteractionOccurred) {
        pizzaAcquiredToken.setPosition(pizzaAcquiredTokenPosition.x, pizzaAcquiredTokenPosition.y, pizzaAcquiredTokenPosition.z);
        pizzaAcquiredToken.draw();
    }

    // Calculate the position to place the pizzaShopModel in front of the token
    glm::vec3 pizzaShopPosition = glm::vec3(500, 15, 500) + glm::vec3(0, 60, -10);

        // Draw the pizzaShopModel in front of the pizza pickup token
        ofSetColor(ofColor::blue);
        pizzaShopModel.setPosition(pizzaShopPosition.x, pizzaShopPosition.y, pizzaShopPosition.z);
        pizzaShopModel.drawFaces();


    // Draw the pizzaDeliveryLocation
    ofSetColor(ofColor::yellow);
    if (orangeTokenInteractionOccurred) {
        pizzaDeliveryLocation.setPosition(pizzaDeliveryLocationPosition.x, pizzaDeliveryLocationPosition.y, pizzaDeliveryLocationPosition.z);
        pizzaDeliveryLocation.draw();
    }

    glm::vec3 forwardVec = block.getLookAtDir() * 100.0f;
    glm::vec3 lineStart = blockPosition + block.getLookAtDir() * blockWidth;
    glm::vec3 lineEnd = lineStart + forwardVec;
    ofDrawLine(lineStart, lineEnd);

    // Draw the person block
    ofSetColor(ofColor::red);
    for (auto& person : persons) {
        person.drawWireframe();
    }



    // Draw houses
    for (const auto& house : houses) {
        ofSetColor(house.color);
        ofDrawBox(house.position.x, house.position.y, house.position.z, house.size);
    }

    camera.end(); // End 3D camera

    // 2D Overlay
    // Draw the speedometer
    std::string pizzaCounter = "Pizzas Delivered: " + ofToString(interactionCount);
    ofSetColor(ofColor::white);
    ofDrawBitmapString(pizzaCounter, 20, 30);

    // Draw the speed
    std::string speedText = "Speed: " + ofToString(speed);
    ofDrawBitmapString(speedText, 20, 45);


    /*
    // Strike message
    if (showStrikeMessage) {
        std::string strikeMessage = "Strike " + ofToString(failedDeliveryCount) + "/3, deliver on time or you're fired!";
        ofSetColor(ofColor::red);
        int textWidth = font.stringWidth(strikeMessage);
        int textX = ofGetWidth() / 2 - textWidth / 2;
        int textY = ofGetHeight() / 2 - 50;
        ofDrawBitmapString(strikeMessage, textX, textY);
    }
    */

    // Fired HUD
    if (showFiredHUD) {
        std::string firedMessage = "Strike 3/3, YOU'RE FIRED!!";
        std::string restartMessage = "ClICK HERE TO RESTART";
        int firedMessageX = ofGetWidth() / 2;
        int firedMessageY = ofGetHeight() / 2;
        int restartMessageX = ofGetWidth() / 2;
        int restartMessageY = firedMessageY + 30;
        ofDrawBitmapStringHighlight(firedMessage, firedMessageX, firedMessageY);
        ofDrawBitmapStringHighlight(restartMessage, restartMessageX, restartMessageY);
    }

    // Pager Box
    const int boxWidth = 330;
    const int boxHeight = 150;
    const int boxX = ofGetWidth() - boxWidth - 20;
    const int boxY = 20;

    std::string pagerText;

    // Draw the pager box
    ofSetColor(0, 0, 0);
    ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);

    // Pager Text
    ofSetColor(255, 255, 255);
    if (isPizzaDeliveryActive) {
        pagerText += "Direction to Delivery: " + directionIndicator + "\n";
    }
    if (displayPizzaPickedUpMessage) {
        pagerText += "Follow the directions above\n Pizza Picked Up!\n";
    }
    if (isCountdownActive) {
        pagerText += "Time Left: " + ofToString(countdownTimer) + "\n";
    }
    if (showStrikeMessage) {
        pagerText += "Strike " + ofToString(failedDeliveryCount) + "/3\nFail to deliver and you're fired!";
    }
    if (!orangeTokenInteractionOccurred) {
        pagerText += "Return back to pizza shop (Orange Orb)!\n";
    }

    // Draw the formatted pager text
    ofDrawBitmapString(pagerText, boxX + 10, boxY + 20);

    // Draw XP Bar
      float xpBarWidth = ofGetWidth() - 40; // Full width for XP bar
      float xpBarHeight = 20; // Height of XP bar
      float xpBarX = 20; // X position of XP bar
      float xpBarY = ofGetHeight() - 30; // Y position of XP bar (30 pixels from bottom)

      // Background of the XP bar
      ofSetColor(150); // Gray color for background
      ofDrawRectangle(xpBarX, xpBarY, xpBarWidth, xpBarHeight);

      // Foreground of the XP bar (actual XP)
      float xpPercentage = (float)currentXP / maxXP; // Calculate XP percentage
      ofSetColor(0, 255, 0); // Green color for XP
      ofDrawRectangle(xpBarX, xpBarY, xpBarWidth * xpPercentage, xpBarHeight);

      // Draw XP Text
      std::string xpText = "XP: " + ofToString(currentXP) + " / " + ofToString(maxXP);
      ofSetColor(255); // White color for text
      ofDrawBitmapString(xpText, xpBarX, xpBarY - 5); // Position text above the bar

      // Draw Level Text
      std::string levelText = "Level " + ofToString(currentLevel);
      ofSetColor(255); // White color for text

      // Calculate text width and position
      int levelTextWidth = font.stringWidth(levelText);
      int levelTextX = (ofGetWidth() / 2) - (levelTextWidth / 2); // Center text horizontally
      int levelTextY = ofGetHeight() - 40; // Position text above the XP bar

      ofDrawBitmapString(levelText, levelTextX, levelTextY);


}
//--------------------------------------------------------------

void ofApp::keyPressed(int key) {
    if (key == 'E' || key == 'e') {
        cout << "E key pressed" << endl;

        if (cameraFollowsCar) {
            // Switch to person controls and camera
            cameraFollowsCar = false;

            if (persons.empty()) {
                // Spawn the person if not already spawned
                glm::vec3 personPosition = blockPosition;
                personPosition.x += 30.0;

                ofBoxPrimitive person;
                person.set(20);
                person.setPosition(personPosition.x, personPosition.y, personPosition.z);

                persons.push_back(person);
            }
        } else {
            // Switch back to car controls and camera
            cameraFollowsCar = true;

            // Despawn the person
            persons.clear();
        }
    }

    if (cameraFollowsCar) {
        if (key == OF_KEY_LEFT) {
            block.rotateDeg(1, glm::vec3(0, 1, 0));
        } else if (key == OF_KEY_RIGHT) {
            block.rotateDeg(-1, glm::vec3(0, 1, 0));
        } else if (key == OF_KEY_UP) {
            speed += acceleration;
            speed = ofClamp(speed, 0, maxSpeed);
            blockPosition += block.getLookAtDir() * speed;
        } else if (key == OF_KEY_DOWN) {
            speed -= acceleration;
            speed = ofClamp(speed, -maxSpeed, maxSpeed);
            blockPosition += block.getLookAtDir() * speed;
        }
    } else {
        if (key == OF_KEY_LEFT) {
            for (auto& person : persons) {
                person.rotateDeg(1, glm::vec3(0, 1, 0));
            }
        } else if (key == OF_KEY_RIGHT) {
            for (auto& person : persons) {
                person.rotateDeg(-1, glm::vec3(0, 1, 0));
            }
        } else if (key == OF_KEY_UP) {
            for (auto& person : persons) {
                personSpeed += personAcceleration;
                personSpeed = ofClamp(personSpeed, 0, personMaxSpeed);
                person.setPosition(person.getPosition() + person.getLookAtDir() * personSpeed);
            }
        } else if (key == OF_KEY_DOWN) {
            for (auto& person : persons) {
                personSpeed -= personAcceleration;
                personSpeed = ofClamp(personSpeed, -personMaxSpeed, personMaxSpeed);
                person.setPosition(person.getPosition() + person.getLookAtDir() * personSpeed);
            }
        }
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'e' || key == 'E') {
        keyPressedFlag = false;
    }
}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if (showFiredHUD) {
        // Define the area for the restart button
        int buttonX = ofGetWidth() / 2 - 50;
        int buttonY = ofGetHeight() / 2 + 20;
        int buttonWidth = 100;
        int buttonHeight = 50;

        // Check if the click is within the restart button area
        if (x > buttonX && x < buttonX + buttonWidth &&
            y > buttonY && y < buttonY + buttonHeight) {

            // Reset the game state for a new start
            failedDeliveryCount = 0;
            showFiredHUD = false;
            isCountdownActive = false;
            countdownTimer = 0;
            interactionCount = 0;
            blockPosition = glm::vec3(ofRandom(-200, 200), 0, ofRandom(-200, 200));
            currentXP = 0;
            currentLevel = 1;

            // Reset pizza delivery location to a specific position
            pizzaDeliveryLocationPosition = glm::vec3(100, 0, 100);

            // Reset other related states for pizza delivery
            isPizzaDeliveryActive = false;
            orangeTokenInteractionOccurred = false;

            // Reset the position and state of the pizzaAcquiredToken
            pizzaAcquiredTokenPosition = glm::vec3(500, 0, 500); // Adjust as needed
            pizzaAcquiredToken.setPosition(pizzaAcquiredTokenPosition.x, pizzaAcquiredTokenPosition.y, pizzaAcquiredTokenPosition.z);

        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

