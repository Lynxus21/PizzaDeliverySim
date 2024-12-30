#pragma once

#include "ofxAssimpModelLoader.h"
#include "ofMain.h"
#include "ofQuaternion.h"

class RedHouse {
public:
    glm::vec3 position;
    float size;
    ofColor color;
    glm::vec3 pizzaPickupTokenPosition;
};


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    std::string directionIndicator; // "Left", "Right", or "Forward"

    ofQuaternion rotation;  // Declare a quaternion for rotation

    // Define the block properties
    ofBoxPrimitive block; // 3D block
    glm::vec3 blockPosition;

    // Easy camera for 3D navigation
    ofEasyCam camera;

    ofxAssimpModelLoader model;
    void centerModel();

     std::string lastTurnDirection;

    // For displaying text
    ofTrueTypeFont font;

    bool isOverlapping(const glm::vec3& newPos, float minDist) {
        for (const auto& house : houses) {
            if (glm::distance(newPos, house.position) < minDist) {
                return true;
            }
        }
        return false;
    }

    RedHouse redHouse;

    ofxAssimpModelLoader carModel;
      ofxAssimpModelLoader pizzaShopModel;
      ofxAssimpModelLoader houseModel;



    struct House {
        glm::vec3 position;
        float size;
        ofColor color;
        glm::vec3 deliveryTokenPosition; // Added field for delivery token position

    };

    std::vector<House> houses; // Vector to store house objects


    int restartButtonX, restartButtonY, restartButtonWidth, restartButtonHeight;


    // For displaying tokens and counting
    ofSpherePrimitive token; // Token object
    glm::vec3 tokenPosition;

    float pizzaPickupTime; // Time when the pizza was picked up
       bool displayPizzaPickedUpMessage; // Flag to control the display of the message


     bool isPizzaDeliveryActive = false;

     float countdownTimer;
     bool isCountdownActive;

    // Pizza acquired token
    ofSpherePrimitive pizzaAcquiredToken; // Pizza acquired token object
    glm::vec3 pizzaAcquiredTokenPosition;

    void generateRandomPizzaDeliveryLocation();


    // Pizza delivery location token
    ofSpherePrimitive pizzaDeliveryLocation; // Pizza delivery location token object
    glm::vec3 pizzaDeliveryLocationPosition;

    //GPS System variables
    float pizzaDeliveryDirectionAngle; // Angle to the pizza delivery location
       glm::vec3 forwardDirection; // Direction in which the block is facing
       glm::vec3 toPizzaDelivery;  // Direction to the pizza delivery location

    int interactionCount = 0; // Count of interactions

    // Declaring blockWidth
    float blockWidth; // Width of the block

    bool showStrikeMessage;
       float strikeMessageStartTime; // Time when the strike message starts showing
       const float strikeMessageDuration = 10.0; // Duration to show the strike message

        std::string debugText;



    // Car variables
    float speed;
    float acceleration;  // Declare acceleration
    float deceleration;  // Declare deceleration
    float maxSpeed;      // Declare maxSpeed

    // Person variables
    float personSpeed;
    float personAcceleration;
    float personMaxSpeed;

    // Pizza pickup and delivery location variables
    glm::vec3 pizzaAcquiredPosition;
    glm::vec3 deliveryLocationPosition;

    // Vector to store "Person" blocks
    std::vector<ofBoxPrimitive> persons;

    int currentXP;
       const int xpPerDelivery = 20;
       const int maxXP = 100;

       ofMesh carMesh;            // Declare the car mesh
         ofMaterial wireframeMaterial;  // Declare the wireframe material
         ofMaterial meshMaterial;       // Declare the mesh material
       float headingAngle = 0.0;  // Declare headingAngle as a member variable

        int currentLevel;

    float maxPossibleDistance;

    int failedDeliveryCount;
      bool showFiredHUD;

    // Camera control variables
    bool cameraFollowsCar;

    // Flag to check if interaction has occurred with the orange token
    bool orangeTokenInteractionOccurred = false;

    float deliverySpeedFactor;
       float extraTimeBuffer;

    // Add this variable to keep track of token visibility
    bool isOrangeTokenVisible = true;

    // Flag to prevent continuous key press
    bool keyPressedFlag = false;

    // Variable to store the time of orange token interaction
    unsigned long long interactionStartTime;

    void drawSceneObjects();
    void drawHUD();

    // Function declarations for drawing car and person scenes
    void drawCarScene();
    void drawPersonScene();

    float lastDirectionChangeTime; // Time since the last direction change
       std::string lastDirectionIndicator; // Last stable direction indicator
       const float directionChangeDelay = 1.0f; // Delay in seconds for a direction change

       void setNewPizzaDeliveryLocation();
          void checkInteractions();



    // Event handlers
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
};
