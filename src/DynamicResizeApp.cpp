//
// Aspect比一定のWindowサイズ変更
// Mouseの座標Window←→Screen相互変換
//

#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"


using namespace ci;
using namespace ci::app;


// Window座標→Screen座標
Vec2i toScreenPos(const Vec2i& pos, const Vec2i& size, const Area& area) {
  // Window座標→正規化座標
  float x = (pos.x - area.x1) / float(area.x2 - area.x1);
  float y = (pos.y - area.y1) / float(area.y2 - area.y1);

  // 正規化座標→Screen座標
  return Vec2i(size.x * x, size.y * y);
}

// Screen座標→Window座標
Vec2i toWindowPos(const Vec2i& pos, const Vec2i& size, const Area& area) {
  // Screen座標→正規化座標
  float x = pos.x / float(size.x);
  float y = pos.y / float(size.y);

  // 正規化座標→Window座標
  return Vec2i((area.x2 - area.x1) * x + area.x1,
               (area.y2 - area.y1) * y + area.y1);
}


class DynamicResizeApp : public AppNative {
  enum Window {
    WIDTH  = 640,
    HEIGHT = 480
  };
  
  
public:
  void prepareSettings(Settings *settings) {
    settings->setWindowSize(Window::WIDTH, Window::HEIGHT);
  }
  
  void setup() {
    screen_size_  = Vec2i(Window::WIDTH, Window::HEIGHT);
    aspect_ratio_ = Window::WIDTH / float(Window::HEIGHT);
    draw_area_    = getWindowBounds();
    
    image_ = loadImage(getAssetPath("miku.png"));
  }

  
  void mouseDown( MouseEvent event ) {
    // Mouse座標→Screen座標→Window座標と変換
    auto mouse_pos  = event.getPos();
    auto screen_pos = toScreenPos(mouse_pos, screen_size_, draw_area_);
    auto window_pos = toWindowPos(screen_pos, screen_size_, draw_area_);
    
    console() << mouse_pos
              << screen_pos
              << window_pos
              << std::endl;
  }

  
  void resize() {
    // Screenサイズは常に一定
    gl::setMatricesWindowPersp(screen_size_.x, screen_size_.y);

    // Viewportの領域を変更
    auto size = getWindowSize();
    int window_width  = size.x;
    int window_height = window_width / aspect_ratio_;
    if (window_height > size.y) {
      window_height = size.y;
      window_width = window_height * aspect_ratio_;
    }

    // 縦横比を維持したサイズを設定。左右に余白ができる場合がある
    draw_area_.x1 = (size.x - window_width) / 2;
    draw_area_.x2 = draw_area_.x1 + window_width;
    draw_area_.y1 = (size.y - window_height) / 2;
    draw_area_.y2 = draw_area_.y1 + window_height;
  }

  
  void update() {}
  
  void draw() {
    // 常にViewportを設定
    gl::setViewport(draw_area_);

    // テスト描画
    gl::clear(Color(0, 0, 0));

    gl::drawSolidCircle(screen_size_ / 2, 300);

    gl::draw(image_, Vec2i(0, 0));
  }


private:
  Vec2i screen_size_;
  float aspect_ratio_;
  Area  draw_area_;
  
  gl::Texture image_;
};


CINDER_APP_NATIVE( DynamicResizeApp, RendererGl )
