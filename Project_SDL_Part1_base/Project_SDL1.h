// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// Defintions
constexpr double frame_rate = 15.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 700; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

// Helper function to initialize SDL
void init();


class renderedObject {
protected:
    SDL_Surface* window_surface_ptr_; // ptr to the surface on which we want the
                                      // renderedObject to be drawn, non-owning
    SDL_Surface*
        image_ptr_; // The texture of the renderedObject (the loaded image)
    int pos_x_;
    int pos_y_;

public:
    renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr,
        int pos_x, int pos_y);
    ~renderedObject();

    virtual void draw() = 0;
    int get_pos_x() const;
    int get_pos_y() const;
    void set_image_ptr(const std::string& file_path);
};

class movingObject : public renderedObject {
protected:
    int vit_x_;
    int vit_y_;

public:
    movingObject(const std::string& file_path, SDL_Surface* window_surface_ptr,
        int pos_x, int pos_y, int vit_x, int vit_y);
    ~movingObject();

    virtual void move() = 0;
    void verifPosition();
    int getVit_x() const;
    int getVit_y() const;
    void setVit_x(int vit_x);
    void setVit_y(int vit_y);
};

class interactiveObject : public movingObject {
protected:
    std::vector<std::string> properties_; // type, sexe, reproduction_timer, alive

public:
    interactiveObject(const std::string& file_path,
        SDL_Surface* window_surface_ptr, int pos_x, int pos_y,
        int vit_x, int vit_y);
    ~interactiveObject();

    virtual bool interact(std::shared_ptr<interactiveObject> interObject) = 0;
    std::vector<std::string>& getProperties();
};

class sheperd : public interactiveObject {

public:
    sheperd(SDL_Surface* window_surface_ptr);
    ~sheperd();

    void draw() override;
    void move() override;
    bool interact(std::shared_ptr<interactiveObject> interObject) override;
};

class animal : public interactiveObject {

public:
    animal(const std::string& file_path, SDL_Surface* window_surface_ptr,
        int pos_x, int pos_y, int vit_x, int vit_y);
    animal(const std::string& file_path, SDL_Surface* window_surface_ptr);
    ~animal();

    void draw() override;
};

// class sheep, derived from animal
class sheep : public animal {

private:
    int reproductionTimer_;
    int escapeTimer_;
    bool escape_;

public:
    sheep(SDL_Surface* window_surface_ptr, int pos_x, int pos_y, int vit_x,
        int vit_y, std::string couleur);
    sheep(SDL_Surface* window_surface_ptr);
    ~sheep();

    void move() override;
    const int getReproductionTimer();
    void setReproductionTimer(int newTime);
    bool interact(std::shared_ptr<interactiveObject> interObject) override;
    bool reproduit(int pos_sheep_x, int pos_sheep_y);
    bool escape(int pos_wolf_x, int pos_wolf_y);
};

// class wolf, derived from animal
class wolf : public animal {

    bool chasse_;
    bool escape_;
    int escapeTimer_;

public:
    wolf(SDL_Surface* window_surface_ptr);
    ~wolf();

    void move() override;
    bool interact(std::shared_ptr<interactiveObject> interObject) override;
    bool croque(int pos_sheep_x, int pos_sheep_y);
    bool escape(int pos_dog_x, int pos_dog_y);
};

// class zombie, derived from animal
class zombie : public animal {
    bool escape_;
    int escapeTimer_;
    bool chasse_;

public:
    zombie(SDL_Surface* window_surface_ptr);
    zombie(SDL_Surface* window_surface_ptr, int pos_x, int pos_y, int vit_x,
        int vit_y);
    ~zombie();

    void move() override;
    bool interact(std::shared_ptr<interactiveObject> interObject) override;
    bool croque(int pos_sheep_x, int pos_sheep_y);
    bool recule(int pos_sheperd_x, int pos_sheperd_y);
};

class dog : public animal {
private:
    std::shared_ptr<sheperd> maitre_;
    bool proche_;
    bool escape_;
    int escapeTimer_;

public:
    dog(SDL_Surface* window_surface_ptr, std::shared_ptr<sheperd> maitre);
    ~dog();

    void move() override;
    bool interact(std::shared_ptr<interactiveObject> interObject) override;
    bool escape(int pos_dog_x, int pos_dog_y);
};

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground {
private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Surface* window_surface_ptr_;
    SDL_Surface* image_ptr_;
    std::string image_ground = "fond.png";
    std::string image_ground_halloween = "fondHalloween.png";
    // Some attribute to store all the wolves and sheep
    // here
    std::vector<std::shared_ptr<interactiveObject>> the_zoo;
    std::vector<std::shared_ptr<interactiveObject>> old_zoo;

    SDL_Rect* sdl_rect_;
    int dead = 0;
    bool halloween_;

public:
    ground(SDL_Surface* window_surface_ptr); // todo: Ctor
    ~ground(); // todo: Dtor, again for clean up (if necessary)
    
    bool get_Haloween();
    void true_halloween();
    void add_animal(
        std::shared_ptr<interactiveObject> an_animal); // todo: Add an animal
    void update(); // todo: "refresh the screen": Move animals and draw them
    // Possibly other methods, depends on your implementation
    void interract(std::shared_ptr<interactiveObject> an_animal);
    void moveSheperd(SDL_Event& window_event_); 
    void clickButton(SDL_Event& window_event_);

    int getScore();
    bool ground::isSheperdAlive();
};

// The application class, which is in charge of generating the window
class application {
private:
    // The following are OWNING ptrs
    SDL_Window* window_ptr_;
    SDL_Surface* window_surface_ptr_;
    SDL_Event window_event_;

    // Other attributes here, for example an instance of ground
    std::shared_ptr<ground> ground_;

    unsigned n_wolf_;
    unsigned n_sheep_;

public:
    application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application();                                 // dtor

    int loop(unsigned period); // main loop of the application.
                               // this ensures that the screen is updated
                               // at the correct rate.
                               // See SDL_GetTicks() and SDL_Delay() to enforce a
                               // duration the application should terminate after
                               // 'period' seconds
};