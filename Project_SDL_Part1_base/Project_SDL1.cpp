// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>
#include <math.h>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

 namespace {
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.

} // namespace

 renderedObject::renderedObject(const std::string& file_path,
     SDL_Surface* window_surface_ptr, int pos_x,
     int pos_y)
     : window_surface_ptr_{ window_surface_ptr }, pos_x_{ pos_x }, pos_y_{ pos_y } {
     image_ptr_ = IMG_Load(file_path.c_str());
     if (!image_ptr_)
         throw std::runtime_error("animal::animal(): "
             "Could not load " +
             file_path +
             "\n Error: " + std::string(SDL_GetError()));
 };

 renderedObject::~renderedObject() {
     SDL_FreeSurface(image_ptr_);
     this->image_ptr_ = nullptr;
 };

 int renderedObject::get_pos_x() const { return this->pos_x_; };
 int renderedObject::get_pos_y() const { return this->pos_y_; };

 void renderedObject::set_image_ptr(const std::string& file_path) {
     image_ptr_ = IMG_Load(file_path.c_str());
     if (!image_ptr_)
         throw std::runtime_error("animal::animal(): "
             "Could not load " +
             file_path +
             "\n Error: " + std::string(SDL_GetError()));
 };

 movingObject::movingObject(const std::string& file_path,
     SDL_Surface* window_surface_ptr, int pos_x,
     int pos_y, int vit_x, int vit_y)
     : renderedObject(file_path, window_surface_ptr, pos_x, pos_y),
     vit_x_{ vit_x }, vit_y_{ vit_y } {};

 movingObject::~movingObject() {};

 void movingObject::verifPosition() {
     if (pos_x_ <= frame_boundary) {
         vit_x_ = std::abs(vit_x_);
         pos_x_ = frame_boundary;
     }
     else if (pos_x_ >= frame_width - frame_boundary) {
         vit_x_ = -std::abs(vit_x_);
         pos_x_ = frame_width - frame_boundary;
     }

     if (pos_y_ <= frame_boundary) {
         vit_y_ = std::abs(vit_y_);
         pos_y_ = frame_boundary;
     }
     else if (pos_y_ >= frame_height - frame_boundary) {
         vit_y_ = -std::abs(vit_y_);
         pos_y_ = frame_height - frame_boundary;
     }
 };

 int movingObject::getVit_x() const { return this->vit_x_; };
 int movingObject::getVit_y() const { return this->vit_y_; };

 void movingObject::setVit_x(int vit_x) { this->vit_x_ = vit_x; };
 void movingObject::setVit_y(int vit_y) { this->vit_y_ = vit_y; };

 interactiveObject::interactiveObject(const std::string& file_path,
     SDL_Surface* window_surface_ptr, int pos_x,
     int pos_y, int vit_x, int vit_y)
     : movingObject(file_path, window_surface_ptr, pos_x, pos_y, vit_x, vit_y) {};

 interactiveObject::~interactiveObject() {};

 std::vector<std::string>& interactiveObject::getProperties() {
     return this->properties_;
 };

 sheperd::sheperd(SDL_Surface* window_surface_ptr)
     : interactiveObject("sheperd.png", window_surface_ptr, 0, 0, 0, 0) {
     pos_x_ = frame_boundary + std::rand() % (frame_width - 2 * frame_boundary);
     pos_y_ = frame_boundary + std::rand() % (frame_height - 2 * frame_boundary);
     this->properties_.push_back("sheperd");
     this->properties_.push_back("male");
     this->properties_.push_back("0");
     this->properties_.push_back("alive");
     this->properties_.push_back("none");
     this->properties_.push_back("false");
 };

 sheperd::~sheperd() {};

 void sheperd::draw() {
     SDL_Rect rect;
     rect.x = pos_x_;
     rect.y = pos_y_;
     rect.w = image_ptr_->w;
     rect.h = image_ptr_->h;
     SDL_BlitScaled(image_ptr_, NULL, window_surface_ptr_, &rect);
 };

 void sheperd::move() {
     pos_x_ += (frame_time * vit_x_);
     pos_y_ += (frame_time * vit_y_);

     this->verifPosition();
 };

 bool sheperd::interact(std::shared_ptr<interactiveObject> interObject) {

     return false;
 };

 animal::animal(const std::string& file_path, SDL_Surface* window_surface_ptr,
     int pos_x, int pos_y, int vit_x, int vit_y)
     : interactiveObject(file_path, window_surface_ptr, pos_x, pos_y, vit_x,
         vit_y) {};

 animal::animal(const std::string& file_path, SDL_Surface* window_surface_ptr)
     : interactiveObject(file_path, window_surface_ptr, 0, 0, 0, 0) {};

 animal::~animal() {};

 void animal::draw() {
     SDL_Rect rect;
     rect.x = pos_x_;
     rect.y = pos_y_;
     rect.w = image_ptr_->w;
     rect.h = image_ptr_->h;
     SDL_BlitScaled(image_ptr_, NULL, window_surface_ptr_, &rect);
 };

 sheep::sheep(SDL_Surface* window_surface_ptr, int pos_x, int pos_y, int vit_x,
     int vit_y, std::string couleur)
     : animal("sheep.png", window_surface_ptr, pos_x, pos_y, vit_x, vit_y),
     reproductionTimer_{ 0 }, escapeTimer_{ 0 }, escape_{ false } {
     this->properties_.push_back("sheep");
     if (std::rand() % 2 == 0) {
         this->properties_.push_back("male");
     }
     else {
         this->properties_.push_back("female");
         this->set_image_ptr("sheep_f.png");
     }
     this->properties_.push_back("0");
     this->properties_.push_back("alive");

     if (couleur != "none") {
         if (couleur == "pink") {
             this->properties_.push_back("pink");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_pink.png");
             }
             else {
                 this->set_image_ptr("sheep_f_pink.png");
             }
         }
         else if (couleur == "orange") {
             this->properties_.push_back("orange");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_orange.png");
             }
             else {
                 this->set_image_ptr("sheep_f_orange.png");
             }
         }
         else if (couleur == "purple") {
             this->properties_.push_back("purple");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_purple.png");
             }
             else {
                 this->set_image_ptr("sheep_f_purple.png");
             }
         }
         else if (couleur == "green") {
             this->properties_.push_back("green");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_green.png");
             }
             else {
                 this->set_image_ptr("sheep_f_green.png");
             }
         }
         else if (couleur == "blue") {
             this->properties_.push_back("blue");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_blue.png");
             }
             else {
                 this->set_image_ptr("sheep_f_blue.png");
             }
         }
         else if (couleur == "red") {
             this->properties_.push_back("red");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_red.png");
             }
             else {
                 this->set_image_ptr("sheep_f_red.png");
             }
         }
         else if (couleur == "yellow") {
             this->properties_.push_back("yellow");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_yellow.png");
             }
             else {
                 this->set_image_ptr("sheep_f_yellow.png");
             }
         }
         else if (couleur == "white") {
             this->properties_.push_back("white");
         }

     }
     else {
         int r = rand() % 4;
         if (r == 0) {
             this->properties_.push_back("red");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_red.png");
             }
             else {
                 this->set_image_ptr("sheep_f_red.png");
             }
         }
         else if (r == 1) {
             this->properties_.push_back("blue");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_blue.png");
             }
             else {
                 this->set_image_ptr("sheep_f_blue.png");
             }
         }
         else if (r == 2) {
             this->properties_.push_back("yellow");
             if (this->getProperties()[1] == "male") {
                 this->set_image_ptr("sheep_yellow.png");
             }
             else {
                 this->set_image_ptr("sheep_f_yellow.png");
             }
         }
         else {
             this->properties_.push_back("white");
         }
     }
     this->properties_.push_back("false");
 }

 sheep::sheep(SDL_Surface* window_surface_ptr)
     : sheep(window_surface_ptr, 0, 0, 0, 0, "none") {
     pos_x_ = frame_boundary + std::rand() % (frame_width - 2 * frame_boundary);
     pos_y_ = frame_boundary + std::rand() % (frame_height - 2 * frame_boundary);

     if (std::rand() % 2 == 0) {
         vit_x_ = -20;
     }
     else {
         vit_x_ = 20;
     }
     if (std::rand() % 2 == 0) {
         vit_y_ = -20;
     }
     else {
         vit_y_ = 20;
     }
 };

 sheep::~sheep() {};

 void sheep::move() {

     if (this->escapeTimer_ != 0) {
         this->escapeTimer_--;
     }
     if (this->reproductionTimer_ != 0) {
         this->reproductionTimer_ = std::stoi(this->properties_[2]);
         this->reproductionTimer_--;
         this->properties_[2] = std::to_string(reproductionTimer_);
     }

     if (!escape_ &&
         (this->properties_[1] != "male" ||
             (this->properties_[1] == "male" && reproductionTimer_ != 0))) {
         if (this->escapeTimer_ == 500) {
             if (this->vit_x_ >= 0) {
                 this->vit_x_ = std::rand() % 40;
             }
             else {
                 this->vit_x_ = -std::rand() % 40;
             }

             if (this->vit_y_ >= 0) {
                 this->vit_y_ = 40 - std::abs(this->vit_x_);
             }
             else {
                 this->vit_y_ = -40 + std::abs(this->vit_x_);
             }
         }
         else if (this->escapeTimer_ < 500) {
             if (std::rand() % 200 == 0) {
                 if (std::rand() % 2 == 0) {
                     this->vit_x_ *= -1;
                     if (std::rand() % 200 == 0) {
                         this->vit_y_ *= -1;
                     }
                 }
                 else {
                     this->vit_y_ *= -1;
                     if (std::rand() % 200 == 0) {
                         this->vit_x_ *= -1;
                     }
                 }
             }

             if (std::rand() % 50 == 0) {
                 if (this->vit_x_ >= 0) {
                     this->vit_x_ = std::rand() % 40;
                 }
                 else {
                     this->vit_x_ = -std::rand() % 40;
                 }

                 if (this->vit_y_ >= 0) {
                     this->vit_y_ = 40 - std::abs(this->vit_x_);
                 }
                 else {
                     this->vit_y_ = -40 + std::abs(this->vit_x_);
                 }
             }
         }
     }

     pos_x_ += (frame_time * vit_x_);
     pos_y_ += (frame_time * vit_y_);

     this->escape_ = false;

     this->verifPosition();
 };

 const int sheep::getReproductionTimer() { return this->reproductionTimer_; };

 void sheep::setReproductionTimer(int newTime) {
     this->reproductionTimer_ = newTime;
 };

 bool sheep::interact(std::shared_ptr<interactiveObject> interObject) {
     if (interObject->getProperties()[0] == "zombie") {
         return this->escape(interObject->get_pos_x(), interObject->get_pos_y());
     }
     else if (interObject->getProperties()[0] == "wolf") {
         return this->escape(interObject->get_pos_x(), interObject->get_pos_y());
     }
     else if (interObject->getProperties()[0] == "sheep" &&
         this->getProperties()[1] == "male") {
         if ((interObject->getProperties()[2] == "0") &&
             this->reproduit(interObject->get_pos_x(), interObject->get_pos_y())) {
             interObject->getProperties()[2] = "1000";
             this->properties_[2] = "1000";
             return true;
         }
         else {
             return false;
         }
     }
     return false;
 };

 bool sheep::reproduit(int pos_sheep_x, int pos_sheep_y) {
     unsigned int distLove =
         sqrt(((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_)) +
             ((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_)));
     if (this->reproductionTimer_ == 0 && distLove < 30) {
         this->reproductionTimer_ = 1000;
         return true;
     }
     else if (this->reproductionTimer_ == 0) {
         int distx =
             sqrt((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_));
         int disty =
             sqrt((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_));
         this->vit_x_ = 40 * (sqrt(distLove * distLove - disty * disty) / distLove);

         this->vit_y_ = 40 - this->vit_x_;

         if ((this->pos_x_ - pos_sheep_x) > 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_sheep_y) > 0) {
             this->vit_y_ = -this->vit_y_;
         }
         return false;
     }
     return false;
 };

 bool sheep::escape(int pos_wolf_x, int pos_wolf_y) {

     this->escape_ = true;
     int distLoup =
         sqrt(((pos_wolf_x - this->pos_x_) * (pos_wolf_x - this->pos_x_)) +
             ((pos_wolf_y - this->pos_y_) * (pos_wolf_y - this->pos_y_)));

     if (this->escapeTimer_ == 0) {
         int distx = sqrt((pos_wolf_x - this->pos_x_) * (pos_wolf_x - this->pos_x_));
         int disty = sqrt((pos_wolf_y - this->pos_y_) * (pos_wolf_y - this->pos_y_));
         this->vit_x_ = 80 * (sqrt(distLoup * distLoup - disty * disty) / distLoup);

         this->vit_y_ = 80 - this->vit_x_;

         if ((this->pos_x_ - pos_wolf_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_wolf_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }

         this->escapeTimer_ = 1000;

     }
     else {
         int distx = sqrt((pos_wolf_x - this->pos_x_) * (pos_wolf_x - this->pos_x_));
         int disty = sqrt((pos_wolf_y - this->pos_y_) * (pos_wolf_y - this->pos_y_));
         this->vit_x_ = 40 * (sqrt(distLoup * distLoup - disty * disty) / distLoup);

         this->vit_y_ = 40 - this->vit_x_;

         if ((this->pos_x_ - pos_wolf_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_wolf_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }
     }

     return true;
 };

 wolf::wolf(SDL_Surface* window_surface_ptr)
     : animal("wolf.png", window_surface_ptr), chasse_{ false } {
     pos_x_ = frame_boundary + std::rand() % (frame_width - 2 * frame_boundary);
     pos_y_ = frame_boundary + std::rand() % (frame_height - 2 * frame_boundary);
     if (std::rand() % 2 == 0) {
         vit_x_ = -40;
     }
     else {
         vit_x_ = 40;
     }
     if (std::rand() % 2 == 0) {
         vit_y_ = -40;
     }
     else {
         vit_y_ = 40;
     }

     this->properties_.push_back("wolf");
     if (std::rand() % 2 == 0) {
         this->properties_.push_back("male");
     }
     else {
         this->properties_.push_back("female");
     }
     this->properties_.push_back("0");
     this->properties_.push_back("alive");
     this->properties_.push_back("none");
     this->properties_.push_back("false");
 };

 wolf::~wolf() {};

 void wolf::move() {

     if (this->escapeTimer_ != 0) {
         this->escapeTimer_--;
     }

     if (!chasse_ && !escape_) {
         if (this->escapeTimer_ == 500) {
             if (this->vit_x_ >= 0) {
                 this->vit_x_ = std::rand() % 80;
             }
             else {
                 this->vit_x_ = -std::rand() % 80;
             }

             if (this->vit_y_ >= 0) {
                 this->vit_y_ = 80 - std::abs(this->vit_x_);
             }
             else {
                 this->vit_y_ = -80 + std::abs(this->vit_x_);
             }
         }
         else if (this->escapeTimer_ < 500) {
             if (std::rand() % 150 == 0) {
                 if (std::rand() % 2 == 0) {
                     this->vit_x_ *= -1;
                     if (std::rand() % 150 == 0) {
                         this->vit_y_ *= -1;
                     }
                 }
                 else {
                     this->vit_y_ *= -1;
                     if (std::rand() % 150 == 0) {
                         this->vit_x_ *= -1;
                     }
                 }
             }

             if (std::rand() % 50 == 0) {
                 if (this->vit_x_ >= 0) {
                     this->vit_x_ = std::rand() % 80;
                 }
                 else {
                     this->vit_x_ = -std::rand() % 80;
                 }

                 if (this->vit_y_ >= 0) {
                     this->vit_y_ = 80 - std::abs(this->vit_x_);
                 }
                 else {
                     this->vit_y_ = -80 + std::abs(this->vit_x_);
                 }
             }
         }
     }

     this->escape_ = false;
     this->chasse_ = false;

     pos_x_ += (frame_time * vit_x_);
     pos_y_ += (frame_time * vit_y_);

     this->verifPosition();
 };

 bool wolf::interact(std::shared_ptr<interactiveObject> interObject) {

     if (interObject->getProperties()[0] == "zombie") {
         return this->escape(interObject->get_pos_x(), interObject->get_pos_y());
     }
     else if (interObject->getProperties()[0] == "sheep") {
         if (this->croque(interObject->get_pos_x(), interObject->get_pos_y())) {
             interObject->getProperties()[3] = "dead";
             return true;
         }
     }
     else if (interObject->getProperties()[0] == "dog") {
         return this->escape(interObject->get_pos_x(), interObject->get_pos_y());
     }
     return false;
 };
 bool wolf::croque(int pos_sheep_x, int pos_sheep_y) {
     unsigned int distMout =
         sqrt(((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_)) +
             ((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_)));

     if (distMout < 50) {
         this->pos_x_ = pos_sheep_x;
         this->pos_y_ = pos_sheep_y;
         return true;
     }
     else {
         int distx =
             sqrt((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_));
         int disty =
             sqrt((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_));
         this->vit_x_ = 80 * (sqrt(distMout * distMout - disty * disty) / distMout);

         this->vit_y_ = 80 - this->vit_x_;

         if ((this->pos_x_ - pos_sheep_x) > 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_sheep_y) > 0) {
             this->vit_y_ = -this->vit_y_;
         }
     }

     this->chasse_ = true;

     return false;
 };

 bool wolf::escape(int pos_dog_x, int pos_dog_y) {
     this->escape_ = true;
     int distDog = sqrt(((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_)) +
         ((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_)));

     if (this->escapeTimer_ == 0) {
         int distx = sqrt((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_));
         int disty = sqrt((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_));
         this->vit_x_ = 100 * (sqrt(distDog * distDog - disty * disty) / distDog);

         this->vit_y_ = 100 - this->vit_x_;

         if ((this->pos_x_ - pos_dog_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_dog_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }

         this->escapeTimer_ = 1000;

     }
     else {
         int distx = sqrt((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_));
         int disty = sqrt((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_));
         this->vit_x_ = 80 * (sqrt(distDog * distDog - disty * disty) / distDog);

         this->vit_y_ = 80 - this->vit_x_;

         if ((this->pos_x_ - pos_dog_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_dog_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }
     }

     return true;
 };

 zombie::zombie(SDL_Surface* window_surface_ptr)
     : zombie(window_surface_ptr,0,0,0,0){
     pos_x_ = frame_boundary + std::rand() % (frame_width - 2 * frame_boundary);
     pos_y_ = frame_boundary + std::rand() % (frame_height - 2 * frame_boundary);
     
 };

 zombie::zombie(SDL_Surface* window_surface_ptr, int pos_x, int pos_y, int vit_x, int vit_y)
     : animal("sheep_z.png", window_surface_ptr, pos_x, pos_y, vit_x, vit_y), chasse_{ false }{
     if (std::rand() % 2 == 0) {
         vit_x_ = -40;
     }
     else {
         vit_x_ = 40;
     }
     if (std::rand() % 2 == 0) {
         vit_y_ = -40;
     }
     else {
         vit_y_ = 40;
     }

     this->properties_.push_back("zombie");
     if (std::rand() % 2 == 0) {
         this->properties_.push_back("male");
     }
     else {
         this->properties_.push_back("female");
     }
     this->properties_.push_back("0");
     this->properties_.push_back("alive");
     this->properties_.push_back("none");
     this->properties_.push_back("false");
 };

 zombie::~zombie() {};

 void zombie::move() {

     if (this->escapeTimer_ != 0) {
         this->escapeTimer_--;
     }

     if (!chasse_ && !escape_ && this->properties_[5] == "false") {
         if (this->escapeTimer_ == 500) {
             if (this->vit_x_ >= 0) {
                 this->vit_x_ = std::rand() % 80;
             }
             else {
                 this->vit_x_ = -std::rand() % 80;
             }

             if (this->vit_y_ >= 0) {
                 this->vit_y_ = 80 - std::abs(this->vit_x_);
             }
             else {
                 this->vit_y_ = -80 + std::abs(this->vit_x_);
             }
         }
         else if (this->escapeTimer_ < 500) {
             if (std::rand() % 150 == 0) {
                 if (std::rand() % 2 == 0) {
                     this->vit_x_ *= -1;
                     if (std::rand() % 150 == 0) {
                         this->vit_y_ *= -1;
                     }
                 }
                 else {
                     this->vit_y_ *= -1;
                     if (std::rand() % 150 == 0) {
                         this->vit_x_ *= -1;
                     }
                 }
             }

             if (std::rand() % 50 == 0) {
                 if (this->vit_x_ >= 0) {
                     this->vit_x_ = std::rand() % 80;
                 }
                 else {
                     this->vit_x_ = -std::rand() % 80;
                 }

                 if (this->vit_y_ >= 0) {
                     this->vit_y_ = 80 - std::abs(this->vit_x_);
                 }
                 else {
                     this->vit_y_ = -80 + std::abs(this->vit_x_);
                 }
             }
         }
     }

     this->escape_ = false;
     this->chasse_ = false;
     
     pos_x_ += (frame_time * vit_x_);
     pos_y_ += (frame_time * vit_y_);

     if (this->properties_[5] == "true") {
         if (this->vit_x_ >= 0) {
             this->vit_x_ = -std::rand() % 80;
         }
         else {
             this->vit_x_ = std::rand() % 80;
         }

         if (this->vit_y_ >= 0) {
             this->vit_y_ = -80 - std::abs(this->vit_x_);
         }
         else {
             this->vit_y_ = 80 + std::abs(this->vit_x_);
         }
         this->properties_[5] = "false";
     }

     this->verifPosition();
 };
 bool zombie::interact(std::shared_ptr<interactiveObject> interObject) {

     if (interObject->getProperties()[0] == "sheperd" && this->properties_[5] == "true") {
         return this->recule(interObject->get_pos_x(), interObject->get_pos_y());
     }

     if (interObject->getProperties()[3] == "alive" &&
         interObject->getProperties()[0] != "zombie") {
         if (this->croque(interObject->get_pos_x(), interObject->get_pos_y())) {
             interObject->getProperties()[3] = "dead";
             return true;
         }
     }
     return false;
 };
 bool zombie::croque(int pos_sheep_x, int pos_sheep_y) {
     unsigned int distMout =
         sqrt(((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_)) +
             ((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_)));

     if (distMout < 50) {
         this->pos_x_ = pos_sheep_x;
         this->pos_y_ = pos_sheep_y;
         return true;
     }
     else {
         int distx =
             sqrt((pos_sheep_x - this->pos_x_) * (pos_sheep_x - this->pos_x_));
         int disty =
             sqrt((pos_sheep_y - this->pos_y_) * (pos_sheep_y - this->pos_y_));
         this->vit_x_ = 80 * (sqrt(distMout * distMout - disty * disty) / distMout);

         this->vit_y_ = 80 - this->vit_x_;

         if ((this->pos_x_ - pos_sheep_x) > 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_sheep_y) > 0) {
             this->vit_y_ = -this->vit_y_;
         }
     }

     this->chasse_ = true;

     return false;
 };

 bool zombie::recule(int pos_sheperd_x, int pos_sheperd_y) {
     int distSheperd = sqrt(((pos_sheperd_x - this->pos_x_) * (pos_sheperd_x - this->pos_x_)) +
         ((pos_sheperd_y - this->pos_y_) * (pos_sheperd_y - this->pos_y_)));

     int distx = sqrt((pos_sheperd_x - this->pos_x_) * (pos_sheperd_x - this->pos_x_));
     int disty = sqrt((pos_sheperd_y - this->pos_y_) * (pos_sheperd_y - this->pos_y_));
     this->vit_x_ = 500 * (sqrt(distSheperd * distSheperd - disty * disty) / distSheperd);

     this->vit_y_ = 500 - this->vit_x_;

     if ((this->pos_x_ - pos_sheperd_x) < 0) {
         this->vit_x_ = -this->vit_x_;
     }
     if ((this->pos_y_ - pos_sheperd_y) < 0) {
         this->vit_y_ = -this->vit_y_;
     }
     return true;
 }

 dog::dog(SDL_Surface* window_surface_ptr, std::shared_ptr<sheperd> maitre)
     : animal("dog.png", window_surface_ptr), maitre_{ maitre }, proche_{ true } {
     this->pos_x_ = this->maitre_->get_pos_x() + 20;
     this->pos_y_ = this->maitre_->get_pos_y() + 20;

     this->properties_.push_back("dog");
     if (std::rand() % 2 == 0) {
         this->properties_.push_back("male");
     }
     else {
         this->properties_.push_back("female");
     }
     this->properties_.push_back("0");
     this->properties_.push_back("alive");
     this->properties_.push_back("none");
     this->properties_.push_back("false");
 };

 dog::~dog() {};

 void dog::move() {

     if (this->proche_) {
         if (std::rand() % 150 == 0) {
             if (std::rand() % 2 == 0) {
                 this->vit_x_ *= -1;
                 if (std::rand() % 150 == 0) {
                     this->vit_y_ *= -1;
                 }
             }
             else {
                 this->vit_y_ *= -1;
                 if (std::rand() % 150 == 0) {
                     this->vit_x_ *= -1;
                 }
             }
         }

         if (std::rand() % 10 == 0) {
             if (this->vit_x_ >= 0) {
                 this->vit_x_ = std::rand() % 80;
             }
             else {
                 this->vit_x_ = -std::rand() % 80;
             }

             if (this->vit_y_ >= 0) {
                 this->vit_y_ = 100 - std::abs(this->vit_x_);
             }
             else {
                 this->vit_y_ = -100 + std::abs(this->vit_x_);
             }
         }
     }

     pos_x_ += (frame_time * vit_x_);
     pos_y_ += (frame_time * vit_y_);

     this->proche_ = true;

     this->verifPosition();
 };

 bool dog::interact(std::shared_ptr<interactiveObject> interObject) {
     if (interObject->getProperties()[0] == "zombie") {
         return this->escape(interObject->get_pos_x(), interObject->get_pos_y());
     }
     if (interObject->getProperties()[0] == "sheperd") {
         int distSheperd = sqrt(((this->maitre_->get_pos_x() - this->pos_x_) *
             (this->maitre_->get_pos_x() - this->pos_x_)) +
             ((this->maitre_->get_pos_y() - this->pos_y_) *
                 (this->maitre_->get_pos_y() - this->pos_y_)));
         if (distSheperd > 50) {
             this->proche_ = false;

             int distx = sqrt((this->maitre_->get_pos_x() - this->pos_x_) *
                 (this->maitre_->get_pos_x() - this->pos_x_));
             int disty = sqrt((this->maitre_->get_pos_y() - this->pos_y_) *
                 (this->maitre_->get_pos_y() - this->pos_y_));
             this->vit_x_ =
                 180 * (sqrt(distSheperd * distSheperd - disty * disty) / distSheperd);

             this->vit_y_ = 180 - this->vit_x_;

             if ((this->pos_x_ - this->maitre_->get_pos_x()) > 0) {
                 this->vit_x_ = -this->vit_x_;
             }
             if ((this->pos_y_ - this->maitre_->get_pos_y()) > 0) {
                 this->vit_y_ = -this->vit_y_;
             }

         }
         else {
             this->proche_ = true;
         }
         return true;
     }
     return false;
 };

 bool dog::escape(int pos_dog_x, int pos_dog_y) {

     this->escape_ = true;
     int distDog = sqrt(((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_)) +
         ((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_)));

     if (this->escapeTimer_ == 0) {
         int distx = sqrt((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_));
         int disty = sqrt((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_));
         this->vit_x_ = 100 * (sqrt(distDog * distDog - disty * disty) / distDog);

         this->vit_y_ = 100 - this->vit_x_;

         if ((this->pos_x_ - pos_dog_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_dog_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }

         this->escapeTimer_ = 1000;

     }
     else {
         int distx = sqrt((pos_dog_x - this->pos_x_) * (pos_dog_x - this->pos_x_));
         int disty = sqrt((pos_dog_y - this->pos_y_) * (pos_dog_y - this->pos_y_));
         this->vit_x_ = 80 * (sqrt(distDog * distDog - disty * disty) / distDog);

         this->vit_y_ = 80 - this->vit_x_;

         if ((this->pos_x_ - pos_dog_x) < 0) {
             this->vit_x_ = -this->vit_x_;
         }
         if ((this->pos_y_ - pos_dog_y) < 0) {
             this->vit_y_ = -this->vit_y_;
         }
     }

     return true;
 };

 ground::ground(SDL_Surface* window_surface_ptr)
     : window_surface_ptr_{ window_surface_ptr }, sdl_rect_{ new SDL_Rect{
                                                    0, 0, frame_width,
                                                    frame_height} }, halloween_{false} {};

 ground::~ground() {
     delete this->sdl_rect_;
     sdl_rect_ = nullptr;
 };

 bool ground::get_Haloween() { return this->halloween_; };

 void ground::true_halloween() { this->halloween_ = true; };

 void ground::add_animal(std::shared_ptr<interactiveObject> an_animal) {
     the_zoo.push_back(an_animal);
 };

 void ground::update() {
     if (!this->halloween_) {
         image_ptr_ = IMG_Load(image_ground.c_str());
     }
     else {
         image_ptr_ = IMG_Load(image_ground_halloween.c_str());
     }

     if (!image_ptr_) {
         std::cout << "Image not found" << std::endl;
     }
     SDL_Rect rect;
     rect.x = 0;
     rect.y = 0;
     rect.w = image_ptr_->w;
     rect.h = image_ptr_->h;
     SDL_BlitScaled(image_ptr_, NULL, window_surface_ptr_, &rect);
     int distance = INT16_MAX;
     old_zoo = the_zoo;
     for (int i = 0; i < old_zoo.size(); i++) {
         if (old_zoo[i]->getProperties()[3] == "alive") {
             this->interract(old_zoo[i]);
             old_zoo[i]->move();
             old_zoo[i]->draw();
         }
         else if (old_zoo[i]->getProperties()[3] == "dead" && !this->halloween_) {
             for (int j = 0; j < the_zoo.size(); j++) {
                 if (the_zoo[j] == old_zoo[i]) {
                     dead++;
                     the_zoo.erase(the_zoo.begin() + j);
                 }
             }
         }
         else if (old_zoo[i]->getProperties()[3] == "dead" && this->halloween_) {
             for (int j = 0; j < the_zoo.size(); j++) {
                 if (the_zoo[j] == old_zoo[i]) {
                     this->add_animal(std::make_shared<zombie>(window_surface_ptr_, the_zoo[j]->get_pos_x(), the_zoo[j]->get_pos_y(), 0, 0));
                     the_zoo.erase(the_zoo.begin() + j);
                 }
             }
         }
         if (this->halloween_) {
             for (int i = 0; i < dead; i++) {
                 int rand = std::rand()%5;
                 if (rand == 0) {
                     this->add_animal(std::make_shared<zombie>(window_surface_ptr_, 0+frame_boundary, 0+frame_boundary , 0, 0));
                 }
                 else if (rand == 1) {
                     this->add_animal(std::make_shared<zombie>(window_surface_ptr_, frame_width + frame_boundary, frame_height+frame_boundary, 0, 0));
                 }
                 else if(rand == 2) {
                     this->add_animal(std::make_shared<zombie>(window_surface_ptr_, 0 + frame_boundary, frame_height + frame_boundary, 0, 0));
                 }
                 else {
                     this->add_animal(std::make_shared<zombie>(window_surface_ptr_, frame_width + frame_boundary, 0 + frame_boundary, 0, 0));
                 }
                 
             }
             dead = 0;
         }

     }
 };

 void ground::interract(std::shared_ptr<interactiveObject> an_animal) {
     if (an_animal->getProperties()[0] == "sheep") {

         unsigned int distLoup = INT16_MAX;
         unsigned int newDistLoup = 0;
         unsigned int distLove = INT16_MAX;
         unsigned int newDistLove = 0;
         unsigned int distZombie = INT16_MAX;
         unsigned int newDistZombie = 0;
         std::shared_ptr<interactiveObject> loup = nullptr;
         std::shared_ptr<interactiveObject> moutone = nullptr;
         std::shared_ptr<interactiveObject> zombie = nullptr;

         for (auto& another_animal : this->the_zoo) {

             if (another_animal->getProperties()[0] == "zombie") {

                 newDistZombie =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistZombie < 200) && (newDistZombie < distZombie)) {
                     distZombie = newDistZombie;
                     zombie = another_animal;
                 }
             }
             else if (another_animal->getProperties()[0] == "wolf") {

                 newDistLoup =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistLoup < 200) && (newDistLoup < distLoup)) {
                     distLoup = newDistLoup;
                     loup = another_animal;
                 }
             }
             else if ((another_animal->getProperties()[0] == "sheep") &&
                 (an_animal->getProperties()[1] == "male") &&
                 (another_animal->getProperties()[1] == "female")) {
                 newDistLove =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if (newDistLove < distLove) {
                     distLove = newDistLove;
                     moutone = another_animal;
                 }
             }
         }
         if (zombie != nullptr) {
             an_animal->interact(zombie);
         }
         else if (loup != nullptr) {
             an_animal->interact(loup);
         }
         else if (moutone != nullptr) {
             if (an_animal->interact(moutone)) {
                 std::string couleurp = an_animal->getProperties()[4];
                 std::string couleurm = moutone->getProperties()[4];
                 if ((couleurp == "white" || couleurm == "white") &&
                     (couleurm == "red" || couleurp == "red")) {
                     this->add_animal(
                         std::make_shared<sheep>(window_surface_ptr_, moutone->get_pos_x(),
                             moutone->get_pos_y(), 0, 0, "pink"));
                 }
                 else if ((couleurp == "blue" || couleurm == "blue") &&
                     (couleurm == "red" || couleurp == "red")) {
                     this->add_animal(
                         std::make_shared<sheep>(window_surface_ptr_, moutone->get_pos_x(),
                             moutone->get_pos_y(), 0, 0, "purple"));
                 }
                 else if ((couleurp == "yellow" || couleurm == "yellow") &&
                     (couleurm == "red" || couleurp == "red")) {
                     this->add_animal(
                         std::make_shared<sheep>(window_surface_ptr_, moutone->get_pos_x(),
                             moutone->get_pos_y(), 0, 0, "orange"));
                 }
                 else if ((couleurp == "blue" || couleurm == "blue") &&
                     (couleurm == "yellow" || couleurp == "yellow")) {
                     this->add_animal(
                         std::make_shared<sheep>(window_surface_ptr_, moutone->get_pos_x(),
                             moutone->get_pos_y(), 0, 0, "green"));
                 }
                 else {
                     if (std::rand() % 2 == 0) {
                         this->add_animal(std::make_shared<sheep>(
                             window_surface_ptr_, moutone->get_pos_x(), moutone->get_pos_y(),
                             0, 0, couleurp));
                     }
                     else {
                         this->add_animal(std::make_shared<sheep>(
                             window_surface_ptr_, moutone->get_pos_x(), moutone->get_pos_y(),
                             0, 0, couleurm));
                     }
                 }
             }
         }

     }
     else if (an_animal->getProperties()[0] == "wolf") {
         unsigned int distMout = INT16_MAX;
         unsigned int newDistMout = 0;
         unsigned int distDog = INT16_MAX;
         unsigned int newDistDog = 0;
         unsigned int distZombie = INT16_MAX;
         unsigned int newDistZombie = 0;
         std::shared_ptr<interactiveObject> mouton = nullptr;
         std::shared_ptr<interactiveObject> dog = nullptr;
         std::shared_ptr<interactiveObject> zombie = nullptr;

         for (auto& another_animal : this->the_zoo) {
             if (another_animal->getProperties()[0] == "zombie") {

                 newDistZombie =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistZombie < 200) && (newDistZombie < distZombie)) {
                     distZombie = newDistZombie;
                     zombie = another_animal;
                 }
             }
             else if (another_animal->getProperties()[0] == "sheep") {

                 newDistMout =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if (newDistMout < distMout) {
                     distMout = newDistMout;
                     mouton = another_animal;
                 }
             }
             else if (another_animal->getProperties()[0] == "dog") {
                 newDistDog =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistDog < 300) && (newDistDog < distDog)) {
                     distDog = newDistDog;
                     dog = another_animal;
                 }
             }
         }
         if (zombie != nullptr) {
             an_animal->interact(zombie);
         }
         else if (dog != nullptr) {
             an_animal->interact(dog);
         }
         else if (mouton != nullptr) {
             an_animal->interact(mouton);
         }
     }
     else if (an_animal->getProperties()[0] == "dog") {
         unsigned int distZombie = INT16_MAX;
         unsigned int newDistZombie = 0;
         unsigned int distSheperd = INT16_MAX;
         unsigned int newDistSheperd = 0;
         std::shared_ptr<interactiveObject> zombie = nullptr;
         std::shared_ptr<interactiveObject> sheperd = nullptr;
         for (auto& another_animal : this->the_zoo) {
             if (another_animal->getProperties()[0] == "zombie") {

                 newDistZombie =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistZombie < 200) && (newDistZombie < distZombie)) {
                     distZombie = newDistZombie;
                     zombie = another_animal;
                 }
             }
             else if (another_animal->getProperties()[0] == "sheperd") {
                 newDistSheperd =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistSheperd < 200) && (newDistSheperd < distSheperd)) {
                     distSheperd = newDistSheperd;
                     sheperd = another_animal;
                 }
             }
             if (zombie != nullptr) {
                 an_animal->interact(zombie);
             }
             else if (sheperd != nullptr) {
                 an_animal->interact(sheperd);
             }
         }
     }
     else if (an_animal->getProperties()[0] == "zombie") {
         unsigned int distTarget = INT16_MAX;
         unsigned int newDistTarget = 0;
         std::shared_ptr<interactiveObject> target = nullptr;
         for (auto& another_animal : this->the_zoo) {
             if (another_animal->getProperties()[0] != "zombie") {

                 newDistTarget =
                     sqrt(((another_animal->get_pos_x() - an_animal->get_pos_x()) *
                         (another_animal->get_pos_x() - an_animal->get_pos_x())) +
                         ((another_animal->get_pos_y() - an_animal->get_pos_y()) *
                             (another_animal->get_pos_y() - an_animal->get_pos_y())));

                 if ((newDistTarget < 200) && (newDistTarget < distTarget)) {
                     distTarget = newDistTarget;
                     target = another_animal;
                 }
             }
         }
         if (target != nullptr) {
             an_animal->interact(target);
         }
     }
 };

 void ground::moveSheperd(SDL_Event& window_event) {
     std::string lettre = SDL_GetKeyName(window_event.key.keysym.sym);
     for (auto& interObject : the_zoo) {
         if (interObject->getProperties()[0] == "sheperd") {
             switch (window_event.type) {
             case SDL_KEYDOWN:
                 if (lettre == "Z") {
                     interObject->setVit_y(-150);
                 }
                 else if (lettre == "Q") {
                     interObject->setVit_x(-150);
                 }
                 else if (lettre == "S") {
                     interObject->setVit_y(150);
                 }
                 else if (lettre == "D") {
                     interObject->setVit_x(150);
                 }
                 break;
             case SDL_KEYUP:
                 if (lettre == "Z") {
                     interObject->setVit_y(0);
                 }
                 else if (lettre == "Q") {
                     interObject->setVit_x(0);
                 }
                 else if (lettre == "S") {
                     interObject->setVit_y(0);
                 }
                 else if (lettre == "D") {
                     interObject->setVit_x(0);
                 }
                 break;
             }
         }
     }
 };

 void ground::clickButton(SDL_Event& window_event) {
     int x = window_event.button.x;
     int y = window_event.button.y;
     std::shared_ptr<interactiveObject> sheperd;

     for (auto& an_animal : this->the_zoo) {
         if (an_animal->getProperties()[0] == "sheperd") {
             sheperd = an_animal;
         }
     }
     int distZombie = INT16_MAX;
     for (auto& an_animal : this->the_zoo) {
         if (an_animal->getProperties()[0] == "zombie") {
             distZombie = sqrt(((sheperd->get_pos_x() - an_animal->get_pos_x()) *
                 (sheperd->get_pos_x() - an_animal->get_pos_x())) +
                 ((sheperd->get_pos_y() - an_animal->get_pos_y()) *
                     (sheperd->get_pos_y() - an_animal->get_pos_y())));
             if (an_animal->get_pos_x() <= x && x <= an_animal->get_pos_x() + 67 && an_animal->get_pos_y() <= y && y <= an_animal->get_pos_y() + 71 && distZombie <= 100 && an_animal->getProperties()[5] == "false") {
                 an_animal->getProperties()[5] = "true";
                 an_animal->interact(sheperd);
             }
         }
     }
     
 };

 int ground::getScore() {
     int score = 0;
     for (const auto an_animal : the_zoo) {
         if (an_animal->getProperties()[0] == "sheep") {
             score++;
         }
     }
     return score;
 };

 bool ground::isSheperdAlive() {
     for (int i = 0; i < the_zoo.size(); i++) {
         if (the_zoo[i]->getProperties()[0] == "sheperd") {
             if (the_zoo[i]->getProperties()[3] == "alive") {
                 return true;
             }
         }
     }
     return false;
 }

 application::application(unsigned n_sheep, unsigned n_wolf)
     : window_ptr_{ SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED, frame_width,
                                    frame_height, 0) },
     window_surface_ptr_{ SDL_GetWindowSurface(window_ptr_) },
     ground_{ std::make_shared<ground>(window_surface_ptr_) }, n_wolf_{ n_wolf },
     n_sheep_{ n_sheep } {
     if (!window_ptr_)
         throw std::runtime_error(std::string(SDL_GetError()));

     if (!window_surface_ptr_)
         throw std::runtime_error(std::string(SDL_GetError()));

     std::srand(time(NULL));

     for (int i = 0; i < n_sheep; i++) {
         this->ground_->add_animal(std::make_shared<sheep>(window_surface_ptr_));
     }

     for (int i = 0; i < n_wolf; i++) {
         this->ground_->add_animal(std::make_shared<wolf>(window_surface_ptr_));
     }

     std::shared_ptr<sheperd> maitre =
         std::make_shared<sheperd>(window_surface_ptr_);
     this->ground_->add_animal(maitre);
     this->ground_->add_animal(std::make_shared<dog>(window_surface_ptr_, maitre));
 };

 application::~application() {
     SDL_DestroyWindow(window_ptr_);
     window_ptr_ = nullptr;
     SDL_FreeSurface(window_surface_ptr_);
     window_surface_ptr_ = nullptr;
 };

 int application::loop(unsigned period) {
     unsigned start_ticks = SDL_GetTicks();
     unsigned last_ticks = SDL_GetTicks();
     while (SDL_GetTicks() <= 1000u * period) {
         while (SDL_PollEvent(&window_event_) > 0) {
             switch (window_event_.type) {
             case SDL_KEYDOWN:
                 this->ground_->moveSheperd(window_event_);
                 break;
             case SDL_KEYUP:
                 this->ground_->moveSheperd(window_event_);
                 break;
             case SDL_MOUSEBUTTONDOWN:
                 this->ground_->clickButton(window_event_);
                 break;
             case SDL_QUIT:
                 return 0;
             }
         }
         if (this->ground_->getScore() == 0) {
             break;
         }
         if (SDL_GetTicks() >= 1000u * floor(period/2) && !this->ground_->get_Haloween()) {
             this->ground_->true_halloween();
             std::cout << "Attention le jeu n'est pas fini !" << std::endl;
             std::cout << "Survivez aux moutons zombie !" << std::endl;
         }
         if (!this->ground_->isSheperdAlive()) {
             std::cout << "Vous etes mort ! :(" << std::endl;
             std::cout << "Perdu. Vous avez resiste "<<(last_ticks-start_ticks)/1000<<" secondes." << std::endl;
             return 0;
         }
         this->ground_->update();
         unsigned nt = SDL_GetTicks();
         if ((nt - last_ticks) < frame_time * 1000.)
             SDL_Delay((unsigned)((frame_time * 1000.) - (nt - last_ticks)));
         SDL_UpdateWindowSurface(window_ptr_);
         last_ticks = SDL_GetTicks();
     }
     std::cout << "Vous avez survecu pendant " << (last_ticks - start_ticks)/1000 << " secondes." << std::endl;
     std::cout << "Et vous avez garde en vie " << this->ground_->getScore()
         << " moutons. Bravo !" << std::endl;
     return 0;
 };