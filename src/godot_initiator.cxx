#include "server/server.hxx"
#include "client/client.hxx"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;
using namespace pnd;


//FIXME
#include "print_utils.hxx"
/*
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/image.hpp>
class Foo : public godot::Node
{
  GDCLASS(Foo, Node)
public:
  Foo() {
  }
  ~Foo() {}
  virtual void _ready() override {
    internal::gdextension_interface_print_error_with_message(
      "Description", "Hello from Foo", __func__, __FILE__, __LINE__, false
    );
  }

  virtual void _process(double) {
  }

  void set_float(float f) {
    that_float = f;
  }

  float get_float() const {
    return that_float;
  }

  void set_scene(Ref<PackedScene> scene) {
    this->scene = scene;
  }

  Ref<PackedScene> get_scene() const {
    return scene;
  }

  void set_image(Ref<Image> i) {
    image = i;
  }

  Ref<Image> get_image() const {
    return image;
  }
protected:
  static void _bind_methods() {
    ClassDB::bind_method(D_METHOD("get_float"), &Foo::get_float);
    ClassDB::bind_method(D_METHOD("set_float", "that_float_setted"),
        &Foo::set_float);
    PropertyInfo prop(Variant::FLOAT, "that_float_prop", PROPERTY_HINT_NONE,
        "", PROPERTY_USAGE_NO_EDITOR);
    ADD_PROPERTY(prop, "set_float", "get_float");

    ClassDB::bind_method(D_METHOD("get_scene"), &Foo::get_scene);
    ClassDB::bind_method(D_METHOD("set_scene", "new_player"), &Foo::set_scene);
    PropertyInfo sprop(Variant::OBJECT, "player", PROPERTY_HINT_NONE,
        "", PROPERTY_USAGE_DEFAULT);
    ADD_PROPERTY(sprop, "set_scene", "get_scene");
    
    ClassDB::bind_method(D_METHOD("set_image", "image"), &Foo::set_image);
    ClassDB::bind_method(D_METHOD("get_image"), &Foo::get_image);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "image"),
        "set_image", "get_image");
  }
private:
  Ref<PackedScene> scene;
  Ref<Image> image;
  float that_float;
};
*/
#include "print_utils.hxx"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
class Test : public godot::Node
{
  GDCLASS(Test, Node)
public:
  Test()
  {
  }

  virtual void _ready() override
  {
    // pnd::print({}, "Test Class Name: {}", get_class().utf8().ptr());
    // print(CUR_PRINT_INFO, "Is editor: {}",
    //     godot::Engine::get_singleton()->is_editor_hint());
    // print(CUR_PRINT_INFO, "Test Node _ready()");
    // void *mem = enet_malloc(4);
    // print(CUR_INFO, "Mem is zero: {}", mem == nullptr);
  }

  static void _bind_methods()
  {
    ADD_SIGNAL(MethodInfo("sig"));
  }

};

void initialize_ppg_server_module(godot::ModuleInitializationLevel level)
{
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;

  ClassDB::register_class<pnd::server::Server>();
  ClassDB::register_class<pnd::User>();
  ClassDB::register_class<pnd::client::DynamicInfo>();
  ClassDB::register_class<pnd::client::Client>();
  ClassDB::register_class<Test>();
}

void terminate_ppg_server_module(godot::ModuleInitializationLevel level)
{
  if (level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;
}

extern "C"
{
  GDExtensionBool GDE_EXPORT init_ppg_library(
      GDExtensionInterfaceGetProcAddress get_proc_addr,
      const GDExtensionClassLibraryPtr library,
      GDExtensionInitialization *initialization)
  {
    godot::GDExtensionBinding::InitObject init_obj(get_proc_addr,
        library,
        initialization);

    init_obj.register_initializer(initialize_ppg_server_module);
    init_obj.register_terminator(terminate_ppg_server_module);
    init_obj.set_minimum_library_initialization_level(
        godot::MODULE_INITIALIZATION_LEVEL_SCENE);
    
    return init_obj.init();
  }
}