#pragma once
#include <QtGlobal> // for Q_ASSERT
#include <array>

namespace Z3DAttr {

  // Type-safe enum for all bound vertex attributes
  enum class Attr : int {
    Vertex              = 0,
    Color               = 1,
    Normal              = 2,
    Tex1D0              = 3,
    Tex2D0              = 4,
    Tex3D0              = 5,
    Axis                = 6,
    Origin              = 7,
    VertexRadius        = 8,
    Colors              = 9,
    Colors2             = 10,
    Flags               = 11,
    SpecularShininess   = 12,
  };

  constexpr inline int loc(Attr a) noexcept { return static_cast<int>(a); }

  // Optional: a compile-time mapping (name → index) used when binding before link
  struct Binding { const char* name; Attr attr; };

  constexpr std::array<Binding, 13> kBindings{{
    {"attr_vertex",             Attr::Vertex},
    {"attr_color",              Attr::Color},
    {"attr_normal",             Attr::Normal},
    {"attr_1dTexCoord0",        Attr::Tex1D0},
    {"attr_2dTexCoord0",        Attr::Tex2D0},
    {"attr_3dTexCoord0",        Attr::Tex3D0},
    {"attr_axis",               Attr::Axis},
    {"attr_origin",             Attr::Origin},
    {"attr_vertex_radius",      Attr::VertexRadius},
    {"attr_colors",             Attr::Colors},
    {"attr_colors2",            Attr::Colors2},
    {"attr_flags",              Attr::Flags},
    {"attr_specular_shininess", Attr::SpecularShininess},
  }};

} // namespace Z3DAttr
